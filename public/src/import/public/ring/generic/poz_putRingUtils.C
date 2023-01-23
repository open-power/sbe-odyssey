/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/ring/generic/poz_putRingUtils.C $    */
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
/// @file   poz_putRingUtils.C
/// @brief  Given a chiplet target, scan address and RS4 image, scan the image
///         contents into the ring(s) targeted by the target & scan address
//------------------------------------------------------------------------------
// *HWP HW Maintainer:    Joachim Fenkes <fenkes@de.ibm.com>
// *HWP FW Maintainer:    Prem S Jha <premjha2@in.ibm.com>
//------------------------------------------------------------------------------

#include <poz_putRingUtils.H>
#include <scan_compression.H>

using namespace fapi2;
using namespace rs4;

enum
{
    OPCG_ALIGN_ADDRESS          =   0x00030001,
    OPCG_REG0_ADDRESS           =   0x00030002,
    OPCG_REG1_ADDRESS           =   0x00030003,
    OPCG_REG2_ADDRESS           =   0x00030004,
    SCAN_REGION_TYPE_ADDRESS    =   0x00030005,
    CLK_REGION_ADDRESS          =   0x00030006,
    OPCG_CAPT1_ADDRESS          =   0x00030010,
    OPCG_CAPT2_ADDRESS          =   0x00030011,
    OPCG_CAPT3_ADDRESS          =   0x00030012,
    ROTATE_ADDRESS_REG          =   0x00039000,
    SCAN64_ADDRESS              =   0x0003E000,
    SCAN64CONTSCAN              =   0x0003F000,
    CHECK_WORD_REG_ADDRESS      =   0x0003F040,
    NET_CTRL0_WAND              =   0x000F0041,
    NET_CTRL0_WOR               =   0x000F0042,
    MAX_RING_LIST               =   512,
    OPCG_GO_SINGLE_LOOP         =   0x4000000000000000ull,
    SCAN_HEADER_DATA            =   0xa5a5a5a5a5a5a5a5ull,
    MASK_RESERVE_REGION_BIT     =   0x0FFFFFFFFFFFFFFFull,
    SUPER_CHIPLET_BASE_ID       =   0x20,
    SUPER_CHIPLET_MAX_ID        =   0x27,
    SELECT_ALL_ECL              =   0x07800000,
    SELECT_ALL_L3               =   0x00780000,
    RING_ADDRESS_FASTINIT       =   0x00080000,
    ENABLE_FASTINIT             =   0x80000000,
    ENABLE_PARALLEL_SCAN        =   0x40000000,
    SHIFT_TO_BIT_ECL0           =   8,
    SHIFT_TO_BIT_L30            =   12,
    SHIFT_TO_BIT_MMA0           =   18,
    SCAN_REGION_ECL0            =   0x04000000,
    SCAN_REGION_L30             =   0x00400000,
    SCAN_REGION_MMA0            =   0x00010000,
    CHIPLET_MASK                =   0xff000000,
    SUPER_CHIPLET_MASK          =   0x20000000,
    MULTICAST_BIT               =   0x40000000,
    CPLT_STAT0                  =   0x00000100,
    CPLT_STAT0_CC_CTRL_OPCG_DONE_DC = 8,
    CPLT_STAT0_CC_CTRL_PARALLEL_SCAN_COMPARE_ERR = 10,

    SVS_LONG_ROTATE_MAX         = 0xFFFFF,
    SVS_SHORT_ROTATE_MAX        = 0x380,
};

/// @brief Constants for operations performed by putRing function.
enum opType_t
{
    ROTATE      =   0,  ///< Indicates a Rotate operation on the ring
    SCAN        =   1   ///< Indicates a Scan operation on the ring
};

///
/// @brief Return a big-endian-indexed nibble from a byte string
/// @param[in] i_rs4Str     The RS4 scan string
/// @param[in] i_nibbleIndx Index into i_rs4Str that need to converted into a nibble
/// @return big-endian-indexed nibble
///
inline uint8_t rs4_get_nibble( const uint8_t* i_rs4Str, const uint32_t i_nibbleIndx )
{
    uint8_t l_byte = i_rs4Str[i_nibbleIndx >> 1];

    return ((i_nibbleIndx % 2) ? (l_byte & 0x0f) : (l_byte >> 4));
}

