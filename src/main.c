#include <string.h>
#include <time.h>

#include "main_loop.h"
#include "menu_interface.h"
#include "display_interface.h"
#include "init.h"
#include "editor_interface.h"
#include "file_interface.h"

int main()
{
  //menu definitions
  menu_type start_menu             = {0};
  menu_type editor_menu            = {0};
  menu_type save_menu              = {0};
  menu_type new_menu               = {0};
  menu_type list_files_edit_menu   = {0};
  menu_type list_files_search_menu = {0};
  menu_type search_type_menu       = {0};
  menu_type search_mode_menu       = {0};
  menu_type results_menu           = {0};
  menu_type entry_config_menu      = {0};


  //menu names and enums
  strcpy(start_menu.name,              "Home Menu");
  strcpy(editor_menu.name,             "Editor Menu = <CTR>+<~>");
  strcpy(new_menu.name,                "Enter name");
  strcpy(list_files_edit_menu.name,    "Choose file");
  strcpy(list_files_search_menu.name,  "Choose file");
  strcpy(search_type_menu.name,        "Search type");
  strcpy(search_mode_menu.name,        "Search files");
  strcpy(results_menu.name,            "<RET> = next result");
  strcpy(entry_config_menu.name,       "Configure entry");

  start_menu.menu_enum             = START;
  editor_menu.menu_enum            = EDITOR;
  save_menu.menu_enum              = SAVE;
  new_menu.menu_enum               = NEW;
  list_files_edit_menu.menu_enum   = LIST_FILES_EDIT;
  list_files_search_menu.menu_enum = LIST_FILES_SEARCH;
  search_type_menu.menu_enum       = SEARCH_TYPE;
  search_mode_menu.menu_enum       = SEARCH_MODE;
  results_menu.menu_enum           = RESULTS;
  entry_config_menu.menu_enum      = ENTRY_CONFIG;

  //menu elements
  menu_element_type start_menu_open = {"Open file", &list_files_edit_menu};
  menu_element_type start_menu_new = {"New file", &new_menu};
  menu_element_type start_menu_search = {"Search", &search_type_menu};
  menu_element_type start_menu_exit = {"Exit", NULL};

  menu_element_type list_files_edit_menu_list = {"Configure entry", &entry_config_menu};

  menu_element_type search_type_menu_title = {"Title", &search_mode_menu};
  menu_element_type search_type_menu_text = {"Text", &search_mode_menu};
  menu_element_type search_type_menu_keyword = {"Keyword", &search_mode_menu};
  menu_element_type search_type_menu_date = {"Date", &search_mode_menu};

  menu_element_type editor_menu_save = {"Save entry", &save_menu};
  menu_element_type editor_menu_discard = {"Discard entry", &start_menu};
  menu_element_type editor_menu_return = {"Return", &editor_menu};

  menu_element_type search_mode_menu_all = {"All files", &results_menu};
  menu_element_type search_mode_menu_specific = {"Specific file", &list_files_search_menu};

  menu_element_type list_files_search_menu_list = {"", &results_menu};

  menu_element_type results_window_results = {"", &start_menu};

  menu_element_type entry_config_menu_title = {"Add title", &entry_config_menu};
  menu_element_type entry_config_menu_keyword = {"Add keyword", &entry_config_menu};
  menu_element_type entry_config_menu_continue = {"Continue", &editor_menu};
  

  //menu arrays
  menu_element_type start_menu_array[] = {
    start_menu_open,
    start_menu_new,
    start_menu_search,
    start_menu_exit
  };
  start_menu.menu_element_array = start_menu_array;
  start_menu.menu_element_array_size = (int)(sizeof(start_menu_array) / sizeof(start_menu_array[0]));
  
  menu_element_type editor_menu_array[] = {
    editor_menu_save,
    editor_menu_discard,
    editor_menu_return
  };
  editor_menu.menu_element_array = editor_menu_array;
  editor_menu.menu_element_array_size = (int)(sizeof(editor_menu_array) / sizeof(editor_menu_array[0]));
  
  menu_element_type list_files_edit_menu_array[] = {
    list_files_edit_menu_list
  };
  list_files_edit_menu.menu_element_array = list_files_edit_menu_array;
  list_files_edit_menu.menu_element_array_size = (int)(sizeof(list_files_edit_menu_array) / sizeof(list_files_edit_menu_array[0]));
  
  menu_element_type list_files_search_menu_array[] = {
    list_files_search_menu_list
  };
  
  menu_element_type search_type_menu_array[] = {
    search_type_menu_title,
    search_type_menu_text,
    search_type_menu_keyword,
    search_type_menu_date
  };
  search_type_menu.menu_element_array = search_type_menu_array;
  search_type_menu.menu_element_array_size = (int)(sizeof(search_type_menu_array) / sizeof(search_type_menu_array[0]));  

  menu_element_type search_file_type_menu_array[] = {
    search_mode_menu_all,
    search_mode_menu_specific
  };
  search_mode_menu.menu_element_array = search_file_type_menu_array;
  search_mode_menu.menu_element_array_size = (int)(sizeof(search_file_type_menu_array) / sizeof(search_file_type_menu_array[0]));

  menu_element_type results_menu_array[] = {
    results_window_results
  };

  menu_element_type entry_config_menu_array[] = {
    entry_config_menu_title,
    entry_config_menu_keyword,
    entry_config_menu_continue
  };
  entry_config_menu.menu_element_array = entry_config_menu_array;
  entry_config_menu.menu_element_array_size = (int)(sizeof(entry_config_menu_array) / sizeof(entry_config_menu_array[0]));


  //win types
  win_type
    edit_win           = {0},
    edit_border_win    = {0},
    menu_win           = {0},
    menu_border_win    = {0},
    results_win        = {0},
    results_border_win = {0};

  //win_type array
  win_type *win_array[] = {
    &edit_win,
    &edit_border_win,
    &menu_win,
    &results_win,
    &menu_border_win,
    &results_win,
    &results_border_win
  };
  int win_array_size = (int)(sizeof(win_array) / sizeof(win_array[0]));

  init_ncurses();

  //verify terminal size and prompt/exit if too small
  if(disp_int_check_term_size()){
    exit_ncurses();
    return 1;
  }



  //display and window configuration
  disp_config_type disp_config = {0};
  disp_config.display_width = DISPLAY_WIDTH;
  disp_config.display_height = DISPLAY_HEIGHT;

  disp_int_set_window_dims(&disp_config, EDITOR_WIDTH_PCT, EDITOR_HEIGHT_PCT);
  disp_int_build_win_types(&edit_win, &edit_border_win, &menu_win, &menu_border_win,
			   &results_win, &results_border_win, &disp_config);

  //display windows and labels
  disp_int_init_windows(win_array, win_array_size);
  disp_int_write_win_labels(win_array, win_array_size);

  //verify integrity of database
  char error_file_name[50] = {0};
  int error_line_num;
  if(file_int_verify_db(error_file_name, &error_line_num)){
    mvwprintw(edit_win.win, 0, 0, "Database parsing error in %s line %d\n", error_file_name, error_line_num);
    wrefresh(edit_win.win);
    wprintw(edit_win.win, "Any key to ext...");
    getch();
    exit_ncurses();
    return 1;
  }

  //print title screen
  disp_int_title_scn(edit_win.win);
  wrefresh(edit_win.win);

  //packing struct to transmit needed vars to main loop
  main_loop_vars_type loop_vars = {0};
  loop_vars.start_menu = &start_menu;
  loop_vars.entry_config_menu = &entry_config_menu;
  loop_vars.editor_menu = &editor_menu;
  loop_vars.search_mode_menu = &search_mode_menu;
  loop_vars.results_menu = &results_menu;
  loop_vars.menu_win = &menu_win;
  loop_vars.edit_win = &edit_win;
  loop_vars.results_win = &results_win;

  //run main loop
  main_loop(&disp_config, &loop_vars);
  
  //clean up ncurses and exti
  exit_ncurses();

  return 0;
}
  