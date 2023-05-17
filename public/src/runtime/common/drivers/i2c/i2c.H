/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/drivers/i2c/i2c.H $                 */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2018,2023                        */
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

#pragma once

#include "i2cdefs.H"
#include "fapi2.H"

/**
 * @brief i2c driver currently supports legacy Mode
 *        TODO: PFSBE-357 - Support fast mode of i2c driver.
 *
 */

namespace fapi2
{

class i2c {

public:

  /**
   * @brief Print all i2c config details
   *
   */
  void printi2cdetails();

  /**
   * @brief Function to populate platform specific details for
   *        i2c communication
   *        This implementation should be in derived class
   *        based on use case
   *
   * @param target FAPI HW target to operate on.
   */
  virtual ReturnCode populatei2cdetails(const Target<TARGET_TYPE_ALL>& target)
  {return 0;};

  /**
   * @brief To read from i2c device
   *        This implementation can be overriden in derived class
   *        based on use case
   *
   * @param target FAPI HW target to operate on.
   * @param get_size Size that getI2c will read from the HW target.
   * @param cfgData Buffer that holds data to write to the HW target.
   *                May be empty if no address/command is required
   *                before the read.
   * @param o_data Buffer that holds data read from HW target.
   * @return ReturnCode FAPI2_RC_SUCCESS if success, else error code.
   */
  virtual ReturnCode getI2c( const Target<TARGET_TYPE_ALL>& target,
                  const size_t get_size,
                  const std::vector<uint8_t>& cfgData,
                  std::vector<uint8_t>& o_data );

  /**
   * @brief To write to i2c device
   *        This implementation can be overriden in derived class
   *        based on use case
   *
   * @param target FAPI HW target to operate on.
   * @param data Buffer that holds data to write to the HW target.
   * @return ReturnCode fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
   */
  virtual ReturnCode putI2c( const Target<TARGET_TYPE_ALL>& target,
                          const std::vector<uint8_t>& data );

protected:

  uint8_t  iv_port;
  uint8_t  iv_engine;
  uint8_t  iv_devAddr;
  bool     iv_skip_mode_setup;
  bool     iv_with_stop;
  bool     iv_read_not_write;
  uint64_t iv_bus_speed;          // in kbits/sec (ie 400KHz)
  uint16_t iv_bit_rate_divisor;   // uint16_t to match size in mode register
  uint64_t iv_polling_interval_ns;// in nanoseconds
  uint64_t iv_timeout_count;

  /**
  * @brief write to a I2C device
  *
  * @param[out] buffer       Buffer with data to be written
  * @param[in]  buflen       Length in bytes to be written
  *
  * @return FAPI RC
  */
  ReturnCode i2cWrite(void *const buffer,
                      size_t buflen);

  /**
  * @brief Read from a I2C device
  *
  * @param[out] o_buffer     Buffer to hold the read data
  * @param[in]  buflen       Length in bytes to be read
  *
  * @return FAPI RC
  */
  ReturnCode i2cRead(void *o_buffer,
                    size_t buflen);

  /**
  * @brief Lock I2C engine
  *
  * @return FAPI RC
  */
  ReturnCode i2cLockEngine();

  /**
  * @brief Unlock I2C engine
  *
  * @return FAPI RC
  */
  ReturnCode i2cUnlockEngine();

  /**
   * @brief Inline function used to calculate Bit Rate Divisor setting
   *        based on I2C Bus Speed and Nest Frequency
   *
   * @param [in] i_bus_speed_khz  Bus Speed in KHz
   * @param [in] i_local_bus_MHZ  Local Bus that feeds I2C Master's clock
   *
   * @return Bit Rate Divisor value
  */
  inline uint16_t i2cGetBitRateDivisor(uint64_t i_bus_speed_khz,
                                     uint64_t i_local_bus_MHZ)
  {
    // Use tmp variable to convert everything to KHZ safely
    uint64_t local_bus_KHZ = ( i_local_bus_MHZ  ) * 1000;

    return uint16_t( ( ( local_bus_KHZ / i_bus_speed_khz ) - 1 ) / 4 );
  }

  /**
   * @brief I2C Polling Interval based on bus speed; set to 1/10th of expected
   *        duration
   *
   * NOTE: I2C Bus Speed in KBits/sec, so multiple by 8
   *       since Device Driver works on a byte-level
   *
   * @param [in] i_bus_speed_khz Bus Speed in KHz
   *
   * @return Polling Interval in nanoseconds
  */
  inline uint64_t i2cGetPollingInterval(uint64_t i_bus_speed_khz )
  {
    // Polling Interval = 8 * (1/bus+speed) * (1/10) -> converted to ns
    return ( ( 8 * NS_PER_SEC ) / ( 10 * i_bus_speed_khz * 1000 ) );
  }

private:

  /**
  * @brief Perform I2C register operation
  *
  * @param[in]       reg           I2C register offset
  * @param[in]       readNotWrite  Read/Write
  * @param[in,out]   io_data       uint64_t data buffer
  *
  * @return FAPI RC
  */
  ReturnCode i2cRegisterOp(i2c_reg_offset_t reg,
                          bool readNotWrite,
                          uint64_t* io_data);

  /**
   * @brief Check if there are any errors during i2c operation
   *
   * @param status status reg read value
   * @return ReturnCode FAPI RC
   */
  ReturnCode i2cCheckForErrors(status_reg_t &status);

  /**
   * @brief Read the i2c status register
   *
   * @param o_status status reg output
   * @return ReturnCode FAPI RC
   */
  ReturnCode i2cReadStatusReg(status_reg_t &o_status);

  /**
  * @brief Wait for previous command complete on i2c master
  *
  * @return FAPI RC
  */
  ReturnCode i2cWaitForCmdComp();

  /**
  * @brief Setup I2C mode register
  *
  * @param[in]       len    Length to be read/written
  *
  * @return FAPI RC
  */
  ReturnCode i2cSetup(size_t len);

  /*
  * @brief Check for a fifo condition on i2c status register
  *
  * @param[in] condition condition to check for
  * @param[in] status    value of i2c status register
  *
  * @return TRUE if condition holds good, else FALSE
  */
  bool fifoCondition(fifo_condition_t condition,
                    status_reg_t status);

  /**
  * @brief Wait for a fifo condition on i2c status register
  *
  * @param[in] condition condition to wait for
  *
  * @return FAPI RC
  */
  ReturnCode i2cWaitForFifo(fifo_condition_t condition);

  /**
   * @brief Reset the i2c engine
   *
   * @return ReturnCode FAPI RC
   */
  ReturnCode i2cResetEngine();
};

} //namespace fapi2