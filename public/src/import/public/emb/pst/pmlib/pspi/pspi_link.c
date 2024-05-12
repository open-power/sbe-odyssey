/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/pspi/pspi_link.c $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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

#include "pspi.h"

///
/// Setup and configure PSPI link. Initialize hardware modes.
///
/// \param target, TAPx or Hub, do not accept broadcast
/// \param config, bit0:31 of PSPI Link Configuration Register( PLC )
///
void
pspi_link_config(uint32_t target,
                 uint32_t config)
{
    out32( PSPI_LINK_REG(LINK_CONFIG, target), config);
    PSPI_TRACE(">>PSPI_LINK_CONFIG Target %x Config %x", target, config);
}

///
/// Return PSPI link Common Status
///
/// \param target,  TAPx or Hub, do not accept broadcast
///
/// \retval status, bit0:31 of PSPI Link Status Register ( PLS )
///
uint32_t
pspi_link_status_common(uint32_t target)
{
    uint32_t data = in32( PSPI_LINK_REG(LINK_STATUS, target) );
    return data;
}

///
/// Return PSPI link FSM Status
///
/// \param target,  TAPx or Hub, do not accept broadcast
/// \param fsm_loc, shift amount of certain fsm field, refer in pspi_regs.h
///
/// \retval status, bit32:63 of PSPI Link Status Register ( PLS )
///
uint32_t
pspi_link_status_fsm(uint32_t target,
                     uint32_t fsm_loc)
{
    uint32_t data = in32_sh( PSPI_LINK_REG(LINK_STATUS, target) );
    data = (data >> fsm_loc) & 0xF;
    return data;
}

///
/// Perform PSPI link Error Injection
///
/// \param target,  TAPx or Hub, do not accept broadcast
/// \param inject, bit0:3: inject config of PSPI Link ECC Register ( PLE )
/// \param vc,     bit4:7: vc select of PSPI Link ECC Register ( PLE )
///
void
pspi_link_err_inj(uint32_t target,
                  uint32_t inject,
                  uint32_t vc)
{
    uint32_t data = ( inject << SHIFT32(3) ) |
                    ( vc << SHIFT32(7) );
    out32( PSPI_LINK_REG(LINK_ECC, target), data );
}

///
/// Send ping through PSPI link with command combination
///
/// \param target,  TAPx or Hub, do not accept broadcast
/// \param command, bit0:31 of PSPI Link Ping Register( PLP )
///
void
pspi_link_ping(uint32_t target,
               uint32_t command)
{
    out32( PSPI_LINK_REG(LINK_PING, target), command);
    PSPI_TRACE(">>PSPI_LINK_PING Target %x Command %x", target, command);
}

///
/// Write the Link ping header and data
///
/// \param target,  TAPx or Hub, do not accept broadcast
/// \param header,  bit0:7 of PSPI Link Ping Write Header Register( PLPWH )
/// \param data,    bit0:63 of PSPI Link Ping Write Data Register( PLPWD )
///
void
pspi_link_ping_write(uint32_t target,
                     uint32_t header,
                     uint64_t data)
{
    out32( PSPI_LINK_REG(LINK_PING_WRITE_HEADER, target), header );
    out64( PSPI_LINK_REG(LINK_PING_WRITE_DATA,   target), data );
    PSPI_TRACE(">>PSPI_LINK_PING_WRITE Target %x Header %x Data %x %x",
               target, header, (uint32_t)(data >> 32),
               (uint32_t)(data & 0xffffffff));

}

///
/// Write the Link ping header and data
///
/// \param target,  TAPx or Hub, do not accept broadcast
/// \param data,    bit0:63 of PSPI Link Ping Read Data Register( PLPRD )
///
/// \retval header,  bit0:7 of PSPI Link Ping Read Header Register( PLPRH )
///
uint32_t
pspi_link_ping_read(uint32_t target,
                    uint64_t* data)
{
    uint32_t header;
    *data  = in64( PSPI_LINK_REG(LINK_PING_READ_DATA,   target) );
    header = in32( PSPI_LINK_REG(LINK_PING_READ_HEADER, target) );
    PSPI_TRACE(">>PSPI_LINK_PING_READ Target %x Header %x Data %x %x",
               target, header, (uint32_t)(((uint64_t)(*data)) >> 32),
               (uint32_t)(((uint64_t)(*data)) & 0xffffffff));
    return header;
}