///
/// @brief Return verbatim data from the RS4 string
/// @param[in] i_rs4Str         The RS4 scan string
/// @param[in] i_nibbleIndx     Index into RS4 string pointed to by i_rs4Str
/// @param[in] i_nibbleCount    The count of nibbles that need to be put in the return value.
/// @return big-endian-indexed double word
///
uint64_t rs4_get_verbatim( const uint8_t* i_rs4Str,
                           const uint32_t i_nibbleIndx,
                           const uint8_t i_nibbleCount )
{
    uint8_t l_byte;
    uint8_t l_nibble;
    uint64_t l_doubleWord   =   0;
    const uint32_t* l_data  =   NULL;
    uint32_t l_cnt          =   0;
    uint32_t l_offset       =   0;
    uint32_t l_index        =   i_nibbleIndx;
    rs4_data_t l_rs4_data;

    for( uint8_t i = 1; i <= i_nibbleCount; i++ )
    {
        l_cnt = l_index >> 1;

        if ( !( l_index %  8 ) || ( i == 1 ) )
        {
            l_offset    =   l_cnt - ( l_cnt % 4 );
            l_data      =   reinterpret_cast<uint32_t*> ( const_cast<uint8_t*>( i_rs4Str ) + l_offset );
            l_rs4_data.iv_data  =   *l_data;
        }

        l_byte      =   l_rs4_data.fields.val[l_cnt % 4];
        l_nibble    =   ( l_index % 2 ) ? ( l_byte & 0x0f ) : ( l_byte >> 4 );

        uint64_t l_tempDblWord = l_nibble;
        l_tempDblWord <<=   ( 64 - ( 4 * i ) );
        l_doubleWord   |=   l_tempDblWord;
        l_index++;
    }

    return l_doubleWord;
}

//-------------------------------------------------------------------------------------------------------

///
/// @brief Decode an unsigned integer from a 4-bit octal stop code.
/// @param[in]  i_rs4Str        The RS4 scan string
/// @param[in]  i_nibbleIndx    Index into i_rs4Str that has the stop-code
/// @param[out] o_numRotate     No.of rotates decoded from the stop-code.
/// @return The number of nibbles decoded.
///
uint64_t stop_decode( const uint8_t* i_rs4Str,
                      uint32_t i_nibbleIndx,
                      uint64_t& o_numRotate )
{
    uint64_t l_numNibblesParsed     =   0; // No.of nibbles that make up the stop-code
    uint64_t l_numNonZeroNibbles    =   0;
    const uint32_t* l_data          =   NULL;
    uint32_t l_cnt                  =   0;
    uint32_t l_offset               =   0;
    rs4_data_t l_rs4_data;
    uint8_t l_nibble;
    uint8_t l_byte;

    do
    {
        l_cnt = i_nibbleIndx >> 1;

        if ( !( i_nibbleIndx %  8 ) || ( l_numNibblesParsed == 0 ) )
        {
            l_offset    =   l_cnt  - ( l_cnt % 4 );
            l_data      =   reinterpret_cast< uint32_t* > ( const_cast<uint8_t*>( i_rs4Str ) + l_offset );
            l_rs4_data.iv_data = *l_data;
        }

        l_byte      =   l_rs4_data.fields.val[l_cnt % 4];
        l_nibble    =   (i_nibbleIndx % 2) ? (l_byte & 0x0f) : (l_byte >> 4);
        l_numNonZeroNibbles     =   (l_numNonZeroNibbles * 8) + (l_nibble & 0x07);
        i_nibbleIndx++;
        l_numNibblesParsed++;

    }
    while( ( l_nibble & 0x08 ) == 0 );

    o_numRotate     =   l_numNonZeroNibbles;

    return l_numNibblesParsed;
}

//-------------------------------------------------------------------------------------------------------

static const uint16_t scan_type_decoder_ring[4] = { 0xDCE0, 0x0820, 0x0440, 0x9000 };
constexpr uint16_t decodeScanType(const uint16_t i_scan_type_encoded)
{
    return (i_scan_type_encoded < 12) ?
           (0x8000 >> i_scan_type_encoded) :
           scan_type_decoder_ring[i_scan_type_encoded - 12];
}

