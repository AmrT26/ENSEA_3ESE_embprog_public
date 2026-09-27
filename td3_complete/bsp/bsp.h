/*
 * bsp.h
 *
 *  Created on: Sep 27, 2026
 *      Author: laurentf
 */

#ifndef TD3_COMPLETE_BSP_BSP_H_
#define TD3_COMPLETE_BSP_BSP_H_

#include <stdint.h>
#include <stdbool.h>

/* BSP vue par l'app */
typedef struct bsp_struct bsp_t;

struct bsp_struct {
	void * ctx;

	void (*led_set)(bsp_t * bsp, uint8_t led, bool on);
	bool (*sw1_pressed)(bsp_t * bsp);
	bool (*sw2_pressed)(bsp_t * bsp);
	uint32_t (*get_tick_ms)(bsp_t * bsp);
};

/* Fonctions à appeler dans le main */
void bsp_init(bsp_t * bsp);
void bsp_process(bsp_t * bsp);

#endif /* TD3_COMPLETE_BSP_BSP_H_ */
