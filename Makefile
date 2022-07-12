############################################################################
# User configurables
############################################################################

PROJ_NAME = main

# Set toolchain
TC = /home/varcain/tools/toolchains/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi

# Set STM32 Cube path
STM32CUBE_PATH = STM32Cube_FW_F7_V1.7.0

# UGFX setup
UGFX_PATH = ugfx_2.5
GFXLIB = $(UGFX_PATH)
include $(UGFX_PATH)/gfx.mk
UGFX_INCLUDES := $(foreach dir, $(GFXINC), $(addprefix -I, $(dir)))

# Add individual .c files
C_SRCS := \
$(GFXSRC) \
$(STM32CUBE_PATH)/Middlewares/Third_Party/FatFs/src/option/ccsbcs.c \
$(STM32CUBE_PATH)/Middlewares/Third_Party/FatFs/src/drivers/sd_diskio.c \
$(STM32CUBE_PATH)/Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.c \
$(STM32CUBE_PATH)/Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sd.c \

# Add full source dirs (all contained .c and .s files will be added)
SRC_DIRS = \
$(STM32CUBE_PATH)/Middlewares/Third_Party/FreeRTOS/Source \
$(STM32CUBE_PATH)/Drivers/CMSIS/DSP_Lib/Source/BasicMathFunctions \
$(STM32CUBE_PATH)/Drivers/CMSIS/DSP_Lib/Source/CommonTables \
$(STM32CUBE_PATH)/Drivers/CMSIS/DSP_Lib/Source/ComplexMathFunctions \
$(STM32CUBE_PATH)/Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions \
$(STM32CUBE_PATH)/Drivers/CMSIS/DSP_Lib/Source/FastMathFunctions \
$(STM32CUBE_PATH)/Drivers/CMSIS/DSP_Lib/Source/FilteringFunctions \
$(STM32CUBE_PATH)/Drivers/CMSIS/DSP_Lib/Source/MatrixFunctions \
$(STM32CUBE_PATH)/Drivers/CMSIS/DSP_Lib/Source/StatisticsFunctions \
$(STM32CUBE_PATH)/Drivers/CMSIS/DSP_Lib/Source/SupportFunctions \
$(STM32CUBE_PATH)/Drivers/CMSIS/DSP_Lib/Source/TransformFunctions \
$(STM32CUBE_PATH)/Utilities/CPU/ \
$(STM32CUBE_PATH)/Middlewares/Third_Party/FatFs/src \
$(STM32CUBE_PATH)/Drivers/STM32F7xx_HAL_Driver/Src \
$(STM32CUBE_PATH)/Drivers/BSP/Components/wm8994 \
$(STM32CUBE_PATH)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 \
src \
src/stm32f7 \
src/ugfx \

# Filter out specific .c files (use to exclude specific .c files from above)
FILTER_ENTRIES := \
$(STM32CUBE_PATH)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_timebase_rtc_alarm_template.c \
$(STM32CUBE_PATH)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_timebase_rtc_wakeup_template.c \
$(STM32CUBE_PATH)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_timebase_tim_template.c \

# Set Include Paths
INCLUDES	+= \
-I$(STM32CUBE_PATH)/Drivers/CMSIS/Include/ \
-I$(STM32CUBE_PATH)/Middlewares/Third_Party/FreeRTOS/Source/include \
-I$(STM32CUBE_PATH)/Utilities/CPU \
-I$(STM32CUBE_PATH)/Middlewares/Third_Party/FatFs/src \
-I$(STM32CUBE_PATH)/Middlewares/Third_Party/FatFs/src/drivers \
-Iinc \
-Iinc/ugfx \
$(UGFX_INCLUDES) \
-I$(STM32CUBE_PATH)/Drivers/CMSIS/Device/ST/STM32F7xx/Include/ \
-I$(STM32CUBE_PATH)/Drivers/STM32F7xx_HAL_Driver/Inc \
-I$(STM32CUBE_PATH)/Drivers/BSP/STM32746G-Discovery \
-I$(STM32CUBE_PATH)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1  \
-I$(STM32CUBE_PATH)/Drivers/BSP/Components/wm8994 \
-Iinc/stm32f7

