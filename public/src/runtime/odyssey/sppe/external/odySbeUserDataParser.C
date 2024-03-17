/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/external/odySbeUserDataParser.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2024                        */
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
#include <sys/stat.h>
#include <errno.h>

#include <cstdlib>
#include <cstring>
#include <sstream>

#include <utilfile.H>
#include <utilmem.H>

#include "sbeffdctypecommon.H"

#define ODY_SBE_TEMP_DUMP_FILE              "/tmp/ody_trace_dump.bin"
#define ODY_SBE_TRACE_BIN                   "/tmp/ody_sbe_trace.bin"
#define ODY_PPE2FSP_TOOL                    "ppe2fsp"

#define ODY_SBE_TRACE_CMD                   "fsp-trace -s"
#define ODY_SBE_TRACE_HEADLINE              "SBE Traces"
#define ODY_SBE_STRING_FILE                 "odysseySppeStringFile_DD1"

#define ODY_SBE_PARSER_TRACE_HEADER_SIZE    56
#define ODY_SBE_PARSER_MAX_CIRC_BUFFER_SIZE 8192
#define ODY_SBE_PARSER_MAX_LOCAL_BUFFER     ODY_SBE_PARSER_TRACE_HEADER_SIZE + \
                                            ODY_SBE_PARSER_MAX_CIRC_BUFFER_SIZE

#define ODY_SBE_PARSER_PRINT_DELIMITER {std::cout << \
"*****************************************************************************"\
<< std::endl;}
#define ODY_SBE_PARSER_PRINT_HEADING(x) { SBE_PARSER_PRINT_DELIMITER \
std::cout << \
"                             " x<< std::endl;\
ODY_SBE_PARSER_PRINT_DELIMITER }

namespace Ody
{
    std::string findSbeFile(const char *name)
    {
        std::string tmp = getenv("PATH");
    #ifdef CONTEXT_x86_nfp
        char *bb = getenv("bb");
        if(bb != NULL)
        {
            std::string bbstr(bb);
            tmp += ":" + bbstr + "/src/sbei/sbfw/img" +
                   ":" + bbstr + "/obj/x86.nfp/sbei/sbfw/simics" +
                   ":" + bbstr + "/obj/ppc/hwsv/server/buildhwpfimport/hwpf2/tools/x86_binaries";
        }
    #elif defined CONTEXT_ppc
        tmp += ":/nfs:/maint";
    #endif
        char *path = strdup(tmp.c_str());
        char *dir = NULL;

        std::string file;

        struct  stat    l_stat;

        for(dir = strtok( path, ":" ); dir; dir = strtok(NULL, ":"))
        {
            file = file + dir + "/" +  name;

            if(stat(file.c_str(), &l_stat) < 0)
            {
                // String file not found, go to next one
                file.clear();
            }
            else
            {
                // update where trexStringFile is
                break;
            }
        }

        free(path);
        path = NULL;        //sm05c

        return file;
    }

    void sbeParserSysCall(const char *cmd)
    {
        FILE *stream;
        char buffer[256];

        stream = popen(cmd, "r" );
        if(stream)
        {
            while(!feof(stream))
            {
                if(fgets(buffer, 256, stream) != NULL)
                {
                    std::cout << buffer;
                }
            }
        }
        else
        {
            std::cout << "command failed :[" << cmd << "]" << std::endl;
            std::cout << "errno [" << errno << "]" << std::endl;
        }
    }
};