uint64_t decodeScanRegionData(
    const ScanTarget& i_target,
    const uint32_t i_ringAddress,
    const RingMode i_ringMode)
{
    const uint32_t l_scan_region  = ( ( i_ringAddress & 0x0000FFF0 ) |
                                      ( ( i_ringAddress & 0x00F00000 ) >> 20 ) ) << 13;

    const uint32_t l_scan_type = decodeScanType(i_ringAddress & 0x0000000F);

    const uint32_t l_parallel = (i_ringMode & RING_MODE_PARALLEL_SCAN) ? ENABLE_PARALLEL_SCAN : 0;

    const uint32_t l_fastinit = (i_ringAddress & RING_ADDRESS_FASTINIT) ? ENABLE_FASTINIT : 0;

    return (uint64_t(l_scan_region | l_parallel | l_fastinit) << 32) | l_scan_type;
}

//---------------------------------------------------------------------------------------------

static inline ReturnCode shortRotate(const ScanTarget& i_target, uint32_t i_nbits)
{
    const uint32_t l_scomAddress = SCAN64CONTSCAN | i_nbits;
    buffer<uint64_t> l_dummy;
    return getScom(i_target, l_scomAddress, l_dummy);
}

static ReturnCode longRotate(const ScanTarget& i_target, uint32_t i_nbits)
{
    // Scom Data needs to have the no.of rotates in the bits 12-31
    FAPI_TRY(putScom(i_target, ROTATE_ADDRESS_REG, uint64_t(i_nbits) << 32));

    // Now wait for OPCG_DONE
    {
        // Some musings on timeout (for P10):
        //
        // The slowest expected scan rate is at bypass speeds:
        //   f=100 MHz, scan ratio = 1:1 => 100e6 bits per second => 10 ns per bit
        // The fastest expected scan rate is at ~4GHz core:
        //   f=4000 MHz, scan ratio = 4:1 => 1e9 bits per second => 1 ns per bit
        //
        // The OPCG won't be done earlier than the minimum time, and after, say,
        // 1.5x the maximum time we can safely assume that something went wrong.
        //
        // So here's the approach:
        //   1. Do the first poll without waiting - small rotates may be done immediately
        //   2. Wait for the minimum time, poll again
        //   3. Continue polling at a faster rate until the maximum time expires

        const uint32_t POLL_DELAY_NS = 1024;      // Using power of two to avoid division at l_attempts below
        const uint32_t POLL_DELAY_CYCLES = 10000;
        const uint32_t MIN_NS_PER_BIT = 1;
        const uint32_t MIN_CYCLES_PER_BIT = 16;   // Replicating Cronus value here
        const uint32_t MAX_NS_PER_BIT = 15;       // Includes 50% extra allowance

        // The minimum time until the OPCG can reasonably be finished
        const uint32_t l_initialDelay_ns = i_nbits * MIN_NS_PER_BIT;
        const uint32_t l_initialDelay_cycles = i_nbits * MIN_CYCLES_PER_BIT;

        // The difference between the maximum time and the minimum
        const uint32_t l_additionalDelay_ns = i_nbits * (MAX_NS_PER_BIT - MIN_NS_PER_BIT);

        // Add 1 to round up, 1 for the error check below, and 1 for the initial delay
        // TODO: Scale number of attempts down if in simulation
        uint32_t l_attempts = (l_additionalDelay_ns / POLL_DELAY_NS) + 3;
        bool l_first_attempt = true;

        while( l_attempts > 0 )
        {
            l_attempts--;

            buffer<uint64_t> l_opcgStatus;

            FAPI_TRY( getScom( i_target, CPLT_STAT0, l_opcgStatus ),
                      "Failure during OPCG Check" );

            if( l_opcgStatus.getBit( CPLT_STAT0_CC_CTRL_OPCG_DONE_DC ) )
            {
                FAPI_INF("OPCG_DONE set");
                break;
            }

            if (l_first_attempt)
            {
                l_first_attempt = false;
                FAPI_TRY( delay( l_initialDelay_ns, l_initialDelay_cycles) );
            }
            else
            {
                FAPI_TRY( delay( POLL_DELAY_NS, POLL_DELAY_CYCLES ) );
            }
        }

        if( 0 == l_attempts )
        {
            FAPI_ERR("Max attempts exceeded checking OPCG_DONE");
            FAPI_ASSERT(false,
                        PUTRING_OPCG_DONE_TIMEOUT()
                        .set_TARGET(i_target)
                        .set_ROTATE_COUNT(i_nbits),
                        "ROTATE operation failed  due to timeout");
        }
    }

fapi_try_exit:
    return current_err;
}

