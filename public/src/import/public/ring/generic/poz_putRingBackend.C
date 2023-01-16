/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/ring/generic/poz_putRingBackend.C $  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
//------------------------------------------------------------------------------
/// @file  poz_putRingBackend.H
/// @brief Backend code for fapi2::putRing
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Joachim Fenkes <fenkes@de.ibm.com>
// *HWP FW Maintainer   : TBD
//------------------------------------------------------------------------------
#include <poz_putRingBackend.H>
#include <file_access.H>
#include <scan_compression.H>
#include <poz_perv_mod_misc.H>
#include <poz_perv_utils.H>
#include <poz_ring_ids.H>

#ifdef __PPE__
    #include "ppe42_string.h"
#endif

#define SCOMT_OMIT_FIELD_ACCESSORS
#include <p11_scom_perv_tpchip.H>

using namespace fapi2;
using namespace rs4;
using namespace scomt::perv;

#ifdef TEST_HARNESS
//////////////////////////////////////////////////////////////////////////////////////////////////
//// TEST HARNESS FOR UNIT TESTING
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstring>

namespace fapi2
{

template< TargetType K, MulticastType M, typename V >
ReturnCode plat_putRingImpl(const Target<K, M, V>& i_target,
                            const void* i_scanImage,
                            const uint32_t i_modifiedRingAddress,
                            const RingMode i_ringMode)
{
    std::cout << std::hex << "   putRing "
              << "target: " << i_target.get() << "; "
              << "Scan image: " << i_scanImage << "; "
              << "modified ring address: " << i_modifiedRingAddress << "; "
              << "ring mode: " << i_ringMode
              << std::endl;

    return FAPI2_RC_SUCCESS;
}

}

fapi2::ReturnCode mod_multicast_setup(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    uint8_t i_group_id,
    uint64_t i_chiplets,
    fapi2::TargetState i_pgood_policy)
{
    std::cout << "   multicast setup group " << int(i_group_id) << " chiplets " << std::hex << i_chiplets << std::dec <<
              " policy " << i_pgood_policy << std::endl;

    return FAPI2_RC_SUCCESS;
}
#endif /* TEST_HARNESS */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
//// CONSTANTS
//////////////////////////////////////////////////////////////////////////////////////////////////

/// We may use a temporary multicast group for scanning; this is the designated group number.
static const MulticastGroup MCGROUP_SCAN_TARGETS = MCGROUP_5;

/// The set of scan regions (in a scan address) for which we support parallel scanning
static const uint32_t EQ_PARALLEL_SCAN_REGIONS = 0x4410; // cl20, l30, mma0

/// The list of subdirectories attempted in order when loading a scan image
const char ringTypeOrder[][8] =
{
    "base/", "cust/", "dynXX/", "bmc/", "hb/", "ovr/"
};

/// The base directory for all scan images
const char ringBaseDir[] = "rings/";

/// Target types that correspond 1:1 to a chiplet, i.e. units of which
/// only one exists per chiplet and therefore to target the unit for
/// scanning we can simply target the entire chiplet without having
/// to play any games with the scan address.
static const TargetType TARGET_TYPE_ACTUALLY_PERV = TARGET_TYPE_NONE
        | TARGET_TYPE_EX
        | TARGET_TYPE_XBUS
        | TARGET_TYPE_ABUS
        | TARGET_TYPE_EQ
        | TARGET_TYPE_PERV
        | TARGET_TYPE_PEC
        | TARGET_TYPE_MC
        | TARGET_TYPE_IOHS
        | TARGET_TYPE_PAUC
        | TARGET_TYPE_TBUSC
        | TARGET_TYPE_PAX
        | TARGET_TYPE_PAXO
        | TARGET_TYPE_PEC6P
        | TARGET_TYPE_PEC2P
        ;

/// Target types that correspond to a unit inside a chiplet of which
/// there are multiples to a chiplet, which means that some scan address
/// modification may be needed.
/// This excludes core/l2/l3/mma since they get special treatment.
static const TargetType TARGET_TYPE_CHIP_UNIT = TARGET_TYPE_NONE
        | TARGET_TYPE_CORE
        | TARGET_TYPE_L3CACHE
        | TARGET_TYPE_NMMU
        | TARGET_TYPE_PAU
        | TARGET_TYPE_TBUSL
        | TARGET_TYPE_INT
        | TARGET_TYPE_NX
        | TARGET_TYPE_PHB16X
        ;


