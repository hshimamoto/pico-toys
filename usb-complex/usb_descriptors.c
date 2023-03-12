// MIT License Copyright (c) 2023 Hiroshi Shimamoto
// Based on TinyUSB Example Copyright (c) 2019 Ha Thach (tinyusb.org)

#include "tusb.h"
#include "usb_descriptors.h"

/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]  VENDOR | VIDEO | AUDIO | MIDI | HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n)  ( (CFG_TUD_##itf) << (n) )
#define USB_PID           (0x4000 | \
			   _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
			   _PID_MAP(MIDI, 3) | _PID_MAP(AUDIO, 4) | \
			   _PID_MAP(VIDEO, 5) | _PID_MAP(VENDOR, 6) )

#define USB_VID   0xCafe
#define USB_BCD   0x0200

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device = {
	.bLength            = sizeof(tusb_desc_device_t),
	.bDescriptorType    = TUSB_DESC_DEVICE,
	.bcdUSB             = USB_BCD,
	.bDeviceClass       = TUSB_CLASS_MISC,
	.bDeviceSubClass    = MISC_SUBCLASS_COMMON,
	.bDeviceProtocol    = MISC_PROTOCOL_IAD,
	.bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

	.idVendor           = USB_VID,
	.idProduct          = USB_PID,
	.bcdDevice          = 0x0100,

	.iManufacturer      = 0x01,
	.iProduct           = 0x02,
	.iSerialNumber      = 0x03,

	.bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
	return (uint8_t const *)&desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

uint8_t const desc_hid_report[] = {
	TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE)),
};

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
	return desc_hid_report;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum {
	ITF_NUM_VIDEO_CTRL,
	ITF_NUM_VIDEO_STRM,
	ITF_NUM_HID,
	ITF_NUM_TOTAL
};

#define TUD_VIDEO_CAPTURE_DESC_UNCOMPR_LEN (\
	TUD_VIDEO_DESC_IAD_LEN \
	+ TUD_VIDEO_DESC_STD_VC_LEN \
	+ (TUD_VIDEO_DESC_CS_VC_LEN + 1) \
	+ TUD_VIDEO_DESC_CAMERA_TERM_LEN \
	+ TUD_VIDEO_DESC_OUTPUT_TERM_LEN \
	+ TUD_VIDEO_DESC_STD_VS_LEN \
	+ (TUD_VIDEO_DESC_CS_VS_IN_LEN + 1) \
	+ TUD_VIDEO_DESC_CS_VS_FMT_UNCOMPR_LEN \
	+ TUD_VIDEO_DESC_CS_VS_FRM_UNCOMPR_CONT_LEN \
	+ TUD_VIDEO_DESC_CS_VS_COLOR_MATCHING_LEN \
	+ TUD_VIDEO_DESC_STD_VS_LEN \
	+ 7)

#define CONFIG_TOTAL_LEN  \
	(TUD_CONFIG_DESC_LEN + TUD_VIDEO_CAPTURE_DESC_UNCOMPR_LEN + TUD_HID_DESC_LEN)

#define string_idx_video  4

// Endpoint for each interface
#define EPNUM_VIDEO_IN    0x81
#define EPNUM_HID         0x82

#define UVC_CLOCK_FREQUENCY    27000000
#define UVC_ENTITY_CAP_INPUT_TERMINAL  0x01
#define UVC_ENTITY_CAP_OUTPUT_TERMINAL 0x02

