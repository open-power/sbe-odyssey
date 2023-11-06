/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/drivers/i2c/i2c.C $                 */
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

#include "i2c.H"
#include "sbetrace.H"
#include "sbeutil.H"

namespace fapi2
{

ReturnCode i2c::i2cRegisterOp(i2c_reg_offset_t reg,
                         bool readNotWrite,
                         uint64_t* io_data)
{
    #define SBE_FUNC " i2cRegisterOp "
    SBE_ENTER(SBE_FUNC);

    Target< TARGET_TYPE_ALL > l_fapiTarget = g_platTarget->plat_getChipTarget();
    uint64_t addr = I2C_MASTER_PIB_BASE_ADDR+
                    (iv_engine * ENGINE_SCOM_OFFSET) +
                    reg;

    do{

        if(readNotWrite)
        {
            buffer<uint64_t> data;
            FAPI_TRY(getScom(l_fapiTarget,
                                    addr,
                                    data),
                                    "Error reading i2c register");
            *io_data = data;
        }
        else
        {
            FAPI_TRY(putScom(l_fapiTarget,
                                    addr,
                                    *io_data),
                                    "Error writing i2c register");
        }

        SBE_DEBUG( SBE_FUNC "ReadNotWrite[%d] Addr [0x%08x] Data [0x%08x%08x]",
        readNotWrite, SBE::lower32BWord(addr),
        SBE::higher32BWord(*io_data), SBE::lower32BWord(*io_data));

    }while(false);

fapi_try_exit:

    SBE_EXIT(SBE_FUNC);
    return current_err;
    #undef SBE_FUNC
}

ReturnCode i2c::i2cCheckForErrors(status_reg_t &status)
{
    #define SBE_FUNC " i2cCheckForErrors "
    SBE_ENTER(SBE_FUNC)

    ReturnCode rc = FAPI2_RC_SUCCESS;
    do
    {
        if(status.invalid_cmd ||
           status.lbus_parity_error ||
           status.backend_overrun_error ||
           status.backend_access_error ||
           status.arbitration_lost_error ||
           status.nack_received ||
           status.stop_error)
        {

            rc = RC_I2C_STATUS_ERROR;
            FAPI_ASSERT(false,
                        I2C_STATUS_ERROR()
                        .set_VALUE(status.value)
                        .set_PORT(iv_port)
                        .set_ENGINE(iv_engine)
                        .set_DEVICEADDR(iv_devAddr),
                        "I2C Status error");
        }

        if(status.any_i2c_interrupt)
        {
            interrupt_reg_t intreg;
            rc = i2cRegisterOp(I2C_REG_INTERRUPT,
                               true,
                               (uint64_t*)&intreg);
            if(rc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " failed for i2cRegisterOp with rc 0x%08X", rc);
                break;
            }
            rc = RC_I2C_STATUS_INTR_ERROR;
            FAPI_ASSERT(false,
                        I2C_STATUS_INTR_ERROR()
                        .set_STATUS(status.value)
                        .set_VALUE(intreg.value)
                        .set_PORT(iv_port)
                        .set_ENGINE(iv_engine)
                        .set_DEVICEADDR(iv_devAddr),
                        "I2C Interrupt Status error");
        }
    } while(0);

fapi_try_exit:
    SBE_EXIT(SBE_FUNC)
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::i2cReadStatusReg(status_reg_t &o_status)
{
    #define SBE_FUNC " i2cReadStatusReg "
    SBE_ENTER(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;

    do
    {
        rc = i2cRegisterOp(I2C_REG_STATUS,
                           true,
                           (uint64_t*)&o_status);
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cRegisterOp with rc 0x%08X", rc);
            break;
        }
    } while(0);

    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::i2cWaitForCmdComp()
{
    #define SBE_FUNC " i2cWaitForCmdComp "
    SBE_ENTER(SBE_FUNC);

    ReturnCode rc = FAPI2_RC_SUCCESS;
    status_reg_t status = {};
    uint64_t interval_ns = iv_polling_interval_ns;
    uint64_t timeoutCount = iv_timeout_count;
    SBE_DEBUG(SBE_FUNC "interval [%d]ns count [%d]", interval_ns, timeoutCount);

    do
    {
        do
        {
            delay(interval_ns,
                  SIM_INTERVAL_DELAY);

            status.value = 0;
            rc = i2cReadStatusReg(status);
            if(rc != FAPI2_RC_SUCCESS)
            {
                break;
            }
            if(--timeoutCount == 0)
            {
                rc = RC_SBE_I2C_WAIT_FOR_CMD_COMP_TIMEOUT_ERROR;
                FAPI_ASSERT(false,
                            SBE_I2C_WAIT_FOR_CMD_COMP_TIMEOUT_ERROR()
                            .set_STATUS(status.value)
                            .set_PORT(iv_port)
                            .set_ENGINE(iv_engine)
                            .set_DEVICEADDR(iv_devAddr)
                            .set_POLLINGINT(iv_polling_interval_ns)
                            .set_TIMEOUTCOUNT(iv_timeout_count),
                            "timedout waiting for cmd completion");
                break;
            }
        } while(!status.command_complete);

        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " timedout waiting for cmd completion with rc 0x%08X", rc);
            break;
        }
    } while(0);

fapi_try_exit:
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::i2cSetup(size_t len)
{
    #define SBE_FUNC " i2cRead "
    SBE_ENTER(SBE_FUNC);

    ReturnCode rc = FAPI2_RC_SUCCESS;
    mode_reg_t mode;
    command_reg_t cmd;

    do
    {
        rc = i2cWaitForCmdComp();
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cWaitForCmdComp with rc 0x%08X", rc);
            break;
        }

        if(!iv_skip_mode_setup)
        {
            mode.value = 0;
            mode.bit_rate_div = iv_bit_rate_divisor;
            mode.port_num = iv_port;

            rc = i2cRegisterOp(I2C_REG_MODE,
                               false,
                               (uint64_t*)&mode.value);
            if(rc != FAPI2_RC_SUCCESS)
            {
                break;
            }
        }

        // Write Command Register:
        //      - with start
        //      - with stop
        //      - RnW
        //      - length
        cmd.value = 0x0ull;
        cmd.with_start = 1;
        cmd.with_stop = (iv_with_stop ? 1 : 0);
        cmd.with_addr = 1;

        // cmd.device_addr is 7 bits
        // devAddr though is a uint64_t
        //  -- value stored in LSB byte of uint64_t
        //  -- LS-bit is unused, creating the 7 bit cmd.device_addr
        //  So will be masking for LSB, and then shifting to push off LS-bit
        cmd.device_addr = (0x000000FF & iv_devAddr) >> 1;

        cmd.read_not_write = (iv_read_not_write ? 1 : 0);
        cmd.length_b = len;

        rc = i2cRegisterOp(I2C_REG_COMMAND,
                           false,
                           (uint64_t*)&cmd.value);
        if(rc != FAPI2_RC_SUCCESS)
        {
            break;
        }

    } while(0);

