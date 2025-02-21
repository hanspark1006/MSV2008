################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/env/m_flash.c \
../Core/Src/env/trigger.c \
../Core/Src/env/uart.c 

OBJS += \
./Core/Src/env/m_flash.o \
./Core/Src/env/trigger.o \
./Core/Src/env/uart.o 

C_DEPS += \
./Core/Src/env/m_flash.d \
./Core/Src/env/trigger.d \
./Core/Src/env/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/env/%.o Core/Src/env/%.su Core/Src/env/%.cyclo: ../Core/Src/env/%.c Core/Src/env/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"D:/project/yns/MSV2008/Frontend/Core/Src/apps" -I"D:/project/yns/MSV2008/Frontend/Core/Src/config" -I"D:/project/yns/MSV2008/Frontend/Core/Src/env" -I"D:/project/yns/MSV2008/Frontend/Core/Src/lcd" -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-env

clean-Core-2f-Src-2f-env:
	-$(RM) ./Core/Src/env/m_flash.cyclo ./Core/Src/env/m_flash.d ./Core/Src/env/m_flash.o ./Core/Src/env/m_flash.su ./Core/Src/env/trigger.cyclo ./Core/Src/env/trigger.d ./Core/Src/env/trigger.o ./Core/Src/env/trigger.su ./Core/Src/env/uart.cyclo ./Core/Src/env/uart.d ./Core/Src/env/uart.o ./Core/Src/env/uart.su

.PHONY: clean-Core-2f-Src-2f-env

