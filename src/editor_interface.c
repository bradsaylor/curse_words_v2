#include <string.h>
#include <ncurses.h>
#include <assert.h>
#include <ctype.h>

#include "editor_interface.h"
#include "init.h"
#include "editor_int_input_handler.h"
#include "editor_int_helper_funcs.h"


int editor_interface(WINDOW *win, int win_width, int win_height, char *output_str){
  int ch = 0;
  enum state_enums state;
  
  while((ch = wgetch(win)) != CTRL_TIL){
    
    get_state_enums(win, &state, win_width, win_height);
    
    switch(ch){
    case KEY_DOWN:
      handle_KEY_DOWN(win);
      break;
    case KEY_UP:
      handle_KEY_UP(win);
      break;
    case KEY_RIGHT:
      handle_KEY_RIGHT(win);
      break;
    case KEY_LEFT:
      handle_KEY_LEFT(win);
      break;
    case KEY_RET:
      handle_KEY_RET(win);
      break;
    case KEY_SPC:
      handle_KEY_SPC(win, state, win_width, win_height);
      break;
    case KEY_DC:
      handle_KEY_DC(win, state, win_width);
      break;
    case KEY_BACKSPACE:
      handle_KEY_BACKSPACE(win, state, win_width);
      break;
    default:
      handle_char_input(win, ch, state, win_width, win_height);
      break;
    }
  }

  dump_editor_string(win, output_str, win_width, win_height);
  
  return 0;
}

int get_state_enums(WINDOW *win, enum state_enums *state, int win_width, int win_height){
  int x_pos = 0, y_pos = 0;

  *state = 0;
  getyx(win, y_pos, x_pos);

  if(x_pos == 0)              (*state) |= FIRST_COL;
  if(x_pos == win_width - 1)  (*state) |= LAST_COL;
  if(y_pos == 0)              (*state) |= FIRST_ROW;
  if(y_pos == win_height - 1) (*state) |= LAST_ROW;
  if(check_last_col(win))     (*state) |= WRAP_FLAG;

  return 0;
}

int wrap_str_trigger(WINDOW *win, int y_pos, int x_pos, char *row_str, int win_width, int win_height){
  int wrap_index = 0;
  char wrap_str[2 * win_width];

  wrap_index = find_wrap_index(win, row_str);
  split_wrap_str(row_str, wrap_str, wrap_index);
  copy_str_clear_eol(win, row_str);

  wrap_str_recursive(win, y_pos, x_pos, wrap_str, win_width, win_height);
  
  move_curs_after_wrap(win, y_pos, x_pos, wrap_index, KEY_GENERAL);

  return 0;
}

int wrap_str_recursive(WINDOW *win, int y_pos, int x_pos, char *wrap_str, int win_width, int win_height){
  char next_row_str[2 * win_width];
  enum space_enum SPC_FLAG = NO_SPC;

  get_next_row_str(win, next_row_str);
  if(check_next_line_begin_char(win, y_pos, x_pos)) SPC_FLAG = ADD_SPC;
  combine_wrap_str(wrap_str, next_row_str, SPC_FLAG);
  wmove(win, y_pos + 1, x_pos);

  if(check_wrap_str_length(win, wrap_str)){
    wrap_str_trigger(win, y_pos + 1, 0, wrap_str, win_width, win_height);
  } else {
    mvwaddstr(win, y_pos + 1, 0, wrap_str);
  }
  
  return 0;
}

int input_char(WINDOW *win, int argc, char *argv[]){
  static int argv_index = 0;
  int ch = 0;

  if(argc == 2){
    if(argv_index == (int)strlen(argv[1])){
      argc = 1;
    }else{
      ch = argv[1][argv_index];
      argv_index++;
    }
  }
  if(argc == 1) {
    ch = wgetch(win);
  }

  return ch;
}

