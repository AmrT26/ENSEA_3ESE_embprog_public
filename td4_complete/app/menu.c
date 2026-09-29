/*
 * menu.c
 *
 *  Created on: Sep 29, 2026
 *      Author: laurentf
 */

#include "menu.h"

#include <stdio.h>

/* Highlighted background + selection marker: identical for every concrete
 * entry, so it's shared here rather than duplicated in each *_draw below --
 * a bit like a protected helper a base class would offer its subclasses. */
static void menu_entry_draw_row(bsp_t *bsp, int y, bool selected, bool editing, const char *line) {
	if (selected) {
		bsp->oled_fill_rect(bsp, 0, y, 127, 13, true);
		bsp->oled_draw_string(bsp, 4, y + 2, editing ? "*" : ">", false);
	}
	bsp->oled_draw_string(bsp, 16, y + 2, line, !selected);
}

/* --- CONCRETE ENTRY: led_toggle_entry_t ---------------------------------- */

void led_toggle_entry_init(led_toggle_entry_t *e, const char *label, bool *led_on, uint8_t led_index) {
	e->super.kind = MENU_ENTRY_LED_TOGGLE;
	e->super.label = label;
	e->led_on = led_on;
	e->led_index = led_index;
}

static void led_toggle_entry_toggle(led_toggle_entry_t *e, bsp_t *bsp) {
	*e->led_on = !*e->led_on;
	bsp->led_set(bsp, e->led_index, *e->led_on);
}

static void led_toggle_entry_draw(const led_toggle_entry_t *e, bsp_t *bsp, int y, bool selected) {
	char line[20];
	snprintf(line, sizeof(line), "%s: %s", e->super.label, *e->led_on ? "ON" : "OFF");
	menu_entry_draw_row(bsp, y, selected, false, line);
}

/* --- CONCRETE ENTRY: screen_switch_entry_t ------------------------------- */

void screen_switch_entry_init(screen_switch_entry_t *e, const char *label, menu_t *target) {
	e->super.kind = MENU_ENTRY_SCREEN_SWITCH;
	e->super.label = label;
	e->target = target;
}

static void screen_switch_entry_draw(const screen_switch_entry_t *e, bsp_t *bsp, int y, bool selected) {
	menu_entry_draw_row(bsp, y, selected, false, e->super.label);
}

/* --- CONCRETE ENTRY: hsv_component_entry_t ------------------------------- */

void hsv_component_entry_init(hsv_component_entry_t *e, const char *label, int *value, int min, int max, int step) {
	e->super.kind = MENU_ENTRY_HSV_COMPONENT;
	e->super.label = label;
	e->value = value;
	e->min = min;
	e->max = max;
	e->step = step;
}

static void hsv_component_entry_adjust(hsv_component_entry_t *e, int32_t delta) {
	int value = *e->value + (int)delta * e->step;
	if (value < e->min) {
		value = e->min;
	}
	if (value > e->max) {
		value = e->max;
	}
	*e->value = value;
}

static void hsv_component_entry_draw(const hsv_component_entry_t *e, bsp_t *bsp, int y, bool selected, bool editing) {
	char line[20];
	snprintf(line, sizeof(line), "%s: %d", e->super.label, *e->value);
	menu_entry_draw_row(bsp, y, selected, editing, line);
}

/* --- menu_entry_t: polymorphic dispatch ---------------------------------- */
/* The one place in the whole program that knows every concrete entry kind
 * exists -- exactly like Shape_draw/Shape_area's switch in the slides.
 * menu_process/menu_draw below only ever call through these three
 * functions. */

menu_t *menu_entry_activate(menu_entry_t *self, bsp_t *bsp) {
	switch (self->kind) {
	case MENU_ENTRY_LED_TOGGLE:
		led_toggle_entry_toggle((led_toggle_entry_t *)self, bsp);
		return NULL;
	case MENU_ENTRY_SCREEN_SWITCH:
		return ((screen_switch_entry_t *)self)->target;
	case MENU_ENTRY_HSV_COMPONENT:
		return NULL; /* editable: activate() never runs, see menu_process */
	}
	return NULL;
}

void menu_entry_adjust(menu_entry_t *self, int32_t delta) {
	switch (self->kind) {
	case MENU_ENTRY_LED_TOGGLE:
	case MENU_ENTRY_SCREEN_SWITCH:
		break; /* not editable, nothing to adjust */
	case MENU_ENTRY_HSV_COMPONENT:
		hsv_component_entry_adjust((hsv_component_entry_t *)self, delta);
		break;
	}
}

void menu_entry_draw(const menu_entry_t *self, bsp_t *bsp, int y, bool selected, bool editing) {
	switch (self->kind) {
	case MENU_ENTRY_LED_TOGGLE:
		led_toggle_entry_draw((const led_toggle_entry_t *)self, bsp, y, selected);
		break;
	case MENU_ENTRY_SCREEN_SWITCH:
		screen_switch_entry_draw((const screen_switch_entry_t *)self, bsp, y, selected);
		break;
	case MENU_ENTRY_HSV_COMPONENT:
		hsv_component_entry_draw((const hsv_component_entry_t *)self, bsp, y, selected, editing);
		break;
	}
}

/* --- MENU LOGIC ----------------------------------------------------------- */

void menu_init(menu_t *m) {
	m->count = 0;
	m->cursor = 0;
	m->editing = false;
}

void menu_add_entry(menu_t *m, menu_entry_t *entry) {
	m->entries[m->count++] = entry;
}

void menu_enter(menu_t *m) {
	m->cursor = 0;
	m->editing = false;
}

menu_t *menu_process(menu_t *m, bsp_t *bsp) {
	int32_t delta = bsp->encoder_get_delta(bsp);
	menu_entry_t *entry = m->entries[m->cursor];

	if (m->editing) {
		if (delta != 0) {
			menu_entry_adjust(entry, delta);
		}
		if (bsp->enc_pressed(bsp)) {
			m->editing = false;
		}
		return NULL;
	}

	if (delta != 0) {
		m->cursor += (int)delta;
		if (m->cursor < 0) {
			m->cursor = 0;
		}
		if (m->cursor >= m->count) {
			m->cursor = m->count - 1;
		}
		entry = m->entries[m->cursor];
	}

	if (bsp->enc_pressed(bsp)) {
		if (entry->kind == MENU_ENTRY_HSV_COMPONENT) {	// Editable
			m->editing = true;
			return NULL;
		}
		return menu_entry_activate(entry, bsp);
	}

	return NULL;
}

void menu_draw(const menu_t *m, bsp_t *bsp) {
	bsp->oled_clear(bsp);

	for (int row = 0; row < m->count; row++) {
		int y = 4 + row * 15;
		bool selected = (row == m->cursor);
		bool editing = selected && m->editing;
		menu_entry_draw(m->entries[row], bsp, y, selected, editing);
	}

	bsp->oled_show(bsp);
}
