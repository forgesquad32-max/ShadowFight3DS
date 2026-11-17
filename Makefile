.SUFFIXES:

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. Install devkitPro and the 3DS development packages.")
endif

TARGET      := ShadowFight3DS
BUILD       := build
SOURCES     := source
INCLUDES    := include
DATA        := romfs
ROMFS       := romfs

APP_TITLE       := Shadow Fight 3DS
APP_DESCRIPTION := Original 2D shadow-fighting game
APP_AUTHOR      := Forge Squad

CFLAGS   := -g -Wall -Wextra -O2 -mword-relocations -ffunction-sections -fdata-sections $(addprefix -I,$(INCLUDES))
CXXFLAGS := $(CFLAGS) -std=gnu++17 -fno-rtti -fno-exceptions
LDFLAGS  := -specs=3dsx.specs -Wl,-Map,$(TARGET).map
LIBS     := -lcitro2d -lcitro3d -lctru -lm
LIBDIRS  := $(LIBCTRU) $(PORTLIBS)

.PHONY: all clean
all: $(TARGET).3dsx

# 3DS build rules are shipped with devkitARM, not at the devkitPro root.
include $(DEVKITPRO)/devkitARM/3ds_rules

# The 3DS rules default to the raw linker; use the C++ driver for this project.
LD := $(CXX)

clean:
	@rm -rf $(BUILD) $(TARGET).3dsx $(TARGET).elf $(TARGET).smdh $(TARGET).map
