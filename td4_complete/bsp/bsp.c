/*
 * bsp.c
 *
 *  Created on: Sep 27, 2026
 *      Author: laurentf
 */

#include "bsp.h"

#include <string.h>	// memcpy, strlen

#include "main.h"
#include "tim.h"

#include "neopixel.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

#define BSP_OLED_WIDTH  128
#define BSP_OLED_HEIGHT 64

static const SSD1306_Font_t *const OLED_FONT = &Font_6x8;

/* Tambouille interne */
typedef struct bsp_ctx_struct {
	bool display_present;

	volatile bool sw1_pressed;
	volatile bool sw2_pressed;
	volatile bool enc_pressed;

	int32_t encoder_last_count;
} bsp_ctx_t;

/* Singleton */
static bsp_ctx_t bsp_ctx;

/* Local prototypes */
static void bsp_led_set(bsp_t * bsp, uint8_t led, bool on);
static void bsp_neopixel_set_hsv(bsp_t * bsp, int hue, int sat, int val);
static bool bsp_sw1_pressed(bsp_t * bsp);
static bool bsp_sw2_pressed(bsp_t * bsp);
static uint32_t bsp_get_tick_ms(bsp_t * bsp);

int32_t bsp_encoder_get_delta(bsp_t * bsp);
bool bsp_enc_pressed(bsp_t * bsp);

void bsp_oled_clear(bsp_t * bsp);
void bsp_oled_fill_rect(bsp_t * bsp, int x, int y, int w, int h, bool on);
void bsp_oled_draw_string(bsp_t * bsp, int x, int y, const char *str, bool on);
void bsp_oled_show(bsp_t * bsp);

/* Init à appeler dans le main */
void bsp_init(bsp_t * bsp) {
	bsp->ctx = (void*)&bsp_ctx;
	bsp_ctx_t * ctx = (bsp_ctx_t *)bsp->ctx;

	/* Push buttons */
	ctx->sw1_pressed = false;
	ctx->sw2_pressed = false;

    /* SSD1306 OLED Display (I2C1) */
    if (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c1, SSD1306_I2C_ADDR, 3, 1000)) {
        ctx->display_present = true;
        ssd1306_Init();
    }

    /* Encoder */
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1);
    ctx->encoder_last_count = (int32_t)(int16_t)__HAL_TIM_GET_COUNTER(&htim3);
    ctx->enc_pressed = false;

    /* Neopixel eteint au demarrage */
    np_led_set_all_RGB(0, 0, 0);
    np_led_render();
    HAL_Delay(10); // Le driver DMA a besoin de temps avant le prochain appel

    /* Function pointers initialization */
	bsp->led_set = bsp_led_set;
	bsp->neopixel_set_hsv = bsp_neopixel_set_hsv;
	bsp->sw1_pressed = bsp_sw1_pressed;
	bsp->sw2_pressed = bsp_sw2_pressed;
	bsp->get_tick_ms = bsp_get_tick_ms;

	bsp->encoder_get_delta = bsp_encoder_get_delta;
	bsp->enc_pressed = bsp_enc_pressed;

	bsp->oled_clear = bsp_oled_clear;
	bsp->oled_fill_rect = bsp_oled_fill_rect;
	bsp->oled_draw_string = bsp_oled_draw_string;
	bsp->oled_show = bsp_oled_show;

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
	if (pin == ENCODER_PB_Pin) {
		ctx->enc_pressed = true;
	}
}

