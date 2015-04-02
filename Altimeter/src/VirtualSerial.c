#include "VirtualSerial.h"
#include "ch.h"
#include "logger.h"
#include <string.h>

extern BinarySemaphore binSem_T5;

static void VS_SendAllLogs(void);
static void VS_DeleteAllLogs(void);

/*! LPCUSBlib CDC Class driver interfész konfiguráció és állapot-információ.
 * Ez a struktúra adódik át minden CDC Class driver függvénynek.
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


/* Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs
 */
// static FILE USBSerialStream;


/*! A hardver és a chip perifériáinak beállítása. */
void VS_setupHardware(void)
{
	USB_Init(VirtualSerial_CDC_Interface.Config.PortNumber, USB_MODE_Device); //VirtualSerial_CDC_Interface.Config.PortNumber, USB_MODE_Device

#if defined(USB_DEVICE_ROM_DRIVER)
	UsbdCdc_Init();
#endif
}

#if (CDC_TASK_SELECT == ECHO_CHARACTER_TASK)
/* Checks for data input, reply back to the host. */
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
/* USB-UART Bridge Task */
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


/*! Eseménykezelő az USB könyvtár "Configuration Changed" eseményéhez. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	//	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}


/*! Eseménykezelő az USB könyvtár "Control Request reception" eseményéhez. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}


/*! Az USB-n keresztül érkező adatokat kezeli le. */
void VS_USBdataHandling(void)
{
	static uint8_t recv_byte[CDC_TXRX_EPSIZE];
	static uint16_t recv_bytes_count = 0;

	if(USB_DeviceState[0] != DEVICE_STATE_Configured)
	{
		recv_bytes_count = 0;
		recv_byte[0] = 0;
		return;
	}

	if ( CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface) )
	{
	//		vTaskSuspendAll();

		recv_byte[recv_bytes_count] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		if (recv_byte[0] == STX)
		{
			recv_bytes_count = (recv_bytes_count + 1) % CDC_TXRX_EPSIZE;
			if (recv_bytes_count == 0)
				recv_bytes_count = 1;
		}

		if ( recv_byte[recv_bytes_count - 1] == ETX )
		{
			if ( recv_bytes_count != 4 )
			{
				recv_byte[0] = 0;
				recv_bytes_count = 0;
				return;
			}

//			chBSemWait(&binSem_T5);
			switch ( *(uint32_t*)recv_byte )
			{
				case 0x032A5202:					/* STX R * ETX - read log (02522A03:) */
					VS_SendAllLogs();
					recv_byte[0] = 0;
					recv_bytes_count = 0;
					break;

				case 0x032A4402:					/* STX D * ETX - delete log (02442A03:)*/
					VS_DeleteAllLogs();
					recv_byte[0] = 0;
					recv_bytes_count = 0;
					break;

				default:
					recv_byte[0] = 0;
					recv_bytes_count = 0;
			}
//			chBSemSignal(&binSem_T5);
		}
	}
}


/*!
 * USB-n keresztül elküldi az EEPROM tartalmát.
 */
static void VS_SendAllLogs()
{
	log_rec_t sendBuffer[8]; //[5];
	uint16_t n = 0;
//	uint16_t nextAddr = 0xFFFF;
//	uint16_t prevAddr = first_addr_to_read;
	chBSemWait(&binSem_T5);
	LCD_writeLOG_delete();
	for (n = 0; n < 512; n++) {
	    if(logger_readFromEE(sendBuffer, 8) == 0xFFFF) {
	    	break;
	    }
		if(USB_DeviceState[0] != DEVICE_STATE_Configured) {
			CDC_Device_Flush(&VirtualSerial_CDC_Interface);
			chBSemSignal(&binSem_T5);
			return;
		}
		CDC_Device_SendData(&VirtualSerial_CDC_Interface, (char *)(sendBuffer), 8 * sizeof(log_rec_t)); //5 *
//		prevAddr = nextAddr;
//		chThdSleepMilliseconds(20);
	}
	sendBuffer[0].altitude = 0xFFFF;
	sendBuffer[0].day = 0xFF;
	sendBuffer[0].hour = 0xFF;
	sendBuffer[0].minute = 0xFF;
	sendBuffer[0].month = 0xFF;
	sendBuffer[0].second = 0xFF;
	sendBuffer[0].year = 0xFF;
	if(USB_DeviceState[0] != DEVICE_STATE_Configured) {
		CDC_Device_Flush(&VirtualSerial_CDC_Interface);
		chBSemSignal(&binSem_T5);
		return;
	}
	CDC_Device_SendData(&VirtualSerial_CDC_Interface, (char *)(sendBuffer), sizeof(log_rec_t));
	chBSemSignal(&binSem_T5);
}


/*!
 * A teljes naplóállományt törli.
 */
static void VS_DeleteAllLogs()
{
	char USB_frame[4] = {STX, 'R', '*', STX};

	CDC_Device_SendData(&VirtualSerial_CDC_Interface, USB_frame, 4);
	USB_frame[0] = ETX;
	USB_frame[3] = ETX;

	logger_deleteLog();

	CDC_Device_SendData(&VirtualSerial_CDC_Interface, USB_frame, 4);
}
