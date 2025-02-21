################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/util/crc32_sw.c 

OBJS += \
./Core/Src/util/crc32_sw.o 

C_DEPS += \
./Core/Src/util/crc32_sw.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/util/%.o Core/Src/util/%.su Core/Src/util/%.cyclo: ../Core/Src/util/%.c Core/Src/util/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I"D:/project/yns/MSV2008/Backend/Drivers/BSP/components/ethernet" -I../Core/Inc -I"D:/project/yns/MSV2008/Backend/Core/Src/config" -I"D:/project/yns/MSV2008/Backend/Core/Src/m_env" -I"D:/project/yns/MSV2008/Backend/Core/Src/m_ether" -I"D:/project/yns/MSV2008/Backend/Core/Src/m_event" -I"D:/project/yns/MSV2008/Backend/Core/Src/m_menu" -I"D:/project/yns/MSV2008/Backend/Core/Src/m_serial" -I"D:/project/yns/MSV2008/Backend/Core/Src/util" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-util

clean-Core-2f-Src-2f-util:
	-$(RM) ./Core/Src/util/crc32_sw.cyclo ./Core/Src/util/crc32_sw.d ./Core/Src/util/crc32_sw.o ./Core/Src/util/crc32_sw.su

.PHONY: clean-Core-2f-Src-2f-util