static ReturnCode scanRotate(const ScanTarget& i_target, uint64_t i_nbits)
{
    uint64_t l_rotateCount       =   i_nbits;

    while (l_rotateCount > SVS_SHORT_ROTATE_MAX)
    {
        const uint32_t l_nbits = std::min(uint64_t(SVS_LONG_ROTATE_MAX), l_rotateCount);
        FAPI_TRY(longRotate(i_target, l_nbits));
        l_rotateCount -= l_nbits;
    }

    if (l_rotateCount)
    {
        FAPI_TRY(shortRotate(i_target, l_rotateCount));
    }

fapi_try_exit:
    return current_err;
}

static inline ReturnCode scanWrite(const ScanTarget& i_target, uint64_t i_nbits, uint64_t i_scanData)
{
    const uint32_t l_scomAddress = SCAN64CONTSCAN | i_nbits;
    return putScom(i_target, l_scomAddress, i_scanData);
}

//---------------------------------------------------------------------------------------------

ReturnCode verifyHeader(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_COMPARE > & i_target_comp,
    uint64_t       i_scanRegion,
    const uint32_t i_bitsDecoded)
{
    buffer<uint64_t> l_scomData;
    buffer<uint64_t> l_readHeader;

    //Trying to give some more time for all the cores to be sync after
    //rotate/scan operation
    delay(1000, 100);   // TODO WHY

    FAPI_TRY(getScom(i_target_comp, SCAN64CONTSCAN, l_readHeader));

    FAPI_INF("Got header - %08x%08x", l_readHeader >> 32, l_readHeader & 0xFFFFFFFF);
    FAPI_ASSERT( l_readHeader == SCAN_HEADER_DATA,
                 PUTRING_CHECKWORD_DATA_MISMATCH()
                 .set_TARGET( i_target_comp )
                 .set_SCOM_DATA( l_readHeader )
                 .set_BITS_DECODED( i_bitsDecoded ),
                 "Read CHECKWORD (%08x%08x) data incorrect and total bit decoded 0x%08x",
                 l_readHeader >> 32, l_readHeader & 0xFFFFFFFF, i_bitsDecoded);

    if(( i_scanRegion  >> 32 ) & ENABLE_PARALLEL_SCAN )
    {
        Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_OR > l_target_or = i_target_comp;
        FAPI_TRY(getScom(l_target_or, CPLT_STAT0, l_scomData));
        FAPI_ASSERT( not l_scomData.getBit<CPLT_STAT0_CC_CTRL_PARALLEL_SCAN_COMPARE_ERR>(),
                     PARALLEL_SCAN_COMPARE_ERR()
                     .set_TARGET( i_target_comp )
                     .set_CPLT_STAT0( l_scomData ),
                     "Failed To Complete Parallel Scan" );
    }

fapi_try_exit:
    return current_err;
}

//----------------------------------------------------------------------------------------------------

ReturnCode setupClockController(
    const ScanTarget& i_target,
    const RingMode i_ringMode,
    const uint64_t i_scanRegion,
    struct restoreOpcgRegisters& o_OPCGData)
{
    bool l_use_setpulse =
        ((i_ringMode & RING_MODE_SET_PULSE_NSL) ||
         (i_ringMode & RING_MODE_SET_PULSE_SL) ||
         (i_ringMode & RING_MODE_SET_PULSE_ALL));

    // **************************
    // Setup Scan-Type and Region
    // **************************

    FAPI_TRY( putScom( i_target, SCAN_REGION_TYPE_ADDRESS, i_scanRegion ) );

    // ***************************
    // Setup Clock Region & OPCG regs
    // ***************************

