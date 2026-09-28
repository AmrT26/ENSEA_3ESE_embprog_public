#include "bsp.h"

#include <SDL3/SDL.h>

#include "buttons.h"
#include "led.h"

#define WINDOW_W 340
#define WINDOW_H 340

/* Etat prive du simulateur : invisible pour l'app, qui ne voit que les
 * champs de bsp_t (bsp.h). Singleton statique, comme bsp_ctx_t cote
 * materiel (td3_complete/bsp/bsp.c) -- ctx pointe dessus. */
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool quit_requested;

    bsp_led_t led;
    bsp_buttons_t buttons;
} bsp_priv_t;

static bsp_priv_t bsp_priv;

static void render(bsp_priv_t *priv) {
    SDL_SetRenderDrawColor(priv->renderer, 25, 26, 30, 255);
    SDL_RenderClear(priv->renderer);

    bsp_led_render(&priv->led, priv->renderer);
    bsp_buttons_render(&priv->buttons, priv->renderer);

    SDL_SetRenderDrawColor(priv->renderer, 150, 150, 155, 255);
    SDL_RenderDebugText(priv->renderer, 20, WINDOW_H - 25, "A/Z: boutons poussoirs SW1/SW2 (ou clic)");

    SDL_RenderPresent(priv->renderer);
}

/* --- API exposee a l'application (voir bsp.h) --------------------------- */

static void bsp_led_set_impl(bsp_t *bsp, uint8_t led, bool on) {
    bsp_priv_t *priv = (bsp_priv_t *)bsp->ctx;
    bsp_led_write(&priv->led, led, on);
}

static bool bsp_sw1_pressed_impl(bsp_t *bsp) {
    bsp_priv_t *priv = (bsp_priv_t *)bsp->ctx;
    return bsp_buttons_take_sw1(&priv->buttons);
}

static bool bsp_sw2_pressed_impl(bsp_t *bsp) {
    bsp_priv_t *priv = (bsp_priv_t *)bsp->ctx;
    return bsp_buttons_take_sw2(&priv->buttons);
}

static uint32_t bsp_get_tick_ms_impl(bsp_t *bsp) {
    (void)bsp;
    return (uint32_t)SDL_GetTicks();
}

/* --- Cycle de vie (reserve a main.c) ------------------------------------ */

bool bsp_init(bsp_t *bsp) {
    bsp_priv_t *priv = &bsp_priv;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return false;
    }

    priv->window = SDL_CreateWindow("BSP simulator - td3 (superloop + BSP)", WINDOW_W, WINDOW_H, 0);
    if (!priv->window) {
        SDL_Quit();
        return false;
    }

    priv->renderer = SDL_CreateRenderer(priv->window, NULL);
    if (!priv->renderer) {
        SDL_DestroyWindow(priv->window);
        SDL_Quit();
        return false;
    }
    SDL_SetRenderVSync(priv->renderer, 1);

    priv->quit_requested = false;
    bsp_led_init(&priv->led);
    bsp_buttons_init(&priv->buttons);

    bsp->ctx = priv;
    bsp->led_set = bsp_led_set_impl;
    bsp->sw1_pressed = bsp_sw1_pressed_impl;
    bsp->sw2_pressed = bsp_sw2_pressed_impl;
    bsp->get_tick_ms = bsp_get_tick_ms_impl;

    render(priv);
    return true;
}

void bsp_deinit(bsp_t *bsp) {
    bsp_priv_t *priv = (bsp_priv_t *)bsp->ctx;

    if (priv->renderer) {
        SDL_DestroyRenderer(priv->renderer);
        priv->renderer = NULL;
    }
    if (priv->window) {
        SDL_DestroyWindow(priv->window);
        priv->window = NULL;
    }
    SDL_Quit();
}

bool bsp_process(bsp_t *bsp) {
    bsp_priv_t *priv = (bsp_priv_t *)bsp->ctx;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                priv->quit_requested = true;
                break;
            case SDL_EVENT_KEY_DOWN:
                bsp_buttons_on_key(&priv->buttons, event.key.key, true, event.key.repeat);
                break;
            case SDL_EVENT_KEY_UP:
                bsp_buttons_on_key(&priv->buttons, event.key.key, false, false);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    bsp_buttons_on_click(&priv->buttons, event.button.x, event.button.y, true);
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    bsp_buttons_on_click(&priv->buttons, event.button.x, event.button.y, false);
                }
                break;
            default:
                break;
        }
    }

    render(priv);

    return !priv->quit_requested;
}