uint8_t const desc_configuration[] = {
	// Config number, interface count, string index, total length, attribute, power in mA
	TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0, 500),

	// UVC
	//
	TUD_VIDEO_DESC_IAD(ITF_NUM_VIDEO_CTRL, /* 2 intf */ 0x02, string_idx_video),
	TUD_VIDEO_DESC_STD_VC(ITF_NUM_VIDEO_CTRL, 0, string_idx_video),
	TUD_VIDEO_DESC_CS_VC(/* UVC 1.5 */ 0x0150,
			     /* wTotalLength - bLength */
			     (TUD_VIDEO_DESC_CAMERA_TERM_LEN + TUD_VIDEO_DESC_OUTPUT_TERM_LEN),
			     UVC_CLOCK_FREQUENCY, ITF_NUM_VIDEO_STRM),
	TUD_VIDEO_DESC_CAMERA_TERM(UVC_ENTITY_CAP_INPUT_TERMINAL, 0, 0, 0, 0, 0, 0),
	TUD_VIDEO_DESC_OUTPUT_TERM(UVC_ENTITY_CAP_OUTPUT_TERMINAL, VIDEO_TT_STREAMING, 0, 1, 0),
	TUD_VIDEO_DESC_STD_VS(ITF_NUM_VIDEO_STRM, 0, 0, string_idx_video),
	TUD_VIDEO_DESC_CS_VS_INPUT(/* bNumFortmats */ 1,
				   /* wTotalLength - bLength */
				   (TUD_VIDEO_DESC_CS_VS_FMT_UNCOMPR_LEN
				   + TUD_VIDEO_DESC_CS_VS_FRM_UNCOMPR_CONT_LEN
				   + TUD_VIDEO_DESC_CS_VS_COLOR_MATCHING_LEN),
				   EPNUM_VIDEO_IN, 0, UVC_ENTITY_CAP_OUTPUT_TERMINAL, 0, 0, 0, 0),
	TUD_VIDEO_DESC_CS_VS_FMT_UNCOMPR(1, 1, TUD_VIDEO_GUID_YUY2, 16, 1, 0, 0, 0, 0),
	TUD_VIDEO_DESC_CS_VS_FRM_UNCOMPR_CONT(1, 0, FRAME_WIDTH, FRAME_HEIGHT,
					      (FRAME_WIDTH * FRAME_HEIGHT * 16),
					      (FRAME_WIDTH * FRAME_HEIGHT * 16 * FRAME_RATE),
					      (FRAME_WIDTH * FRAME_HEIGHT * 16),
					      (10000000 / FRAME_RATE),
					      (10000000 / FRAME_RATE),
					      (10000000), /* (10000000 / FRAME_RATE) * FRAME_RATE */
					      (10000000 / FRAME_RATE)),
	TUD_VIDEO_DESC_CS_VS_COLOR_MATCHING(VIDEO_COLOR_PRIMARIES_BT709,
					    VIDEO_COLOR_XFER_CH_BT709,
					    VIDEO_COLOR_COEF_SMPTE170M),
	TUD_VIDEO_DESC_STD_VS(ITF_NUM_VIDEO_STRM, 1, 1, string_idx_video),
	TUD_VIDEO_DESC_EP_ISO(EPNUM_VIDEO_IN, CFG_TUD_VIDEO_STREAMING_EP_BUFSIZE, 1),

	// Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
	TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 5),
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
	return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const* string_desc_arr [] = {
	(const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
	"TinyUSB",                     // 1: Manufacturer
	"TinyUSB Device",              // 2: Product
	"123456",                      // 3: Serials, should use chip ID
	"TinyUSB UVC",                 // 4: UVC Interface
};

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
	uint8_t chr_count;

	if (index == 0) {
		memcpy(&_desc_str[1], string_desc_arr[0], 2);
		chr_count = 1;
	} else {
		// Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
		// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

		if ( !(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) )
			return NULL;

		const char* str = string_desc_arr[index];

		// Cap at max char
		chr_count = strlen(str);
		if (chr_count > 31)
			chr_count = 31;

		// Convert ASCII string into UTF-16
		for(uint8_t i = 0; i < chr_count; i++) {
			_desc_str[i + 1] = str[i];
		}
	}

	// first byte is length (including header), second byte is string type
	_desc_str[0] = (TUSB_DESC_STRING << 8) | (2*chr_count + 2);

	return _desc_str;
}
