#!/bin/sh
tcc src/armor.c src/cpairs.c src/entity.c src/entity_name.c src/entity_types.c src/fight.c src/globals.c src/item.c src/item_ui.c src/location.c src/log.c src/map_actions.c src/map_ai.c src/map_ai_hm.c src/map_fov.c src/map_path.c src/map_ui.c src/mapgen.c src/mapmode.c src/pqueue.c src/queue.c src/random.c src/squad.c src/stealth.c src/ui.c src/weapon.c -lncurses -g -run src/main.c
