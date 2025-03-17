################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/config/app_config.c 

OBJS += \
./Core/Src/config/app_config.o 

C_DEPS += \
./Core/Src/config/app_config.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/config/%.o Core/Src/config/%.su Core/Src/config/%.cyclo: ../Core/Src/config/%.c Core/Src/config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"D:/project/yns/MSV2008/Frontend/Core/Src/apps" -I"D:/project/yns/MSV2008/Frontend/Core/Src/config" -I"D:/project/yns/MSV2008/Frontend/Core/Src/env" -I"D:/project/yns/MSV2008/Frontend/Core/Src/lcd" -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-config

clean-Core-2f-Src-2f-config:
	-$(RM) ./Core/Src/config/app_config.cyclo ./Core/Src/config/app_config.d ./Core/Src/config/app_config.o ./Core/Src/config/app_config.su

.PHONY: clean-Core-2f-Src-2f-config

