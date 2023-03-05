// MIT License Copyright (c) 2023 Hiroshi Shimamoto

#include <stdio.h>
#include "pico/stdlib.h"

int main(void) {
	stdio_init_all();

	gpio_init(10);
	gpio_set_dir(10, GPIO_IN);

	printf("Start reading\n");

	for (;;) {
		bool state = gpio_get(10);
		// Assert GPIO10 and 3v3 to show GPIO10 ON
		if (state) {
			printf("GPIO10 ON\n");
		}
		sleep_ms(1000);
	}

	return 0;
}
