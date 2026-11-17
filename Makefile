.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. Install devkitPro and the 3DS development packages.")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

# This is a C++ project: use the compiler driver for linking, not raw ld.
LD := $(CXX)

TARGET      := ShadowFight3DS
BUILD       := build
SOURCES     := source
INCLUDES    := include
DATA        := romfs
ROMFS       := romfs

APP_TITLE       := Shadow Fight 3DS
APP_DESCRIPTION := Original 2D shadow-fighting game
APP_AUTHOR      := Forge Squad

# Nintendo 3DS ARM11 target settings from the official devkitPro template.
ARCH := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

# libctru is exposed by 3ds_rules as CTRULIB; 3DS port libraries use PORTLIBS.
# Both directories must be added explicitly to the compile and link search paths.
LIBDIRS  := $(CTRULIB) $(PORTLIBS)
INCLUDE  := $(addprefix -I,$(INCLUDES)) $(foreach dir,$(LIBDIRS),-I$(dir)/include)
LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

CFLAGS   := -g -Wall -Wextra -O2 -mword-relocations -ffunction-sections -fdata-sections $(ARCH) $(INCLUDE) -D__3DS__
CXXFLAGS := $(CFLAGS) -std=gnu++17 -fno-rtti -fno-exceptions
ASFLAGS  := -g $(ARCH)
LDFLAGS  := -specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(TARGET).map
LIBS     := -lcitro2d -lcitro3d -lctru -lm

.PHONY: all clean
all: $(TARGET).3dsx

clean:
	@rm -rf $(BUILD) $(TARGET).3dsx $(TARGET).elf $(TARGET).smdh $(TARGET).map
