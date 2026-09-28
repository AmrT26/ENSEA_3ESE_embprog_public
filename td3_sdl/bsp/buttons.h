#ifndef BSP_BUTTONS_H
#define BSP_BUTTONS_H

#include <stdbool.h>

#include <SDL3/SDL.h>

/* Module simulateur des 2 boutons poussoirs SW1/SW2 : touches A/Z ou clic
 * sur les rectangles a l'ecran. */

typedef struct {
    bool sw1_edge;
    bool sw2_edge;
    bool sw1_held;
    bool sw2_held;
} bsp_buttons_t;

void bsp_buttons_init(bsp_buttons_t *btn);
void bsp_buttons_on_key(bsp_buttons_t *btn, SDL_Keycode key, bool down, bool repeat);
void bsp_buttons_on_click(bsp_buttons_t *btn, float x, float y, bool down);
bool bsp_buttons_take_sw1(bsp_buttons_t *btn);
bool bsp_buttons_take_sw2(bsp_buttons_t *btn);
void bsp_buttons_render(const bsp_buttons_t *btn, SDL_Renderer *renderer);

#endif /* BSP_BUTTONS_H */
