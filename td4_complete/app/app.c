/*
 * app.c
 *
 *  Created on: Sep 27, 2026
 *      Author: laurentf
 */

#include "app.h"

#include <stddef.h>

static const char *const LED_LABELS[3] = {"LED1", "LED2", "LED3"};

static led_toggle_entry_t led_entries[3];
static screen_switch_entry_t neopixel_entry;

static hsv_component_entry_t hue_entry;
static hsv_component_entry_t sat_entry;
static hsv_component_entry_t val_entry;
static screen_switch_entry_t back_entry;

void app_init(app_t *app, bsp_t *bsp) {
	app->bsp = bsp;

	menu_init(&app->main_menu);
	for (int i = 0; i < 3; i++) {
		app->led_on[i] = false;
		led_toggle_entry_init(&led_entries[i], LED_LABELS[i], &app->led_on[i], (uint8_t)i);
		menu_add_entry(&app->main_menu, &led_entries[i].super);
	}
	screen_switch_entry_init(&neopixel_entry, "NEOPIXEL", &app->hsv_menu);
	menu_add_entry(&app->main_menu, &neopixel_entry.super);

	app->hue = 0;
	app->sat = 255;
	app->val = 30;
	menu_init(&app->hsv_menu);
	hsv_component_entry_init(&hue_entry, "HUE", &app->hue, 0, 359, 1);
	hsv_component_entry_init(&sat_entry, "SAT", &app->sat, 0, 255, 1);
	hsv_component_entry_init(&val_entry, "VAL", &app->val, 0, 255, 1);
	screen_switch_entry_init(&back_entry, "BACK", &app->main_menu);
	menu_add_entry(&app->hsv_menu, &hue_entry.super);
	menu_add_entry(&app->hsv_menu, &sat_entry.super);
	menu_add_entry(&app->hsv_menu, &val_entry.super);
	menu_add_entry(&app->hsv_menu, &back_entry.super);

	app->active = &app->main_menu;
}

void app_process(app_t *app) {
	bsp_t * bsp = app->bsp;

	menu_t *next = menu_process(app->active, bsp);
	if (next != NULL) {
		app->active = next;
		menu_enter(app->active);
	}

	bsp->neopixel_set_hsv(bsp, app->hue, app->sat, app->val);

	menu_draw(app->active, bsp);
}
