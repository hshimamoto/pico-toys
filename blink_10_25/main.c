// MIT License Copyright (c) 2023 Hiroshi Shimamoto

#include "pico/stdlib.h"

int main(void) {
	const uint LED10_PIN = 10;
	const uint LED25_PIN = 25;

	gpio_init(LED10_PIN);
	gpio_init(LED25_PIN);
	gpio_set_dir(LED10_PIN, GPIO_OUT);
	gpio_set_dir(LED25_PIN, GPIO_OUT);

	for (;;) {
		gpio_put(LED10_PIN, 0);
		gpio_put(LED25_PIN, 1);
		sleep_ms(1000);
		gpio_put(LED10_PIN, 1);
		gpio_put(LED25_PIN, 0);
		sleep_ms(1000);
	}

	return 0;
}
