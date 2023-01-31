#include <string.h>

#include "editor_int_input_handler.h"
#include "editor_int_helper_funcs.h"
#include "editor_interface.h"

int handle_KEY_DOWN(WINDOW *win){
  int x_pos = 0, y_pos = 0;

  getyx(win, y_pos, x_pos);
  wmove(win, y_pos + 1, x_pos);
  
  return 0;
}

int handle_KEY_UP(WINDOW *win){
  int x_pos = 0, y_pos = 0;

  getyx(win, y_pos, x_pos);
  wmove(win, y_pos - 1, x_pos);

  return 0;
}

int handle_KEY_RIGHT(WINDOW *win){
  int x_pos = 0, y_pos = 0;

  getyx(win, y_pos, x_pos);
  wmove(win, y_pos, x_pos + 1);

  return 0;
}

int handle_KEY_LEFT(WINDOW *win){
  int x_pos = 0, y_pos = 0;

  getyx(win, y_pos, x_pos);
  wmove(win, y_pos, x_pos - 1);

  return 0;
}

int handle_KEY_RET(WINDOW *win){
  int x_pos = 0, y_pos = 0;
  int win_width = 0, win_height = 0;

  getmaxyx(win, win_height, win_width);
  
  char row_str [2 * win_width];
  char wrap_str [2 * win_width];
  int wrap_index = 0;

  getyx(win, y_pos, x_pos);

  wrap_index = x_pos;

  mvwinstr(win, y_pos, 0, row_str);

  //if on first column
  if(wrap_index == 0){
    wclrtoeol(win);
    wmove(win, y_pos + 1, 0);
    winsertln(win);
    mvwinsstr(win, y_pos + 1, 0, row_str);
  }
  //otherwise
  else{
    split_ret_str(row_str, wrap_str, wrap_index);
    copy_str_clear_eol(win, row_str);
    trim_trailing_spcs(wrap_str);
    wrap_str_recursive(win, y_pos, x_pos, wrap_str, win_width, win_height);
    wmove(win, y_pos + 1, 0);
  }

  return 0;
}

int handle_KEY_SPC(WINDOW *win, enum state_enums state, int win_width, int win_height){
  int x_pos = 0, y_pos = 0;
  char next_row_str[2 * win_width];
  char row_str[2 * win_width];
  char last_char = 0;
  int wrap_index = 0;

  getyx(win, y_pos, x_pos);

  //if cursor on last column
  if(state & LAST_COL){
    last_char = winch(win);
    row_str[0] = last_char;

    //if next line begins with alpha-numerica char
    if(check_next_line_begin_char(win, y_pos, x_pos)){
      //if char at current position not SPC
      if(last_char != ' ') strcat(row_str, " ");
    }
 
    get_next_row_str(win, next_row_str);
    strcat(row_str, next_row_str);

    //if resultant wrap string is wider than screen width
    //then trigger wrap string process
    if((int)strlen(next_row_str) > win_width){
      wrap_str_trigger(win, y_pos, x_pos, next_row_str, win_width, win_height);
    }
    //otherwise clear this line to end move to next line
    //add the wrap string and position at line head
    else{
      wclrtoeol(win);
      wmove(win, y_pos + 1, 0);
      waddstr(win, row_str);
      wmove(win, y_pos + 1, 0);
    }

  }
  //if a wrap is needed (char in last col of line)
  else if(state & WRAP_FLAG){
    mvwinstr(win, y_pos, 0, row_str);
    wrap_index = find_wrap_index(win, row_str);
    //if cursor is within the wrapped region
    if(x_pos > wrap_index){
      str_ins_spc_at_index(row_str, x_pos);
      x_pos++;
    }
    //trigger wrap process
    wrap_str_trigger(win, y_pos, x_pos, row_str, win_width, win_height);
    //if cursor not within wrap region
    if(x_pos <= wrap_index)ins_char_move_right(win, ' ');     
  }
  //otherwise
  else {
    ins_char_move_right(win, ' ');     
  }
  
  return 0;
}

