#include "app.h"
#include "bsp/bsp.h"

/*
 * Point de composition : seul endroit du projet a connaitre a la fois
 * bsp.h et app.h. Cree bsp_t sur la pile (aucune variable globale cote
 * app), puis orchestre le meme superloop que td3 (bsp_process puis
 * app_process), ici tournant sur le simulateur au lieu de la carte.
 */
int main(void) {
    bsp_t bsp;
    if (!bsp_init(&bsp)) {
        return 1;
    }

    app_t app;
    app_init(&app, &bsp);

    while (bsp_process(&bsp)) {
        app_process(&app);
    }

    bsp_deinit(&bsp);
    return 0;
}
