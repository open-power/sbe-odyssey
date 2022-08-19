# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/emb/odyssey/io/ioo_memregs/odyssey_ioo_memregs.mk $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022
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
GENERATED=odyssey_ioo_memregs
$(GENERATED)_COMMAND_PATH=$(ROOTPATH)/public/emb/odyssey/io/common/
COMMAND=memregs
SOURCES+=$(ROOTPATH)/public/emb/odyssey/io/ioo_memregs/generic_reg_attribute_ioo.txt
SOURCES+=$(ROOTPATH)/public/emb/odyssey/io/ioo_memregs/ppe_reg_attribute_ioo.txt
SOURCES+=$(ROOTPATH)/public/emb/odyssey/io/ioo_memregs/fw_reg_attribute_ioo.txt
SOURCES+=$(ROOTPATH)/public/emb/odyssey/io/ioo_memregs/img_reg_attribute_ioo.txt

OUTPUT_DIR=$(IMAGEPATH)/$(GENERATED)
OUTPUT_IMG=$(OUTPUT_DIR)/$(GENERATED).bin

TARGETS+=$(OUTPUT_IMG)

define $(GENERATED)_RUN
		$(C1) mkdir -p $(OUTPUT_DIR)
		$(C1) $$< --image-type="ioo" --output-image=$(OUTPUT_IMG) $$(filter-out $$<,$$^)
endef

$(call BUILD_GENERATED)
