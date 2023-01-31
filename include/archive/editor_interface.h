#ifndef _EDITOR_H
#define _EDITOR_H

#include <ncurses.h>

#define KEY_RET 10
#define KEY_SPC 32
#define CTRL_TIL 0
#define KEY_GENERAL 0

enum state_enums {
  FIRST_COL = 0b00000001,
  LAST_COL  = 0b00000010,
  FIRST_ROW = 0b00000100,
  LAST_ROW  = 0b00001000,
  WRAP_FLAG = 0b00010000,
};

enum space_enum {
  NO_SPC,
  ADD_SPC,
};


//function prototypes
int editor_interface(WINDOW *win, int win_width, int win_height, char *output_str);
int input_char(WINDOW *win, int argc, char *argv[]);
int get_state_enums(WINDOW *win, enum state_enums *state, int win_width, int win_height);
int wrap_str_trigger(WINDOW *win, int y_pos, int x_pos, char *row_str, int win_width, int win_height);
int wrap_str_recursive(WINDOW *win, int y_pos, int x_pos, char *wrap_str, int win_width, int win_height);


#endif //_EDITOR_H
