# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/pklib/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2024
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

# dictionary import files as collection of groups where each group can be compiled
#   using same set of commands.
import_file_dict = {
    'pk_kernel' : {
        'dir'   : '/emb/generic/ppe/pk/kernel/',
        'type'  : 'c',
        'extra_flags' : [],
        'files' : [
            'pk_core.c',
            'pk_init.c',
            'pk_stack_init.c',
            'pk_bh_core.c',
            'pk_debug_ptrs.c',
            'pk_timer_core.c',
            'pk_timer_init.c',
            'pk_thread_init.c',
            'pk_thread_core.c',
            'pk_thread_util.c',
            'pk_semaphore_init.c',
            'pk_semaphore_core.c',
        ],
    },

    'ppe_base_c' : {
        'dir'   : '/emb/generic/ppe/baselib/',
        'type'  : 'c',
        'extra_flags' : [],
        'files' : [
            'ppe42_gcc.c',
            'ppe42_scom.c',
            'eabi.c',
            'ppe42_math.c'
        ],
    },

    'ppe_base_S' : {
        'dir'   : '/emb/generic/ppe/baselib/',
        'type'  : 'S',
        'extra_flags' : [],
        'files' : [
            'div64.S',
            'div32.S',
        ],
    },

    'ppe_base_c_opt' : {
        'dir'   : '/emb/generic/ppe/baselib/',
        'type'  : 'c',
        'extra_flags' : ['-O2 -falign-loops=32'],
        'files' : [
            'ppe42_string.c',
        ],
    },

    'ppe_trace' : {
        'dir'   : '/emb/generic/ppe/ppetrace/',
        'type'  : 'c',
        'extra_flags' : [],
        'files' : [
            'pk_trace_core.c',
            'pk_trace_big.c',
            'pk_trace_binary.c',
        ],
    },

    'bolton_lib' : {
        'dir'   : '/emb/generic/ppe/boltonlib/standard/',
        'type'  : 'c',
        'extra_flags' : [],
        'files' : [
            'std_init.c',
            'std_irq_init.c',
        ],
    },

    'ppe_42_S' : {
        'dir'   : '/emb/generic/ppe/pk/ppe42/',
        'type'  : 'S',
        'extra_flags' : [],
        'files' : [
            'ppe42_boot.S',
            'ppe42_exceptions.S',
            'ppe42_timebase.S',
            'ppe42_thread_init.S'
        ],
    },

    'ppe_42_c' : {
        'dir'   : '/emb/generic/ppe/pk/ppe42/',
        'type'  : 'c',
        'extra_flags' : [],
        'files' : [
            'ppe42_core.c',
            'ppe42_init.c',
            'ppe42_irq_core.c',
            'ppe42_irq_init.c'
        ],
    },
}

obj_files = []

foreach group_name, gdict : import_file_dict

    import_dir_path = pub_import_dir_path + gdict['dir']

    foreach file : gdict['files']
        if(gdict['type'] == 'S')
            assembly_preprocessed = custom_target(
                    output  : '@BASENAME@.s',
                    input   : import_dir_path + file,
                    command : [ cc,
                                cmn_abs_inc_paths, ekb_cmn_abs_inc_paths,
                                meson.get_external_property('cc_flags_cmn'),
                                meson.get_external_property('gcc_defs_cmn'),
                                meson.get_external_property('pre_processing_args'),
                                '-o','@OUTPUT@','@INPUT@'],
                    depfile : '@BASENAME@.d'
                )

            obj_files += custom_target(
                    output  : '@BASENAME@.o',
                    input   : assembly_preprocessed,
                    command : [ as,
                                '-mppe42x',
                                cmn_abs_inc_paths, ekb_cmn_abs_inc_paths,
                                '-o','@OUTPUT@','@INPUT@']
                )
        elif(gdict['type'] == 'c')
            obj_files += custom_target(
                        output  : '@BASENAME@.o',
                        input   : import_dir_path + file,
                        command : [
                            ppetracepp_wrap,
                            ppetracepp.full_path(),
                            meson.get_external_property('powerpc_eabi_gcc'),
                            meson.get_external_property('cc_flags_cmn'),
                            cmn_abs_inc_paths, ekb_cmn_abs_inc_paths,
                            meson.get_external_property('gcc_defs_cmn'),
                            '-o','@OUTPUT@','@INPUT@'
                            ],
                        depfile : '@BASENAME@.d',
                        depends : ppetracepp,
                    )
        else
            error('Invalid type ' + gdict['type'])
        endif
    endforeach

endforeach

lib_file_name = 'cmnpklib'

cmnpklib = custom_target(
                lib_file_name,
                output : 'lib' + lib_file_name,
                input : obj_files,
                command : [  ar,
                            'crs',
                            '@OUTPUT@'+'.a',
                            '@INPUT@'],
                build_by_default: true, # TODO: remove once it started using by the image
            )

cmn_pk_libs = '-l' + lib_file_name
cmn_pk_lib_paths = '-L' + meson.current_build_dir()
cmn_pk_dep_libs = cmnpklib