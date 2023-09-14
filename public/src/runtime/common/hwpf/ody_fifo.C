/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/hwpf/ody_fifo.C $                   */
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
//------------------------------------------------------------------------------
/// @file  ody_fifo.C

#include <fapi2.H>
#include <ody_fifo.H>

#include "sbeFifoMsgUtils.H"
//#include "sbeFFDC.H"
#include "sberegaccess.H"
#include "sbestates.H"
#include "sbeglobals.H"
//#include "sbeXipUtils.H"
#include "chipop_handler.H"

fapi2::ReturnCode waitUsFifoReady( const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target_chip){
  fapi2::buffer<uint32_t> statusReg;
  while(1){
    //Verify that Fifo is not full, if bit10 is cleared Fifo is not full
    FAPI_TRY(fapi2::getCfamRegister(i_target_chip, 0x2401, statusReg));
    if(!statusReg.getBit(10))
      break;
  }

  fapi_try_exit:
  return fapi2::current_err;
}

fapi2::ReturnCode writeUsFifo( const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target_chip, uint32_t data){
  waitUsFifoReady(i_target_chip);
  FAPI_TRY(fapi2::putCfamRegister(i_target_chip, 0x2400, data));
  fapi_try_exit:
  return fapi2::current_err;
}

fapi2::ReturnCode signalEOT(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target_chip){
  waitUsFifoReady(i_target_chip);
  FAPI_TRY(fapi2::putCfamRegister(i_target_chip, 0x2402, 0x00000001));
  fapi_try_exit:
  return fapi2::current_err;
}

fapi2::ReturnCode waitDsFifoReady(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target_chip){
  fapi2::buffer<uint32_t> statusReg;
  while(1){
    //Verify that Fifo is not empty, bit 12-15 indicate how many words are available
    FAPI_TRY(fapi2::getCfamRegister(i_target_chip, 0x2411, statusReg));
    if(statusReg&0x000F0000) //wait for an non empty Fifo
      break;
  }

  fapi_try_exit:
  return fapi2::current_err;
}

fapi2::ReturnCode readDsFifo(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target_chip, uint32_t& o_data){
  fapi2::buffer<uint32_t> data;
  FAPI_TRY(waitDsFifoReady(i_target_chip));
  FAPI_TRY(fapi2::getCfamRegister(i_target_chip, 0x2410, data));
  o_data=data;
  fapi_try_exit:
  return fapi2::current_err;
}

fapi2::ReturnCode ackDsEot(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target_chip){
  fapi2::buffer<uint32_t> statusReg;
  while(1){
    //Verify that Fifo is not full, if bit10 is cleared Fifo is not full
    FAPI_TRY(fapi2::getCfamRegister(i_target_chip, 0x2411, statusReg));
    if(statusReg.getBit(24))
      break;
  }
  uint32_t dummy;
  readDsFifo(i_target_chip, dummy);
  fapi_try_exit:
  return fapi2::current_err;
}


fapi2::ReturnCode push_data_to_ody(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target, const uint32_t *i_data, size_t word_count, uint32_t *o_data, size_t max_reply){
  fapi2::buffer<uint32_t> statusReg;
  uint32_t replyCounter=0;
  uint32_t word;
  while(word_count>0){
    FAPI_TRY(writeUsFifo(i_target,*i_data));
    word_count--;
    i_data++;
  }
  signalEOT(i_target);

  while(1){
    readDsFifo(i_target, word);
    FAPI_TRY(fapi2::getCfamRegister(i_target, 0x2411, statusReg));
    if(statusReg.getBit(8)){ //EOT
      FAPI_TRY(fapi2::putCfamRegister(i_target, 0x2415, 0)); //ACK the EOT
      break;
    }
    if(replyCounter<max_reply)
      o_data[replyCounter]=word;
    replyCounter++;
  }
  fapi_try_exit:
  return fapi2::current_err;
}

fapi2::ReturnCode push_data_to_ody_eot(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target, const uint32_t *i_data, size_t word_count){
  fapi2::buffer<uint32_t> statusReg;
  while(word_count>0){
    FAPI_TRY(writeUsFifo(i_target,*i_data));
    word_count--;
    i_data++;
  }
  signalEOT(i_target);


  fapi_try_exit:
  return fapi2::current_err;
}

