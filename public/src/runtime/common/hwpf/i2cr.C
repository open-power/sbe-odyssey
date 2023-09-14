/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/hwpf/i2cr.C $                       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
/**
 *  @file src/usr/i2cr/i2cr.C
 *
 *  @brief Implementation of the I2CR device driver for Odyssey (DDR5)
 *         access via SCOM and CFAM(FSI) for Read & Write operations.
 */

/*****************************************************************************/
// I n c l u d e s
/*****************************************************************************/
#include "i2cr.H"
#include <i2c_access.H>
#include <fapi2.H>
#include <ppe42_string.h>


//Globals/Constants


//#define TRACUCOMP(args...)


namespace I2CR
{

    void dump_io_buffer(const void *io_buffer, size_t io_buflen){
      uint32_t *ptr_32=(uint32_t*)io_buffer;
      uint8_t *ptr_8=(uint8_t*)io_buffer;
      if(io_buflen>=16){
        FAPI_DBG("iobuffer[]=%08X %08X %08X %08X", ptr_32[0], ptr_32[1], ptr_32[2], ptr_32[3]);
        ptr_8+=16;
        ptr_32+=4;
        io_buflen-=16;
      }
      if(io_buflen>=12){
          FAPI_DBG("iobuffer[]=%08X %08X %08X", ptr_32[0], ptr_32[1], ptr_32[2]);
          ptr_8+=12;
          ptr_32+=3;
          io_buflen-=12;
      }
      if(io_buflen>=8){
          FAPI_DBG("iobuffer[]=%08X %08X", ptr_32[0], ptr_32[1]);
          ptr_8+=8;
          ptr_32+=2;
          io_buflen-=8;
      }
      if(io_buflen>=4){
          FAPI_DBG("iobuffer[]=%08X", ptr_32[0]);
          ptr_8+=4;
          ptr_32+=1;
          io_buflen-=4;
      }
    }



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 *
 * @brief A utility function to swap bytes of specified size
 * @note  The caller is responsible for the input and output buffers
 *        are of valid i_sz size and also that they are different.
 *        This routine, with the current implementation will not support
 *        both the input and output buffers to be the same.
 *
 * @param [in]  i_val : input value
 * @param [out] o_val : buffer to return swapped value
 * @param [in]  i_sz  : input size
 *
 */
void swapBytes(void *i_val, void *o_val, size_t i_sz)
{
    uint8_t *l_iptr =  reinterpret_cast<uint8_t*>(i_val);
    uint8_t *l_optr =  reinterpret_cast<uint8_t*>(o_val);

    for (size_t i=0; i<i_sz; i++)
    {
            l_optr[i] = l_iptr[i_sz-1-i];
    }
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 *
 * @brief An inline function to convert the FSI/CFAM address to the i2CR
 *        format.
 * @note  Please refer to I2CR FW doc
 * @param [in]  i_addr : input FSI/CFAM address
 * @return l_addr : output i2cr address
 *
 */
inline uint32_t convertFsiCfamAddrToI2crAddr(const uint32_t i_addr)
{
    // Convert the FSI CFAM WORD address to the I2CR SCOM address used by OCMBs.
    uint32_t l_addr = (((i_addr & 0xFFFFFC00) >> 2) | (i_addr & 0x000001FF));
    FAPI_DBG("convertFsiCfamAddrToI2CrAddr: 0x%x->0x%x", i_addr, l_addr);
    return l_addr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 *
 * @brief A utility routine to check the number of 1s in the input data
 * @note  This routine only supports 32bit or 64bit inputs
 *
 * @param [in]  i_data : input data
 * @param [in]  i_sz : input data size
 * @return value : 1(true) if number of 1s is odd and 0(false) if even
 *
 */
bool checkOddParity(void *i_data, size_t i_sz)
{
    uint32_t l_ones_count = 0;

    if (i_sz == sizeof(uint32_t))
    {
        l_ones_count = __builtin_popcount(*(reinterpret_cast<uint32_t *>(i_data)));
    }
    else
    {
        l_ones_count = __builtin_popcountll(*(reinterpret_cast<uint64_t *>(i_data)));
    }

    FAPI_DBG("checkOddParity: l_ones_count=%d", l_ones_count);

    /* if l_ones_count is odd, least significant bit will be 1 */
    return (l_ones_count & 1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 *
 * @brief A utility routine to add the odd parity bit to the specified address.
 *        Only the bits 1 to 31 of Scom/Cfam address are used to compute the
 *        the I2CR address to access the OCMBs. These bits are left shifted
 *        by 1 bit to get the 0 to 30 bits of the I2CR address. The 31st
 *        bit of the I2CR address is the odd parity bit.
 *
 *        The way the parity bit value is computed depends of whether we are
 *        doing a read or a write operation. For a read operation, only the addr
 *        is considered. For a write operation, the parity bit is computed
 *        taking both the address and the data value that needs to be written
 *        out into account.
 *
 *        This routine assumes that the specified address is already left
 *        shifted by 1 position. The second input, i_writeDataIsOdd, says if
 *        the "data to be written" has an odd number of 1s. This should have
 *        already been checked by the caller using checkOddParity and that
 *        result is passed in this parameter. Please note this information
 *        is not relevant/applicable for a read operation!
 *
 * @note  From I2CR FW doc:
 *        -----------------
 *        Only bits 0 through 30 of the scom/PCB address are used to access the
 *        on chip registers. Bit 31 is the odd parity bit.
 *
 *        32 bit I2CR address = (SCOM/CFAM address [1:31]<<1) + Odd 31st Parity Bit
 *
 *        For a Read operation:
 *        Parity bit calculation is based on address only
 *
 *        For a Write operation:
 *        Parity bit calculation is done based on address + data to be written
 *
 *        Final Address word calculation : Input address [0:30] + Parity bit_31
 *
 * @param [in] i_addrData:  address data that needs the parity bit added
 * @param [in] i_writeDataIsOdd: This field tells if the associated data has odd
 *                               number of 1's or not. This field is false for a
 *                               read operation. For write, a value of true implies
 *                               data has odd number of 1s and false implies even.
 * @return i_addrData : addr data with the parity bit added
 *
 */
 uint32_t setOddParityBit( uint32_t i_addrData, bool i_writeDataIsOdd = false )
 {
     // The input address is expected to be shifted; so the valid
     // address bits are 0 to 30. We don't care the bit value of
     // of the 31st bit for parity calculation.
     uint32_t l_addr = i_addrData & 0xFFFFFFFE;

     bool l_addrIsOddParity = checkOddParity(&l_addr, sizeof(uint32_t));

     // Check if the other data fields (for a write operation)
     // need to be considered to adjust the final l_parity bit
     // setting.
     // NOTE: For Read operations, writeDataIsOdd is false.
     if (i_writeDataIsOdd)
     {
         // Flip the  addr parity as data has odd # of 1s.
         l_addrIsOddParity = l_addrIsOddParity ^ 1;
     }

     // Set the odd parity bit in the passed in address.
     if ( !l_addrIsOddParity )
     {
         i_addrData |= 0x00000001;
     }
     else
     {
         i_addrData &= ~0x00000001;
     }

     FAPI_DBG("setOddParityBit: With l_parity: 0x%x", i_addrData);
     return i_addrData;
 }

void copyVectorIn(std::vector<uint8_t> &vectorTarget, uint8_t *data, size_t dataSize){
  while(dataSize>0){
    vectorTarget.push_back(*data);
    dataSize--; data++;
  }
}

void copyVectorOut(const std::vector<uint8_t> &vectorTarget, uint8_t *data, size_t dataSize){
    if(dataSize!=vectorTarget.size()){
        FAPI_ERR("copyVectorOut size does not match")
        return;
    }
    for(size_t i=0;i<dataSize;i++)
        data[i]=vectorTarget[i];
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 *
 * @brief A function to do the generic i2cr Read Operation
 *
 * @param [in] i_regaddr : I2CR register address already shifted appropriately
 *                         by the caller based on whether its SCOM or CFAM
 * @param [in] i_target : Target OCMB chip
 * @param [in/out] io_buffer : buffer to return data read
 * @param [in/out] io_buflen : read size
 * @param [in] io_i2cInfo : i2c control info
 * @return l_err : An error that is set in this routine
 *
 */
fapi2::ReturnCode i2cr_generic_read( uint32_t i_regaddr,
                               fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                               void* io_buffer,
                               size_t& io_buflen)
 {
     fapi2::ReturnCode l_err = NULL;
     uint32_t l_regaddr = 0;
     uint8_t l_regaddr8[sizeof(l_regaddr)] = {0};


     FAPI_DBG("i2cr_generic_read> regaddr:0x%x",i_regaddr);

     // Add the parity bit. The address is expected to be already shifted based on
     // the access type SCOM Vs CFAM.
     l_regaddr = setOddParityBit(i_regaddr);

     // Bytes are sent in reverse order as HB is big endian and
     // we need to send data to OCMBs in little endian format.
     swapBytes(&l_regaddr, l_regaddr8, sizeof(l_regaddr));

     /*
      *
      * template< TargetType K, MulticastType M, typename V >
      * inline ReturnCode getI2c(const Target<K, M, V>& i_target,
      *                         const size_t i_get_size,
      *                         const std::vector<uint8_t>& i_data,
      *                         std::vector<uint8_t>& o_data);
      *
      * /// @brief Writes data via i2c to the target.
      * ///
      * /// Example use (seeprom write 4 bytes of zeros w/ 2-byte address: 0x0208):
      * /// std::vector<uint8_t> addr_data = {0x02, 0x08, 0x00, 0x00, 0x00, 0x00};
      * /// FAPI_TRY(putI2c(target, addr_data));
      * ///
      * /// Example use (smbus write 1 data length byte + 4 bytes of zeros w/ 1-byte command: 0x01):
      * /// std::vector<uint8_t> command_data = {0x01, 0x04, 0x00, 0x00, 0x00, 0x00};
      * /// FAPI_TRY(putI2c(target, command_data));
      * ///
      * /// @tparam K the type (Kind) of target, from i_target
      * /// @tparam V the type of the target's Value, from i_target
      * /// @param[in] i_target     HW target to operate on.
      * /// @param[in] i_data       Buffer that holds data to write to the HW target.
      * /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
      * template< TargetType K, MulticastType M, typename V >
      * inline ReturnCode putI2c(const Target<K, M, V>& i_target,
      *                         const std::vector<uint8_t>& i_data);
      *
      */

     do
     {
         FAPI_DBG( "i2cr_generic_read> Reading (0x%x bytes) from %x", io_buflen, *(reinterpret_cast<uint32_t *>(l_regaddr8)));
         std::vector<uint8_t> addr, data;
         copyVectorIn(addr, l_regaddr8, sizeof(l_regaddr8));
         l_err=fapi2::getI2c(i_target, io_buflen, addr, data);
         copyVectorOut(data, (uint8_t*)io_buffer, io_buflen);

         if( l_err )
         {
             FAPI_ERR("i2cr_generic_read> Read Error" );
             break;
         }

         // data comes back in reverse order
         // Please note HB is BE and we get data in LE.
         uint8_t swapped[io_buflen] = {0};
         swapBytes(io_buffer, swapped, io_buflen);


         memcpy(io_buffer, swapped, io_buflen);

     } while (0);

     return l_err;
 }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 *
 * @brief A function to do the generic i2cr Write Operation
 *
 * @param [in] i_regaddr : I2CR register address already shifted appropriately
 *                         by the caller based on whether its SCOM or CFAM
 * @param [in] i_target : Target OCMB chip
 * @param [in/out] io_buffer : buffer with data to be written
 * @param [in/out] io_buflen : buffer size
 * @param [in] i_i2cInfo : i2c control info
 * @param [out] l_err : An error that is set in this routine
 *
 */
 fapi2::ReturnCode i2cr_generic_write( uint32_t i_regaddr,
                                fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                void* io_buffer,
                                size_t& io_buflen)
 {
     fapi2::ReturnCode l_err = NULL;
     uint32_t l_regaddr = 0;
     uint8_t l_regaddr8[sizeof(l_regaddr)] = {0};
     uint8_t l_swapped[io_buflen] = {0};

     // Determine the I2CR Master HUB chip associated with the OCMB chip

     FAPI_DBG("i2cr_generic_write> Enter: regaddr:0x%x", i_regaddr);
     FAPI_DBG("io_bufferlen=0x%x", io_buflen);

     // Add the parity bit. In order to set the parity bit for a write operation,
     // we need to detemine if the data that needs to be written out has an odd
     // parity or not. The final parity bit setting is based on the number of 1s
     // in both data and address.
     bool l_data_parity = checkOddParity(io_buffer, io_buflen);

     // Now pass the info about data's parity to the routine that checks the
     // parity of the specified  address and sets the odd parity bit based on
     // the parity of both address and data.
     l_regaddr = setOddParityBit(i_regaddr, l_data_parity);

     // Bytes are sent in reverse order
     // Please note HB is BE and we need to send data in LE.
     swapBytes(&l_regaddr, l_regaddr8, sizeof(l_regaddr));

     // Swap the input data that needs to be written
     swapBytes(io_buffer, l_swapped, io_buflen);

     do
     {
         std::vector<uint8_t> writeData;
         copyVectorIn(writeData, l_regaddr8, sizeof(l_regaddr8));
         copyVectorIn(writeData, l_swapped, io_buflen);
         l_err=fapi2::putI2c(i_target, writeData);

         if( l_err )
         {
             FAPI_ERR( "i2cr_generic_write> Write Error" );
             break;
         }

     } while(0);

     return l_err;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/**
 *
 * @brief A function to do the I2CR R/W operations
 *
 * @param [in] i_opType : operation type (READ or WRITE)
 * @param [in] i_target : Target OCMB chip
 * @param [in/out] io_buffer : buffer with data to be written
 * @param [in/out] io_buflen : buffer size
 * @param [in] i_accessType : access type (SCOM/CFAM)
 * @param [in] i_args : i2cr register address (SCOM or CFAM word addr)
 * @return l_err : An error that is set in this routine
 *
 */
fapi2::ReturnCode i2crPerformOp(OperationType i_opType,
                             fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                             void* io_buffer,
                             size_t& io_buflen,
                             AccessType i_accessType,
                             uint32_t i_i2crAddr)
{
    // The input i2cr address is either a SCOM address or CFAM(FSI) word address
    uint32_t l_shiftedI2crAddr = 0;
    fapi2::ReturnCode l_err = NULL;

    FAPI_DBG( "i2crPerformOp Addr=0x%08x io_buflen=0x%x", i_i2crAddr, io_buflen );
    do
    {

        // Validate Scom/CFAM Address
        if( (i_i2crAddr & 0x80000000) != 0)
        {
            FAPI_ERR("i2crPerformOp> Address contains more than 31 bits : i_i2crAddr=0x%08X", i_i2crAddr );
            /*@
             * @errortype
             * @moduleid     I2CR::MOD_I2CR_PERFORM_OP
             * @reasoncode   I2CR::RC_INVALID_ADDRESS
             * @userdata1    SCOM Address
             * @userdata2    Target HUID
             * @devdesc      i2crPerformOp> Address contains
             *               more than 31 bits.
             * @custdesc     A problem occurred during the IPL of the system:
             *               Invalid address on a register R/W operation.
             */
            l_err = fapi2::FAPI2_RC_FALSE;
            break;
        }

        // Handle any address shifts based on the access type
        // --------------------------------------------------------------------------------
        //
        // There are two I2CR access types supported: Scom & FSI/CFAM.
        //
        // The I2CR address is formed using the bits [1:31] of the Scom address. These
        // bits are left shifted by 1 to form the bits [0:30] of the I2CR address. The
        // 31st bit of the I2CR address is the Odd Parity bit.
        // The FSI/CFAM word address is first converted to a Scom address and then shifted
        // left by 1 bit to form the I2CR address.
        // Note: The I2CR address is equal to the FSI Byte address divided by 4.
        //       This driver expects only the FSI/CFAM word address.
        //
        // The following algorithm is used to determine the Odd parity bit value.
        //
        // Parity (write operation) = Odd Parity (SCOM address [1:31] & Write data [0:63])
        // Parity (read operation) = Odd Parity (SCOM address [1:31] )
        // Address_on_I2C_lines (0 : 31) = SCOM address [1:31] & parity
        //
        // Examples:
        //   Scom addr: 0xf000f, Shifted I2CR addr: 0x1e001e, With Parity: 0x1e001f
        //   CFAM register 0x100A (FSI byte address 0x1028)
        //   Converted I2CR addr (no shift): 0x040A, Shifted I2CR addr: 0x814,
        //   With Parity: 0x814
        //
        // --------------------------------------------------------------------------------
        if (i_accessType == I2CR_SCOM)
        {
            l_shiftedI2crAddr = i_i2crAddr << 1;
        }
        else if ((i_accessType == I2CR_CFAM) ||
                 (i_accessType == CFAM))
        {
            l_shiftedI2crAddr = convertFsiCfamAddrToI2crAddr(i_i2crAddr) << 1;
        }
        else
        {
            FAPI_ERR("i2crPerformOp> Unsupported Access Type: i_accessType=0x%x)",i_accessType);
            /*@
             * @errortype
             * @moduleid     I2CR::MOD_I2CR_PERFORM_OP
             * @reasoncode   I2CR::RC_INVALID_ACCESSTYPE
             * @userdata1[0:31]    Access type: I2CR_SCOM or I2CR_CFAM
             * @userdata1[32:64]   Input Scom address
             * @userdata2    Target HUID
             * @devdesc      i2crPerformOp> Unsupported Access Type specified
             * @custdesc     A problem occurred during the IPL of the system:
             *               Unsupported I2CR Access type.
             */
            l_err = fapi2::FAPI2_RC_FALSE;
            break;
        }

         // Handle the specified operation
         if(i_opType == READ)
         {
             FAPI_DBG("i2crPerformOp> Read(l_i2crAddr=0x%.8X shifted=0x%.8X)",i_i2crAddr, l_shiftedI2crAddr);

             // All I2CR operations (both Scom & CFAM) are 8 bytes long.
             // For CFAM access type, we may need to strip the extra 4 data bytes.
             if (((i_accessType == I2CR_CFAM) ||
                 (i_accessType == CFAM)) && (io_buflen == sizeof(uint32_t)))
             {
                 size_t l_sz = 8;
                 uint8_t l_buf64[l_sz] = {0};

                 // Read 8 bytes even though CFAM is typically 4 bytes long
                 l_err = i2cr_generic_read(l_shiftedI2crAddr, i_target, l_buf64, l_sz);

                 // Just copy over the most significant 4 bytes.
                 memcpy(io_buffer, l_buf64, io_buflen);
             }
             else
             {
                 l_err = i2cr_generic_read(l_shiftedI2crAddr, i_target,
                                           reinterpret_cast<uint8_t*>(io_buffer),
                                           io_buflen);
             }
             dump_io_buffer(io_buffer, io_buflen);
         }
         else if(i_opType == WRITE)
         {
             FAPI_DBG( "i2crPerformOp> Write(l_i2crAddr=0x%.8X shifted=0x%.8X",i_i2crAddr, l_shiftedI2crAddr);
             dump_io_buffer(io_buffer, io_buflen);

             // All I2CR operations (both Scom & CFAM) are 8 bytes long.
             // For CFAM access type, we may need to append the extra 4 data bytes.
             if (((i_accessType == I2CR_CFAM) ||
                 (i_accessType == CFAM)) && (io_buflen == sizeof(uint32_t)))
             {
                 size_t l_sz = 8;
                 uint8_t l_buf64[l_sz] = {0};

                 // Copy the 4 bytes data to be written in the most significant bits.
                 // Rest of the 4 bytes is initialized to zero.
                 memcpy(l_buf64, io_buffer, io_buflen);

                 // Write 8 bytes data.
                 l_err = i2cr_generic_write(l_shiftedI2crAddr, i_target, l_buf64, l_sz);
             }
             else
             {
                 l_err = i2cr_generic_write(l_shiftedI2crAddr, i_target,
                                            reinterpret_cast<uint8_t*>(io_buffer),
                                            io_buflen);
             }

             FAPI_DBG("i2crPerformOp> Final write out: Addr=0x%X",l_shiftedI2crAddr);
             dump_io_buffer(io_buffer, io_buflen);
         }
         else
         {
             FAPI_ERR( "i2crPerformOp> Unsupported Operation Type: i_opType=0x%x)",i_opType);
             /*@
              * @errortype
              * @moduleid     I2CR::MOD_I2CR_PERFORM_OP
              * @reasoncode   I2CR::RC_INVALID_OPTYPE
              * @userdata1[0:31]    Operation Type (i_opType) : 0=READ, 1=WRITE
              * @userdata1[32:64]   Input Scom address
              * @userdata2    Target HUID
              * @devdesc      i2crPerformOp> Unsupported Operation Type specified
              * @custdesc     A problem occurred during the IPL of the system:
              *               Unsupported SCOM operation type.
              */
             l_err = fapi2::FAPI2_RC_FALSE;
             break;
         }

    } while(0);

    return l_err;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/**
 *
 * @brief A wrapper function to do the I2CR CFAM/FSI R/W operations
 *
 * @param [in] i_opType : operation type (READ or WRITE)
 * @param [in] i_target : Target OCMB chip
 * @param [in] io_buffer : buffer with data to be written
 * @param [in] io_buflen : buffer size
 * @param [in] i_accessType : access type (CFAM)
 * @param [in] i_args : i2cr register address (CFAM word addr)
 * @return l_err : An error that is set in this routine
 *
 */
fapi2::ReturnCode i2crCfamPerformOp(OperationType i_opType,
                             fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                             void* io_buffer,
                             size_t& io_buflen,
                             AccessType i_accessType,
                             uint32_t i_i2crAddr)
{
    // The input i2cr address is a CFAM(FSI) word address
    fapi2::ReturnCode l_err = NULL;

    FAPI_DBG( "i2crCfamPerformOp> Addr=0x%.8x io_buflen=0x%x chipId=%x",i_i2crAddr, io_buflen );
    do
    {
        if (
            ((i_accessType == I2CR_CFAM) ||
             (i_accessType == CFAM)))
        {
            l_err = i2crPerformOp(i_opType, i_target, io_buffer,
                                  io_buflen, i_accessType,
                                  i_i2crAddr);
            break;
        }

        // If we are here, the chip Id indicates either its not Odyssey type
        // or we do not have the correct access type. Log an error.

        FAPI_ERR("i2crCfamPerformOp> Unsupported OCMB Type or invalid access type! accessType=%d", i_accessType );

        /*@
         * @errortype
         * @moduleid     I2CR::MOD_I2CR_PERFORM_OP
         * @reasoncode   I2CR::RC_INVALID_OCMBTYPE
         * @userdata1[0:31]    Operation Type (i_opType) : 0=READ, 1=WRITE
         * @userdata1[32:64]   Input Scom address
         * @userdata2    Target HUID
         * @devdesc      i2crCfamPerformOp> Unsupported OCMB Type specified
         * @custdesc     A problem occurred during the IPL of the system:
         *               Unsupported CFAM OCMB type for I2CR operation.
         */
         l_err = fapi2::FAPI2_RC_FALSE;

    } while(0);

    return l_err;
}


} // end I2CR namespace
