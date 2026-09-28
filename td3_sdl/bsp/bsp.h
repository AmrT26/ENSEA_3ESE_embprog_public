#ifndef BSP_H
#define BSP_H

#include <stdbool.h>
#include <stdint.h>

/*
 * BSP vue par l'app -- meme struct (memes noms de champs) que la BSP
 * materielle de td3 (td/td3_complete/bsp/bsp.h) : app/ (repris tel quel,
 * voir CMakeLists.txt) n'a aucune idee qu'elle tourne sur un simulateur
 * plutot que sur la carte.
 *
 * REGLE : c'est bsp.c (et les modules qu'il assemble) le seul a inclure
 * SDL3 et a savoir que l'on tourne dans un simulateur -- pas ce header.
 */
typedef struct bsp_struct bsp_t;

struct bsp_struct {
    void *ctx;

    void (*led_set)(bsp_t *bsp, uint8_t led, bool on);
    bool (*sw1_pressed)(bsp_t *bsp);
    bool (*sw2_pressed)(bsp_t *bsp);
    uint32_t (*get_tick_ms)(bsp_t *bsp);
};

/* Cycle de vie du simulateur (fenetre SDL a ouvrir/fermer, boucle
 * d'evenements a depiler) : n'existe pas cote materiel, reserve a main.c. */
bool bsp_init(bsp_t *bsp);
void bsp_deinit(bsp_t *bsp);
bool bsp_process(bsp_t *bsp);

#endif /* BSP_H */
