#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <ncurses.h>

#define DISPLAY_WIDTH 100
#define DISPLAY_HEIGHT 40
#define EDITOR_WIDTH_PCT 0.6
#define EDITOR_HEIGHT_PCT 0.6

// /////////////
// Data types///
// /////////////

typedef struct {
  WINDOW *win;
  char title[50];
  int width;
  int height;
  int startx;
  int starty;
  int has_box;
}win_type;

typedef struct {
  int display_width;
  int display_height;
  int editor_width;
  int editor_height;
  int results_width;
  int results_height;
  int menu_width;
  int menu_height;
}disp_config_type;

// //////////////////////////////
//Interface function prototypes//
// //////////////////////////////

int disp_int_set_window_dims(disp_config_type *disp_config, float editor_width_pct, float editor_height_pct);
int disp_int_build_win_types(win_type *edit_win, win_type *edit_border_win, win_type *menu_win, win_type *menu_border_win,
		    win_type *results_win, win_type *results_border_win, disp_config_type *d);
int disp_int_write_win_labels(win_type **win_array, int win_array_size);
int disp_int_init_windows(win_type **win_array, int win_array_size);
int disp_int_check_term_size();
int disp_int_title_scn(WINDOW *edit_win);

#endif //_DISPLAY_H
