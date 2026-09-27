/*
 * bsp.c
 *
 *  Created on: Sep 27, 2026
 *      Author: laurentf
 */

#include "bsp.h"

#include "main.h"

/* Tambouille interne */
typedef struct bsp_ctx_struct {
	volatile bool sw1_pressed;
	volatile bool sw2_pressed;
} bsp_ctx_t;

/* Singleton */
static bsp_ctx_t bsp_ctx;

/* Local prototypes */
static void bsp_led_set(bsp_t * bsp, uint8_t led, bool on);
static bool bsp_sw1_pressed(bsp_t * bsp);
static bool bsp_sw2_pressed(bsp_t * bsp);
static uint32_t bsp_get_tick_ms(bsp_t * bsp);

/* Init à appeler dans le main */
void bsp_init(bsp_t * bsp) {
	bsp->ctx = (void*)&bsp_ctx;

	bsp_ctx.sw1_pressed = false;
	bsp_ctx.sw2_pressed = false;

	bsp->led_set = bsp_led_set;
	bsp->sw1_pressed = bsp_sw1_pressed;
	bsp->sw2_pressed = bsp_sw2_pressed;
	bsp->get_tick_ms = bsp_get_tick_ms;
}

/* Process à appeler à chaque boucle. Vide pour l'instant */
void bsp_process(struct bsp_struct * bsp) {
	(void) bsp;
}

void HAL_GPIO_EXTI_Callback(uint16_t pin) {
	bsp_ctx_t * ctx = &bsp_ctx;

	if (pin == BTN_SW1_Pin) {
		ctx->sw1_pressed = true;
	}
	if (pin == BTN_SW2_Pin) {
		ctx->sw2_pressed = true;
	}
}

static void bsp_led_set(bsp_t * bsp, uint8_t led, bool on) {
	(void) bsp;

	GPIO_PinState state = on ? GPIO_PIN_SET : GPIO_PIN_RESET;
	switch (led) {
	case 0:
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, state);
		break;
	case 1:
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, state);
		break;
	case 2:
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, state);
		break;
	}
}

static bool bsp_sw1_pressed(bsp_t * bsp) {
	bsp_ctx_t * ctx = (bsp_ctx_t *)bsp->ctx;

	bool pressed = ctx->sw1_pressed;
	ctx->sw1_pressed = false;
	return pressed;
}

static bool bsp_sw2_pressed(bsp_t * bsp) {
	bsp_ctx_t * ctx = (bsp_ctx_t *)bsp->ctx;

	bool pressed = ctx->sw2_pressed;
	ctx->sw2_pressed = false;
	return pressed;
}

static uint32_t bsp_get_tick_ms(bsp_t * bsp) {
	(void) bsp;

	return HAL_GetTick();
}