int parseOdySbeFFDC(ErrlUsrParser & i_parser, const void * i_pBuffer,
                    const uint32_t i_buflen)
{
    int l_rc = 0;
    uint32_t fapiRc = 0;
    sbePlatHeader_t l_pData = {0};
    char l_buffer[ODY_SBE_PARSER_MAX_LOCAL_BUFFER] = {0};
    char *l_pBuffer = (char*)i_pBuffer;
    uint32_t l_buflen = i_buflen;

    do
    {
        if(i_pBuffer == NULL)
        {
            l_rc = -1;
            break;
        }
        //seek l_mem to the binary blob of FFDC package
        UtilMem l_mem(const_cast<void*>(i_pBuffer),i_buflen);

        // The data is a buffer of SBE FFDC data
        i_parser.PrintHeading("SBE FFDC Parser");

        if(l_buflen < sizeof(fapiRc))
        {
            i_parser.PrintHexDump(l_pBuffer, l_buflen);
            break;
        }
        l_buflen -= sizeof(fapiRc);
        l_pBuffer += sizeof(fapiRc);
        l_mem >> fapiRc;
        i_parser.PrintNumber("FAPI RC ", "0x%08X", ntohl(fapiRc));

        if(l_buflen < sizeof(l_pData))
        {
            i_parser.PrintHexDump(l_pBuffer, l_buflen);
            break;
        }
        l_buflen -= sizeof(l_pData);
        l_pBuffer += sizeof(l_pData);
        l_mem >> l_pData;

        i_parser.PrintNumber("Primary Status ", "0x%04X",
                (uint16_t)ntohs(l_pData.priRc));
        i_parser.PrintNumber("Secondary Status ", "0x%04X",
                (uint16_t)ntohs(l_pData.secRc));
        i_parser.PrintNumber("FW Commit ID ", "0x%08X",
                (uint32_t)ntohl(l_pData.fwCommitId));
        i_parser.PrintNumber("DD Major ", "0x%02X",
                l_pData.ddMajor);
        i_parser.PrintNumber("DD Minor", "0x%02X",
                l_pData.ddMinor);
        i_parser.PrintNumber("Thread Id", "0x%02X",
                l_pData.threadId);

        //loop through the number of fields configured
        uint32_t l_data = 0;

        if(l_buflen < sizeof(l_data))
        {
            i_parser.PrintHexDump(l_pBuffer, l_buflen);
            break;
        }
        l_buflen -= sizeof(l_data);
        l_pBuffer += sizeof(l_data);

        l_mem >> l_data;
        uint32_t l_dumpFields = ntohl(l_data);
        while(l_dumpFields && !l_rc)
        {
            if(l_dumpFields & SBE_FFDC_ALL_PLAT_DATA)
            {
                if(l_buflen < sizeof(uint32_t))
                {
                    //Complete this loop and let the tools print as much data
                    //as possible but break from next loop
                    l_rc = -1;
                }
                packageBlobField_t l_ffdcUserDataId = {0};
                l_buflen -= sizeof(uint32_t); //l_ffdcUserDataId
                l_pBuffer += sizeof(uint32_t); //l_ffdcUserDataLength

                l_mem >> l_ffdcUserDataId;

                uint16_t fieldId = ntohs(l_ffdcUserDataId.fieldId);
                uint16_t fieldLen = ntohs(l_ffdcUserDataId.fieldLen);

                if((l_buflen < fieldLen) &&
                    (fieldId != SBE_FFDC_TRACE_DATA))
                {
                    i_parser.PrintHexDump(l_pBuffer, l_buflen);
                    break;
                }

                l_buflen -= fieldLen;
                l_pBuffer += fieldLen;

                l_mem.read(l_buffer, fieldLen);
                std::ostringstream  l_strFile;
                //Generate temp dump file name
                l_strFile << ODY_SBE_TEMP_DUMP_FILE;

                //Write dump into the temporary file
                UtilFile l_fileObj(l_strFile.str().c_str());
                errlHndl_t l_errlHndl = l_fileObj.open("w");
                if ( l_errlHndl )
                {
                    std::cerr << "Error opening "
                              << l_strFile.str() << std::endl;
                    l_errlHndl->commit(HWSV_COMP_ID, ERRL_ACTION_REPORT);
                    delete l_errlHndl;
                    l_errlHndl = NULL;
                    i_parser.PrintHexDump(l_buffer, fieldLen);
                    break;
                }
                else
                {
                    l_fileObj.write( l_buffer, fieldLen);
                    l_fileObj.Close();
                }

                if(fieldId == SBE_FFDC_TRACE_DATA)
                {
                    ODY_SBE_PARSER_PRINT_HEADING(ODY_SBE_TRACE_HEADLINE)
                    //command
                    std::ostringstream  l_strCmd1, l_strCmd2;
                    // ppe2fsp <trace dump file> <trace bin file> 2>&1
                    l_strCmd1 << Ody::findSbeFile(PPE2FSP_TOOL)
                              << " "
                              << l_strFile.str().c_str()
                              << " "
                              << ODY_SBE_TRACE_BIN
                              << " "
                              << "2>&1";

                    // fsp-trace -s <sbe string file> <trace bin file> 2>&1
                    l_strCmd2 << ODY_SBE_TRACE_CMD
                              << " "
                              << Ody::findSbeFile(ODY_SBE_STRING_FILE)
                              << " "
                              << SBE_TRACE_BIN
                              << " "
                              << "2>&1";

                    //Call out the commands
                    Ody::sbeParserSysCall( l_strCmd1.str().c_str() );
                    Ody::sbeParserSysCall( l_strCmd2.str().c_str() );
                }

                //Delete the temp file
                l_fileObj.Remove();
            }
            l_dumpFields >>= 1;
            if(l_rc != 0)
            {
                break;
            }
        }
    } while(false);

    return l_rc;
}
