/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/pspi/pspi_vc.c $       */
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
/// Return Push/DB/MsgDB Status Summary regs (OCC Only)
///
/// \param type,   PUSH_RECV_SUMMARY, MSG_DB_SUMMARY, DB_SUMMARY refer to pspi_regs.h
/// \param ch,     Virtual or Doorbell Channel 0-3 (though VC0 is not allowed)
/// \param tap,    0-7:  Return [right-shifted] Status of specific Tap ID
///                >=8:  Return [right-shifted]  Status of Tap ID bits(0-7)
///
/// \retval  One, or all eight, of Status bits(0-7) of OPPRCSV/OPMDBSR/OPDBSR
///
uint32_t
pspi_summary(uint32_t type,
             uint32_t ch,
             uint32_t tap)
{
    //CMO-TODO: If ch outside valid range, we need to do fail in some way.
    {
        //TBD
    }

    uint32_t data = in32( PSPI_SUMMARY_REG(type, PSPI_REG_OP_RD, ch) );

    if( tap < 8 )
    {
        data = ( data >> SHIFT32(tap) ) & 0x1;
    }
    else
    {
        data = ( data >> SHIFT32(7) ) & 0xFF;
    }

    return data;
}


///
/// Clear the Tap status bits of Push/DB/MsgDB Status Summary regs (OCC only)
///
/// \param type,     PUSH_RECV_SUMMARY, MSG_DB_SUMMARY, DB_SUMMARY refer to pspi_regs.h
/// \param ch,       Virtual or Doorbell Channel 0-3 (though VC0 is not allowed)
/// \param tap_vec,  8-bit [right-shifted] Tap ID vector indicating which Tap Status bits to clear
///
/// \retval  None
///
void
pspi_summary_clear(uint32_t type,
                   uint32_t ch,
                   uint32_t tap_vec)
{
    //CMO-TODO: If ch outside valid range, we need to do fail in some way.
    {
        //TBD
    }

    out32( PSPI_SUMMARY_REG(type, PSPI_REG_OP_CLR, ch), tap_vec << 24);
}


///
/// Return Transmit or Receive Engine Status (OCC Only)
///
/// \param type,   0 for Transmit, 1 for Receive
/// \param vc_loc, shift amount of fields for each VC refer to pspi_regs.h
/// \param tap,    select bit for each tap using tap id,
///                0xFF: return all taps
///
/// \retval status corresponding status bit(s) of PVCTS/PVCRS
///
uint32_t
pspi_vc_status(uint32_t type,
               uint32_t vc_loc,
               uint32_t tap)
{
    uint32_t data = in32( (PVC_TRAN_STATUS + type) );
    data = (data >> vc_loc) & 0xFF;

    if ( tap < 8 )
    {
        data = ( data >> (7 - tap) ) & 0x1;
    }

    return data;
}

///
/// Return VC0 Error Status
///
/// \param target TAPx or Hub, do not accept broadcast
///
/// \retval error status of PERRV0
///
uint32_t
pspi_vc0_error(uint32_t target)
{
    uint32_t data = in32( PSPI_LINK_REG(PERR_VC0, target) );
    return data;
}

///
/// Configure VC0 functions
///
/// \param target TAPx or Hub, do not accept broadcast
/// \param config settings of PCFGV0
///
void
pspi_vc0_config(uint32_t target,
                uint32_t config)
{
    uint64_t data = config;
    data = data << 32;
    out64( PSPI_LINK_REG(PCFG_VC0, target), data );
    PSPI_TRACE(">>PSPI_VC0_CONFIG %x", config);
}

//
// Methods of buffer setup
//
// The given buffer in sram can be either empty(user prepare) or
// pre-fill with data to transmit(user prepare)
//
// These methods only config the push/pull engine to operate on the buffer,
// as either use other functions below to insert data to the empty buffer then transmit
// or configure the buffer that already filled with data, then simply transmit
//

