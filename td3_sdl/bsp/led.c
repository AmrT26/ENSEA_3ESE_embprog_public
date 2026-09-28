#include "led.h"

#include <string.h>

static const SDL_FRect RECT_LED[BSP_LED_COUNT] = {
    {30,  20, 24, 24},
    {70,  20, 24, 24},
    {110, 20, 24, 24},
};

void bsp_led_init(bsp_led_t *led) {
    memset(led->on, 0, sizeof(led->on));
}

void bsp_led_write(bsp_led_t *led, uint8_t index, bool on) {
    if (index < BSP_LED_COUNT) {
        led->on[index] = on;
    }
}

void bsp_led_render(const bsp_led_t *led, SDL_Renderer *renderer) {
    for (int i = 0; i < BSP_LED_COUNT; i++) {
        if (led->on[i]) {
            SDL_SetRenderDrawColor(renderer, 230, 60, 40, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 60, 30, 28, 255);
        }
        SDL_RenderFillRect(renderer, &RECT_LED[i]);
        SDL_SetRenderDrawColor(renderer, 150, 150, 155, 255);
        SDL_RenderRect(renderer, &RECT_LED[i]);

        char label[8];
        SDL_snprintf(label, sizeof(label), "LED%d", i + 1);
        SDL_SetRenderDrawColor(renderer, 200, 200, 205, 255);
        SDL_RenderDebugText(renderer, RECT_LED[i].x - 2, RECT_LED[i].y + 28, label);
    }
}
