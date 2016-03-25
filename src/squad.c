// vim: cin:sts=4:sw=4
#include "squad.h"

bool squad_alive(struct t_squad* sq) {

    int alive = 0;

    for (int i=0; i < SQUAD_MAXSIZE; i++){
	if ((sq->squad[i]) && (sq->squad[i]->alive)) alive++;
    }

    return (alive>0);
}

struct t_creature** squad_find_empty(struct t_squad* sq) {
    
    for (int i=0; i < SQUAD_MAXSIZE; i++){
	if (!(sq->squad[i])) return &(sq->squad)[i];
    }

    return NULL;
}

int squad_add(struct t_squad* sq, struct t_creature* cr) {

    struct t_creature** e = squad_find_empty(sq);

    if (e) { *e = cr; return 0; }
    return 1;
}
int squad_del(struct t_squad* sq, struct t_creature* cr) {
    
    for (int i=0; i < SQUAD_MAXSIZE; i++){
	if ( (sq->squad)[i] == cr ) { (sq->squad)[i] = NULL; return 0; }
	    
    }

    return 1;
}
int squad_remove_dead( struct t_squad* sq) {
    
    for (int i=0; i < SQUAD_MAXSIZE; i++){
	if ( (sq->squad[i]) && (!(sq->squad[i]->alive)) ) sq->squad[i] = NULL;
    }
}