//////////////////////////////////////////////////////////////////////////////////////////////////
//// UTILITY FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Given a scan address, return the chiplet ID
constexpr uint32_t getScanChiplet(uint32_t i_scanAddr)
{
    return i_scanAddr >> 24;
}

/// @brief Given a scan address, return the scan regions
constexpr uint32_t getScanRegions(uint32_t i_scanAddr)
{
    return (i_scanAddr & 0x0000FFF0) | ((i_scanAddr & 0x00F00000) >> 20);
}

/// @brief Replace the scan regions in a scan address, return the updated scan address
constexpr uint32_t setScanRegions(uint32_t i_origAddr, uint32_t i_scanRegion)
{
    return (i_origAddr & 0xFF0F000F) | (i_scanRegion & 0x0000FFF0) | ((i_scanRegion << 20) & (0x00F00000));
}

/// @brief Expand the log2 encoded target type in an RS4 image into its one-hot bitmask representation
static inline TargetType rs4_getTargetType(const CompressedScanData& i_rs4)
{
    return static_cast<TargetType>(1ULL << i_rs4.iv_targetType.get());
}

/// @brief Determine the member chiplets in a (potentially) multicast perv target
/// @return A bit mask with bits set corresponding to each member chiplet
static inline uint64_t getGroupMembers(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target)
{
    buffer<uint64_t> l_result;

    for (auto cplt : i_target.getChildren<TARGET_TYPE_PERV>())
    {
        l_result.setBit(cplt.getChipletNumber());
    }

    return l_result;
}

/// @brief Get a perv target corresponding to a given chiplet ID
static inline ReturnCode getChiplet(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_chip_target,
    const int i_chiplet_id,
    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > &o_perv_target)
{
    for (auto cplt : i_chip_target.getChildren<TARGET_TYPE_PERV>())
    {
        if (cplt.getChipletNumber() == i_chiplet_id)
        {
            o_perv_target = cplt;
            return FAPI2_RC_SUCCESS;
        }
    }

    FAPI_ASSERT(false,
                SCAN_CHIPLET_NOT_FUNCTIONAL()
                .set_CHIP_TARGET(i_chip_target)
                .set_CHIPLET_ID(i_chiplet_id),
                "Requested chiplet (%d) is not functional",
                i_chiplet_id);

fapi_try_exit:
    return current_err;
}

/*
/// A variant of strcpy that returns a pointer to the end of the string for easy appending.
/// A rather obscure part of the C standard library, it's present on the Cronus platform
/// but will have to be provided by other platforms.
char* stpcpy(char* dest, const char* src)
{
    while (*src)
    {
        *dest++ = *src++;
    }

    *dest = *src;
    return dest;
}
*/


//////////////////////////////////////////////////////////////////////////////////////////////////
//// INSTANCE TRAITS
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief A class encapsulating the instance targeting behavior needed for a given RS4 image
 *
 * Assumptions:
 *   * All instances of a given target type are contained within a consecutive range of chiplets
 *   * If there are multiple instances of a given target per chiplet, their scan regions
 *     are equidistant (e.g. regions 1, 3, 5, 7 or regions 2, 3, 4)
 *   * The amount of instances per chiplet is equal across all chiplets in the group
 */
struct InstanceTraits
{
    int base_chiplet;            ///< The first chiplet in the group
    int chiplets_in_group;       ///< Amount of chiplets in the group
    int instances_per_chiplet;   ///< Amount of instances per single chiplet
    int scan_region_shift;       ///< How much to shift a scan region to get from one instance to the next

    bool is_instance_image;      ///< Is this image an instance image (true) or a common one (false)?

    int image_local_instance;    ///< For instance images, the instance number inside the chiplet
    int image_instance_chiplet;  ///< For instance images, the chiplet id containing the instance

    /// @brief Determine instance traits from a given RS4 image
    inline InstanceTraits(const CompressedScanData& i_rs4) :
        base_chiplet(getScanChiplet(i_rs4.iv_scanAddr.get())),
        chiplets_in_group((i_rs4.iv_instanceTraits.get() & 0x1F) + 1),
        instances_per_chiplet(((i_rs4.iv_instanceTraits.get() >> 5) & 0x7) + 1),
        scan_region_shift(((i_rs4.iv_instanceTraits.get() >> 8) & 0x7) + 1),
        is_instance_image(i_rs4.iv_type.get() & RS4_TYPE_INSTANCE),
        image_local_instance(i_rs4.iv_instanceNum.get())
    {
        split_instance(image_local_instance, image_instance_chiplet);
    }

