#include <ncurses.h>

#include "init.h"

int init_ncurses(){
  initscr();
  keypad(stdscr, TRUE);
  cbreak();
  noecho();
  start_color();
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  init_pair(2, COLOR_WHITE, COLOR_BLACK);
  refresh();


  return 0;
}

int exit_ncurses(){
  endwin();
  return 0;
}

int init_windows(win_type **win_array, int win_array_size){
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
