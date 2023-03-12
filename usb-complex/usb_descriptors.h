// MIT License Copyright (c) 2023 Hiroshi Shimamoto
// Based on TinyUSB Example Copyright (c) 2019 Ha Thach (tinyusb.org)

#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

// for HID
enum {
	REPORT_ID_MOUSE = 1,
	REPORT_ID_COUNT
};

// for UVC
#define FRAME_WIDTH  128
#define FRAME_HEIGHT 72
#define FRAME_RATE   10

#endif /* USB_DESCRIPTORS_H_ */
