// MIT License Copyright (c) 2023 Hiroshi Shimamoto

#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "smoothblink.pio.h"

int main() {
	PIO pio = pio0;
	int sm = 0;
	uint offset = pio_add_program(pio, &smoothblink_program);

	smoothblink_program_init(pio, sm, offset, PICO_DEFAULT_LED_PIN);

	// PIO still disabled here
	// set period
	pio_sm_put_blocking(pio, sm, 65535);
	pio_sm_exec(pio, sm, pio_encode_pull(false, false));
	pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
	// and start PIO
	pio_sm_set_enabled(pio, sm, true);

	for (;;) {
		for (int level = 0; level < 100; level++) {
			pio_sm_put_blocking(pio, sm, level * 10);
			sleep_ms(10);
		}
		sleep_ms(100);
		for (int level = 100; level >= 0; level--) {
			pio_sm_put_blocking(pio, sm, level * 10);
			sleep_ms(10);
		}
		sleep_ms(100);
	}

	return 0;
}
