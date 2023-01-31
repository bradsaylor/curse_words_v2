#include <string.h> //strcmp()
#include <stdlib.h>
#include "main_loop.h"
#include "file_interface.h" //FILE_LINE_SIZE

int main_loop(disp_config_type *disp_config, main_loop_vars_type *loop_vars)
{
  //local variables for main program loop
  char temp_filename[50] = {0};
  menu_type *current_menu = loop_vars->start_menu;
  menu_type *next_menu = NULL;
  int selection = 0, menu_select_rtn = 0, quit_flag = 0;

  menu_choice_type menu_choice = {
    current_menu,
    next_menu,
    &selection,
    loop_vars->menu_win->win
  };

  entry_struct entry = {0};
  char editor_string[DISPLAY_WIDTH * DISPLAY_HEIGHT] = {0};
  entry.text = editor_string;

  search_query_struct search_query = {0};

  //program loop
  while(1)
    {
      selection = 0;
      //print current menu to menu window
      menu_int_execute_menu(&menu_choice);      

      //switch on current menu
      switch(menu_choice.current_menu->menu_enum){
	
      case START:
	wclear(loop_vars->results_win->win);
	wrefresh(loop_vars->results_win->win);
	wclear(loop_vars->edit_win->win);
	wrefresh(loop_vars->edit_win->win);
	//print title screen
	disp_int_title_scn(loop_vars->edit_win->win);
	menu_select_rtn = menu_int_selection(&menu_choice);
	if(menu_select_rtn == - 1) invalid_selection_note(loop_vars);
	if(menu_select_rtn == 1){
	  quit_flag = 1;
	}
	break;

      case NEW:
	new_handler(&menu_choice, &entry);
	//proceed to entry config next
	menu_choice.current_menu = loop_vars->entry_config_menu;
	break;
	
      case EDITOR:
	wclear(loop_vars->edit_win->win);
	wrefresh(loop_vars->edit_win->win);
	editor_handler(loop_vars, disp_config, &entry);
	menu_select_rtn = menu_int_selection(&menu_choice);
	if(menu_select_rtn == - 1) invalid_selection_note(loop_vars);
	break;	
	
      case SAVE:
	file_int_save_entry(&entry);
	//proceed to start menu next
	menu_choice.current_menu = loop_vars->start_menu;
	break;
	
      case LIST_FILES_EDIT:
	list_files_handler(&menu_choice, temp_filename, loop_vars);
	strcpy(entry.file_name, temp_filename);
	//proceed to entry config next
	menu_choice.current_menu = loop_vars->entry_config_menu;
	break;		

      case LIST_FILES_SEARCH:
	list_files_handler(&menu_choice, temp_filename, loop_vars);
	strcpy(search_query.file_name, temp_filename);
	//proceed to results menu
	menu_choice.current_menu = loop_vars->results_menu;
	break;
	
      case SEARCH_TYPE:
	menu_select_rtn = menu_int_selection(&menu_choice);
	if(menu_select_rtn == - 1){
	  invalid_selection_note(loop_vars);
	  break;
	}
	search_query.type = *menu_choice.selection - 1;
	search_type_handler(&menu_choice, &search_query);
	break;	
	
      case SEARCH_MODE:
	menu_select_rtn = menu_int_selection(&menu_choice);
	if(menu_select_rtn == - 1) invalid_selection_note(loop_vars);	
	else search_query.mode = *menu_choice.selection - 1;
	break;	

      case RESULTS:
	wclear(loop_vars->edit_win->win);
	wrefresh(loop_vars->edit_win->win);	
	results_handler(&search_query, loop_vars);
	//proceed to start menu
	menu_choice.current_menu = loop_vars->start_menu;
	break;
	
      case ENTRY_CONFIG:
	entry_config_handler(&menu_choice, &entry);
	//proceed to editor next
	menu_choice.current_menu = loop_vars->editor_menu;
	break;
	
      default:
	break;
      }

      if(quit_flag) break;
    }
  
  return 0;
}

//menu handler functions
int editor_handler(main_loop_vars_type *loop_vars, disp_config_type *disp_config, entry_struct *entry)
{
  char date_str[50] = {0};
  win_type *edit_win = loop_vars->edit_win;
  win_type *results_win = loop_vars->results_win;

  print_keywords_file_info(results_win->win, entry->keywords, entry->file_name);

  //print date and time to edit window
  file_int_fill_date_str(date_str);
  wmove(edit_win->win, 0, 0);
  wprintw(edit_win->win, "[%s] ", date_str);
  wattron(edit_win->win, A_BOLD | A_UNDERLINE);
  wprintw(edit_win->win, "%s", entry->title);  
  wattroff(edit_win->win, A_BOLD | A_UNDERLINE);
  wmove(edit_win->win, 1, 0);
  wrefresh(edit_win->win);

  //run editor interface
  editor_interface(
		   edit_win->win,
		   disp_config->editor_width,
		   disp_config->editor_height,
		   entry->text
 );

  return 0;
}

