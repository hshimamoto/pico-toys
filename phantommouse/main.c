// MIT License Copyright (c) 2023 Hiroshi Shimamoto
// Based on TinyUSB Example Copyright (c) 2019 Ha Thach (tinyusb.org)

#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

void hid_task(void);

int main(void)
{
	set_sys_clock_48mhz();

	board_init();
	tusb_init();

	for (;;) {
		tud_task(); // tinyusb device task
		hid_task();
	}

	return 0;
}

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

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
	const uint32_t interval_ms = 60 * 1000;
	static uint32_t start_ms = 0;
	uint32_t now = board_millis();

	if (now - start_ms < interval_ms)
		return;
	start_ms = now;

	const struct {
		int x, y;
	} move[4] = {
		{1, 0}, {0, 1}, {-1, 0}, {0, -1},
	};

	for (int i = 0; i < 4; i++) {
		int x = move[i].x & 0xff;
		int y = move[i].y & 0xff;
		tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, x, y, 0, 0);
		sleep_ms(10);
	}
}

// Invoked when sent REPORT successfully to host
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
}

// Invoked when received GET_REPORT control request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
	return 0;
}

// Invoked when received SET_REPORT control request or
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
}
