# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/odyssey/security/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2024
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG

############## Build Required Runtime Genfiles ###############################
import_ody_security_dir_base_path = pub_import_dir_path + '/common/odyssey/security/'
odyssey_security_allow_deny_list = 'Odyssey_Security_Allow_Deny_list.csv'

odyssey_abs_security_file = import_ody_security_dir_base_path + odyssey_security_allow_deny_list

run_result = run_command(securityRegListGen,'-f', odyssey_abs_security_file,
            '-o', odyssey_sppe_genfiles_dir_path,
            # we can keep -v options to get the backtrace if it failed.
            '-v')

if (run_result.returncode() != 0)
    error(run_result.stderr())
    message(run_result.stdout())
endif
