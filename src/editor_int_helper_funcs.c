#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "editor_interface.h"
#include "editor_int_helper_funcs.h"

int ins_char_move_right(WINDOW *win, int ch){
  int x_pos = 0, y_pos = 0;

  getyx(win, y_pos, x_pos);
  winsch(win, ch);
  wmove(win, y_pos, x_pos + 1);
  wrefresh(win);

  return 0;
}

int check_last_col(WINDOW *win){
  int result = 0;
  int x_pos = 0, y_pos = 0;
  int ch = 0;
  int win_width = 0, win_height = 0;

  getmaxyx(win, win_height, win_width);

  getyx(win, y_pos, x_pos);
  if((ch = mvwinch(win, y_pos, win_width - 1)) != ' ') result = 1;
 
  wmove(win, y_pos, x_pos);
  wrefresh(win);
  
  return result;
}

int find_wrap_index(WINDOW *win, char *str){
  int win_width = 0, win_height = 0;

//find index of last <space> within first SCN_WIDTH chars
  //return -1 if no <space> is found

  
  getmaxyx(win, win_height, win_width);
  
  assert((int)strlen(str) >= win_width);

  for(int i = (win_width - 1); i >= 0; i--){
    if(*(str + i) == ' ') return i;
  }

  return -1;  
}

int null_term_last_char(char *str){
  //find first non <space> char from end of string
  //null term string at following space
  int count = 0;

  for(count = (strlen(str) - 1); count >= 0; count--){
    if(*(str + count) != ' ') break;
  }

  *(str + count + 1) = '\0';

  return 0;
}

int split_wrap_str(char *row_str, char *wrap_str, int wrap_index){

  null_term_last_char(row_str);
  row_str[wrap_index] = '\0';
  strcpy(wrap_str, row_str + wrap_index + 1);
  
  return 0;
}

int split_unwrap_str(char *row_str, char *unwrap_str, int wrap_index){
  //entire line passed in with wrap_str, split at wrap index
  //string part past wrap_index stored in row_str
  
  null_term_last_char(unwrap_str);
  unwrap_str[wrap_index] = '\0';
  strcpy(row_str, unwrap_str + wrap_index + 1);
  
  return 0;
}

int split_ret_str(char *row_str, char *ret_str, int wrap_index){
  
  null_term_last_char(row_str);
  strcpy(ret_str, row_str + wrap_index);
  row_str[wrap_index] = '\0';

  return 0;
}

int copy_str_clear_eol(WINDOW *win, char* str){
  //copies passed string to beginning of current row
  //clears to end of line  

  int x_pos = 0; int y_pos = 0;

  getyx(win, y_pos, x_pos);
  mvwaddstr(win, y_pos, 0, str);
  wclrtoeol(win);

  wmove(win, y_pos, x_pos);
  wrefresh(win);
  

  return 0;
}

int get_next_row_str(WINDOW *win, char *str){
  //fetch string on next row
  //null terminate after last non <space> character

  int x_pos = 0, y_pos = 0;
  getyx(win, y_pos, x_pos);

  mvwinstr(win, y_pos + 1, 0, str);
  null_term_last_char(str);

  wmove(win, y_pos, x_pos);

  return 0;
}

int combine_wrap_str(char *wrap_str, char *row_str, enum space_enum SPC_FLAG){

  if(SPC_FLAG) strcat(wrap_str, " ");
  strcat(wrap_str, row_str);
  
  return 0;
}

int check_wrap_str_length(WINDOW *win, char *str){
  int win_width = 0, win_height = 0;

  getmaxyx(win, win_height, win_width);

  if((int)strlen(str) > win_width - 1) return 1;
  return 0;
}

int check_next_line_begin_char(WINDOW *win, int y_pos, int x_pos){
  int result = 0;
  char ch = 0;

  ch = (char)mvwinch(win, y_pos + 1, 0);
  if(isalnum(ch)) result = 1;

  wmove(win, y_pos, x_pos);

  return result;
}

int move_curs_after_wrap(WINDOW *win, int y_pos, int x_pos, int wrap_index, int WRAP_TYPE){
  int win_width = 0, win_height = 0;

  getmaxyx(win, win_height, win_width);

 // Decide where to put cursor after wrap occurs
  //1. handle x_pos beyond wrap point
  //  a) before end row - move to next do not advance one column
  //  b) at end row - move to next line advance one column
  //2. handle x_pos before wrap point - char at current point

  if(WRAP_TYPE == KEY_GENERAL){
    if(x_pos > wrap_index){
      if(x_pos == win_width - 1) wmove(win, y_pos + 1, x_pos - wrap_index);
      else  wmove(win, y_pos + 1, x_pos - wrap_index - 1);
    }
    else if(x_pos == wrap_index + 1){
      wmove(win, y_pos + 1, 0);
    }
    else {
      wmove(win, y_pos, x_pos);
    }
  }
  else if(WRAP_TYPE == KEY_RET){
    wmove(win, y_pos + 1, 0);
  }
   
  return 0;
}

