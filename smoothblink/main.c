// MIT License Copyright (c) 2023 Hiroshi Shimamoto
#include "pico/stdlib.h"
#include "hardware/pwm.h"

int main(void) {
	gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_PWM);

	uint slice_num = pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN);
	pwm_config config = pwm_get_default_config();
	pwm_init(slice_num, &config, true);

	for (;;) {
		for (int level = 0; level < 100; level++) {
			pwm_set_gpio_level(PICO_DEFAULT_LED_PIN, level * 10);
			sleep_ms(10);
		}
		sleep_ms(100);
		for (int level = 100; level >= 0; level--) {
			pwm_set_gpio_level(PICO_DEFAULT_LED_PIN, level * 10);
			sleep_ms(10);
		}
		sleep_ms(100);
	}

	return 0;
}