    SBE_EXIT(SBE_FUNC)
    return rc;
    #undef SBE_FUNC
}

bool i2c::fifoCondition(fifo_condition_t condition,
                   status_reg_t status)
{
    #define SBE_FUNC " fifoCondition "
    SBE_ENTER(SBE_FUNC);

    bool conditionStatus = false;

    if(condition == FIFO_DATA_AVAILABLE)
    {
        conditionStatus =  ((0 == status.fifo_entry_count) &&
                        (0 == status.data_request));
    }
    else if(condition == FIFO_FREE)
    {
        conditionStatus = ((I2C_MAX_FIFO_CAPACITY <= status.fifo_entry_count) &&
                        (0 == status.data_request));
    }

    SBE_EXIT(SBE_FUNC);
    return conditionStatus;
    #undef SBE_FUNC
}

ReturnCode i2c::i2cWaitForFifo(fifo_condition_t condition)
{
    #define SBE_FUNC " i2cWaitForFifo "
    SBE_ENTER(SBE_FUNC);

    ReturnCode rc = FAPI2_RC_SUCCESS;
    uint64_t interval_ns = iv_polling_interval_ns;
    uint64_t timeoutCount = iv_timeout_count;

    status_reg_t status = {};
    do
    {
        rc = i2cReadStatusReg(status);
        if(rc != FAPI2_RC_SUCCESS)
        {
            break;
        }

        while(fifoCondition(condition, status))
        {
            delay(interval_ns,
                  SIM_INTERVAL_DELAY);

            status.value = 0;
            rc = i2cReadStatusReg(status);
            if(rc != FAPI2_RC_SUCCESS)
            {
                break;
            }
            if(--timeoutCount == 0)
            {
                rc = RC_I2C_FIFO_TIMEOUT_ERROR;
                FAPI_ASSERT(false,
                            I2C_FIFO_TIMEOUT_ERROR()
                            .set_STATUS(status.value)
                            .set_PORT(iv_port)
                            .set_ENGINE(iv_engine)
                            .set_DEVICEADDR(iv_devAddr)
                            .set_POLLINGINT(iv_polling_interval_ns)
                            .set_TIMEOUTCOUNT(iv_timeout_count),
                            "timedout waiting for fifo condition");
                break;
            }
        }
    } while(0);

fapi_try_exit:
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::i2cWrite(void *const buffer,
                    size_t buflen)
{
    #define SBE_FUNC " i2cWrite "
    SBE_ENTER(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;
    size_t bytesWritten = buflen;

    fifo_reg_t fifo = {};

    do
    {
        iv_read_not_write = false;
        rc = i2cSetup(buflen);
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cSetup with rc 0x%08X", rc);
            break;
        }

        for(bytesWritten = 0; bytesWritten < buflen; bytesWritten++)
        {
            rc = i2cWaitForFifo(FIFO_FREE);
            if(rc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " failed for i2cWaitForFifo with rc 0x%08X", rc);
                break;
            }

            fifo.value = 0;
            // Write data to FIFO
            fifo.byte_0 = *((uint8_t*)buffer + bytesWritten);
            rc = i2cRegisterOp(I2C_REG_FIFO,
                               false,
                               (uint64_t*)&fifo);
            if(rc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " failed for i2cRegisterOp with rc 0x%08X", rc);
                break;
            }
        }
        if(rc != FAPI2_RC_SUCCESS)
        {
            break;
        }
        rc = i2cWaitForCmdComp();
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cWaitForCmdComp with rc 0x%08X", rc);
            break;
        }
    } while(0);

    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::i2cRead(void *o_buffer,
                   size_t buflen)
{
    #define SBE_FUNC " i2cRead "
    SBE_ENTER(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;

    size_t bytesRead = buflen;

    fifo_reg_t fifo = {};

    do
    {
        iv_read_not_write = true;
        rc = i2cSetup(buflen);
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cSetup with rc 0x%08X", rc);
            break;
        }

        for(bytesRead = 0; bytesRead < buflen; bytesRead++)
        {
            rc = i2cWaitForFifo(FIFO_DATA_AVAILABLE);
            if(rc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " failed for i2cWaitForFifo with rc 0x%08X", rc);
                break;
            }

            fifo.value = 0;
            rc = i2cRegisterOp(I2C_REG_FIFO,
                               true,
                               (uint64_t*)&fifo);
            if(rc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " failed for i2cRegisterOp with rc 0x%08X", rc);
                break;
            }

            *((uint8_t*)o_buffer + bytesRead) = fifo.byte_0;
        }
        if(rc != FAPI2_RC_SUCCESS)
        {
            break;
        }
        rc = i2cWaitForCmdComp();
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cWaitForCmdComp with rc 0x%08X", rc);
            break;
        }
    } while(0);

    SBE_EXIT(SBE_FUNC)
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::i2cLockEngine()
{
    #define SBE_FUNC " i2cLockEngine "
    SBE_ENTER(SBE_FUNC)

    ReturnCode rc = FAPI2_RC_SUCCESS;
    uint64_t interval_ns = 160000; // 160 us
    uint64_t timeoutCount = 4000/160; // total 4 ms
    uint64_t lock_data = 0x8000000000000000;

    do
    {
        rc = i2cRegisterOp(I2C_REG_ATOMIC_LOCK,
                           false,
                           &lock_data);

        if(rc == FAPI2_RC_SUCCESS)
        {
            SBE_DEBUG( SBE_FUNC " succeeded in grabbing i2c lock. ");
            break;
        }

        // Incase HB,CRONUS etc have the i2c lock and we are trying to
        // grab the lock as well, we will fail with RC_SBE_PIB_XSCOM_ERROR
        if(rc != (const uint32_t) RC_SBE_PIB_XSCOM_ERROR)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cRegisterOp with rc 0x%08X", rc);
            break;
        }

        delay(interval_ns, SIM_INTERVAL_DELAY);
    } while(--timeoutCount);

    if(timeoutCount == 0)
    {
        lock_data = 0x0;
        ReturnCode l_rc = FAPI2_RC_SUCCESS;
        //TODO: PFSBE-394 i2c driver: Error handling improvement.
        //RC handling when multiple rc are generated
        l_rc = i2cRegisterOp(I2C_REG_ATOMIC_LOCK,
                            true,
                            &lock_data);
        if(l_rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR("Failed to read atomic lock reg. RC: 0x%08X", l_rc);
        }

        rc = RC_SBE_I2C_FAILED_TO_LOCK_ENGINE_TIMEOUT_ERROR;
        FAPI_ASSERT(false,
                    SBE_I2C_FAILED_TO_LOCK_ENGINE_TIMEOUT_ERROR()
                    .set_ATOMICLOCKREG(lock_data)
                    .set_PORT(iv_port)
                    .set_ENGINE(iv_engine)
                    .set_DEVICEADDR(iv_devAddr)
                    .set_POLLINGINT(interval_ns)
                    .set_TIMEOUTCOUNT(timeoutCount),
                    "timedout waiting to lock i2c engine");
    }

