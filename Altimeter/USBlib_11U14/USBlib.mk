# List of all the USBlib files
USBSRC = ${USBlib_DIR}/Drivers/USB/Core/ConfigDescriptor.c \
		 ${USBlib_DIR}/Drivers/USB/Core/Device.c \
		 ${USBlib_DIR}/Drivers/USB/Core/DeviceStandardReq.c \
		 ${USBlib_DIR}/Drivers/USB/Core/Endpoint.c \
		 ${USBlib_DIR}/Drivers/USB/Core/EndpointStream.c \
		 ${USBlib_DIR}/Drivers/USB/Core/Events.c \
		 ${USBlib_DIR}/Drivers/USB/Core/Pipe.c \
		 ${USBlib_DIR}/Drivers/USB/Core/PipeStream.c \
		 ${USBlib_DIR}/Drivers/USB/Core/USBController.c \
		 ${USBlib_DIR}/Drivers/USB/Core/USBMemory.c \
		 ${USBlib_DIR}/Drivers/USB/Core/USBTask.c \
		 ${USBlib_DIR}/Drivers/USB/Core/HAL/LPC11UXX/HAL_LPC11Uxx.c \
		 ${USBlib_DIR}/Drivers/USB/Core/DCD/LPC11UXX/Endpoint_LPC11Uxx.c \
		 ${USBlib_DIR}/Drivers/USB/Class/Device/CDCClassDevice.c

#		 ${USBlib_DIR}/Drivers/USB/Class/Common/HIDParser.c \

# Required include directories
USBINC = ${USBlib_DIR} \
		 ${USBlib_DIR}/Drivers/USB \
		 ${USBlib_DIR}/Common \
		 ${USBlib_DIR}/Drivers/USB/Core \
		 ${USBlib_DIR}/Drivers/USB/Core/HAL \
		 ${USBlib_DIR}/Drivers/USB/Core/HAL/LPC11UXX \
		 ${USBlib_DIR}/Drivers/USB/Core/DCD \
		 ${USBlib_DIR}/Drivers/USB/Core/DCD/LPC11UXX \
		 ${USBlib_DIR}/Drivers/USB/Class \
		 ${USBlib_DIR}/Drivers/USB/Class/Device \
		 ${USBlib_DIR}/Drivers/USB/Class/Common