///
/// Initialize buffer with default setting and sram location user provides
///
/// \param buffer,  pre-defined buffer struct
/// \param target,  TAPx or Hub, do not accept broadcast
/// \param vc,      Virtual Channel 1-3
/// \param op,      pre-defined encoding to use push/pull/send/recv
/// \param base,    base address of the buffer
/// \param size,    size of the buffer in sram
/// \param pp_mode, ping pong mode enablement
///
void
pspi_vc_buffer_init(pspi_buffer_t* buffer,
                    uint32_t target,
                    uint32_t vc,
                    uint32_t op,
                    uint32_t base,
                    uint32_t size,
                    uint32_t pp_mode)
{
    buffer->config.words.upper = VC_BUFFER_CONFIG_DEFAULT;
    buffer->config.fields.ping_pong_mode = pp_mode;
    buffer->config.fields.buffer_bar     = base;
    buffer->config.fields.buffer_size    = size;
    out64( PSPI_VC_REG( op, vc, target ), buffer->config.value );
    PSPI_TRACE(">>PSPI_VC_BUFFER_INIT Base %x Size %x PPMode %x", base, size, pp_mode);
}

///
/// Setup buffer given user provided config
///
/// \param buffer, pre-defined buffer struct
/// \param target, TAPx or Hub, do not accept broadcast
/// \param vc,     Virtual Channel 1-3
/// \param op,     pre-defined encoding to use push/pull/send/recv
///
void
pspi_vc_buffer_config(pspi_buffer_t* buffer,
                      uint32_t target,
                      uint32_t vc,
                      uint32_t op)
{
    out64( PSPI_VC_REG( op, vc, target ), buffer->config.value );
    PSPI_TRACE(">>PSPI_VC_BUFFER_CONFIG target %x vc %x op %x", target, vc, op);
    PSPI_TRACE(">>PSPI_VC_BUFFER_CONFIG data %x %x",
               (uint32_t)(buffer->config.value >> 32),
               (uint32_t)(buffer->config.value & 0xFFFFFFFF));
}

// Manual insert methods of using pull/push engine

///
/// Setup manual pointer and lock to the buffer
///
/// \param buffer, pre-defined buffer struct
///
/// \retval return 1: success locked, 0: buffer lock acquire failed
///
uint32_t
pspi_vc_buffer_write_setup(pspi_buffer_t* buffer)
{
    PSPI_TRACE(">>PSPI_VC_BUFFER_WRITE_SETUP wlock %x rlock %x wptr %x current %x",
               buffer->write_lock[buffer->current],
               buffer->read_lock[buffer->current],
               buffer->write_ptr[buffer->current],
               buffer->current);

    if( buffer->write_lock[buffer->current] == 0 &&
        buffer->read_lock[buffer->current]  == 0 )
    {
        PSPI_TRACE(">>PSPI_VC_BUFFER_WRITE_SETUP Base %x Size %x PPMode %x",
                   buffer->config.fields.buffer_bar,
                   buffer->config.fields.buffer_size,
                   buffer->config.fields.ping_pong_mode);
        buffer->write_lock[buffer->current] = 1;
        buffer->write_ptr[buffer->current] = buffer->config.fields.ping_pong_mode ?
                                             ( buffer->config.fields.buffer_bar + (
                                                     (buffer->config.fields.buffer_size / 2) * buffer->current ) ) :
                                             (buffer->config.fields.buffer_bar);
        return 1;
    }
    else
    {
        return 0;
    }
}

///
/// Write 32bit data to buffer
///
/// \param buffer, pre-defined buffer struct
/// \param data,   32bits data to write to the buffer
///
void
pspi_vc_buffer_write_fill(pspi_buffer_t* buffer,
                          uint32_t data)
{
    PSPI_TRACE(">>PSPI_VC_BUFFER_WRITE_FILL data %x wptr %x current %x",
               data,
               buffer->write_ptr[buffer->current],
               buffer->current);
    uint32_t* sram_data = (uint32_t*)(buffer->write_ptr[buffer->current]);
    *sram_data = data;
    buffer->write_ptr[buffer->current] += 4;
}

///
/// Mark and unlock buffer when write is done
///
/// \param buffer, pre-defined buffer struct
///
void
pspi_vc_buffer_write_done(pspi_buffer_t* buffer)
{
    PSPI_TRACE(">>PSPI_VC_BUFFER_WRITE_DONE new_data %x wlock %x wptr %x current %x",
               buffer->new_data[buffer->current],
               buffer->write_lock[buffer->current],
               buffer->write_ptr[buffer->current],
               buffer->current);
    buffer->write_lock[buffer->current] = 0;
    buffer->new_data[buffer->current] = 1;
}