int handle_KEY_DC(WINDOW *win, enum state_enums state, int win_width){
  //  wdelch(win);
  //  return 0;
  int y_pos = 0, x_pos = 0, wrap_size = 0, trailing_spaces = 0;
  int unwrap_index = 0, un_wrap_count = 0, unwrap_reset_index = 0;
  int first_unwrap_flag = 0;
  char unwrap_str[2 * win_width];
  char row_str[2 * win_width];

  getyx(win, y_pos, x_pos);

  //if in bottom right do nothing
  if((state & LAST_COL) && (state & LAST_ROW)) return 1;

  //delete current character under window
  wdelch(win);

  if(state & LAST_ROW) return 1;

  un_wrap_count++;
  
  //if cursor on first column
  if(state & FIRST_COL){
    while(1){
      mvwinstr(win, y_pos + un_wrap_count, 0, unwrap_str);
      wrap_size = return_leading_alnums(win);
      trailing_spaces = return_trailing_spaces(win, y_pos - 1 + un_wrap_count);
      unwrap_index = win_width - 1 - (trailing_spaces - 2);

      //if there is no word to wrap perform delete on current line
      //move up one line to last space after last word
      if(wrap_size == 0){
	wdelch(win);
	if(!first_unwrap_flag) unwrap_reset_index = unwrap_index;
      }

      //if wrap word is longer than space available on line above
      //increment count move to head of next line down restart loop
      if(wrap_size > trailing_spaces){
	un_wrap_count++;
	wmove(win, y_pos + un_wrap_count, 0);
	continue;
      }

      //if there is no word to wrap
      if(wrap_size == 0){

	//if there is word to wrap and line above is empty
	if (trailing_spaces == win_width + 1){
	  wmove(win, y_pos + 1, 0);
	  wdeleteln(win);
	  break;
	} else break;
      }
      
      //otherwise
      else{
	split_unwrap_str(row_str, unwrap_str, wrap_size);
	//if this is the first line wrapped set return index
	if(first_unwrap_flag == 0){
	  unwrap_reset_index = unwrap_index;
	  first_unwrap_flag = 1;
	}
	un_wrap_str(win, unwrap_str, unwrap_index);
	wmove(win, y_pos + un_wrap_count, 0);
	left_justify_line(win);
      }
    }
    //move cursor to approrpiate spot
    wmove(win, y_pos - 1, unwrap_reset_index);      
  }
  
  wmove(win, y_pos, x_pos);

  return 0;
  
}

int handle_KEY_BACKSPACE(WINDOW *win, enum state_enums state, int win_width){
  int y_pos = 0, x_pos = 0, wrap_size = 0, trailing_spaces = 0;
  int unwrap_index = 0, un_wrap_count = 0, reset_index_x = 0, reset_index_y = 0;
  int first_unwrap_flag = 0;
  char unwrap_str[2 * win_width];
  char row_str[2 * win_width];

  getyx(win, y_pos, x_pos);
  reset_index_x = x_pos;
  reset_index_y = y_pos - 1;

  //if in top left do nothing
  if((state & FIRST_COL) && (state & FIRST_ROW)) return 1;
  
  //if cursor on first column

  while(1){
    if(state & FIRST_COL){
      mvwinstr(win, y_pos + un_wrap_count, 0, unwrap_str);
      wrap_size = return_leading_alnums(win);
      trailing_spaces = return_trailing_spaces(win, y_pos - 1 + un_wrap_count);
      unwrap_index = win_width - 1 - (trailing_spaces - 2);

      //if there is no word to wrap perform delete on current line
      //move up one line to last space after last word
      if(wrap_size == 0){
	wdelch(win);
	if(!first_unwrap_flag) reset_index_x = unwrap_index;
      }

      //if wrap word is longer than space available on line above
      //increment count move to head of next line down restart loop
      if(wrap_size > (trailing_spaces - 1)){
	un_wrap_count++;
	if(!first_unwrap_flag) reset_index_x = unwrap_index;	
	wmove(win, y_pos + un_wrap_count, 0);
	continue;
      }

      //if there is no word to wrap
      if(wrap_size == 0){

	//if there is word to wrap and line above is empty
	if (trailing_spaces == win_width + 1){
	  wmove(win, y_pos - 1, 0);
	  wdeleteln(win);
	  break;
	} else break;
      }

      //otherwise
      else{
	split_unwrap_str(row_str, unwrap_str, wrap_size);
	//if this is the first line wrapped set return index
	if(first_unwrap_flag == 0){
	  reset_index_x = unwrap_index;
	  first_unwrap_flag = 1;
	}
	un_wrap_str(win, unwrap_str, unwrap_index);
	wmove(win, y_pos + un_wrap_count, 0);
	left_justify_line(win);
      }
    }
    //all other cases
    else{
      //perform backspace and reposition to next row down head of row
      mvwdelch(win, y_pos + un_wrap_count, x_pos - 1);
      un_wrap_count++;
      first_unwrap_flag = 1;
      wmove(win, y_pos + un_wrap_count, 0);
      state |= FIRST_COL;
      reset_index_x = x_pos - 1;
      reset_index_y = y_pos;
    }

  }

  //move cursor to approrpiate spot
  wmove(win, reset_index_y, reset_index_x);        


  return 0;
}

int handle_char_input(WINDOW *win, int ch, enum state_enums state, int win_width, int win_height){
  int x_pos = 0, y_pos = 0;
  char row_str[2 * win_width];

  getyx(win, y_pos, x_pos);
  
  ins_char_move_right(win, ch);
  get_state_enums(win, &state, win_width, win_height);
  x_pos++;

  if(state & WRAP_FLAG){
    mvwinstr(win, y_pos, 0, row_str);
    wrap_str_trigger(win, y_pos, x_pos, row_str, win_width, win_height);
   } 
  
  return 0;
}