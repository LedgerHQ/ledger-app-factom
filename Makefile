#*******************************************************************************
#   Ledger App
#   (c) 2017 Ledger
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#*******************************************************************************

ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif
include $(BOLOS_SDK)/Makefile.defines

APPNAME = Factom
APP_LOAD_PARAMS=--appFlags 0x40 --path "44'/131'" --path "44'/132'" --path "44'/143165576'"  --curve secp256k1 $(COMMON_LOAD_PARAMS) 

APPVERSION_M=1
APPVERSION_N=2
APPVERSION_P=0
APPVERSION=$(APPVERSION_M).$(APPVERSION_N).$(APPVERSION_P)
DEFINES   += UNUSED\(x\)=\(void\)x
DEFINES   += APPVERSION=\"$(APPVERSION)\"

#prepare hsm generation
ifeq ($(TARGET_NAME),TARGET_BLUE)
ICONNAME=images/app_mfw.gif
else
ICONNAME=images/factom_nano.gif
endif


################
# Default rule #
################
all: default

############
# Platform #
############

DEFINES   += OS_IO_SEPROXYHAL IO_SEPROXYHAL_BUFFER_SIZE_B=128
DEFINES   += HAVE_BAGL HAVE_SPRINTF
#DEFINES   += HAVE_PRINTF PRINTF=screen_printf
DEFINES   += PRINTF\(...\)=
DEFINES   += HAVE_IO_USB HAVE_L4_USBLIB IO_USB_MAX_ENDPOINTS=6 IO_HID_EP_LENGTH=64 HAVE_USB_APDU
DEFINES   +=  LEDGER_MAJOR_VERSION=$(APPVERSION_M) LEDGER_MINOR_VERSION=$(APPVERSION_N) LEDGER_PATCH_VERSION=$(APPVERSION_P)


# U2F
DEFINES   += HAVE_U2F HAVE_IO_U2F
DEFINES   += USB_SEGMENT_SIZE=64
DEFINES   += BLE_SEGMENT_SIZE=32 #max MTU, min 20
DEFINES   += U2F_PROXY_MAGIC=\"TFA\"

##############
#  Compiler  #
##############
#GCCPATH   := $(BOLOS_ENV)/gcc-arm-none-eabi-5_3-2016q1/bin/
#CLANGPATH := $(BOLOS_ENV)/clang-arm-fropi/bin/
CC       := $(CLANGPATH)clang 

#CFLAGS   += -O0
CFLAGS   += -O3 -Os -I$(GCCPATH)/../arm-none-eabi/include/

AS     := $(GCCPATH)arm-none-eabi-gcc
LD       := $(GCCPATH)arm-none-eabi-gcc
LDFLAGS  += -O3 -Os
LDLIBS   += -lm -lgcc -lc 

# import rules to compile glyphs(/pone)
include $(BOLOS_SDK)/Makefile.glyphs

### computed variables
APP_SOURCE_PATH  += src  
SDK_SOURCE_PATH  += lib_stusb lib_stusb_impl lib_u2f


load: all
	python -m ledgerblue.loadApp $(APP_LOAD_PARAMS)

delete:
	python -m ledgerblue.deleteApp $(COMMON_DELETE_PARAMS)

# import generic rules from the sdk
include $(BOLOS_SDK)/Makefile.rules

#add dependency on custom makefile filename
dep/%.d: %.c Makefile.genericwallet

