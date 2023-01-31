#ifndef _INPUT_HANDLER
#define _INPUT_HANDLER

#include <ncurses.h>

#include "editor_interface.h"


//function declarations
int handle_KEY_DOWN(WINDOW *win);
int handle_KEY_UP(WINDOW *win);
int handle_KEY_RIGHT(WINDOW *win);
int handle_KEY_LEFT(WINDOW *win);
int handle_KEY_RET(WINDOW *win);
int handle_KEY_SPC(WINDOW *win, enum state_enums state, int win_width, int win_height);
int handle_KEY_DC(WINDOW *win, enum state_enums state, int win_width);
int handle_KEY_BACKSPACE(WINDOW *win, enum state_enums state, int win_width);
int handle_char_input(WINDOW *win, int ch, enum state_enums state, int win_width, int win_height);


#endif //_INPUT_HANDLER