    if ( l_use_setpulse )
    {
        //First 32 bits tells clock region
        FAPI_TRY( putScom( i_target, CLK_REGION_ADDRESS, (i_scanRegion & 0xFFFFFFFF00000000ULL ) ) );

        buffer<uint64_t> l_opcgAlign;
        uint64_t l_opcg_reg0 = 0x0ULL;
        uint64_t l_opcg_reg1 = 0x0ULL;
        uint64_t l_opcg_reg2 = 0x0ULL;
        // NSL for slow regions
        uint64_t l_opcg_capt1 = 0x0ULL;
        // NSL for fast regions
        uint64_t l_opcg_capt2 = 0x0ULL;
        uint64_t l_opcg_capt3 = 0x0ULL;

        // setup NSL mode
        if ( i_ringMode & RING_MODE_SET_PULSE_NSL )
        {
            // NSL for slow regions
            l_opcg_capt1 = 0x1480000000000000;
            // NSL for fast regions
            l_opcg_capt2 = 0x0480000000000000;
        }
        else if ( i_ringMode & RING_MODE_SET_PULSE_SL )
        {
            // NSL for slow regions
            l_opcg_capt1 = 0x1800000000000000;
            // NSL for fast regions
            l_opcg_capt2 = 0x0800000000000000;
        }
        else   //set pulse all
        {
            // NSL for slow regions
            l_opcg_capt1 = 0x1E80000000000000;
            // NSL for fast regions
            l_opcg_capt2 = 0x0E80000000000000;
        }

        // save register state
        FAPI_TRY( getScom( i_target, OPCG_ALIGN_ADDRESS, o_OPCGData.l_opcgAlign ) );
        FAPI_TRY( getScom( i_target, OPCG_REG0_ADDRESS,  o_OPCGData.l_opcgReg0  ) );
        FAPI_TRY( getScom( i_target, OPCG_REG1_ADDRESS,  o_OPCGData.l_opcgReg1  ) );
        FAPI_TRY( getScom( i_target, OPCG_REG2_ADDRESS,  o_OPCGData.l_opcgReg2  ) );
        FAPI_TRY( getScom( i_target, OPCG_CAPT1_ADDRESS, o_OPCGData.l_opcgCapt1 ) );
        FAPI_TRY( getScom( i_target, OPCG_CAPT2_ADDRESS, o_OPCGData.l_opcgCapt2 ) );
        FAPI_TRY( getScom( i_target, OPCG_CAPT3_ADDRESS, o_OPCGData.l_opcgCapt3 ) );

        // bits: 4:7   SNOP_ALIGN(0:3) 5: 8:1
        // bits: 20:31 SNOP_WAIT(0:11)
        //set SNOP Align=8:1 and SNOP Wait=7
        l_opcgAlign = o_OPCGData.l_opcgAlign;
        l_opcgAlign.insertFromRight<4, 4>(5);
        l_opcgAlign.insertFromRight<20, 12>(7);

        // apply setpulse setup
        FAPI_TRY( putScom( i_target, OPCG_ALIGN_ADDRESS, l_opcgAlign ) );
        FAPI_TRY( putScom( i_target, OPCG_REG0_ADDRESS,  l_opcg_reg0  ) );
        // Maintain SCAN_CLK_USE_EVEN bit 49
        l_opcg_reg1 = o_OPCGData.l_opcgReg1 & 0x0000000000004000ULL;
        FAPI_TRY( putScom( i_target, OPCG_REG1_ADDRESS,  l_opcg_reg1  ) );
        FAPI_TRY( putScom( i_target, OPCG_REG2_ADDRESS,  l_opcg_reg2  ) );
        FAPI_TRY( putScom( i_target, OPCG_CAPT1_ADDRESS, l_opcg_capt1 ) );
        FAPI_TRY( putScom( i_target, OPCG_CAPT2_ADDRESS, l_opcg_capt2 ) );
        FAPI_TRY( putScom( i_target, OPCG_CAPT3_ADDRESS, l_opcg_capt3 ) );

        if ( ( i_ringMode & RING_MODE_SET_PULSE_SL ) ||
             ( i_ringMode & RING_MODE_SET_PULSE_ALL ) )
        {
            // SET_PULSE_SL is applied before we start scanning
            FAPI_TRY( putScom( i_target, OPCG_REG0_ADDRESS, OPCG_GO_SINGLE_LOOP ) );
        }
    }

fapi_try_exit:
    return current_err;
}

//----------------------------------------------------------------------------------------------------