///
/// Function: pspi_link_init_verif()
///
/// Description: If links are disabled, return error and vector of pre-disabled
///              links.
///
/// Args:
///   i_chip_type;         // Hub or tap. (See PSPI_CHIP_TYPE in pspi.h.)
///   i_enabled_taps;      // If called from hub: TCSR reg content(*). No-care if tap.
///   o_predisabled_links: // Output vector of the pre-disabled links.
///
/// *) This must be the TCSR content of the spinal chip, ie the configured/enabled taps.
///    Do not supply a local variable representing "CEs enabled".
///
/// Returns:
///   rc;            // Return code (See PSPI_RETURN_CODES in pspi.h.)
///
int
pspi_link_init_verif(uint32_t i_chip_type, uint32_t i_enabled_taps, uint32_t* o_predisabled_links)
{
    uint8_t  tap = 0;
    uint8_t  tap_tgt = 0;
    bool     b_link_enabled = false;

    *o_predisabled_links = 0x00000000;

    if (i_chip_type == PSPI_CT_HUB)
    {
        for (tap = 0; tap < 8; tap++)
        {
            if ((0x80000000 >> tap) & i_enabled_taps)
            {
                tap_tgt = PSPI_TAP0 + tap;

                b_link_enabled = (in32(PSPI_LINK_REG(LINK_CONFIG, tap_tgt)) & PSPI_LINK_ENABLE) ?
                                 true :
                                 false;

                if ( !b_link_enabled )
                {
                    // Log predisabled link. Go to next tap..
                    *o_predisabled_links |= 0x80000000 >> tap;
                }
            }
        }
    }
    else if (i_chip_type == PSPI_CT_TAP)
    {
        b_link_enabled = (in32(PSPI_LINK_REG(LINK_CONFIG, PSPI_HUB)) & PSPI_LINK_ENABLE) ?
                         true :
                         false;

        if ( !b_link_enabled )
        {
            // Log predisabled link.
            *o_predisabled_links = 0x80000000; //Can only be the hub.
        }
    }
    else
    {
        return PSPI_RC_UNSUPPORTED_CHIP_TYPE;
    }

    if ( *o_predisabled_links )
    {
        return PSPI_RC_LINK_DISABLED;
    }
    else
    {
        return PSPI_RC_SUCCESS;
    }
}


///
/// Function: pspi_link_init_enable()
///
/// Description: If links are disabled, enable them with a default configuration and
///              return those links that were pre-disabled.
///
/// Args:
///   i_chip_type;         // Hub or tap. (See PSPI_CHIP_TYPE in pspi.h.)
///   i_enabled_taps;      // If called from hub: TCSR reg content(*). No-care if tap.
///   o_predisabled_links: // Output vector of the pre-disabled links.
///
/// *) This must be the TCSR content of the spinal chip, ie the configured/enabled taps.
///    Do not supply a local variable representing "CEs enabled".
///
/// Returns:
///   rc;                  // Return code. (See PSPI_RETURN_CODES in pspi.h.)
///
int
pspi_link_init_enable(uint32_t i_chip_type, uint32_t i_enabled_taps, uint32_t* o_predisabled_links)
{
    uint8_t  tap = 0;
    uint8_t  tap_tgt = 0;
    bool     b_link_enabled = false;

    *o_predisabled_links = 0x00000000;

    if (i_chip_type == PSPI_CT_HUB)
    {
        for (tap = 0; tap < 8; tap++)
        {
            if ((0x80000000 >> tap) & i_enabled_taps)
            {
                tap_tgt = PSPI_TAP0 + tap;

                b_link_enabled = (in32(PSPI_LINK_REG(LINK_CONFIG, tap_tgt)) & PSPI_LINK_ENABLE) ?
                                 true :
                                 false;

                if ( !b_link_enabled )
                {
                    //
                    // Log predisabled link. Enable and configure PSPI link. Go to next tap..
                    //
                    *o_predisabled_links |= 0x80000000 >> tap;

                    pspi_link_config( tap_tgt,
                                      PSPI_LINK_ENABLE        |   //BIT32(0)
                                      PSPI_CLK_DIV_DEFAULT    |   //BIT32(15)
                                      PSPI_LINK_NOT_WRAP_MODE |   //bit(16) *not* set
                                      PSPI_LINK_PKT_CAPTURE   |   //BIT32(18) // OCC Only
                                      PSPI_LINK_ECC_GEN_EN    |   //BIT32(20)
                                      PSPI_LINK_ECC_CHECK_EN  |   //BIT32(21)
                                      PSPI_LINK_RX_FEM_FREEZE );  //BIT32(22)
                }
            }
        }
    }
    else if (i_chip_type == PSPI_CT_TAP)
    {
        b_link_enabled = (in32(PSPI_LINK_REG(LINK_CONFIG, PSPI_HUB)) & PSPI_LINK_ENABLE) ?
                         true :
                         false;

        if ( !b_link_enabled )
        {
            //
            // Enable and configure PSPI link.
            //
            *o_predisabled_links = 0x80000000; //Can only be the hub.

            pspi_link_config( PSPI_HUB,
                              PSPI_LINK_ENABLE        |   //BIT32(0)
                              PSPI_CLK_DIV_DEFAULT    |   //BIT32(15)
                              PSPI_LINK_NOT_WRAP_MODE |   //bit(16) *not* set
                              PSPI_LINK_PKT_CAPTURE   |   //BIT32(18) // OCC Only
                              PSPI_LINK_ECC_GEN_EN    |   //BIT32(20)
                              PSPI_LINK_ECC_CHECK_EN  |   //BIT32(21)
                              PSPI_LINK_RX_FEM_FREEZE );  //BIT32(22)
        }
    }
    else
    {
        return PSPI_RC_UNSUPPORTED_CHIP_TYPE;
    }

    return PSPI_RC_SUCCESS;
}