fapi_try_exit:
    SBE_EXIT(SBE_FUNC)
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::i2cUnlockEngine()
{
    #define SBE_FUNC " i2cUnlockEngine "
    SBE_ENTER(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;
    uint64_t unlock_data = 0;

    rc = i2cRegisterOp(I2C_REG_ATOMIC_LOCK,
                       false,
                       &unlock_data);

    SBE_EXIT(SBE_FUNC)
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::i2ccResetEngine()
{
    #define SBE_FUNC " i2ccResetEngine "
    SBE_ENTER(SBE_FUNC);

    ReturnCode rc = FAPI2_RC_SUCCESS;

    SBE_INFO(SBE_FUNC "Resetting i2c controller ENGINE......");

    do{
        //Set bit 0 in IMM_RESET_I2C_X to
        //reset command,mode,watermark,interrupt mask,status registers
        uint64_t reset_data = 0x8000000000000000;
        rc = i2cRegisterOp(I2C_REG_RESET,
                        false,
                        &reset_data);
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cRegisterOp with rc 0x%08X", rc);
            break;
        }

        //NOTE: Do not check for cmd completion here as it works in case of
        //      its good path, in bad path it will not be seen and we need to
        //      reset bus by sending stop command.
    }while (false);

    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::i2cBusReset()
{
    #define SBE_FUNC " i2cBusReset "
    SBE_ENTER(SBE_FUNC);

    ReturnCode rc = FAPI2_RC_SUCCESS;

    //Resetting i2c responder/end device by sending the STOP only command on the i2c bus
    // by i2c controller via automatic mode.
    SBE_INFO(SBE_FUNC "Resetting i2c responder/end-device by sending stop command......");

    do{

        // Write mode register with proper speed , proper port number and Enhanced_mode bit = 1.
        // Bit 28 FGAT_MODE_000: fgat mode == Enhanced_mode bit.
        // In automatic mode STOP command is sent by HW more than 2 times automatically
        mode_reg_t mode;

        mode.value = 0x0ull;
        mode.bit_rate_div = iv_bit_rate_divisor;
        mode.port_num = iv_port;
        mode.enhanced_mode = 1;

        rc = i2cRegisterOp(I2C_REG_MODE,
                            false,
                            (uint64_t*)&mode.value);
        if(rc != FAPI2_RC_SUCCESS)
        {
            break;
        }

        //Write command register/control register with only with_stop bit asserted to trigger the operation
        command_reg_t cmd;
        cmd.value = 0x0ull;
        cmd.with_stop = 1;

        rc = i2cRegisterOp(I2C_REG_COMMAND,
                           false,
                           (uint64_t*)&cmd.value);
        if(rc != FAPI2_RC_SUCCESS)
        {
            break;
        }

        // Poll status register’s command_complete bit for the value 1
        rc = i2cWaitForCmdComp();
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cWaitForCmdComp with rc 0x%08X", rc);
            break;
        }

    }while(false);

    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::i2cReset()
{
    #define SBE_FUNC " i2cReset "
    SBE_ENTER(SBE_FUNC)

    ReturnCode rc = FAPI2_RC_SUCCESS;

    do{

        rc = i2cLockEngine();
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cLockEngine with rc 0x%08X", rc);
            break;
        }

        rc = i2ccResetEngine();
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2ccResetEngine with rc 0x%08X", rc);
            break;
        }

        rc = i2cBusReset();
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cBusReset with rc 0x%08X", rc);
            break;
        }

    }while(false);

    //TODO: PFSBE-394
    ReturnCode l_rc = i2cUnlockEngine();
    if(l_rc != FAPI2_RC_SUCCESS)
    {
        SBE_ERROR(SBE_FUNC " failed for i2cUnlockEngine with l_rc 0x%08X", l_rc);
        if(rc == FAPI2_RC_SUCCESS)
            rc = l_rc;
    }

    SBE_EXIT(SBE_FUNC)
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::getI2c( const Target<TARGET_TYPE_ALL>& target,
                        const size_t get_size,
                        const std::vector<uint8_t>& cfgData,
                        std::vector<uint8_t>& o_data )
{
    #define SBE_FUNC " getI2c "
    SBE_ENTER(SBE_FUNC)

    ReturnCode rc = FAPI2_RC_SUCCESS;

    do {

        rc = populatei2cdetails(target);
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for populatei2cdetails with rc 0x%08X", rc);
            break;
        }

        rc = i2cLockEngine();
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cLockEngine with rc 0x%08X", rc);
            break;
        }

        uint8_t data[MAX_I2C_CMD_SIZE] = {};
        ////////////////////////////////
        // I2C read with offset ///////
        ///////////////////////////////
        if(!cfgData.empty())
        {
            std::copy(cfgData.begin(), cfgData.end(), data);
            // First write offset to device without stop
            iv_with_stop = false;
            iv_skip_mode_setup = false;

            rc = i2cWrite(data,
                            cfgData.size());
            if(rc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " failed for i2cWrite(with offset) with rc 0x%08X", rc);
                break;
            }
            // Now do the READ with a stop
            iv_with_stop = true;
            // Skip mode setup, cmd already set
            iv_skip_mode_setup = true;
        }
        ////////////////////////////////
        // I2C read without offset ////
        ///////////////////////////////
        else
        {
            // Do a direct read
            iv_with_stop = true;
            iv_skip_mode_setup = false;
        }
        rc = i2cRead(data,
                        get_size);
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cRead(without offset) with rc 0x%08X", rc);
            break;
        }

        o_data.clear();
        for(size_t i = 0; i < get_size; i++)
        {
            o_data.push_back(data[i]);
        }
    } while(0);

    //TODO: PFSBE-394
    ReturnCode l_rc = i2cUnlockEngine();
    if(l_rc != FAPI2_RC_SUCCESS)
    {
        SBE_ERROR(SBE_FUNC " failed for i2cUnlockEngine with l_rc 0x%08X", l_rc);
        if(rc == FAPI2_RC_SUCCESS)
            rc = l_rc;
    }

    SBE_EXIT(SBE_FUNC)
    return rc;
    #undef SBE_FUNC
}