    /**
     * @brief Split a chip instance number into a chiplet ID and chiplet-local instance number
     *
     * @param[inout] The chip level instance number coming in, the chiplet-local instance coming out
     * @param[out]   The chiplet ID containing the requested instance
     */
    inline void split_instance(int& io_instance, int& o_chiplet) const
    {
        // This is basically just a combined div+mod operation but
        // div+mod are very expensive on PPE platforms and there is no
        // combined divmod operation.
        // Since our quotients are very small we can get away with
        // rolling our own naive implementation.
        o_chiplet = base_chiplet;

        FAPI_DBG(">> split_instance(%d) base_chiplet=0x%02x instances_per_chiplet=%d",
                 io_instance, base_chiplet, instances_per_chiplet);

        while (io_instance >= instances_per_chiplet)
        {
            o_chiplet++;
            io_instance -= instances_per_chiplet;
        }

        FAPI_DBG("<< split_instance -> cplt=0x%02x inst=%d",
                 o_chiplet, io_instance);
    }

    /// @brief Get a mask of chiplets matching this scan image
    inline uint64_t chiplet_mask() const
    {
        // For an instance image, return a mask containing the single targeted chiplet
        // For a common image, return a mask with bits set according to the entire chiplet group
        return is_instance_image ?
               (1ULL << (63 - image_instance_chiplet)) :
               bit_mask(base_chiplet, chiplets_in_group);
    }

    /// @brief Get a core select value for core scan images
    inline uint8_t core_select() const
    {
        return is_instance_image ? (8 >> image_local_instance) : 0xF;
    }
};


//////////////////////////////////////////////////////////////////////////////////////////////////
//// TEMPORARY MULTICAST GROUP MANAGEMENT
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Utility class to set up a temporary multicast group, saving/restoring group members if necessary
 */
class ChipTarget
{
    public:
        /**
         * @brief Create a ChipTarget object
         * @param[in]  i_chip_target   The chip target to encapsulate
         */
        ChipTarget(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_chip_target) :
            iv_chip_target(i_chip_target), iv_mc_saved(false), iv_mc_saved_members(0) {}

        /**
         * @brief Clean up the target object, potentially restoring saved multicast groups
         */
        ~ChipTarget();

        /**
         * @brief Set up the temporary multicast group, saving its previous set of members
         */
        ReturnCode setup_scan_mc_group(uint64_t i_chiplets);

        /**
         * @brief Return the underlying chip target
         */
        inline const Target<TARGET_TYPE_ANY_POZ_CHIP>& operator()(void)
        {
            return iv_chip_target;
        }

    private:
        const Target<TARGET_TYPE_ANY_POZ_CHIP>& iv_chip_target;
        bool iv_mc_saved;
        uint64_t iv_mc_saved_members;
};

ChipTarget::~ChipTarget()
{
    // Restore members of the temporary group iff we saved them off earlier
    if (iv_mc_saved)
    {
        mod_multicast_setup(iv_chip_target, MCGROUP_SCAN_TARGETS, iv_mc_saved_members, TARGET_STATE_PRESENT);
    }
}

ReturnCode ChipTarget::setup_scan_mc_group(uint64_t i_chiplets)
{
    // Save the members of the temporary group before we set it up for the first time
    if (not iv_mc_saved)
    {
        iv_mc_saved_members = getGroupMembers(iv_chip_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_SCAN_TARGETS));
        iv_mc_saved = true;
    }

    return mod_multicast_setup(iv_chip_target, MCGROUP_SCAN_TARGETS, i_chiplets, TARGET_STATE_FUNCTIONAL);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//// SCAN APPLY ENGINE
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Encapsulates the code and data needed to apply an RS4 image to an arbitrary target
 *
 * Since we have to drag a bunch of data with us across all possible code paths,
 * instead of a bunch of functions with a lot of parameters each we create a class
 * that has all this persistent data as constant member variables.
 *
 * The basic flow is that we break the complex task of "any image, any target type" down
 * into simpler and simpler code paths until we have a single (potentially multicast)
 * TARGET_TYPE_PERV target and a scan address (which may have been modified from the image's
 * to target a different target instance). In other words, we reduce any target and image
 * combination into a chiplet (or multiples thereof) and a set of scan regions.
 */
