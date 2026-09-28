# td3_sdl

Simulateur du TD3 : même code applicatif (`app/`) que `td3_complete`,
tournant sur une BSP simulée avec SDL3 au lieu de la carte `bidule`.

## Prérequis

SDL3 doit être installable via `pkg-config` (c'est ce que `find_package
(SDL3)` utilise). 

Sur une distribution où SDL3 n'est pas packagé, compiler et installer SDL3
depuis les sources (voir https://github.com/libsdl-org/SDL).

## Compiler

```sh
cmake -S . -B build
cmake --build build
```

## Exécuter

```sh
./build/td3_sdl
```

Une fenêtre s'ouvre avec les 3 LEDs et les 2 boutons poussoirs (SW1/SW2).
Commandes :

- Touches `A` / `Z` : appui sur SW1 / SW2
- Clic souris sur les rectangles SW1 / SW2 : idem
- Fermer la fenêtre pour quitter
