/*
 * neopixel.h
 *
 *  Created on: Jul 9, 2026
 *      Author: laurentf
 */

#ifndef BIDULE_DRIVERS_DRV_BIDULE_NEOPIXEL_H_
#define BIDULE_DRIVERS_DRV_BIDULE_NEOPIXEL_H_

#include <stdint.h>

void np_led_set_RGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void np_led_set_RGBW(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void np_led_set_all_RGB(uint8_t r, uint8_t g, uint8_t b);
void np_led_set_all_RGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void np_led_render();

#endif /* BIDULE_DRIVERS_DRV_BIDULE_NEOPIXEL_H_ */
