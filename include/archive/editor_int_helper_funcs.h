#ifndef _HELPER_FUNCS_H
#define _HELPER_FUNCS_H

#include <ncurses.h>
#include "editor_interface.h"

//function declarations
int ins_char_move_right(WINDOW *win, int ch);
int check_last_col(WINDOW *win);
int find_wrap_index(WINDOW *win, char *str);
int null_term_last_char(char *str);
int split_wrap_str(char *row_str, char *wrap_str, int wrap_index);
int split_unwrap_str(char *row_str, char *unwrap_str, int wrap_index);
int split_ret_str(char *row_str, char *ret_str, int wrap_index);
int copy_str_clear_eol(WINDOW *win, char* str);
int get_next_row_str(WINDOW *win, char *str);
int combine_wrap_str(char *wrap_str, char *row_str, enum space_enum SPC_FLAG);
int check_wrap_str_length(WINDOW *win, char *str);
int check_next_line_begin_char(WINDOW *win, int y_pos, int x_pos);
int move_curs_after_wrap(WINDOW *win, int y_pos, int x_pos, int wrap_index, int WRAP_TYPE);
int str_ins_spc_at_index(char *str, int index);
int ins_row_push_down(WINDOW *win, int y_pos, int x_pos, char *str, int win_width, int win_height);
int return_trailing_spaces(WINDOW *win, int row_index);
int return_leading_alnums(WINDOW *win);
int un_wrap_str(WINDOW *win, char *unwrap_str, int index);
int left_justify_line(WINDOW *win);
int trim_trailing_spcs(char *str);
int dump_editor_string(WINDOW *win, char *str, int win_width, int win_height);
int zero_char_array(char *str, int size);

#endif //_HELPER_FUNCS_H
