#ifndef BSP_LED_H
#define BSP_LED_H

#include <stdbool.h>
#include <stdint.h>

#include <SDL3/SDL.h>

#include "bsp_defs.h"

/* Module simulateur des 3 LEDs. Toutes les fonctions recoivent l'etat
 * qu'elles manipulent par pointeur : pas de variable globale. */

typedef struct {
    bool on[BSP_LED_COUNT];
} bsp_led_t;

void bsp_led_init(bsp_led_t *led);
void bsp_led_write(bsp_led_t *led, uint8_t index, bool on);
void bsp_led_render(const bsp_led_t *led, SDL_Renderer *renderer);

#endif /* BSP_LED_H */