#ifndef __SSX__  //Only to be used by PPEs
///
/// Function: pspi_link_init()
///
/// Description: Depending on build environment flag, BOOT_STANDALONE, this API
///              either enables PSPI links or fails if said PSPI links are
///              pre-disabled (ie, already disabled at time of checking their
///              status).
///
/// Args:
///   i_chip_type;         // Hub or tap. (See PSPI_CHIP_TYPE in pspi.h.)
///   i_enabled_taps;      // If called from hub: TCSR reg content(*). No-care if called from tap.
///
/// *) This must be the TCSR content of the spinal chip, ie the configured/enabled taps.
///    Do not supply a local variable representing "CEs enabled".
///
/// Returns:
///   None
///
void
pspi_link_init(uint32_t i_chip_type, uint32_t i_enabled_taps)
{
    int      rc = PSPI_RC_SUCCESS;
    uint32_t predisabled_links = 0x00000000; //bit(0) or bits(0:7) represent hub or taps, respectively.

#ifdef BOOT_STANDALONE //Lab mode - Here it is ok if links are pre-disabled.

    //
    // If PSPI link is disabled, enable it.
    //
    rc = pspi_link_init_enable(i_chip_type, i_enabled_taps, &predisabled_links);

    switch (rc)
    {
        case PSPI_RC_SUCCESS:

            if (predisabled_links)
            {
                PSPI_TRACE("These PSPI links were pre-disabled and have been enabled");
            }
            else
            {
                PSPI_TRACE("The PSPI links were pre-enabled. All is good.");
            }

            break;

        case PSPI_RC_UNSUPPORTED_CHIP_TYPE:

            PSPI_TRACE_ERR("chip_type=%d is an unsupported chip type. Fix code.", i_chip_type);

            APPCFG_PANIC(PMHW_PSPI_UNSUPPORTED_CHIP_TYPE);

        default: // Any other RC will end up here and indicates code bug.

            PSPI_TRACE_ERR("rc=%d is not expected here. Fix code.", rc);

            APPCFG_PANIC(PMHW_CODE_BUG);
    }

#else //HWP (product) mode - Here's it is *NOT* ok if links are pre-disabled.

    //
    // Verify the PSPI link is enabled. If not, fail.
    //
    rc = pspi_link_init_verif(i_chip_type, i_enabled_taps, &predisabled_links);

    switch (rc)
    {
        case PSPI_RC_SUCCESS:

            PSPI_TRACE("The PSPI links were pre-enabled. All is good.");

            break;

        case PSPI_RC_LINK_DISABLED:

            PSPI_TRACE_ERR("Pre-disabled PSPI links are not allowed in HWP (product) mode.");
            PSPI_TRACE_ERR("Check PSPI initializing HWP.");

            APPCFG_PANIC(PMHW_PSPI_LINK_DISABLED);

        case PSPI_RC_UNSUPPORTED_CHIP_TYPE:

            PSPI_TRACE_ERR("chip_type=%d is an unsupported chip type. Fix code.", i_chip_type);

            APPCFG_PANIC(PMHW_PSPI_UNSUPPORTED_CHIP_TYPE);

        default: // Any other RC will end up here and indicates code bug.

            PSPI_TRACE_ERR("rc=%d is not expected here. Fix code.", rc);

            APPCFG_PANIC(PMHW_CODE_BUG);
    }

#endif

}
#endif