fapi2::ReturnCode fetch_ody_reply(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target, uint32_t *o_data, size_t max_reply){
  fapi2::buffer<uint32_t> statusReg;
  uint32_t replyCounter=0;
  uint32_t word;

  while(1){
    readDsFifo(i_target, word);
    FAPI_TRY(fapi2::getCfamRegister(i_target, 0x2411, statusReg));
    if(statusReg.getBit(8)){ //EOT
      FAPI_TRY(fapi2::putCfamRegister(i_target, 0x2415, 0)); //ACK the EOT
      break;
    }
    if(replyCounter<max_reply)
      o_data[replyCounter]=word;
    replyCounter++;
  }
  fapi_try_exit:
  return fapi2::current_err;
}


fapi2::ReturnCode ody_isetp(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target, uint8_t i_class, uint8_t hwp_num){
  uint32_t hwp[]={03, 0xa104,((uint32_t)i_class)<<16|hwp_num};
  FAPI_TRY(push_data_to_ody_eot(i_target, hwp, sizeof(hwp)/sizeof(uint32_t)));

  fapi_try_exit:
  return fapi2::current_err;
}

fapi2::ReturnCode ody_isetp_reply(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target){
  uint32_t reply[8];

  fetch_ody_reply(i_target, reply,sizeof(reply)/sizeof(uint32_t));
  if((reply[0]==0xc0dea104)&&(reply[1]==0)){
    FAPI_INF("IStep done");
  }
  else{
    FAPI_ERR("IStep failed: %08x", reply[1]);
    fapi2::current_err=reply[1]>>16;
    fapi2::current_err=fapi2::FAPI2_RC_FALSE;
  }

  fapi_try_exit:
  return fapi2::current_err;
}

uint32_t streamFifo(uint8_t *i_pArg){
  uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
  uint32_t len = 0;
  fapi2::buffer<uint32_t> statusReg;
  sbeFifoType type;

  do {

    fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP> i_proc_chip = g_platTarget->plat_getChipTarget();
    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    type = static_cast<sbeFifoType>(configStr->fifoType);
    fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP> i_target;

    uint32_t channel, words;
    len=1;
    l_rc = sbeUpFifoDeq_mult (len, (uint32_t *)&channel, false, false, type);
    if(l_rc) break;
    l_rc = sbeUpFifoDeq_mult (len, (uint32_t *)&words, false, false, type);
    if(l_rc) break;


    for (auto & l_c : i_proc_chip.template getChildren<fapi2::TARGET_TYPE_OCMB_CHIP>()){
      uint8_t chipPos;
      FAPI_ATTR_GET(fapi2::ATTR_BUS_POS, l_c, chipPos);
      if(chipPos==channel){
        i_target=l_c;

        break;
      }
    }
    FAPI_INF("Pushing %d data", words)
    FAPI_TRY(writeUsFifo(i_target,words));
    words--;
    while(words>0){
      uint32_t data;
      if(words==1) l_rc = sbeUpFifoDeq_mult (len, (uint32_t *)&data, true, false, type);
      else l_rc = sbeUpFifoDeq_mult (len, (uint32_t *)&data, false, false, type);
      words--;
      FAPI_TRY(writeUsFifo(i_target,data));
    }
    signalEOT(i_target);
    break;
  }while(1);

  fapi_try_exit:

  FAPI_INF("streamFifo done");
  return l_rc;
}

uint32_t streamFifoReply(uint8_t *i_pArg){
  uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
  uint32_t len = 0, words = 0;
  fapi2::buffer<uint32_t> statusReg;
  sbeFifoType type;

  do {

    fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP> i_proc_chip = g_platTarget->plat_getChipTarget();
    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    type = static_cast<sbeFifoType>(configStr->fifoType);
    fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP> i_target;

    uint32_t channel;
    len=1;
    l_rc = sbeUpFifoDeq_mult (len, (uint32_t *)&channel, true, false, type);
    if(l_rc) break;


    for (auto & l_c : i_proc_chip.template getChildren<fapi2::TARGET_TYPE_OCMB_CHIP>()){
      uint8_t chipPos;
      FAPI_ATTR_GET(fapi2::ATTR_BUS_POS, l_c, chipPos);
      if(chipPos==channel){
        i_target=l_c;

        break;
      }
    }

    words=0;
    while(1){
      uint32_t word;
      readDsFifo(i_target, word);
      words++;
      len=1;
      sbeDownFifoEnq_mult (len,&word, type);
      FAPI_TRY(fapi2::getCfamRegister(i_target, 0x2411, statusReg));
      if(statusReg.getBit(8)){ //EOT
        FAPI_TRY(fapi2::putCfamRegister(i_target, 0x2415, 0)); //ACK the EOT
        break;
      }
    }
    FAPI_INF("Fetched words: %08x", words)
    break;
  }while(1);

  fapi_try_exit:

  FAPI_INF("streamFifo done");
  return l_rc;
}

