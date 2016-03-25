// vim: cin:sts=4:sw=4 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "cpairs.h"
#include "mapmode.h"
#include "globals.h"
#include "entity_types.h"
#include "squad.h"

int init_memory() {

    memset(pool,0,sizeof(struct t_creature*) * POOLSIZE);
    return 0;
}

void inthandler(int sig) {
    endwin();
        exit(0);
}

int main(int argc, char** argv) {

    srand(time(NULL));

    int opt = -1;

    dbgmode = 0;

    while ((opt = getopt(argc, argv, "dv")) != -1) {
	switch (opt) {
	    case 'd':
		dbgmode = 1;
		printf("Press ENTER to start (feel free to attach a debugger to this process at this moment).\n");
		getc(stdin);
		break;
	    case 'v':
		printf("insert version info here\n");
		exit(0);
		break;
	    case '?':
	    default: /* '?' */
		fprintf(stderr, "Usage: %s [-d] [-v]\n"
			"\n"
			" -d : Debug mode.\n"
			" -v : Version info.\n"
			"\n"
			,argv[0]);
		exit(0);
	}
    }


    initscr();	
    cbreak();
    noecho();

    refresh();

    signal(SIGINT,inthandler);

    init_memory();
    init_pairs();

    struct t_squad squad1;

    memset(&squad1,0,sizeof squad1);
    strcpy(squad1.name, "Liberal Crime Squad");

    squad1.id = 0;

    struct t_creature player1;
    creature_init(&player1, &type_rules[ET_POLITICALACTIVIST]);   

    squad1.squad[0] = &player1;
    //activesquad = &squad1;

    while (squad_alive(&squad1)) {

    mapmode(&squad1);

    }

    endwin();

    return 0;

}
