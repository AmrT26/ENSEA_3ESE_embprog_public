/*
 * app.h
 *
 *  Created on: Sep 27, 2026
 *      Author: laurentf
 */

#ifndef TD3_COMPLETE_APP_APP_H_
#define TD3_COMPLETE_APP_APP_H_

#include "bsp.h"

typedef struct {
	bsp_t * bsp;
	uint32_t last_tick;
	bool led1_on, led2_on, led3_on;
} app_t;

void app_init(app_t *app, bsp_t *bsp);
void app_process(app_t *app);


#endif /* TD3_COMPLETE_APP_APP_H_ */
