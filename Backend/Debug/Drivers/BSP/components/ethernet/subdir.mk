################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/components/ethernet/Ethernet.c \
../Drivers/BSP/components/ethernet/dhcp.c \
../Drivers/BSP/components/ethernet/dnslkup.c \
../Drivers/BSP/components/ethernet/enc28j60.c 

OBJS += \
./Drivers/BSP/components/ethernet/Ethernet.o \
./Drivers/BSP/components/ethernet/dhcp.o \
./Drivers/BSP/components/ethernet/dnslkup.o \
./Drivers/BSP/components/ethernet/enc28j60.o 

C_DEPS += \
./Drivers/BSP/components/ethernet/Ethernet.d \
./Drivers/BSP/components/ethernet/dhcp.d \
./Drivers/BSP/components/ethernet/dnslkup.d \
./Drivers/BSP/components/ethernet/enc28j60.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/components/ethernet/%.o Drivers/BSP/components/ethernet/%.su Drivers/BSP/components/ethernet/%.cyclo: ../Drivers/BSP/components/ethernet/%.c Drivers/BSP/components/ethernet/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I"D:/project/yns/MSV2008/Backend/Drivers/BSP/components/ethernet" -I../Core/Inc -I"D:/project/yns/MSV2008/Backend/Core/Src/config" -I"D:/project/yns/MSV2008/Backend/Core/Src/m_env" -I"D:/project/yns/MSV2008/Backend/Core/Src/m_ether" -I"D:/project/yns/MSV2008/Backend/Core/Src/m_event" -I"D:/project/yns/MSV2008/Backend/Core/Src/m_menu" -I"D:/project/yns/MSV2008/Backend/Core/Src/m_serial" -I"D:/project/yns/MSV2008/Backend/Core/Src/util" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-components-2f-ethernet

clean-Drivers-2f-BSP-2f-components-2f-ethernet:
	-$(RM) ./Drivers/BSP/components/ethernet/Ethernet.cyclo ./Drivers/BSP/components/ethernet/Ethernet.d ./Drivers/BSP/components/ethernet/Ethernet.o ./Drivers/BSP/components/ethernet/Ethernet.su ./Drivers/BSP/components/ethernet/dhcp.cyclo ./Drivers/BSP/components/ethernet/dhcp.d ./Drivers/BSP/components/ethernet/dhcp.o ./Drivers/BSP/components/ethernet/dhcp.su ./Drivers/BSP/components/ethernet/dnslkup.cyclo ./Drivers/BSP/components/ethernet/dnslkup.d ./Drivers/BSP/components/ethernet/dnslkup.o ./Drivers/BSP/components/ethernet/dnslkup.su ./Drivers/BSP/components/ethernet/enc28j60.cyclo ./Drivers/BSP/components/ethernet/enc28j60.d ./Drivers/BSP/components/ethernet/enc28j60.o ./Drivers/BSP/components/ethernet/enc28j60.su

.PHONY: clean-Drivers-2f-BSP-2f-components-2f-ethernet

