// MIT License Copyright (c) 2023 Hiroshi Shimamoto
//
// use external TinyUSB

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
	const uint32_t long_interval_ms = 60 * 1000;
	const uint32_t short_interval_ms = 10;
	static uint32_t prev_ms = 0;
	static int state = 0;
	uint32_t now = board_millis();

	if (prev_ms == 0)
		prev_ms = now;

	switch (state) {
	case 0:
		if ((now - prev_ms) < long_interval_ms)
			return;
	default:
		if ((now - prev_ms) < short_interval_ms)
			return;
	}
	prev_ms = now;

	const struct {
		int x, y;
	} move[4] = {
		{1, 0}, {0, 1}, {-1, 0}, {0, -1},
	};

	int x = move[state].x & 0xff;
	int y = move[state].y & 0xff;
	tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, x, y, 0, 0);

	state = (state + 1) % 4;
}

// Invoked when sent REPORT successfully to host
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
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