ReturnCode cleanupClockController(
    const ScanTarget& i_target,
    const RingMode i_ringMode,
    const uint64_t i_scanRegion,
    const struct restoreOpcgRegisters& i_OPCGData)
{
    bool l_use_setpulse =
        ((i_ringMode & RING_MODE_SET_PULSE_NSL) ||
         (i_ringMode & RING_MODE_SET_PULSE_SL) ||
         (i_ringMode & RING_MODE_SET_PULSE_ALL));

    if ( l_use_setpulse )
    {
        if ( i_ringMode & RING_MODE_SET_PULSE_NSL )
        {
            // SET_PULSE_NSL and SET_PULSE_ALL are applied after we're done scanning
            FAPI_TRY( putScom( i_target, OPCG_REG0_ADDRESS, OPCG_GO_SINGLE_LOOP ) );
        }

        FAPI_TRY( putScom( i_target, CLK_REGION_ADDRESS, 0x0ULL ) );
        // restore register state
        FAPI_TRY( putScom( i_target, OPCG_ALIGN_ADDRESS, i_OPCGData.l_opcgAlign ) );
        FAPI_TRY( putScom( i_target, OPCG_REG0_ADDRESS,  i_OPCGData.l_opcgReg0  ) );
        FAPI_TRY( putScom( i_target, OPCG_REG1_ADDRESS,  i_OPCGData.l_opcgReg1  ) );
        FAPI_TRY( putScom( i_target, OPCG_REG2_ADDRESS,  i_OPCGData.l_opcgReg2  ) );
        FAPI_TRY( putScom( i_target, OPCG_CAPT1_ADDRESS, i_OPCGData.l_opcgCapt1 ) );
        FAPI_TRY( putScom( i_target, OPCG_CAPT2_ADDRESS, i_OPCGData.l_opcgCapt2 ) );
        FAPI_TRY( putScom( i_target, OPCG_CAPT3_ADDRESS, i_OPCGData.l_opcgCapt3 ) );
    }

    FAPI_TRY( putScom( i_target, SCAN_REGION_TYPE_ADDRESS, 0x0ULL ) );

fapi_try_exit:
    return current_err;
}

//----------------------------------------------------------------------------------------------------

ReturnCode flushRTIM(
    const ScanTarget& i_target,
    const uint64_t i_scanRegion)
{
    struct restoreOpcgRegisters l_OPCGData;

    // alter type to unused (bndy)
    uint64_t l_scanRegion = (i_scanRegion & 0xFFFFFFFFFFFF0000ULL) | 0x0000000000000080ULL;

    // setup 64-bit writes to zero
    uint32_t l_scomAddress = SCAN64_ADDRESS | 0x40;
    uint64_t l_scomData = 0x0ULL;

    // Prep clock controller for ring scan with modified type
    FAPI_TRY(setupClockController(i_target, RING_MODE_HEADER_CHECK, l_scanRegion, l_OPCGData));

    // ensure RTIMs in chain are flushed with 0s, skip header check
    for (auto i = 0; i < 4; i++)
    {
        FAPI_TRY(putScom(i_target, l_scomAddress, l_scomData));
    }

    // Cleanup clock controller
    FAPI_TRY(cleanupClockController(i_target, RING_MODE_HEADER_CHECK, l_scanRegion, l_OPCGData));

fapi_try_exit:
    return current_err;
}

//----------------------------------------------------------------------------------------------------

static ReturnCode checkBounds(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
    const CompressedScanData* i_hdr,
    const uint64_t i_nibbleCount,
    const uint64_t i_nibbleIndx)
{
    FAPI_ASSERT(i_nibbleIndx < i_nibbleCount,
                PUTRING_RS4_STRING_OVERRUN()
                .set_TARGET(i_target)
                .set_SCAN_ADDR(i_hdr->iv_scanAddr.get())
                .set_NIBBLE_COUNT(i_nibbleCount)
                .set_NIBBLE_INDEX(i_nibbleIndx),
                "Putring ran beyond the end of the compressed RS4 string; likely corrupted RS4 data. "
                "nibbleCount=%d nibbleIndex=%d", i_nibbleCount, i_nibbleIndx);

fapi_try_exit:
    return current_err;
}