// Auto insert methods of using push/pull engine
// pspi_vc_get(PSPI_TAP2, PSPI_VC1, PSPI_PULL|PSPI_RECV|PSPI_START, data)

///
/// Send Data to push/pull engine
///
/// \param target, TAPx or Hub, do not accept broadcast
/// \param vc,     Virtual Channel 1-3
/// \param op,     pre-defined encoding to use [push/pull],[send/recv],[start/next/last]
///                refer to pspi_regs.h
/// \param data,   data to be sent to the pspi link
///
void
pspi_vc_put(uint32_t target,
            uint32_t vc,
            uint32_t op,
            uint64_t data)
{
    PSPI_TRACE(">>PSPI_VC_PUT target %x vc %x op %x", target, vc, op);
    PSPI_TRACE(">>PSPI_VC_PUT data %x %x",
               (uint32_t)(data >> 32),
               (uint32_t)(data & 0xFFFFFFFF));
    out64( PSPI_VC_REG( op, vc, target ), data );
}

///
/// Receive Data from push/pull engine
///
/// \param target, TAPx or Hub, do not accept broadcast
/// \param vc,     Virtual Channel 1-3
/// \param op,     pre-defined encoding to use [push/pull],[send/recv],[start/next/last]
///                refer to pspi_regs.h
/// \param data,   data to be received from the pspi link
///
void
pspi_vc_get(uint32_t target,
            uint32_t vc,
            uint32_t op,
            uint64_t* data)
{
    *data = in64( PSPI_VC_REG( op, vc, target ) );
    PSPI_TRACE(">>PSPI_VC_GET target %x vc %x op %x", target, vc, op);
    PSPI_TRACE(">>PSPI_VC_GET data %x %x",
               (uint32_t)(*data >> 32),
               (uint32_t)(*data & 0xFFFFFFFF));
}

// Methods on PSPI transmition

///
/// Start transmition of unicast push/pull
///
/// \param target, TAPx or Hub, do not accept broadcast
/// \param vc,     Virtual Channel 1-3
/// \param op,     pre-defined encoding to use [push/pull],[send/recv],[start/next/last]
///                refer to pspi_regs.h
///
void
pspi_vc_unicast_go(uint32_t target,
                   uint32_t vc,
                   uint32_t op)
{
    out32( PSPI_VC_REG(op, vc, target), BIT32(0) );
    PSPI_TRACE(">>PSPI_VC_UNICAST_GO Target %x VC %x OP %x", target, vc, op);
}

///
/// Start transmition of broadcast push/pull
///
/// \param broadcast_mask, mask of broadcast targets of TAPx, bit8:15 of PSSOB/PLROB
/// \param vc,             Virtual Channel 1-3
/// \param op,             pre-defined encoding to use [push/pull],[send/recv],[start/next/last]
///                        refer to pspi_regs.h
///
void
pspi_vc_broadcast_go(uint32_t broadcast_mask,
                     uint32_t vc,
                     uint32_t op)
{
    out32( PSPI_VC_BROADCAST_REG(op, vc),
           ( PSPI_OP_START | ( broadcast_mask << SHIFT32(15) ) ) );
    PSPI_TRACE(">>PSPI_VC_BROADCAST_GO BroadcastMask %x VC %x OP %x", broadcast_mask, vc, op);
}

///
/// check status of push/pull transmition
///
/// \param target, TAPx or Hub, do not accept broadcast
/// \param vc,     Virtual Channel 1-3
/// \param op,     pre-defined encoding to use [push/pull],[send/recv],[start/next/last]
///                refer to pspi_regs.h
///
uint32_t
pspi_vc_in_progress(uint32_t target,
                    uint32_t vc,
                    uint32_t op)
{
    uint32_t data = in32( PSPI_VC_REG(op, vc, target) );
    PSPI_TRACE(">>PSPI_VC_IN_PROGRESS Target %x VC %x OP %x Data %x", target, vc, op, data);
    return (data >> SHIFT32(1));
}