/* h: 0..359, s/v: 0..255 */
static void hsv_to_rgb(int h, int s, int v, uint8_t *r, uint8_t *g, uint8_t *b) {
	h = ((h % 360) + 360) % 360;
	if (s <= 0) {
		*r = *g = *b = (uint8_t)v;
		return;
	}

	int region = h / 60;
	int rem = h % 60;
	int p = (v * (255 - s)) / 255;
	int q = (v * (255 - (s * rem) / 60)) / 255;
	int t = (v * (255 - (s * (60 - rem)) / 60)) / 255;

	switch (region) {
	case 0: *r = (uint8_t)v; *g = (uint8_t)t; *b = (uint8_t)p; break;
	case 1: *r = (uint8_t)q; *g = (uint8_t)v; *b = (uint8_t)p; break;
	case 2: *r = (uint8_t)p; *g = (uint8_t)v; *b = (uint8_t)t; break;
	case 3: *r = (uint8_t)p; *g = (uint8_t)q; *b = (uint8_t)v; break;
	case 4: *r = (uint8_t)t; *g = (uint8_t)p; *b = (uint8_t)v; break;
	default: *r = (uint8_t)v; *g = (uint8_t)p; *b = (uint8_t)q; break;
	}
}

static void bsp_neopixel_set_hsv(bsp_t * bsp, int hue, int sat, int val) {
	(void) bsp;

	uint8_t r, g, b;
	hsv_to_rgb(hue, sat, val, &r, &g, &b);
	np_led_set_all_RGB(r, g, b);
	np_led_render();
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

int32_t bsp_encoder_get_delta(bsp_t * bsp) {
    bsp_ctx_t * ctx = (bsp_ctx_t *)bsp->ctx;

    int32_t count = (int32_t)(int16_t)__HAL_TIM_GET_COUNTER(&htim3);
    int32_t delta = count - ctx->encoder_last_count;
    ctx->encoder_last_count = count;

    return delta;
}

bool bsp_enc_pressed(bsp_t * bsp) {
	bsp_ctx_t * ctx = (bsp_ctx_t *)bsp->ctx;

    bool pressed = ctx->enc_pressed;
    ctx->enc_pressed = false;
    return pressed;
}

void bsp_oled_clear(bsp_t * bsp) {
	bsp_ctx_t * ctx = (bsp_ctx_t *)bsp->ctx;

    if (!ctx->display_present) {
        return;
    }
    ssd1306_Fill(Black);
}

void bsp_oled_fill_rect(bsp_t * bsp, int x, int y, int w, int h, bool on) {
	bsp_ctx_t * ctx = (bsp_ctx_t *)bsp->ctx;

    if (!ctx->display_present) {
        return;
    }
    if (w <= 0 || h <= 0) {
        return;
    }

    int x1 = x < 0 ? 0 : x;
    int y1 = y < 0 ? 0 : y;
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    if (x2 >= BSP_OLED_WIDTH) {
        x2 = BSP_OLED_WIDTH - 1;
    }
    if (y2 >= BSP_OLED_HEIGHT) {
        y2 = BSP_OLED_HEIGHT - 1;
    }
    if (x1 > x2 || y1 > y2) {
        return;
    }

    ssd1306_FillRectangle((uint8_t)x1, (uint8_t)y1, (uint8_t)x2, (uint8_t)y2, on ? White : Black);
}

void bsp_oled_draw_string(bsp_t * bsp, int x, int y, const char *str, bool on) {
	bsp_ctx_t * ctx = (bsp_ctx_t *)bsp->ctx;

    if (!ctx->display_present) {
        return;
    }
    if (x < 0 || y < 0 || x >= BSP_OLED_WIDTH || y >= BSP_OLED_HEIGHT) {
        return;
    }

    char buf[32];
    size_t n = strlen(str);
    if (n >= sizeof(buf)) {
        n = sizeof(buf) - 1;
    }
    memcpy(buf, str, n);
    buf[n] = '\0';

    ssd1306_SetCursor((uint8_t)x, (uint8_t)y);
    ssd1306_WriteString(buf, *OLED_FONT, on ? White : Black);
}

void bsp_oled_show(bsp_t * bsp) {
	bsp_ctx_t * ctx = (bsp_ctx_t *)bsp->ctx;

    if (!ctx->display_present) {
        return;
    }
    ssd1306_UpdateScreen();
}

