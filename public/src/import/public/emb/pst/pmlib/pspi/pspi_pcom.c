/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/pspi/pspi_pcom.c $     */
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
/// GetPcom from certain target given address
///
/// \param target,  TAPx or Hub, do not accept broadcast
/// \param address, bit0:31 of PSPI PCOM Read Address Register( PPRA )
/// \param data,    data to read from PSPI PCOM Data Register( PPD )
///
void
pspi_pcom_get(uint32_t target,
              uint32_t address,
              uint64_t* data)
{
    // Write Address triggers the transfer
    out32( PSPI_PCOM_REG( PCOM_RADDR, target ), address );
    *data = in64( PSPI_PCOM_REG( PCOM_DATA, target ) );
    //PSPI_TRACE(">>PSPI_PCOM_GET Target %x Address 0x%08x Data 0x%08x %08x",
    //           target, address, (uint32_t)(((uint64_t)(*data)) >> 32),
    //           (uint32_t)(((uint64_t)(*data)) & 0xffffffff));
}

///
/// PutPcom to certain target given address and data
///
/// \param target,  TAPx or Hub, do not accept broadcast
/// \param address, bit0:31 of PSPI PCOM Write Address Register( PPWA )
/// \param data,    data to write to PSPI PCOM Data Register( PPD )
///
void
pspi_pcom_put(uint32_t target,
              uint32_t address,
              uint64_t data)
{
    out64( PSPI_PCOM_REG( PCOM_DATA,  target ), data );
    // Write Address triggers the transfer
    out32( PSPI_PCOM_REG( PCOM_WADDR, target ), address );
    //commenting for now. It should be made a debug trace
    //    PSPI_TRACE(">>PSPI_PCOM_PUT Target %x Address %x Data %x %x",
    //               target, address, (uint32_t)(data >> 32),
    //               (uint32_t)(data & 0xffffffff));
}

#if __OCC_PLAT

///
/// GetPcom from all targets given address
///
/// \param address, bit0:31 of PSPI PCOM Read Broadcast Address Register( PPRA )
/// \param data,    data array to read from each PSPI PCOM Data Register( PPD )
///
void
pspi_pcom_getall(uint32_t address, uint64_t* data)
{
    uint32_t i = 0;
    PSPI_TRACE(">>PSPI_PCOM_GETALL Address %x Data Pointter %x", address, (uint32_t)data);
    out32( PSPI_PCOM_REG( PCOM_RADDR_BCST, PSPI_BROADCAST ), address );

    for( i = 0; i < PSPI_MAX_TAPS; i++ )
    {
        data[i] = in64( PSPI_PCOM_REG( PCOM_DATA, i ) );
    }
}

///
/// PuPcom to all targets given address and data
///
/// \param address, bit0:31 of PSPI PCOM Write Broadcast Address Register( PPWA )
/// \param data,    data to write to PSPI PCOM Data Broadcast Register( PPD )
///
void
pspi_pcom_putall(uint32_t address, uint64_t data)
{
    out64( PSPI_PCOM_REG( PCOM_DATA_BCST,  PSPI_BROADCAST ), data );
    // Write Address triggers the transfer
    out32( PSPI_PCOM_REG( PCOM_WADDR_BCST, PSPI_BROADCAST ), address );
    PSPI_TRACE(">>PSPI_PCOM_PUTALL Address %x Data %x %x",
               address, (uint32_t)(data >> 32), (uint32_t)(data & 0xffffffff));
}

#endif