int list_files_handler(menu_choice_type *menu_choice, char *temp_filename, main_loop_vars_type *loop_vars)
{
  char **file_list = NULL;
  //temp menu_type to store found files
  menu_type files_menu = {0};
  int selection = 0;

  //list files found in ../journal_fiels/ to menu window
  int file_count = file_int_count_files();
  file_list = file_int_allocate_list(file_count);
  file_int_list_files(file_list);

  //fill temp element array with found files
  menu_element_type file_menu_elements[file_count];
  for(int i = 0; i < file_count; i++){
    strcpy(file_menu_elements[i].name, file_list[i]);
  }

  //build temp menu type
  strcpy(files_menu.name, "Choose file");
  files_menu.menu_element_array = file_menu_elements;
  files_menu.menu_element_array_size = file_count;

  //make temp menu the current menu
  menu_choice->current_menu = &files_menu;

  //print temp menu to menu window
  menu_int_execute_menu(menu_choice);
  
  //choose a file
  while((selection = menu_int_prompt(menu_choice)) == -1){
    invalid_selection_note(loop_vars);
  };
  
  //copy chosen file name to passed temp_filename
  strcpy(temp_filename, files_menu.menu_element_array[selection - 1].name);

  //free allocated file list
  file_int_free_list(file_count, file_list);

  return 0;
}

int entry_config_handler(menu_choice_type *menu_choice, entry_struct *entry)
{
  //enum to better handle config selection
  typedef enum{
    TITLE,
    KEYWORD,
    CONTINUE
  }config_enum;
  config_enum selection;

  char reply[50] = {0};
  int continue_flag = 0;

  //fill entry date field with current date and time
  entry->date = time(NULL);

  //loop until 'continue' is selected
  while(1){
    selection = (config_enum)menu_int_prompt(menu_choice);

    //switch on config item selected
    switch(selection - 1){

    case TITLE:
      //get title, trimp trailing spaces, store to entry struct, flush input
      menu_int_text_prompt(menu_choice->menu_win, "Title: ", reply);
      trim_str(reply);

      strcpy(entry->title, reply);
      flushinp();
      break;

    case KEYWORD:
      //get keyword, trimp trailing spaces, store to entry struct, flush input      
      menu_int_text_prompt(menu_choice->menu_win, "Keyword: ", reply);
      trim_str(reply);
      strcat(entry->keywords, " ");
      strcat(entry->keywords, reply);
      strcat(entry->keywords, ",");
      flushinp();
      break;

    case CONTINUE:
      //if continue selected then break while loop
      continue_flag = 1;
      break;

    default:
      break;
    }

    if(continue_flag) break;
  }
  

  return 0;
}

int new_handler(menu_choice_type *menu_choice, entry_struct *entry)
{
  char reply[50] = {0};
  FILE *fid = NULL;

  menu_int_text_prompt(menu_choice->menu_win, "Name: ", reply);
  trim_str(reply);
  strcat(reply, ".jnl");
  chdir("../journal_files/");
  fid = fopen(reply, "a+");
  fclose(fid);
  strcpy(entry->file_name, reply);
  
  return 0;
}

int search_type_handler(menu_choice_type *menu_choice, search_query_struct *search_query)
{
  char reply[50] = {0};
  time_t temp_date = 0;

  if(search_query->type == (date)){
    menu_int_text_prompt(menu_choice->menu_win, "Start [mm/dd/yyyy]: ", reply);
    trim_str(reply);
    search_query->search_date_start = file_int_ret_date_int(reply);
    
    menu_int_text_prompt(menu_choice->menu_win, "End [mm/dd/yyyy]: ", reply);
    trim_str(reply);
    temp_date = file_int_ret_date_int(reply);
    search_query->search_date_end = file_int_ret_date_int(reply);    
  }
  else{
    menu_int_text_prompt(menu_choice->menu_win, "Search for: ", reply);
    trim_str(reply);
    strcpy(search_query->search_text, reply);
  }


  return 0;
}

