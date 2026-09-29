/*
 * menu.h
 *
 *  Created on: Sep 29, 2026
 *      Author: laurentf
 */

#ifndef TD4_COMPLETE_APP_MENU_H_
#define TD4_COMPLETE_APP_MENU_H_

#include <stdint.h>
#include <stdbool.h>

#include "bsp.h"

/* MENU ENTRY -- base "class" : a tag saying which concrete entry this
 * really is, plus what every entry has in common. Every concrete entry
 * embeds this as its *first* field ("inheritance"), so a pointer to a
 * concrete entry is always also a valid menu_entry_t*. */

typedef struct menu menu_t; /* defined below; entries only need the pointer type here */

typedef enum {
	MENU_ENTRY_LED_TOGGLE,
	MENU_ENTRY_SCREEN_SWITCH,
	MENU_ENTRY_HSV_COMPONENT,
} menu_entry_kind_t;

typedef struct {
	menu_entry_kind_t kind;
	const char *label;
} menu_entry_t;

/* Polymorphic operations: one call site per operation, declared once here on
 * the base type. Each dispatches on `kind` (see menu.c) to the matching
 * concrete entry's behaviour -- callers (menu_process/menu_draw) never need
 * to know which concrete entry they're talking to. */
menu_t *menu_entry_activate(menu_entry_t *self, bsp_t *bsp);
void menu_entry_adjust(menu_entry_t *self, int32_t delta);
void menu_entry_draw(const menu_entry_t *self, bsp_t *bsp, int y, bool selected, bool editing);

/* CONCRETE ENTRY: toggles one LED on/off */
typedef struct {
	menu_entry_t super; /* inherited menu_entry_t */

	bool *led_on;
	uint8_t led_index;
} led_toggle_entry_t;

void led_toggle_entry_init(led_toggle_entry_t *e, const char *label, bool *led_on, uint8_t led_index);

/* CONCRETE ENTRY: navigates to another menu_t (used both to open the HSV
 * page and to come back from it) */
typedef struct {
	menu_entry_t super; /* inherited menu_entry_t */

	menu_t *target;
} screen_switch_entry_t;

void screen_switch_entry_init(screen_switch_entry_t *e, const char *label, menu_t *target);

/* CONCRETE ENTRY: edits one bounded int value (used for HUE/SAT/VAL) */
typedef struct {
	menu_entry_t super; /* inherited menu_entry_t */

	int *value;
	int min;
	int max;
	int step;
} hsv_component_entry_t;

void hsv_component_entry_init(hsv_component_entry_t *e, const char *label, int *value, int min, int max, int step);

/* MENU LOGIC -- a list of entries; talks to them only through the
 * menu_entry_* polymorphic operations above, never through a concrete type. */

#define MAX_ENTRIES 4

struct menu {
	menu_entry_t *entries[MAX_ENTRIES];
	int count;
	int cursor;
	bool editing;
};

void menu_init(menu_t *m);
void menu_add_entry(menu_t *m, menu_entry_t *entry);
void menu_enter(menu_t *m);

/* Returns the menu to switch to if a screen_switch entry was just
 * activated, or NULL to stay on `m`. */
menu_t *menu_process(menu_t *m, bsp_t *bsp);
void menu_draw(const menu_t *m, bsp_t *bsp);

#endif /* TD4_COMPLETE_APP_MENU_H_ */