int str_ins_spc_at_index(char *str, int index){
  int length = strlen(str);

  for(int i = length; i > index; i--){
    *(str + i) = *(str + i - 1);
  }

  *(str + index) = ' ';
  *(str + length + 1) = '\0';

  return 0;
}

int ins_row_push_down(WINDOW *win, int y_pos, int x_pos, char *str, int win_width, int win_height){
  char temp_str[2 * win_width];

  for(int count = win_height - 1; count > y_pos + 1; count--){
    mvwinstr(win, count - 1, 0, temp_str);
    mvwaddstr(win, count, 0, temp_str);
    wclrtoeol(win);
  }

  mvwaddstr(win, y_pos + 1, 0, str);
  wclrtoeol(win);
  
  wmove(win, y_pos, x_pos);

  return 0;
}

int return_trailing_spaces(WINDOW *win, int row_index){
  //returns number of blank spaces (SPC or 0) at end of line at row_index
  //returns cursor to previous pos
  
  int y_pos = 0, x_pos = 0, space_count = 0;
  int win_width = 0, win_height = 0;
  char ch = 0;

  getyx(win, y_pos, x_pos);
  getmaxyx(win, win_height, win_width);

  for(space_count = 0; space_count <= win_width - 1; space_count++){
    ch = (char)mvwinch(win, row_index, win_width - 1 - space_count);
    if(isalnum(ch)){
      wmove(win, y_pos, x_pos);
      return space_count;
    }
  }

  wmove(win, y_pos, x_pos);

  return space_count + 1;
}

int return_leading_alnums(WINDOW *win){
  //returns number of consecutive alpha-numeric chars (until a space)
  //starting from beginning of current line

  int y_pos = 0, x_pos = 0, alnum_count = 0;
  int win_width = 0, win_height = 0;
  char ch = 0;

  getyx(win, y_pos, x_pos);
  getmaxyx(win, win_height, win_width);
  wmove(win, y_pos, 0);

  for(alnum_count = 0; alnum_count < win_width; alnum_count ++){
    ch = (char)mvwinch(win, y_pos, alnum_count);
    if(!isalnum(ch)){
      wmove(win, y_pos, x_pos);
      return alnum_count;
    }
  }

  return win_width - 1;
}

int un_wrap_str(WINDOW *win, char *unwrap_str, int index){
  //Copies passed string to line above at x_pos = index
  //Deletes passed string from current line
  //Replaces cursor position

  int y_pos = 0, x_pos = 0;

  getyx(win, y_pos, x_pos);

  mvwaddstr(win, y_pos - 1, index, unwrap_str);
  
  for(int i = 0; i < (int)strlen(unwrap_str); i++){
    mvwaddch(win, y_pos, i, ' ');
  }
  wmove(win, y_pos, x_pos);

  return 0;
}

int left_justify_line(WINDOW *win){
  //Delete non alpha-num chars from beginning of line
  //return cursor to previous position

  int y_pos = 0, x_pos = 0;
  int win_width = 0, win_height = 0;
  char ch = 0;

  getyx(win, y_pos, x_pos);
  getmaxyx(win, win_height, win_width);
  wmove(win, y_pos, 0);

  for(int i = 0; i < win_width; i++){
    ch = winch(win);
    if(!isalnum(ch)) wdelch(win);
  }

  wmove(win, y_pos, x_pos);

  return 0;
}

int trim_trailing_spcs(char *str){
  //trim non-alpha-numberic chars from string starting
  //at end until first alpha-numeric character
  //if none found null terminate at first index (0)

  for(int i = ((int)strlen(str)) -1 ; i >= 0; i --){
    if(isalnum(*(str + i))){
      *(str + i + 1) = '\0';
      return 0;
    }
  }

  *str = '\0';
  
  return 0;
}

int dump_editor_string(WINDOW *win, char *str, int win_width, int win_height)
{
  char temp_str[win_width];

  //step through each line of editor window
  //append to str, each line ending gets a '\n'
  for(int i = 1; i < win_height - 1; i++){
    wmove(win, i, 0);
    winstr(win, temp_str);
    strcat(temp_str, "\n");
    strcat(str, temp_str);
  }

  //remove extra spaces from editor string
  trim_trailing_spcs(str);

  return 0;
}

int zero_char_array(char *str, int size){
  char *temp_char = str;

  for(int i = 0; i < size; i++){
    *temp_char = '\0';
    temp_char++;
  }

  return 0;
}