class ScanApplyEngine
{
    public:
        ScanApplyEngine(ChipTarget& i_chip_target,
                        const Target<TARGET_TYPE_ALL_MC>& i_target,
                        const CompressedScanData& i_image,
                        const RingMode i_ringMode) :
            iv_chip_target(i_chip_target), iv_target(i_target), iv_image(i_image),
            iv_ringMode(i_ringMode), iv_instanceTraits(i_image)
        {
            FAPI_DBG(">> ScanApplyEngine (version=%d type=0x%02x scanAddr=0x%08x size=%d",
                     i_image.iv_version.get(), i_image.iv_type.get(),
                     i_image.iv_scanAddr.get(), i_image.iv_size.get());
            FAPI_DBG("                   instanceNum=%d targetType=%d instanceTraits=0x%04x reserved=0x%04x)",
                     i_image.iv_instanceNum.get(), i_image.iv_targetType.get(),
                     i_image.iv_instanceTraits.get(), i_image.iv_reserved.get());
            FAPI_DBG("  instance traits: base_chiplet=0x%02x chiplets_in_group=%d instances_per_chiplet=%d scan_region_shift=%d",
                     iv_instanceTraits.base_chiplet, iv_instanceTraits.chiplets_in_group,
                     iv_instanceTraits.instances_per_chiplet, iv_instanceTraits.scan_region_shift);
            FAPI_DBG("                   image_instance_chiplet=0x%02x image_local_instance=%d",
                     iv_instanceTraits.image_instance_chiplet, iv_instanceTraits.image_local_instance);
        }

        // The main entry point
        ReturnCode apply(void);

    private:
        // The constant data we're dragging along through the call chain
        ChipTarget& iv_chip_target;
        const Target<TARGET_TYPE_ALL_MC>& iv_target;
        const CompressedScanData& iv_image;
        const RingMode iv_ringMode;
        const InstanceTraits iv_instanceTraits;

        // A utility function for apply
        ReturnCode checkScanTargetType(const TargetType i_allowedTypes);

        // The various code paths depending on the target
        ReturnCode applyToPervMCTarget(
            const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > &i_perv_mc_target);
        ReturnCode applyToCoreMCTarget(
            const Target < TARGET_TYPE_CORE | TARGET_TYPE_MULTICAST > &i_target);
        ReturnCode applyToChipUnitTarget(
            const Target<TARGET_TYPE_CHIP_UNIT>& i_target);

        // Apply the image to a single instance inside a chiplet
        ReturnCode applyToSingleInstance(
            const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > &i_target,
            const int i_instance);
};


/**
 * @brief Check that the target type of the RS4 image matches a list of allowed types
 */
ReturnCode ScanApplyEngine::checkScanTargetType(const TargetType i_allowedTypes)
{
    const int l_logTargetType = iv_image.iv_targetType.get();
    const uint64_t l_targetType = 1ULL << l_logTargetType;
    FAPI_ASSERT(l_targetType & i_allowedTypes,
                INVALID_SCAN_TARGET_TYPE()
                .set_TARGET(iv_target)
                .set_VALID_TARGET_TYPES(i_allowedTypes)
                .set_RS4_TARGET_TYPE(l_targetType)
#ifndef __PPE__
                .set_RS4_HEADER(iv_image),
#else
                .set_RS4_HEADER((uint64_t)(iv_image.iv_magic.get()) << 48 |
                                (uint64_t)(iv_image.iv_version.get()) << 40 |
                                (uint64_t)(iv_image.iv_type.get()) << 32  |
                                (uint64_t)(iv_image.iv_scanAddr.get()) ) ,
#endif
                "Scan image target type (%d) does not fit expected target types (0x%08x%08x) based on target",
                l_logTargetType, i_allowedTypes >> 32, i_allowedTypes & 0xFFFFFFFF);

fapi_try_exit:
    return current_err;
}

