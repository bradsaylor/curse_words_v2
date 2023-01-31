#include <ncurses.h>
#include <string.h>
#include <sys/ioctl.h>

#include "display_interface.h"
const char *title[] ={
  " @@@@  @@@    @@ @@@@@      @@@@   @@@@@@ ",
  "@@   @  @@    @@  @@  @@   @@   @  @@     ",
  "@@      @@    @@  @@   @@   @@@    @@@@   ",
  "@@      @@    @@  @@ @@@      @@@  @@     ",
  "@@   @  @@    @@  @@  @@    @   @@ @@     ",
  " @@@@    @@@@@@   @@   @@@   @@@@  @@@@@@ ",
  "                                          ",
  "@@      @ @@@@@@  @@@@@     @@@@    @@@@  ",
  " @     @ @@    @@  @@  @@   @@  @  @@   @ ",
  " @     @ @@    @@  @@   @@  @@   @  @@@   ",
  " @  @  @ @@    @@  @@ @@@   @@   @    @@@ ",
  " @ @@@ @ @@    @@  @@  @@   @@  @   @   @@",
  "  @   @   @@@@@@   @@   @@@ @@@@     @@@@ ",
  "                                          ",
  "v1.0 RBS Jan 2023"
};


// /////////////////////
// Interface functions//
// /////////////////////

int disp_int_title_scn(WINDOW *edit_win){
  int ymax = 0, xmax = 0;
  getmaxyx(edit_win, ymax, xmax);
  int title_width = strlen(title[0]);
  int xstart = (xmax - title_width) / 2;
  int title_size = ((int)sizeof(title) / (int)sizeof(title[0]));
  int ystart = (ymax - title_size) / 2;

  wmove(edit_win, 0, 0);
  for(int i = 0; i < title_size; i++){
    mvwprintw(edit_win, ystart + i, xstart, title[i]);
  }
  wrefresh(edit_win);

  return 0;
}

int disp_int_set_window_dims(disp_config_type *d, float editor_width_pct, float editor_height_pct)
{

  d->editor_width  = (int)(editor_width_pct * d->display_width);
  d->editor_height = (int)(editor_height_pct * d->display_height);
  d->menu_width    = (int)(d->display_width - d->editor_width - 4);
  d->menu_height   = d->editor_height / 2;
  d->results_width    = (int)(d->display_width - d->editor_width - 4);
  d->results_height   = d->editor_height - d->menu_height - 3;
  
  //  d->menu_width    = (int)(d->display_width - d->editor_width - 4);
  //  d->menu_height   = d->editor_height;
  //  d->results_height  = (int)(d->editor_height / 4);
  //  d->results_width   = d->display_width - 2;

  return 0;
}

int disp_int_build_win_types(win_type *edit_win, win_type *edit_border_win, win_type *menu_win, win_type *menu_border_win,
		    win_type *results_win, win_type *results_border_win, disp_config_type *d){

  int y_offset = 2;

  edit_win->win = NULL;
  strcpy(edit_win->title, "Editor");
  edit_win->width = d->editor_width;
  edit_win->height = d->editor_height;
  edit_win->startx = d->display_width - (d->editor_width + 1);
  edit_win->starty = 1 + y_offset;
  edit_win->has_box = 0;

  edit_border_win->win = NULL;
  strcpy(edit_border_win->title, "");
  edit_border_win->width = d->editor_width + 2;
  edit_border_win->height = d->editor_height + 2;
  edit_border_win->startx = d->display_width - (d->editor_width + 2);
  edit_border_win->starty = 0 + y_offset;
  edit_border_win->has_box = 1;

  menu_win->win = NULL;
  strcpy(menu_win->title, "Menu");
  menu_win->width = d->menu_width;
  menu_win->height = d->menu_height;
  menu_win->startx = 1;
  menu_win->starty = 1 + y_offset;
  menu_win->has_box = 0;
  
  menu_border_win->win = NULL;
  strcpy(menu_border_win->title, "");
  menu_border_win->width = d->menu_width + 2;
  menu_border_win->height = d->menu_height + 2;
  menu_border_win->startx = 0;
  menu_border_win->starty = 0 + y_offset;
  menu_border_win->has_box = 1;

  results_win->win = NULL;
  strcpy(results_win->title, "Entry Info");
  results_win->width = d->results_width;
  results_win->height = d->results_height;
  results_win->startx = 1;
  results_win->starty = menu_border_win->height + 4;
  //  results_win->starty = d->editor_height + 4 + y_offset;  
  results_win->has_box = 0;

  results_border_win->win = NULL;
  strcpy(results_border_win->title, "");
  results_border_win->width = d->results_width + 2;
  results_border_win->height = d->results_height + 2;
  results_border_win->startx = 0;
  results_border_win->starty = menu_border_win->height + 3;
  //  results_border_win->starty = d->editor_height + 3 + y_offset;
  results_border_win->has_box = 1;  

  return 0;
}		    

int disp_int_write_win_labels(win_type **win_array, int win_array_size){
  win_type *wt;

  attron(A_REVERSE | A_BOLD);
  for(int i = 0; i < win_array_size; i++){
    wt = win_array[i];
    mvprintw(wt->starty - 2, wt->startx, "%s", wt->title);
  }

  attroff(A_REVERSE | A_BOLD);

  refresh();
  
  return 0;
}

int disp_int_init_windows(win_type **win_array, int win_array_size){
  win_type *wt;

  for(int i = 0; i < win_array_size; i++){
    wt = win_array[i];
    win_array[i]->win  = newwin(wt->height,
			  wt->width,
			  wt->starty,
			  wt->startx);
    if(wt->has_box == 1) box(wt->win, 0, 0);
    keypad(wt->win, TRUE);
    wrefresh(wt->win);
  }
    
  return 0;
}

int disp_int_check_term_size()
{
  int result = 0;
  int ysize = 0, xsize = 0;

  getmaxyx(stdscr, ysize, xsize);

  if((ysize < DISPLAY_HEIGHT) || (xsize < DISPLAY_WIDTH)) {
    printw("Increase window size and restart\nany key to exit...\n\n");
    refresh();
    getch();
    result = 1;
  }

  return result;
}