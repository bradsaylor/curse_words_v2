#ifndef _MAIN_LOOP_H
#define _MAIN_LOOP_H

#include <unistd.h> //chdir()

#include "menu_interface.h"
#include "file_interface.h"
#include "display_interface.h"
#include "editor_interface.h"

typedef struct {
  menu_type *start_menu;
  menu_type *entry_config_menu;
  menu_type *editor_menu;
  menu_type *search_mode_menu;
  menu_type *results_menu;
  win_type *menu_win;
  win_type *edit_win;
  win_type *results_win;
}main_loop_vars_type;

int main_loop(disp_config_type *disp_config, main_loop_vars_type *loop_vars);
int editor_handler(main_loop_vars_type *loop_vars, disp_config_type *disp_config, entry_struct *entry);
int list_files_handler(menu_choice_type *menu_choice, char *temp_filename, main_loop_vars_type *loop_vars);
int entry_config_handler(menu_choice_type *menu_choice, entry_struct *entry);
int new_handler(menu_choice_type *menu_choice, entry_struct *entry);
int search_type_handler(menu_choice_type *menu_choice, search_query_struct *search_query);
int results_handler(search_query_struct *search_query, main_loop_vars_type *loop_vars);
int trim_str(char *str);
int remove_str_nls(char *str);
int trim_str_spc(char *str);
int invalid_selection_note(main_loop_vars_type *loop_vars);
int print_keywords_file_info(WINDOW *results_win, char *keywords, char *file_name);

#endif //_MAIN_LOOP_H
