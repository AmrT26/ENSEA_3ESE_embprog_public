#include "buttons.h"

static const SDL_FRect RECT_SW1 = {30,  240, 120, 60};
static const SDL_FRect RECT_SW2 = {180, 240, 120, 60};

static void draw_button(SDL_Renderer *renderer, const SDL_FRect *rect, bool held, const char *label) {
    if (held) {
        SDL_SetRenderDrawColor(renderer, 90, 130, 180, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 55, 60, 70, 255);
    }
    SDL_RenderFillRect(renderer, rect);
    SDL_SetRenderDrawColor(renderer, 170, 175, 185, 255);
    SDL_RenderRect(renderer, rect);

    float tx = rect->x + rect->w / 2.0f - (float)SDL_strlen(label) * 4.0f;
    float ty = rect->y + rect->h / 2.0f - 4.0f;
    SDL_SetRenderDrawColor(renderer, 235, 235, 240, 255);
    SDL_RenderDebugText(renderer, tx, ty, label);
}

void bsp_buttons_init(bsp_buttons_t *btn) {
    btn->sw1_edge = false;
    btn->sw2_edge = false;
    btn->sw1_held = false;
    btn->sw2_held = false;
}

void bsp_buttons_on_key(bsp_buttons_t *btn, SDL_Keycode key, bool down, bool repeat) {
    switch (key) {
        case SDLK_A:
            btn->sw1_held = down;
            if (down && !repeat) {
                btn->sw1_edge = true;
            }
            break;
        case SDLK_Z:
            btn->sw2_held = down;
            if (down && !repeat) {
                btn->sw2_edge = true;
            }
            break;
        default:
            break;
    }
}

void bsp_buttons_on_click(bsp_buttons_t *btn, float x, float y, bool down) {
    SDL_FPoint p = {x, y};

    bool in_sw1 = SDL_PointInRectFloat(&p, &RECT_SW1);
    bool in_sw2 = SDL_PointInRectFloat(&p, &RECT_SW2);

    if (down) {
        if (in_sw1) {
            btn->sw1_held = true;
            btn->sw1_edge = true;
        }
        if (in_sw2) {
            btn->sw2_held = true;
            btn->sw2_edge = true;
        }
    } else {
        btn->sw1_held = false;
        btn->sw2_held = false;
    }
}

bool bsp_buttons_take_sw1(bsp_buttons_t *btn) {
    bool edge = btn->sw1_edge;
    btn->sw1_edge = false;
    return edge;
}

bool bsp_buttons_take_sw2(bsp_buttons_t *btn) {
    bool edge = btn->sw2_edge;
    btn->sw2_edge = false;
    return edge;
}

void bsp_buttons_render(const bsp_buttons_t *btn, SDL_Renderer *renderer) {
    draw_button(renderer, &RECT_SW1, btn->sw1_held, "SW1");
    draw_button(renderer, &RECT_SW2, btn->sw2_held, "SW2");
}
