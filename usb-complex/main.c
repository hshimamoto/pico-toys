// MIT License Copyright (c) 2023 Hiroshi Shimamoto
// Based on TinyUSB Example Copyright (c) 2019 Ha Thach (tinyusb.org)
//
// use external TinyUSB

#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
void tud_suspend_cb(bool remote_wakeup_en)
{
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

// declaration
void video_task(void);
void hid_task(void);

int main(void)
{
	board_init();
	tusb_init();

	for (;;) {
		tud_task();
		// user defined tasks
		video_task();
		hid_task();
	}

	return 0;
}