# Set Board
DEFINES 	= -D__FPU_PRESENT -DSTM32F746xx -DUSE_HAL_DRIVER -DARM_MATH_CM7
CPU_FLAGS	= -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16
LD_SCRIPT	= STM32F746NGHx_FLASH.ld

USER_CFLAGS	= -g -O0 -ffunction-sections -fdata-sections -ffast-math -fsingle-precision-constant

# Set Libraries
LIBS		= -lm

FLASH_START_ADDR = 0x0

############################################################################
# Internal (usually do not edit)
############################################################################

# Output paths
OBJ_DIR		= obj
OUT_DIR		= out

PROJ_NAME := $(addprefix $(OUT_DIR)/, $(PROJ_NAME))

# Set Tools
CC	= $(TC)-gcc
LD	= $(TC)-ld
AR	= $(TC)-ar
OBJCOPY	= $(TC)-objcopy
OBJDUMP	= $(TC)-objdump
SIZE	= $(TC)-size

# Set Compilation and Linking Flags
CFLAGS 		= $(DEFINES) $(INCLUDES) \
			-Wall -Wno-missing-braces -std=c99 \
			$(CPU_FLAGS) $(USER_CFLAGS)

ASFLAGS 	= -g -Wa,--warn -x assembler-with-cpp $(CPU_FLAGS)

LDFLAGS 	= -g -T$(LD_SCRIPT) $(LIBS) $(CPU_FLAGS) \
			-Xlinker --gc-sections \
			-Wl,-Map,$(PROJ_NAME).map \
			-Wl,--undefined=uxTopUsedPriority \


C_SRCS_DIRS = $(sort $(dir $(C_SRCS)))

C_SRCS += $(foreach dir, $(SRC_DIRS),$(wildcard $(dir)/*.c))
S_SRCS := $(foreach dir, $(SRC_DIRS),$(wildcard $(dir)/*.s))

C_SRCS := $(filter-out $(FILTER_ENTRIES),$(C_SRCS))

# Dir fixup
SRC_DIRS += $(C_SRCS_DIRS)

OBJS := $(foreach src, $(C_SRCS),$(addprefix $(OBJ_DIR)/,$(notdir $(C_SRCS:.c=.o))))
OBJS += $(foreach src, $(S_SRCS),$(addprefix $(OBJ_DIR)/,$(notdir $(S_SRCS:.s=.o))))

all: mkdir $(PROJ_NAME).hex $(PROJ_NAME).lst info

$(PROJ_NAME).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo $@

$(PROJ_NAME).bin: $(PROJ_NAME).elf
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin
	@echo $@

$(PROJ_NAME).hex: $(PROJ_NAME).elf
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	@echo $@

$(PROJ_NAME).lst: $(PROJ_NAME).elf
	@$(OBJDUMP) -h -S $(PROJ_NAME).elf > $(PROJ_NAME).lst
	@echo $@

info: $(PROJ_NAME).elf
	@$(SIZE) --format=berkeley $(PROJ_NAME).elf

vpath %.c $(SRC_DIRS)
vpath %.s $(SRC_DIRS)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo $@

$(OBJ_DIR)/%.o: %.s
	$(CC) $(ASFLAGS) -c -o $@ $<
	@echo $@

mkdir:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OUT_DIR)

# debug tool to print variables
#print-%  : ; @echo $* = $($*)

flash: all
	openocd \
        -f board/stm32f7discovery.cfg \
        -c "init" \
        -c "reset init" \
        -c "flash probe 0" \
        -c "flash write_image erase $(PROJ_NAME).elf $(FLASH_START_ADDR)" \
        -c "reset run" -c shutdown

.SILENT:
.PHONY:	clean
clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)
