// MIT License Copyright (c) 2023 Hiroshi Shimamoto

#include "pico/stdlib.h"

int main(void) {
	const uint LED_PIN = PICO_DEFAULT_LED_PIN;

	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	for (;;) {
		gpio_put(LED_PIN, 1);
		sleep_ms(1000);
		gpio_put(LED_PIN, 0);
		sleep_ms(1000);
	}

	return 0;
}