ReturnCode ScanApplyEngine::apply()
{
    /* Defer to sub-functions depending on target type */
    /* We exclude most code paths on the QME to reduce compiled code size */

    /* Chip targets */
    if (not is_platform<PLAT_QME>())
    {
        Target<TARGET_TYPE_ANY_POZ_CHIP> l_chip_target;

        if (FAPI2_RC_SUCCESS == iv_target.reduceType(l_chip_target))
        {
            /*
             * If we get a pure chip target, we pretend that instead we got
             * a multicast PERV target pointing to the "all good chiplets" group.
             *
             * NOTE this will work from istep 2 onwards since the multicast groups
             * are set up. We have to be careful to target the TP chiplet directly
             * during istep 1.
             */
            auto l_perv_mc_target = l_chip_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_GOOD);

            // Schlimme Dinge: We're assuming that in this case l_chip_target == iv_chip_target
            return applyToPervMCTarget(l_perv_mc_target);
        }
    }

    /* Special case handling for multicast+multiregion core targets */
    /* Unicast core targets will be handled by the chip unit path below */
    {
        Target < TARGET_TYPE_CORE | TARGET_TYPE_MULTICAST > l_core_mc_target;

        if (FAPI2_RC_SUCCESS == iv_target.reduceType(l_core_mc_target)
            && (iv_target.getCoreSelect() != 0))
        {
            FAPI_TRY(checkScanTargetType(TARGET_TYPE_CORE));
            return applyToCoreMCTarget(l_core_mc_target);
        }
    }

    /* Targets of any type that resolves to entire chiplets. May be multicast. */
    if (not is_platform<PLAT_QME>())
    {
        Target < TARGET_TYPE_ACTUALLY_PERV | TARGET_TYPE_MULTICAST > l_perv_mc_target;

        if (FAPI2_RC_SUCCESS == iv_target.reduceType(l_perv_mc_target))
        {
            FAPI_TRY(checkScanTargetType(TARGET_TYPE_ACTUALLY_PERV));

            // Reduce the target type to PERV.
            // For unicast targets, the platform will conveniently remap the
            // chiplet ID for us.
            // For multicast targets this will basically just be a typecast.
            auto l_perv_mc_target_narrowed =
                l_perv_mc_target.getParent < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > ();

            return applyToPervMCTarget(l_perv_mc_target_narrowed);
        }
    }

    /* Chip unit targets - can't sensibly be multicast */
    if (not is_platform<PLAT_QME>())
    {
        Target<TARGET_TYPE_CHIP_UNIT> l_chip_unit_target;

        if (FAPI2_RC_SUCCESS == iv_target.reduceType(l_chip_unit_target))
        {
            FAPI_TRY(checkScanTargetType(TARGET_TYPE_CHIP_UNIT));
            return applyToChipUnitTarget(l_chip_unit_target);
        }
    }

    /* Catch all unsupported target types */
    FAPI_ASSERT(false,
                UNSUPPORTED_SCAN_TARGET_TYPE()
                .set_TARGET(iv_target),
                "Unsupported target type for scanning");

fapi_try_exit:
    return current_err;
}

/**
 * @brief Apply the image to a (potentially multicast) perv (i.e. chiplet) target
 *
 * @param[in] i_perv_mc_target The chiplet(s) to consider
 */
ReturnCode ScanApplyEngine::applyToPervMCTarget(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > &i_perv_mc_target)
{
    // Determine the chiplets in the multicast group (or the single targeted chiplet)
    // and of those, the ones that would be covered by the scan image.
    const uint64_t l_group_members = getGroupMembers(i_perv_mc_target);
    const uint64_t l_target_chiplets = l_group_members & iv_instanceTraits.chiplet_mask();

    FAPI_DBG(">> applyToPervMCTarget l_group_members=0x%08x_%08x l_target_chiplets=0x%08x_%08x",
             l_group_members >> 32, l_group_members & 0xFFFFFFFF,
             l_target_chiplets >> 32, l_target_chiplets & 0xFFFFFFFF);

    // Start with the assumption that we can use the incoming target for scanning.
    // We will verify this soon and update the target if necessary.
    auto l_scan_target = i_perv_mc_target;

    if (!l_target_chiplets)
    {
        // The image touches none of the group members.
        // We have nothing to do, and this isn't an error either.
        return FAPI2_RC_SUCCESS;
    }
    else if (is_power_of_two(l_target_chiplets))
    {
        // The chiplet vector is a power of two, i.e. we're down to a single chiplet.
        // Construct a unicast target for further use.
        const int l_chiplet_id = __builtin_clzll(l_target_chiplets);  // clzll == count leading zeros long long
        FAPI_DBG("single chiplet %d", l_chiplet_id);
        FAPI_TRY(getChiplet(iv_chip_target(), l_chiplet_id, l_scan_target));
    }
    else if (l_target_chiplets != l_group_members)
    {
        // We have multiple chiplets but the incoming MC group has more members
        // than we need. Construct a new group to match our requirements.
        FAPI_TRY(iv_chip_target.setup_scan_mc_group(l_target_chiplets));
        l_scan_target = iv_chip_target().getMulticast<TARGET_TYPE_PERV>(MCGROUP_SCAN_TARGETS);
    }

    // We now know our target. Let's figure out the instance(s) within the chiplet(s) next.
    if (iv_instanceTraits.is_instance_image)
    {
        // For an instance image, apply to the single target instance
        FAPI_TRY(applyToSingleInstance(l_scan_target, iv_instanceTraits.image_local_instance));
    }
    else
    {
        // For a common target we have to attempt all instances one by one;
        // there is no generic way to parallel scan multiple instances as we can for cores.
        for (int l_instance = 0; l_instance < iv_instanceTraits.instances_per_chiplet; l_instance++)
        {
            FAPI_TRY(applyToSingleInstance(l_scan_target, l_instance));
        }
    }

fapi_try_exit:
    return current_err;
}