ReturnCode poz_putRingUtils(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
    const uint8_t* i_rs4,
    const uint32_t i_modifiedRingAddress,
    const RingMode i_ringMode)
{
    FAPI_INF(">> poz_putRingUtils");

    const ScanTarget& l_target          = i_target;
    const CompressedScanData* l_hdr     = (CompressedScanData*) i_rs4;
    const uint8_t* l_rs4Str             = i_rs4 + sizeof(CompressedScanData);
    const uint64_t l_nibbleCount        = 2 * (l_hdr->iv_size.get() - sizeof(CompressedScanData));
    uint64_t l_nibbleIndx               = 0;
    uint64_t l_bitsDecoded              = 0;
    uint64_t l_scomData                 = 0x0;
    bool l_skip_64bits                  = true;

    struct restoreOpcgRegisters l_OPCGData;

    const uint64_t l_scanRegion    = decodeScanRegionData(l_target, i_modifiedRingAddress, i_ringMode);
    const bool l_bOverride         = l_hdr->iv_type.get() & RS4_TYPE_OVERRIDE;

    // optionally flush RTIM staging
    if (i_ringMode & RING_MODE_PARALLEL_SCAN)
    {
        FAPI_TRY(flushRTIM(l_target, l_scanRegion));
    }

    // Prep clock controller for ring scan
    FAPI_TRY(setupClockController(l_target, i_ringMode, l_scanRegion, l_OPCGData));

    // Write a 64 bit value for header, once without shift to preload the scan
    // buffer on all chiplets, then again with shift to shift out the header
    FAPI_TRY(putScom(l_target, SCAN64CONTSCAN, SCAN_HEADER_DATA));
    FAPI_TRY(putScom(l_target, CHECK_WORD_REG_ADDRESS, SCAN_HEADER_DATA));

    // Decompress the RS4 string and scan
    do
    {
        {
            // Determine the no.of ROTATE operations encoded in stop-code
            uint64_t l_count    =   0;
            l_nibbleIndx       +=   stop_decode( l_rs4Str, l_nibbleIndx, l_count );

            // Determine the no.of rotates in bits
            uint64_t l_bitRotates   =   ( 4 * l_count );

            // Need to skip 64 bits because we have already written header data
            if ( l_skip_64bits && ( l_bitRotates >= 64 ) )
            {
                l_bitRotates    -=  64;
                l_skip_64bits    =  false;
            }

            l_bitsDecoded   +=  l_bitRotates;

            // Do the ROTATE operation
            if ( l_bitRotates != 0 )
            {
                FAPI_TRY(scanRotate(l_target, l_bitRotates));
            }
        }

        {
            uint8_t l_scanCount = rs4_get_nibble( l_rs4Str, l_nibbleIndx );
            l_nibbleIndx++;

            if ( 0 == l_scanCount )
            {
                FAPI_INF("SCAN COUNT %u", l_scanCount);
                break;
            }

            if (( !l_bOverride ) && ( l_scanCount != 0xF ) )  // Process flush ring (and ensure no '0'-writes)
            {
                l_bitsDecoded += (4 * l_scanCount);

                // Parse the non-zero nibbles of the RS4 string and
                // scan them into the ring
                l_scomData = rs4_get_verbatim( l_rs4Str,
                                               l_nibbleIndx,
                                               l_scanCount );
                l_nibbleIndx += l_scanCount;

                FAPI_TRY(scanWrite(l_target, l_scanCount * 4, l_scomData));
            }
            else    // Process override ring (plus the occasional flush ring with '0'-write bits)
            {
                if( 0xF == l_scanCount ) // Process ring with '0'-write bits
                {
                    uint8_t l_careMask = rs4_get_nibble( l_rs4Str, l_nibbleIndx );
                    l_nibbleIndx++;
                    uint8_t l_spyData = rs4_get_nibble( l_rs4Str, l_nibbleIndx );
                    l_nibbleIndx++;

                    for( uint8_t i = 0; i < 4; i++ )
                    {
                        l_bitsDecoded   +=  1;

                        if( ( l_careMask & ( 8 >> i ) ) )
                        {
                            const uint64_t l_scomData = (l_spyData & (8 >> i)) ? 0xFFFFFFFFFFFFFFFF : 0;
                            FAPI_TRY(scanWrite(l_target, 1, l_scomData));
                        }
                        else
                        {
                            FAPI_TRY(shortRotate(l_target, 1));
                        }
                    }
                }
                else    // Process ring with only '1'-write bits
                {
                    for ( uint8_t x = 0; x < l_scanCount; x++ )
                    {
                        // Parse the non-zero nibbles of the RS4 string and
                        // scan them into the ring
                        uint8_t l_data      =    rs4_get_nibble( l_rs4Str, l_nibbleIndx );
                        l_nibbleIndx       +=    1;

                        FAPI_INF ( "VERBATIm l_nibbleIndx %u l_scanCount %u "
                                   "l_bitsDecoded %u", l_nibbleIndx, l_scanCount, l_bitsDecoded );

                        for( uint8_t i = 0; i < 4; i++ )
                        {
                            l_scomData      =    0x0;

                            if( ( l_data & ( 8 >> i ) ) )
                            {
                                FAPI_TRY(scanWrite(l_target, 1, 0xFFFFFFFFFFFFFFFF));
                            }
                            else
                            {
                                FAPI_TRY(shortRotate(l_target, 1));
                            }
                        }

                    } // end of looper for bit-parsing a non-zero nibble
                }
            }
        }

        // Check that we didn't overrun the RS4 string
        FAPI_TRY(checkBounds(i_target, l_hdr, l_nibbleCount, l_nibbleIndx));
    }
    while(1);

    {
        // Handle the string termination
        uint8_t l_nibble    =   rs4_get_nibble( l_rs4Str, l_nibbleIndx );
        l_nibbleIndx++;

        if ( l_nibble != 0 )
        {
            if ( ( !l_bOverride ) && !( l_nibble & 0x8 ) ) // Process flush ring (and ensure no '0'-writes))
            {
                l_bitsDecoded += l_nibble;
                l_scomData = rs4_get_verbatim( l_rs4Str,
                                               l_nibbleIndx,
                                               1 ); // return 1 nibble

                FAPI_TRY(scanWrite(l_target, l_nibble & 0x3, l_scomData));
            }
            else // Process override ring (plus the occasional flush ring with '0'-write bits)
            {
                if( 0x8 & l_nibble ) // Process ring with '0'-write bits
                {
                    uint8_t l_careMask  =   rs4_get_nibble(l_rs4Str, l_nibbleIndx);
                    l_nibbleIndx++;
                    uint8_t l_spyData   =   rs4_get_nibble(l_rs4Str, l_nibbleIndx);
                    l_nibbleIndx++;

                    for( uint8_t i = 0; i < ( l_nibble & 0x3 ); i++ )
                    {
                        l_bitsDecoded += 1;

                        if( ( l_careMask & ( 8 >> i ) ) )
                        {
                            const uint64_t l_scomData = (l_spyData & (8 >> i)) ? 0xFFFFFFFFFFFFFFFF : 0;
                            FAPI_TRY(scanWrite(l_target, 1, l_scomData));
                        }
                        else
                        {
                            FAPI_TRY(shortRotate(l_target, 1));
                        }
                    }
                }
                else // Process ring with only '1'-write bits
                {
                    // scan them into the ring
                    uint8_t l_data      =   rs4_get_nibble( l_rs4Str, l_nibbleIndx );
                    l_nibbleIndx       +=   1;

                    for( uint8_t i = 0; i < l_nibble; i++ )
                    {
                        l_bitsDecoded   +=  1;

                        if(( l_data & ( 8 >> i ) ) )
                        {
                            FAPI_TRY(scanWrite(l_target, 1, 0xFFFFFFFFFFFFFFFF));
                        }
                        else
                        {
                            FAPI_TRY(shortRotate(l_target, 1));
                        }
                    } //end of for
                }
            }
        } // end of if(l_nibble != 0)
    }

    // Double check we didn't overrun the RS4 string
    FAPI_TRY(checkBounds(i_target, l_hdr, l_nibbleCount, l_nibbleIndx));

    // Verify header
    FAPI_TRY(verifyHeader(l_target, l_scanRegion, l_bitsDecoded));

    // Cleanup clock controller post scan
    FAPI_TRY(cleanupClockController(l_target, i_ringMode, l_scanRegion, l_OPCGData));

fapi_try_exit:
    FAPI_INF( "<< poz_putRingUtils" );
    return current_err;
}
