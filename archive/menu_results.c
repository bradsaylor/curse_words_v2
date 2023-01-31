#include <ncurses.h>
#include <string.h>

#include "menu.h"


int execute_menu(WINDOW *win, menu_type *menu, int size){

  if (menu == NULL) return 1;

  wprintw(win, "%s\n", menu->name);

  for(int i = 0; i < size; i++){
    wprintw(win, "%d. %s\n", i + 1, menu->menu_element_array[i].name);
  }

  wrefresh(win);

  return 0;
}