/**
 * @brief Apply the image to a multiast + multiregion core target
 *
 * The unicast core case will be taken care of by a different code path
 * so we can assume the target to be an actual multicast target.
 *
 * This is the only code path supported on the QME platform.
 *
 * @param[in] i_target The multicast core target
 */
ReturnCode ScanApplyEngine::applyToCoreMCTarget(
    const Target < TARGET_TYPE_CORE | TARGET_TYPE_MULTICAST > & i_target)
{
    FAPI_DBG(">> applyToCoreMCTarget");

    // Determine a couple parameters
    auto perv_mc_target = i_target.getParent < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > ();
    const uint32_t scan_regions = getScanRegions(iv_image.iv_scanAddr.get());

    // Bail out early if an instance image cannot possibly match the target's core selection
    const uint8_t core_select = i_target.getCoreSelect() & iv_instanceTraits.core_select();

    if (!core_select)
    {
        return FAPI2_RC_SUCCESS;
    }

    // Prepare the modified scan address; bitop magic ahead!

    // Broadcast the core select across all three possible region ranges:
    // 0b1234 * 0b0000100010000010 = 0b0123412340012340
    const uint32_t core_select_expanded = core_select * 0x0882;

    // Broadcast 0b1111 across the actually selected scan regions:
    // 0b0000C000L000000M0 * 0b1111 = 0b0CCCCLLLL00MMMM0
    const uint32_t scan_regions_expanded = (scan_regions >> 3) * 0xF;

    // Then combine the two to get the actually selected scan regions
    const uint32_t target_regions = core_select_expanded & scan_regions_expanded;
    const uint32_t modified_scan_addr = setScanRegions(iv_image.iv_scanAddr.get(), target_regions);

    // Double check that this code path is only used for images that can be
    // parallel scanned if necessary.
    FAPI_ASSERT(i_target.getCoreSelect() && (scan_regions & EQ_PARALLEL_SCAN_REGIONS) == scan_regions,
                CORE_MULTICAST_SCAN_INVALID_ARGS()
                .set_TARGET(i_target)
                .set_CORE_SELECT(core_select)
                .set_REGIONS(scan_regions)
#ifndef __PPE__
                .set_RS4_HEADER(iv_image),
#else
                .set_RS4_HEADER((uint64_t)(iv_image.iv_magic.get()) << 48 |
                                (uint64_t)(iv_image.iv_version.get()) << 40 |
                                (uint64_t)(iv_image.iv_type.get()) << 32  |
                                (uint64_t)(iv_image.iv_scanAddr.get()) ) ,
#endif
                "Core multicast scan attempted with empty core selection or "
                "on regions that don't support parallel scan: "
                "core_select=%d scan_regions=0x%04x", core_select, scan_regions);

    if (iv_instanceTraits.is_instance_image)
    {
        // Since we have an instance image, see if one of the MC group members matches.
        // If none of them match - welp, we gave it our best.
        for (auto perv_target : perv_mc_target.getChildren<TARGET_TYPE_PERV>())
        {
            if (perv_target.getChipletNumber() == iv_instanceTraits.image_instance_chiplet)
            {
                return plat_putRingImpl(perv_target, &iv_image, modified_scan_addr, iv_ringMode);
            }
        }
    }
    else
    {
        // On a common image, defer right to the platform impl
        const bool parallel_scan = not is_power_of_two(core_select);
        const RingMode ringMode = parallel_scan ? iv_ringMode : (iv_ringMode | RING_MODE_PARALLEL_SCAN);
        return plat_putRingImpl(perv_mc_target, &iv_image, modified_scan_addr, ringMode);
    }

fapi_try_exit:
    return current_err;
}

/**
 * @brief Apply the image to a chip unit target
 *
 * Chip unit targets can't possibly be multicast so this path is rather simple.
 *
 * @param[in] i_target The chip unit target
 */
