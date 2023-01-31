#ifndef _INIT_H
#define _INIT_H

#include <ncurses.h>

#include "display_interface.h"

//function declarations
int init_ncurses();
int exit_ncurses();
int init_windows(win_type **win_array, int win_array_size);

#endif //_INIT_H
