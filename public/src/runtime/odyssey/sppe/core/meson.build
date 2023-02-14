# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/odyssey/sppe/core/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2023
# [+] International Business Machines Corp.
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

ody_core_dir_path = meson.global_source_root() + '/public/src/runtime/odyssey/sppe/core/'

infile_c = ['main.C',
            'initthreads.C',
            'chipop_handler.C',
            'sbecmdreceiver.C',
            'sbecmdprocessor.C',
            'ipl.C',
            'ipl_table.C',
            'istep.C',
            'sbecmdscomaccess.C',
            'sbescom.C',
            'sbecmdringaccess.C',
            'sbecmdbist.C',
            'sbecmdgetrawattrdump.C',
            'sbecmdpak.C',
            'sbecmdexecutehwp.C',
            'hwpWrapper.C',
            'sbestreampaktohwp.C',
            'codeupdateutils.C',
            'sbecmdupdateattr.C',
            'sbecmddtsread.C',
            'sbecmdstopclockswrapper.C',
            'sberegaccessplat.C',
          ]

odyssey_abs_core_c_file_paths = []
foreach infile : infile_c

  abs_infile_c_path = ody_core_dir_path + infile
  odyssey_abs_core_c_file_paths += abs_infile_c_path

endforeach

odyssey_abs_core_c_file_paths += [ abs_pak_c_path,
                                    abs_cmn_core_c_path,
                                    abs_cmn_utils_c_path,
                                    abs_runtime_cmn_core_c_path,
                                    abs_runtime_cmn_ffdc_c_path,
                                    abs_runtime_cmn_attribute_c_path,
                                    abs_runtime_cmn_hwpf_c_path,
                                    abs_runtime_cmn_utils_c_path,
                                    abs_runtime_cmn_code_update_c_path,
                                    abs_runtime_cmn_stopclock_c_path,
                                    odyssey_abs_hwpf_c_file_paths,
                                    odyssey_abs_tgt_c_file_paths,
                                    odyssey_abs_attr_c_file_paths
           ]

# List of image specific customize files
#These functions vary for PNOR and Pibmem
#Odyssey Lab PNOR will still use same list of files
odyssey_abs_cust_img_c_file_paths = [
                        files('imgcustomize.C',)
]

# List of image specific customize files
#These functions vary for product and lab
#Odysseylab will have its own implementation of belwow list of files
#for both PNOR and Pibmem image
odyssey_img_cust_c_file = [
                  'atefw.C',
]

ODYSSEY_CHIPOPS_JSON = files('chipop_table.json')[0]

ody_cmn_chipop_json_list = [
                    COMMON_CHIPOPS_JSON,
                    CMN_STOP_CLOCK_CHIPOPS_JSON,
                    CMN_CODE_UPDATE_CHIPOPS_JSON]

ody_chipop_json_list = [ody_cmn_chipop_json_list,
                        ODYSSEY_CHIPOPS_JSON]

#We can break off at this point if its a lab image as lab would do below stuff in internal
if platform == 'odysseylab'
  subdir_done()
endif

############### Chip-op table generation ############
result_chipoptablegen = run_command(chipoptablegen,
                        '-d', odyssey_sppe_genfiles_dir_path,
                        '-c', ody_chipop_json_list)

if not (result_chipoptablegen.returncode() == 0)
  message(result_chipoptablegen.stdout())
  error(result_chipoptablegen.stderr())
endif

#####################################################

lib_file_name = 'odysseysppe'

#TODO: JIRA PFSBE-283: Generator enhancement
#NOTE: Add all genfiles directly below as lab images use different genfils.
#      Adding genfiles in odyssey_abs_core_c_file_paths will cause lab image fails
#      We need to add odyssey_abs_cust_img_c_file_paths directly in here as lab may use
#      different set of files for SPPE pibmem image
odyssey_sppe_obj_file_cpp = obj_gen_cpp.process([odyssey_abs_core_c_file_paths,
                                                 odyssey_abs_genfiles_c_file_paths,
                                                 odyssey_abs_cust_img_c_file_paths,
                                                 odyssey_img_cust_c_file
                                                ],
                                        extra_args : [
                                        meson.get_external_property('gcc_defs_cmn_odyssey'),
                                        meson.get_external_property('gcc_defs_odyssey_sppe'),
                                        odyssey_runtime_abs_inc_paths,
                                        odyssey_cmn_abs_inc_paths,
                                        runtime_cmn_abs_inc_paths,
                                        odyssey_hwp_abs_inc_paths,
                                        ekb_hwp_abs_inc_paths,
                                        runtime_cmn_abs_code_update_inc_paths])

# tinflate.C must be compiled with -O2 -falign-loops=32 for significantly improved decompression performance
#   -O2 enables a few additional speed optimizations
#   -falign-loops=32 is _crucial_ to performance since the hottest loop in decompression is
#      tiny enough to fit within the PPE's 32-byte instruction buffer, so if we align the loop the PPE
#      can execute it without fetching instructions from PIBMEM, creating a massive speedup.
#      (we're talking a factor of 1.5 to 2 here!)
odyssey_sppe_obj_file_cpp_opt = obj_gen_cpp.process(abs_tinflate_c_path,
                                                    extra_args: ['-O2 -falign-loops=32',
                                                    odyssey_cmn_abs_inc_paths])

#compile common HWP
generic_hwp_obj_file_c = obj_gen_cpp_hwp.process([ekb_generic_hwp_c_path,
                                                  ekb_generic_ring_c_path,
                                                  odyssey_hwp_abs_infiles_c_path],
                                        extra_args:[
                                                    odyssey_runtime_abs_inc_paths,
                                                    runtime_cmn_abs_inc_paths,
                                                    odyssey_cmn_abs_inc_paths,
                                                    ekb_hwp_abs_inc_paths,
                                                    odyssey_hwp_abs_inc_paths])

infile  = [odyssey_sppe_obj_file_cpp, odyssey_sppe_obj_file_cpp_opt,
           generic_hwp_obj_file_c]

libodysseysppe = custom_target( lib_file_name,
                                 output : 'lib' + lib_file_name,
                                 input : infile,
                                 command : [ ar,
                                             'crs',
                                             '@OUTPUT@'+'.a',
                                             '@INPUT@'],
                                 build_by_default: true,
                             )

odyssey_sppe_libs += '-l' + lib_file_name
odyssey_sppe_lib_paths += '-L' + meson.current_build_dir()
odyssey_sppe_dep_libs += libodysseysppe
