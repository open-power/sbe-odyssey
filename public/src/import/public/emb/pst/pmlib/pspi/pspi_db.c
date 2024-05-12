/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/pspi/pspi_db.c $       */
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
/// Receive 8bit Doorbell[x] without payload from certain target
///
/// \param target, TAPx or Hub, do not accept broadcast
/// \param db,     Doorbell 0-3
/// \param cmd,    bit0:7 of PSPI Doorbell Receive Register( PDBR )
///
void
pspi_doorbell_receive(uint32_t target,
                      uint32_t db,
                      uint32_t* cmd)
{
    *cmd = ( in32( PSPI_DB_REG( DB_RCV, db, target ) ) ) >> SHIFT32(7);
    PSPI_TRACE(">>PSPI_DB_RECV Target %x DB %x Cmd %x", target, db, (uint32_t)(*cmd));
}

///
/// Send 8bit Doorbell[x] without payload to certain target
///
/// \param target, TAPx or Hub, do not accept broadcast
/// \param db,     Doorbell 0-3
/// \param cmd,    bit0:7 of PSPI Doorbell Send Register( PDBS )
///
void
pspi_doorbell_send(uint32_t target,
                   uint32_t db,
                   uint32_t cmd)
{
    out32( PSPI_DB_REG( DB_SND, db, target ), ( cmd << SHIFT32(7) ) );
    PSPI_TRACE(">>PSPI_DB_SEND Target %x DB %x Cmd %x", target, db, cmd);
}

#if __OCC_PLAT

///
/// Send 8bit Doorbell[x] without payload to a set of broadcast targets given mask
///
/// \param broadcast_mask, mask of broadcast targets of TAP[0-7], bit8:15 of PDBS
/// \param db,             Doorbell 0-3
/// \param cmd,            bit0:7 of PSPI Doorbell Send Broadcast Register( PDBS )
///
void
pspi_doorbell_sendall(uint32_t broadcast_mask,
                      uint32_t db,
                      uint32_t cmd)
{
    if( !broadcast_mask )
    {
        PSPI_TRACE(">>PSPI_DB_SENDALL ERROR: broadcast_mask=0 not allowed");
//CMO_20230202: If we're going to call out an error, we need to return an rc as well. David?
        return;
    }

    out32( PSPI_DB_REG( DB_SND_BCST, db, PSPI_BROADCAST ),
           ( ( cmd << SHIFT32(7) ) | ( broadcast_mask << SHIFT32(15) ) ) );
    PSPI_TRACE(">>PSPI_DB_SENDALL BroadcastMask %x DB %x Cmd %x",
               broadcast_mask, db, cmd);
}

#endif

///
/// Receive 64b Message Doorbell[x] with payload from certain target
///
/// \param target, TAPx or Hub, do not accept broadcast
/// \param db,     Doorbell 0-3
/// \param msg,    bit0:63 of PSPI Message Doorbell Receive Register( PMDBR )
///
void
pspi_msg_doorbell_receive(uint32_t target,
                          uint32_t db,
                          uint64_t* msg)
{
    *msg = ( in64( PSPI_DB_REG( MSG_DB_RCV, db, target ) ) );
    PSPI_TRACE(">>PSPI_MSG_DB_RECV Target %x DB %x Msg %x %x",
               target, db, (uint32_t)(((uint64_t)(*msg)) >> 32),
               (uint32_t)(((uint64_t)(*msg)) & 0xffffffff));
}

///
/// Send 64b Message Doorbell[x] with payload to certain target
///
/// \param target, TAPx or Hub, do not accept broadcast
/// \param db,     Doorbell 0-3
/// \param msg,    msg_num and msg_data of PSPI Message Doorbell Send Register( PMDBS )
///
void
pspi_msg_doorbell_send(uint32_t target,
                       uint32_t db,
                       uint64_t msg)
{
    out64( PSPI_DB_REG( MSG_DB_SND, db, target), msg );
    PSPI_TRACE(">>PSPI_MSG_DB_SEND Target %x DB %x Msg %x %x",
               target, db, (uint32_t)(msg >> 32),
               (uint32_t)(msg & 0xffffffff));
}

#if __OCC_PLAT

///
/// Send 64b Message Doorbell[x] with payload to multicast target
///
/// \param broadcast_mask, mask of broadcast targets of TAP[0-7], bit8:15 of PMDBS
/// \param db,             Doorbell 0-3
/// \param msg,            msg_num and msg_data of PSPI Message Doorbell Send Broadcast Register( PMDBS )
///
void
pspi_msg_doorbell_sendall(uint32_t broadcast_mask,
                          uint32_t db,
                          uint64_t msg)
{
    if( !broadcast_mask )
    {
        PSPI_TRACE(">>PSPI_MSG_DB_SENDALL ERROR: broadcast_mask=0 not allowed");
//CMO_20230202: If we're going to call out an error, we need to return an rc as well. David?
        return;
    }

    out64( PSPI_DB_REG( MSG_DB_SND_BCST, db, PSPI_BROADCAST ),
           ( msg | ( ((uint64_t)broadcast_mask) << SHIFT64(15) ) ) );
    PSPI_TRACE(">>PSPI_MSG_DB_SENDALL BroadcastMask %x DB %x Msg %x %x",
               broadcast_mask, db, (uint32_t)(msg >> 32), (uint32_t)(msg & 0xffffffff));
}

#endif
