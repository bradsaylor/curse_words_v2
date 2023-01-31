#ifndef _MENU_H
#define _MENU_H

#include <ncurses.h>

#include "display_interface.h" //win_type

typedef struct menu_struct menu_type;
typedef struct menu_element_struct menu_element_type;

typedef enum {
  START,
  EDITOR,
  SAVE,
  NEW,
  LIST_FILES_EDIT,
  LIST_FILES_SEARCH,
  SEARCH_TYPE,
  SEARCH_MODE,
  RESULTS,
  NAME_PROMPT,
  ENTRY_CONFIG 
}menu_enums;

struct menu_struct{
  menu_enums menu_enum; 
  char name[50];
  menu_element_type *menu_element_array;
  int menu_element_array_size;
};

struct menu_element_struct{
  char name[50];
  menu_type *next_menu;
};

typedef struct{
  menu_type *current_menu;
  menu_type *next_menu;
  int       *selection;
  WINDOW    *menu_win;
}menu_choice_type;

//function prototypes
int menu_int_execute_menu(menu_choice_type *menu_choice);
int menu_int_note(WINDOW *win, char *note);
int menu_int_prompt(menu_choice_type *menu_choice);
int menu_int_selection(menu_choice_type *menu_choice);
int menu_int_text_prompt(WINDOW *win, char *prompt_str, char *prompt_reply);

#endif //_MENU_H
