#include <ncurses.h>
#include <string.h>

#include "menu_interface.h"

int menu_int_execute_menu(menu_choice_type *menu_choice){

  if (menu_choice->current_menu == NULL) return 1;

  wclear(menu_choice->menu_win);
  wattron(menu_choice->menu_win, A_BOLD);
  mvwprintw(menu_choice->menu_win, 0, 0, "%s\n", menu_choice->current_menu->name);
  wattroff(menu_choice->menu_win, A_BOLD);

  for(int i = 0; i < menu_choice->current_menu->menu_element_array_size; i++){
    wprintw(menu_choice->menu_win, "%d. %s\n", i + 1, menu_choice->current_menu->menu_element_array[i].name);
  }  

  wrefresh(menu_choice->menu_win);

  return 0;
}

int menu_int_selection(menu_choice_type *menu_choice){
  //executes a menu selection and assigns current menu to indicated next menu

  //prompt for and store menu element selection 
  *(menu_choice->selection) = menu_int_prompt(menu_choice);
  
  //return error if selection is out of bounds
  if(*(menu_choice->selection) == -1) return -1;
  
  //assign next menu to menu pointed to by selected menu element
  menu_choice->next_menu = menu_choice->current_menu->menu_element_array[*(menu_choice->selection) - 1].next_menu;

  if(menu_choice->next_menu == NULL) return 1;

  //assign current menu to menu pointed to by selected menu element
  menu_choice->current_menu = menu_choice->next_menu;
  
  return 0;
}
  
int menu_int_note(WINDOW *win, char *note){
  //prints note at bottom of window

  int ypos = 0, xpos = 0;

  getmaxyx(win, ypos, xpos);

  mvwprintw(win, ypos - 1, 1, note);
  wclrtoeol(win);
  wrefresh(win);

  return 0;
}

int menu_int_prompt(menu_choice_type *menu_choice){
  //prompts and gathers input for menu element selection
  WINDOW *win = menu_choice->menu_win;
  int num_items = menu_choice->current_menu->menu_element_array_size;

  char selection = 0;
  int ypos = 0, xpos = 0, selection_int = 0;

  getmaxyx(win, ypos, xpos);

  //prompt for and store selection from printed menu elements
  mvwprintw(win, ypos - 1, 1, "Selection: ");
  wclrtoeol(win);
  wrefresh(win);
  selection = wgetch(win);

  //input range check
  selection_int = (int)(selection  - '0');
  if((selection_int < 1) || (selection_int > num_items)) return -1;
  
  return selection_int;
}

int menu_int_text_prompt(WINDOW *win, char *prompt_str, char *prompt_reply){
  char reply[50] = {0};
  int temp_char = 0;
  int end_check = 0;
  int ypos = 0, xpos = 0;
  int ymax = 0, xmax = 0, xhome = 0;
  int quit_flag = 0;
  
  getmaxyx(win, ymax, xmax);

  //print passed prompt string
  mvwprintw(win, ymax - 1, 1, prompt_str);
  wclrtoeol(win);
  wrefresh(win);

  getyx(win, ypos, xpos);
  //store starting x position for use in reading resposne
  xhome = xpos + 1;
  //flank input space with '[ ]'
  waddch(win, '[');
  mvwaddch(win, ypos, xmax - 1, ']');
  wmove(win, ypos, xpos + 1);

  //display user input
  while((temp_char = wgetch(win))){
    
    getyx(win, ypos, xpos);
    
    switch(temp_char)
      {
	// if nl entered quit loop
      case '\n':
	quit_flag = 1;
	break;

	//if BKSPC pressed evaluate if next position is opening '['
	//if so stay in place and break switch
      case KEY_BACKSPACE:
	if((end_check = mvwinch(win, ypos, xpos - 1)) == '['){
	  wmove(win, ypos, xpos);
	  break;
	}
	//if not perform the BKSPC
	mvwaddch(win, ypos, xpos - 1, ' ');
	wmove(win, ypos, xpos - 1);
	break;

	//for all other cases check to see if we are at end of input space
	//if so do nothting and break switch
      default:
	if((end_check = mvwinch(win, ypos, xpos + 1)) == ']'){
	  wmove(win, ypos, xpos);
	  break;
	}
	//if not add the entered character
	else{
	  wmove(win, ypos, xpos);
	  waddch(win, temp_char);
	}
	wrefresh(win);
      }

     if(quit_flag) break;
  }

  //read input string into 'reply' will include trailing whitespace
  //and ']', both to be removed by trim function in main_loop.c
  mvwinstr(win, ypos, xhome, reply);
  strcpy(prompt_reply, reply);
  
  return 0;
}


