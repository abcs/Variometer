/*
 * @brief USB Device definitions for the LPC11Uxx microcontrollers
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

/** @ingroup Group_Device
 *  @defgroup Group_Device_LPC11Uxx Device Management (LPC11Uxx, LPC1347)
 *  @brief USB Device definitions for the LPC1347 and LPC11Uxx microcontrollers.
 *
 *  Architecture specific USB Device definitions for the LPC microcontrollers.
 *
 *  @{
 */

#ifndef __USBDEVICE_LPC11UXX_H__
#define __USBDEVICE_LPC11UXX_H__

		#include "../../../../../Common/Common.h"
		#include "../../USBController.h"
		#include "../../StdDescriptors.h"
		#include "../../USBInterrupt.h"
		#include "../../Endpoint.h"
		#include "../../HAL/HAL.h"

		#if defined(USB_DEVICE_ROM_DRIVER)
			#include "../USBRom/usbd_rom_api.h"
		#endif

		#if defined(__cplusplus)
extern "C" {
		#endif

		#if !defined(__INCLUDE_FROM_USB_DRIVER)
			#error Do not include this file directly. Include lpcroot/libraries/LPCUSBlib/Drivers/USB/USB.h instead.
		#endif

		#if (defined(USE_RAM_DESCRIPTORS) && defined(USE_EEPROM_DESCRIPTORS))
			#error USE_RAM_DESCRIPTORS and USE_EEPROM_DESCRIPTORS are mutually exclusive.
		#endif

			#if defined(USB_SERIES_4_AVR) || defined(USB_SERIES_6_AVR) || defined(USB_SERIES_7_AVR) || \
	defined(__DOXYGEN__)

				#define USB_DEVICE_OPT_LOWSPEED            (1 << 0)
			#endif

			#define USB_DEVICE_OPT_FULLSPEED               (0 << 0)

			#if (!defined(NO_INTERNAL_SERIAL) && \
	(defined(USB_SERIES_7_AVR) || defined(USB_SERIES_6_AVR) || defined(USB_SERIES_4_AVR) ||	\
	(defined(USB_SERIES_2_AVR) && (!defined(__AVR_AT90USB82__) || defined(__AVR_AT90USB162__))) || \
	defined(__DOXYGEN__)))

				#define USE_INTERNAL_SERIAL            0xDC

				#define INTERNAL_SERIAL_LENGTH_BITS    80

				#define INTERNAL_SERIAL_START_ADDRESS  0x0E
			#else
				#define USE_INTERNAL_SERIAL            NO_DESCRIPTOR

				#define INTERNAL_SERIAL_LENGTH_BITS    0
				#define INTERNAL_SERIAL_START_ADDRESS  0
			#endif

void USB_Device_SendRemoteWakeup(void);

PRAGMA_ALWAYS_INLINE
static inline uint16_t USB_Device_GetFrameNumber(void) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;

static inline uint16_t USB_Device_GetFrameNumber(void)
{
	return 0;
}

			#if !defined(NO_SOF_EVENTS)

PRAGMA_ALWAYS_INLINE
static inline void USB_Device_EnableSOFEvents(void) ATTR_ALWAYS_INLINE;

static inline void USB_Device_EnableSOFEvents(void)
{}

PRAGMA_ALWAYS_INLINE
static inline void USB_Device_DisableSOFEvents(void) ATTR_ALWAYS_INLINE;

static inline void USB_Device_DisableSOFEvents(void)
{}

			#endif

	#if !defined(__DOXYGEN__)
			#if defined(USB_DEVICE_OPT_LOWSPEED)
PRAGMA_ALWAYS_INLINE
static inline void USB_Device_SetLowSpeed(void) ATTR_ALWAYS_INLINE;

static inline void USB_Device_SetLowSpeed(void)
{
	//              UDCON |=  (1 << LSM);
}

PRAGMA_ALWAYS_INLINE
static inline void USB_Device_SetFullSpeed(void) ATTR_ALWAYS_INLINE;

static inline void USB_Device_SetFullSpeed(void)
{
	//              UDCON &= ~(1 << LSM);
}

			#endif

PRAGMA_ALWAYS_INLINE
static inline void USB_Device_SetDeviceAddress(uint8_t corenum, const uint8_t Address) ATTR_ALWAYS_INLINE;

static inline void USB_Device_SetDeviceAddress(uint8_t corenum, const uint8_t Address)
{
	HAL_SetDeviceAddress(Address);
}

PRAGMA_ALWAYS_INLINE
static inline bool USB_Device_IsAddressSet(void) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;

static inline bool USB_Device_IsAddressSet(void)
{
	return true;			/* temporarily */
}

			#if (USE_INTERNAL_SERIAL != NO_DESCRIPTOR)
static inline void USB_Device_GetSerialString(uint16_t *const UnicodeString) ATTR_NON_NULL_PTR_ARG(1);

static inline void USB_Device_GetSerialString(uint16_t *const UnicodeString)
{
	uint_reg_t CurrentGlobalInt = GetGlobalInterruptMask();
	GlobalInterruptDisable();

	uint8_t SigReadAddress = INTERNAL_SERIAL_START_ADDRESS;

	for (uint8_t SerialCharNum = 0; SerialCharNum < (INTERNAL_SERIAL_LENGTH_BITS / 4); SerialCharNum++) {
		uint8_t SerialByte = boot_signature_byte_get(SigReadAddress);

		if (SerialCharNum & 0x01) {
			SerialByte >>= 4;
			SigReadAddress++;
		}

		SerialByte &= 0x0F;

		UnicodeString[SerialCharNum] = cpu_to_le16((SerialByte >= 10) ?
												   (('A' - 10) + SerialByte) : ('0' + SerialByte));
	}

	SetGlobalInterruptMask(CurrentGlobalInt);
}

			#endif

	#endif

		#if defined(__cplusplus)
}
		#endif

#endif

/** @} */

