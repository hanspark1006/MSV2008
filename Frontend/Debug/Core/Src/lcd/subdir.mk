################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/lcd/lcd.c \
../Core/Src/lcd/screen.c 

OBJS += \
./Core/Src/lcd/lcd.o \
./Core/Src/lcd/screen.o 

C_DEPS += \
./Core/Src/lcd/lcd.d \
./Core/Src/lcd/screen.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/lcd/%.o Core/Src/lcd/%.su Core/Src/lcd/%.cyclo: ../Core/Src/lcd/%.c Core/Src/lcd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"D:/project/yns/MSV2008/Frontend/Core/Src/apps" -I"D:/project/yns/MSV2008/Frontend/Core/Src/config" -I"D:/project/yns/MSV2008/Frontend/Core/Src/env" -I"D:/project/yns/MSV2008/Frontend/Core/Src/lcd" -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-lcd

clean-Core-2f-Src-2f-lcd:
	-$(RM) ./Core/Src/lcd/lcd.cyclo ./Core/Src/lcd/lcd.d ./Core/Src/lcd/lcd.o ./Core/Src/lcd/lcd.su ./Core/Src/lcd/screen.cyclo ./Core/Src/lcd/screen.d ./Core/Src/lcd/screen.o ./Core/Src/lcd/screen.su

.PHONY: clean-Core-2f-Src-2f-lcd