ReturnCode ScanApplyEngine::applyToChipUnitTarget(
    const Target<TARGET_TYPE_CHIP_UNIT>& i_target)
{
    FAPI_DBG(">> applyToChipUnitTarget");

    // Determine the instance number of the target
    ATTR_CHIP_UNIT_POS_Type target_instance;
    FAPI_TRY(FAPI_ATTR_GET(ATTR_CHIP_UNIT_POS, i_target, target_instance));

    // Scan if we have a common image or the instance matches
    if (not iv_instanceTraits.is_instance_image or
        (target_instance == iv_image.iv_instanceNum.get()))
    {
        // Split the target instance number into chiplet ID and chiplet-local instance number
        int local_instance = target_instance;
        int chiplet; // unused but we need the parameter
        iv_instanceTraits.split_instance(local_instance, chiplet);

        // Determine the chiplet containing the target instance; use platform code for this
        auto perv_target = i_target.getParent<TARGET_TYPE_PERV>();

        FAPI_TRY(applyToSingleInstance(perv_target, local_instance));
    }

fapi_try_exit:
    return current_err;
}

/**
 * @brief Apply the image to a single instance within the targeted chiplet(s)
 *
 * @param[in] i_target   The chiplet(s) to scan into
 * @param[in] i_instance The targeted instance within the chiplet(s)
 */
