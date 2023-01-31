#include <stdio.h>
#include <ncurses.h>

#include <display_interface.h>
#include <init.h>

int main()
{
  win_type
    edit_win           = {0},
    edit_border_win    = {0},
    menu_win           = {0},
    menu_border_win    = {0},
    results_win        = {0},
    results_border_win = {0};

  win_type *win_array[] = {
    &edit_win,
    &edit_border_win,
    &menu_win,
    &menu_border_win,
    &results_win,
    &results_border_win
  };

  int win_array_size = (int)(sizeof(win_array) / sizeof(win_array[0]));

  init_ncurses();

  if(disp_int_check_term_size()){
    exit_ncurses();
    return 1;
  }

  disp_config_type disp_config = {0};
  disp_config.display_width = DISPLAY_WIDTH;
  disp_config.display_height = DISPLAY_HEIGHT;

  disp_int_set_window_dims(&disp_config, EDITOR_WIDTH_PCT, EDITOR_HEIGHT_PCT);
  disp_int_build_win_types(&edit_win, &edit_border_win, &menu_win, &menu_border_win,
			   &results_win, &results_border_win, &disp_config);


  disp_int_init_windows(win_array, win_array_size);
  disp_int_write_win_labels(win_array, win_array_size);

  getch();
  
  exit_ncurses();

  return 0;
}







