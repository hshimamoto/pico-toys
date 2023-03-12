// MIT License Copyright (c) 2023 Hiroshi Shimamoto
//
// use external TinyUSB

#include "pico/rand.h"
#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

// random
uint32_t rand(void)
{
	static uint32_t r = 0;

	if (r == 0) {
		r = get_rand_32();
	}
	r ^= r << 13;
	r ^= r >> 17;
	r ^= r << 5;
	return r;
}

//--------------------------------------------------------------------+
// UVC
//--------------------------------------------------------------------+
#define WIDTH  (FRAME_WIDTH / 2)
#define HEIGHT (FRAME_HEIGHT / 2)
static uint8_t yuv_buffer[FRAME_WIDTH * FRAME_HEIGHT * 2];
static uint8_t canvas[HEIGHT][WIDTH];
static int drops[WIDTH];
static uint32_t frameno;

// RGB to YUV
// python3
// r, g, b = 255, 255, 255
// y = 0.256788 * r + 0.504129 * g + 0.097906 * b + 16
// u = -0.148223 * r - 0.290993 * g + 0.439216 * b + 128
// v = 0.439216 * r - 0.367788 * g - 0.071427 * b + 128
// print(y, u, v)

static uint8_t pallet[64][4] = {
#include "pallet.h"
};

static uint8_t digit[16][5][3] = {
	{
		{1, 1, 1},
		{1, 0, 1},
		{1, 0, 1},
		{1, 0, 1},
		{1, 1, 1},
	},
	{
		{0, 1, 0},
		{0, 1, 0},
		{0, 1, 0},
		{0, 1, 0},
		{0, 1, 0},
	},
	{
		{1, 1, 1},
		{0, 0, 1},
		{1, 1, 1},
		{1, 0, 0},
		{1, 1, 1},
	},
	{
		{1, 1, 1},
		{0, 0, 1},
		{1, 1, 1},
		{0, 0, 1},
		{1, 1, 1},
	},
	{
		{1, 0, 1},
		{1, 0, 1},
		{1, 1, 1},
		{0, 0, 1},
		{0, 0, 1},
	},
	{
		{1, 1, 1},
		{1, 0, 0},
		{1, 1, 1},
		{0, 0, 1},
		{1, 1, 1},
	},
	{
		{1, 1, 1},
		{1, 0, 0},
		{1, 1, 1},
		{1, 0, 1},
		{1, 1, 1},
	},
	{
		{1, 1, 1},
		{0, 0, 1},
		{0, 0, 1},
		{0, 1, 0},
		{0, 1, 0},
	},
	{
		{1, 1, 1},
		{1, 0, 1},
		{1, 1, 1},
		{1, 0, 1},
		{1, 1, 1},
	},
	{
		{1, 1, 1},
		{1, 0, 1},
		{1, 1, 1},
		{0, 0, 1},
		{1, 1, 1},
	},
	{
		{0, 1, 0},
		{1, 0, 1},
		{1, 1, 1},
		{1, 0, 1},
		{1, 0, 1},
	},
	{
		{1, 1, 0},
		{1, 0, 1},
		{1, 1, 0},
		{1, 0, 1},
		{1, 1, 0},
	},
	{
		{0, 1, 0},
		{1, 0, 1},
		{1, 0, 0},
		{1, 0, 1},
		{0, 1, 0},
	},
	{
		{1, 1, 0},
		{1, 0, 1},
		{1, 0, 1},
		{1, 0, 1},
		{1, 1, 0},
	},
	{
		{1, 1, 1},
		{1, 0, 0},
		{1, 1, 1},
		{1, 0, 0},
		{1, 1, 1},
	},
	{
		{1, 1, 1},
		{1, 0, 0},
		{1, 1, 1},
		{1, 0, 0},
		{1, 0, 0},
	},
};

void draw_digit(int x, int y, int n, int fg, int bg)
{
	for (int yy = 0; yy < 5; yy++) {
		for (int xx = 0; xx < 3; xx++) {
			uint8_t c = digit[n][yy][xx];
			canvas[y + yy][x + xx] = c ? fg : bg;
		}
	}
}

void prepare_yuv_buffer(void)
{
	// rain drop animation
	for (int x = 0; x < WIDTH; x++) {
		int d = drops[x] - 16;
		int y = 0;
		for (y = 0; y < HEIGHT; y++)
			canvas[y][x] = 0;
		y = d;
		for (int i = 0; i < 10; i++) {
			if (y >= 0 && y < HEIGHT)
				canvas[y][x] = 4;
			y++;
		}
		for (int i = 0; i < 4; i++) {
			if (y >= 0 && y < HEIGHT)
				canvas[y][x] = 8;
			y++;
		}
		if (y >= 0 && y < HEIGHT)
			canvas[y][x] = 12;
		y++;
		if (y >= 0 && y < HEIGHT)
			canvas[y][x] = 29;
		y++;
		// next
		drops[x]++;
		if (drops[x] > (HEIGHT + 32))
			drops[x] = -(rand() & 0x3f);
	}

	uint32_t ms = board_millis();
	// 4000000000
#if 0
	for (int i = 0; i < 10; i++) {
		int n = ms % 10;
		ms /= 10;
		draw_digit((10 - i) * 4, 1, n, 63, 0);
	}

	ms = frameno;
	for (int i = 0; i < 10; i++) {
		int n = ms % 10;
		ms /= 10;
		draw_digit((10 - i) * 4, 7, n, 60, 0);
	}
#else
	for (int i = 0; i < 7; i++) {
		int n = ms % 10;
		ms /= 10;
		draw_digit((7 - i) * 4, 1, n, 42, 0);
	}

	ms = frameno;
	for (int i = 0; i < 7; i++) {
		int n = ms % 10;
		ms /= 10;
		draw_digit((7 - i) * 4, 7, n, 40, 0);
	}
#endif

	// make YUV buffer from canvas
	uint8_t *p = yuv_buffer;
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			uint8_t c = canvas[y][x] & 0x3f;
			memcpy(p, &pallet[c], 4);
			p += 4;
		}
		for (int x = 0; x < WIDTH; x++) {
			uint8_t c = canvas[y][x] & 0x3f;
			memcpy(p, &pallet[c], 4);
			p += 4;
		}
	}
}

static bool busy = false;

void video_task(void)
{
	const uint32_t interval_ms = 1000 / FRAME_RATE;
	static bool initialized = false;
	static uint32_t prev_ms = 0;
	static bool nextframe = false;

	if (!tud_video_n_streaming(0, 0))
		initialized = false;

	uint32_t now = board_millis();

	if (!initialized) {
		initialized = true;
		nextframe = false;
		prev_ms = now;
	}

	if (!nextframe) {
		prepare_yuv_buffer();
		nextframe = true;
	}

	if (now < prev_ms) {
		if (now < (prev_ms + interval_ms))
			return;
	} else {
		if ((now - prev_ms) < interval_ms)
			return;
	}

	prev_ms += interval_ms;

	if (!busy) {
		tud_video_n_frame_xfer(0, 0, (void*)yuv_buffer, FRAME_WIDTH * FRAME_HEIGHT * 2);
		busy = true;
	}

	nextframe = false;
}

void tud_video_frame_xfer_complete_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx)
{
	busy = false;
	frameno++;
}

int tud_video_commit_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, video_probe_and_commit_control_t const *parameters)
{
	return VIDEO_ERROR_NONE;
}