int results_handler(search_query_struct *search_query, main_loop_vars_type *loop_vars){
  search_result results_array[100] = {0};
  WINDOW *edit_win = loop_vars->edit_win->win, *menu_win = loop_vars->menu_win->win;
  time_t time_t_date = 0;
  char read_date_str[50] = {0}, date_str[50] = {0}, x_of_x[50] = {0};
  char title_str[50] = {0}, file_line[FILE_LINE_SIZE] = {0};
  int num_results = file_int_count_search_results(search_query, results_array);
  int index = 0;

  char date_delim[]     = "<***date***>\n";
  char text_delim[]     = "<***text***>\n";
  char text_end_delim[] = "<***/text***>\n";
  char title_delim[]    = "<***title***>\n";

  if(num_results == 0) return 1;

  //switch to files directory and open file
  chdir("../journal_files/");
  
  while(index < num_results){
    FILE *fid = fopen(results_array[index].file_name, "r");
    //turn date into string
    sprintf(date_str, "%ld\n", results_array[index].date);

    while(1){
      //read file lines until a date_delim
      while(strcmp(fgets(file_line, FILE_LINE_SIZE, fid), date_delim));
      //get the next line which is the date
      fgets(file_line, FILE_LINE_SIZE, fid);
      //if this matches the date string
      if(!strcmp(file_line, date_str)){
	
	//print date and title at top of edit window
	time_t_date = atoi(date_str);
	wmove(edit_win, 0, 0);
	strcpy(read_date_str, ctime(&time_t_date));
	trim_str(read_date_str);
	wprintw(edit_win, "[%s] ", read_date_str);

	while(strcmp(fgets(file_line, FILE_LINE_SIZE, fid), title_delim));
	fgets(title_str, 50, fid);
	trim_str(title_str);
	wattron(edit_win, A_BOLD | A_UNDERLINE);
	wprintw(edit_win, "%s", title_str);  
	wattroff(edit_win, A_BOLD | A_UNDERLINE);

	wmove(edit_win, 1, 0);
	wrefresh(edit_win);

	//read file lines until a text_delim
	while(strcmp(fgets(file_line, FILE_LINE_SIZE, fid), text_delim));

	//print text lines to edit win until the text_end_delim
	while(strcmp(fgets(file_line, FILE_LINE_SIZE, fid), text_end_delim)){
	  remove_str_nls(file_line);
	  for(int i = 0; i < (int)strlen(file_line); i++) waddch(edit_win, file_line[i]);
	}

	//print [x of x] at bottom of menu
	sprintf(x_of_x, "(%d of %d)", index + 1, num_results);
	menu_int_note(menu_win, x_of_x);

	print_keywords_file_info(loop_vars->results_win->win,
				 results_array[index].keywords,
				 results_array[index].file_name);

	//refresh and wait for input then clear win and proceed
	wrefresh(edit_win);
	getch();

	wclear(edit_win);
	wrefresh(edit_win);
	
	break;
      }
    }
    index++;
  }

return 0;
}

int trim_str(char *str)
{
  //trim str from back erasing whitespace until alnum char reached
  for(int i = (int)strlen(str) - 1; i >= 0; i--){
    if((str[i] == ' ') || (str[i] == '\n') || (str[i] == ']')) str[i] = '\0';
    else break;
  }

  return 0;
}

int remove_str_nls(char *str){
  for(int i = 0; i < (int)strlen(str); i++){
    if(str[i] == '\n'){
      for(int j = i; j < (int)strlen(str); j++){
	str[j] = str[j + 1];
      }
    }
  }

  return 0;
}

int trim_str_spc(char *str)
{
  //trim str from back erasing whitespace until alnum char reached
  for(int i = (int)strlen(str) - 1; i >= 0; i--){
    if(str[i] == ' ') str[i] = '\0';
    else break;
  }

  return 0;
}

int invalid_selection_note(main_loop_vars_type *loop_vars)
{
  menu_int_note(loop_vars->menu_win->win, "Invalid choice, press any key");
  wrefresh(loop_vars->menu_win->win);
  getch();


  return 0;
}

int print_keywords_file_info(WINDOW *results_win, char *keywords, char *file_name){

  char temp_char = 0;

  //print file and keywords in info box
  wclear(results_win);
  wmove(results_win, 0, 0);
  wattron(results_win, A_UNDERLINE);
  wprintw(results_win, "File");
  wattroff(results_win, A_UNDERLINE);
  wprintw(results_win, ": %s\n", file_name);
  wattron(results_win, A_UNDERLINE);
  wprintw(results_win, "Keywords");
  wattroff(results_win, A_UNDERLINE);
  wprintw(results_win, ":");
  for(int i = 0; i < (int)strlen(keywords); i++){
    temp_char = keywords[i];
    if(temp_char == ',') wprintw(results_win, "\n");
    else wprintw(results_win, "%c", temp_char);
  }
  wrefresh(results_win);

  return 0;
}