ReturnCode ScanApplyEngine::applyToSingleInstance(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > &i_target,
    const int i_instance)
{
    FAPI_DBG(">> applyToSingleInstance i_instance=%d", i_instance);

    // Shift the scan region from the image header according to the instance number
    // and the image's instance traits, then update the scan region with the new regions.
    const uint32_t l_scanRegion = getScanRegions(iv_image.iv_scanAddr.get())
                                  >> (iv_instanceTraits.scan_region_shift * i_instance);
    const uint32_t l_modifiedScanAddr = setScanRegions(iv_image.iv_scanAddr.get(), l_scanRegion);

    // Read CPLT_CTRL2 (with OR combination in case of multicast)
    // to determine all regions that are enabled in at least one target chiplet.
    buffer<uint64_t> l_cplt_ctrl2;
    FAPI_TRY(getScom(i_target, CPLT_CTRL2_RW, l_cplt_ctrl2));

    // Attempt to scan only if the selected regions are enabled in at least one target chiplet.
    // If the target regions are disabled on some of the chiplets their clock controller will
    // prevent scanning and fake a good scan result.
    if ((l_cplt_ctrl2.getBits<4, 15>() & l_scanRegion) == l_scanRegion)
    {
        // FINALLY \o/
        return plat_putRingImpl(i_target, &iv_image, l_modifiedScanAddr, iv_ringMode);
    }

fapi_try_exit:
    return current_err;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//// PROCEDURE CODE
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief internal implementation of poz_applyCompositeImage
 */
static ReturnCode applyCompositeImage(ChipTarget& i_chip_target,
                                      const Target<TARGET_TYPE_ALL_MC>& i_target,
                                      const void* i_image,
                                      const size_t i_size,
                                      const RingMode i_ringMode)
{
    const uint8_t* start = (uint8_t*)i_image;
    size_t offset = 0;

    // Keep walking the chain of RS4s until we reach the end
    while (offset < i_size)
    {
        const uint8_t* ptr = start + offset;
        const CompressedScanData* hdr = (CompressedScanData*)ptr;

        // Check that we didn't veer off course
        FAPI_ASSERT(hdr->iv_magic.get() == RS4_MAGIC && hdr->iv_version.get() == RS4_VERSION,
                    INVALID_RING_IMAGE()
#ifndef __PPE__
                    .set_RS4_HEADER(*hdr)
#else
                    .set_RS4_HEADER((uint64_t)(hdr->iv_magic.get()) << 48 |
                                    (uint64_t)(hdr->iv_version.get()) << 40 |
                                    (uint64_t)(hdr->iv_type.get()) << 32  |
                                    (uint64_t)(hdr->iv_scanAddr.get()) )
#endif
                    .set_OFFSET(offset)
                    .set_MAGIC(hdr->iv_magic.get())
                    .set_VERSION(hdr->iv_version.get())
                    .set_VERSION_REQUIRED(RS4_VERSION),
                    "Invalid RS4 header in scan image! offset=0x%x magic=0x%04X version=%d version_required=%d",
                    offset, hdr->iv_magic.get(), hdr->iv_version.get(), RS4_VERSION);

        // Apply the RS4
        FAPI_TRY(ScanApplyEngine(i_chip_target, i_target, *hdr, i_ringMode).apply());

        // Skip to the end of the current RS4
        offset += hdr->iv_size.get();
    }

fapi_try_exit:
    return current_err;
}

ReturnCode poz_applyCompositeImage(const Target<TARGET_TYPE_ALL_MC>& i_target,
                                   const void* i_image,
                                   const size_t i_size,
                                   const RingMode i_ringMode)
{
    auto chip_target = ChipTarget(i_target.getParent<TARGET_TYPE_ANY_POZ_CHIP>());
    return applyCompositeImage(chip_target, i_target, i_image, i_size, i_ringMode);
}

/**
 * @brief Try to load a composite scan image by file name and apply it if found
 *
 * @param[in] i_chip_target Parent chip of the scan target
 * @param[in] i_target      Target to apply to
 * @param[in] i_fname       Full path name of the scan image
 * @param[in] i_ringMode    Scan mode flags
 *
 * @return FAPI2_RC_SUCCESS if an image was found and successfully scanned in
 *         FAPI2_RC_SUCCESS also if the image was not found(!!)
 *         something else otherwise
 */
static ReturnCode tryLoadCompositeImage(ChipTarget& i_chip_target,
                                        const Target<TARGET_TYPE_ALL_MC>& i_target,
                                        const char* i_fname,
                                        const RingMode i_ringMode)
{
    void* image = NULL;
    size_t image_size;
    ReturnCode rc = loadEmbeddedFile(i_chip_target(), i_fname, image, image_size);

    if (rc == FAPI2_RC_FILE_NOT_FOUND)
    {
        // We're feeling our way around the embedded archive; don't fail if a file does not exist
        return FAPI2_RC_SUCCESS;
    }
    else if (rc != FAPI2_RC_SUCCESS)
    {
        // Not using FAPI_TRY here because we don't want to call freeEmbeddedImage below
        return rc;
    }

    // Hooray, we loaded the image; now do something with it!
#ifndef __PPE__
    FAPI_DBG("Loaded image %s", i_fname);
#endif
    FAPI_TRY(applyCompositeImage(i_chip_target, i_target, image, image_size, i_ringMode));

fapi_try_exit:
    freeEmbeddedFile(image);
    return current_err;
}

ReturnCode poz_putRingBackend(const Target<TARGET_TYPE_ALL_MC>& i_target,
                              const char* i_ring_id,
                              const RingMode i_ringMode)
{
    char fname[RING_ID_MAXLEN + 16];
    memcpy(fname, ringBaseDir, sizeof(ringBaseDir));

    auto chip_target = ChipTarget(i_target.getParent<TARGET_TYPE_ANY_POZ_CHIP>());

    // Grab the vector of dynamic chip features
    ATTR_DYNAMIC_INIT_FEATURE_VEC_Type dynamic_features;
    FAPI_TRY(FAPI_ATTR_GET(ATTR_DYNAMIC_INIT_FEATURE_VEC, chip_target(), dynamic_features));

    // Iterate over all possible ring subdirs in order
    for (auto type : ringTypeOrder)
    {
        // Start constructing the directory by appending the subdir name to the base
        char* ptr = fname + sizeof(ringBaseDir) - 1;
        ptr = stpcpy(ptr, type);

        // Special handling for dynXX
        if (type[3] == 'X')
        {
            // be prepared for the dynamic features vector outgrowing the first 64 bits :)
            for (unsigned int word = 0; word < ARRAY_SIZE(dynamic_features); word++)
            {
                const buffer<uint64_t> dynamic_word = dynamic_features[word];

                for (int ovid = 0; ovid < 64; ovid++)
                {
                    if (dynamic_word.getBit(ovid))
                    {
                        // If a feature is enabled, overwrite the XX with the features hex ID
                        // and append the image name
                        strhex(ptr - 3, word * 64 + ovid, 2);
                        stpcpy(ptr, i_ring_id);
                        FAPI_TRY(tryLoadCompositeImage(chip_target, i_target, fname, i_ringMode));
                    }
                }
            }
        }
        else
        {
            // On anything other than dynXX we just append the image name and can proceed
            stpcpy(ptr, i_ring_id);
            FAPI_TRY(tryLoadCompositeImage(chip_target, i_target, fname, i_ringMode));
        }
    }

fapi_try_exit:
    return current_err;
}
