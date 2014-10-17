#include "VirtualSerial.h"
#include "ch.h"
#include "logger.h"

/** LPCUSBlib CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface = {
	.Config = {
		.ControlInterfaceNumber         = 0,

		.DataINEndpointNumber           = CDC_TX_EPNUM,
		.DataINEndpointSize             = CDC_TXRX_EPSIZE,
		.DataINEndpointDoubleBank       = false,

		.DataOUTEndpointNumber          = CDC_RX_EPNUM,
		.DataOUTEndpointSize            = CDC_TXRX_EPSIZE,
		.DataOUTEndpointDoubleBank      = false,

		.NotificationEndpointNumber     = CDC_NOTIFICATION_EPNUM,
		.NotificationEndpointSize       = CDC_NOTIFICATION_EPSIZE,
		.NotificationEndpointDoubleBank = false,
		.PortNumber		                = 0
	},

	.State = {
		.LineEncoding = {
				.BaudRateBPS			= 115200,
				.CharFormat        		= CDC_LINEENCODING_OneStopBit,
				.ParityType        		= CDC_PARITY_None,
				.DataBits          		= 8
		}
	}
};


/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs
 */
// static FILE USBSerialStream;


/** Configures the board hardware and chip peripherals for the functionality. */
void VS_setupHardware(void)
{
	USB_Init(VirtualSerial_CDC_Interface.Config.PortNumber, USB_MODE_Device); //VirtualSerial_CDC_Interface.Config.PortNumber, USB_MODE_Device

#if defined(USB_DEVICE_ROM_DRIVER)
	UsbdCdc_Init();
#endif
}

#if (CDC_TASK_SELECT == ECHO_CHARACTER_TASK)
/** Checks for data input, reply back to the host. */
void VS_echoCharacter(void)
{
	uint8_t recv_byte[CDC_TXRX_EPSIZE];
	if (CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface)) {
		recv_byte[0] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

		//CDC_Device_SendData(&VirtualSerial_CDC_Interface, (char *) recv_byte, 1);
		CDC_Device_SendData(&VirtualSerial_CDC_Interface, (char *)recv_byte, 1); //"Hello World\r\n", 14
	}
}

#elif (CDC_TASK_SELECT == CDC_BRIDGE_TASK)
/** USB-UART Bridge Task */
void CDC_Bridge_Task(void)
{
	/* Echo back character */
	uint8_t out_buff[CDC_TXRX_EPSIZE], in_buff[CDC_TXRX_EPSIZE];
	uint32_t recv_count;
#if !defined(USB_DEVICE_ROM_DRIVER)
	recv_count = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
	while (recv_count--) {
		out_buff[0] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		Serial_Send((uint8_t *) out_buff, 1, BLOCKING);
	}

	recv_count = Serial_Revc(in_buff, CDC_TXRX_EPSIZE, NONE_BLOCKING);
	if (recv_count) {
		CDC_Device_SendData(&VirtualSerial_CDC_Interface, (char *) in_buff, recv_count);
	}
#else
	recv_count = UsbdCdc_RecvData(out_buff, CDC_TXRX_EPSIZE);
	if (recv_count) {
		Serial_Send((uint8_t *) out_buff, recv_count, BLOCKING);
	}

	recv_count = Serial_Revc(in_buff, CDC_TXRX_EPSIZE, NONE_BLOCKING);
	if (recv_count) {
		UsbdCdc_SendData(in_buff, recv_count);
	}
#endif
}

#endif


/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	//	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}


/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}


void VS_USBdataHandling(void)
{
	uint8_t recv_byte[CDC_TXRX_EPSIZE];
	uint16_t recv_bytes_count = 0;

	recv_bytes_count = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);

	if ( recv_bytes_count > 1 )
	{
		uint16_t c = 0;
		log_rec_t sendBuffer[5];


//		vTaskSuspendAll();

		do
		{
			recv_byte[c] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
			c++;
			recv_byte[c] = 0;
		}
		while( (c < recv_bytes_count) && (c < CDC_TXRX_EPSIZE) );

		if (recv_byte[0] == 'S' && recv_byte[1] == '*')
		{
			(void)logger_readFromEE(sendBuffer, 5);
			CDC_Device_SendData(&VirtualSerial_CDC_Interface, (char *)sendBuffer, 5*10);
		}

		if (recv_byte[0] == 'D' && recv_byte[1] == '*')
		{

		}

//		xTaskResumeAll();
	}
}