ReturnCode i2c::putI2c( const Target<TARGET_TYPE_ALL>& target,
                        const std::vector<uint8_t>& data )
{
    #define SBE_FUNC " putI2c "
    SBE_ENTER(SBE_FUNC)

    ReturnCode rc = FAPI2_RC_SUCCESS;

    do {

        rc = populatei2cdetails(target);
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for populatei2cdetails with rc 0x%08X", rc);
            break;
        }

        rc = i2cLockEngine();
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cLockEngine with rc 0x%08X", rc);
            break;
        }

        uint8_t buffer[MAX_I2C_CMD_SIZE] = {};
        std::copy(data.begin(), data.end(), buffer);

        // Do a write with stop
        iv_with_stop = true;
        iv_skip_mode_setup = false;
        rc = i2cWrite(buffer,
                        data.size());
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for i2cWrite with rc 0x%08X", rc);
            break;
        }
    } while(0);

    //TODO: PFSBE-394
    ReturnCode l_rc = i2cUnlockEngine();
    if(l_rc != FAPI2_RC_SUCCESS)
    {
        SBE_ERROR(SBE_FUNC " failed for i2cUnlockEngine with l_rc 0x%08X", l_rc);
        if(rc == FAPI2_RC_SUCCESS)
            rc = l_rc;
    }

    SBE_EXIT(SBE_FUNC)
    return rc;
    #undef SBE_FUNC
}

void i2c::printi2cdetails()
{
    #define SBE_FUNC " printi2cdetails "
    SBE_ENTER(SBE_FUNC);

    SBE_INFO(SBE_FUNC "iv_port: %02X Engine: %02X DevAddr: %02x set.",
                    iv_port, iv_engine, iv_devAddr);
    SBE_INFO(SBE_FUNC "Bit rate Divisor 0x%08X ", iv_bit_rate_divisor);
    SBE_INFO(SBE_FUNC "Bus speed 0x%08X %08X", SBE::higher32BWord(iv_bus_speed),
                                        SBE::lower32BWord(iv_bus_speed));
    SBE_INFO(SBE_FUNC "Polling interval 0x%08X %08X", SBE::higher32BWord(iv_polling_interval_ns),
                                        SBE::lower32BWord(iv_polling_interval_ns));
    SBE_INFO(SBE_FUNC "Time out count 0x%08X %08X", SBE::higher32BWord(iv_timeout_count),
                                        SBE::lower32BWord(iv_timeout_count));

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

} //namespace fapi2