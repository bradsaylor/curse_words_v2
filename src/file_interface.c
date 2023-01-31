#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "file_interface.h"

// ///////////////////////
// Local data structures//
// ///////////////////////

//wrapper struct to store vars needed for fseek() operations
typedef struct{
  FILE *fid;
  int fid_offset;
  int count;
}file_loc;

// ////////////////////
// Local variables/////
// ////////////////////

static const char entry_delim[]       = "<***entry***>\n";
static const char entry_end_delim[]   = "<***/entry***>\n";
static const char date_delim[]        = "<***date***>\n";
static const char date_end_delim[]    = "<***/date***>\n";
static const char title_delim[]       = "<***title***>\n";
static const char title_end_delim[]   = "<***/title***>\n";
static const char keyword_delim[]     = "<***keyword***>\n";
static const char keyword_end_delim[] = "<***/keyword***>\n";
static const char text_delim[]        = "<***text***>\n";
static const char text_end_delim[]    = "<***/text***>\n";


// ///////////////////////////
// Local function prototypes//
// ///////////////////////////

static int trim_nl(char *str, int size);
static int check_search_date(search_query_struct *search_query, char *file_line);
static int evaluate_search(search_query_struct *search_query,
			   search_result *results_array,
			   int results_so_far,
			   char *file_name,
			   char *file_line,
			   file_loc *file_location,
  			   char *search_delim,
			   char *search_end_delim);

static int str_to_lowercase(char *str);
static int verify_db(char *file_name);

// ////////////////////////
// Interface functions/////
// ////////////////////////

int file_int_fill_date_str(char *date_str){
  char str_time[100] = {0};
  char str_date[100] = {0};    

  struct tm *tm;
  time_t t;

  t = time(NULL);
  tm = localtime(&t);
    
  strftime(str_time, sizeof(str_time), "%H:%M:%S", tm);
  strftime(str_date, sizeof(str_date), "%Y-%m-%d_", tm);
  strcat(date_str, str_date);
  strcat(date_str, str_time);

  return 0;
}

int file_int_count_search_results(search_query_struct *search_query, search_result *results_array)
{
  int num_results = 0, num_files = 0;
  char file_line[FILE_LINE_SIZE] = {0};
  char search_delim[20] = {0}, search_end_delim[20] = {0};
  //  char temp_text_end_delim[strlen(text_end_delim)];
  //  strcpy(temp_text_end_delim, text_end_delim);
  char **file_name_array;

  //wrapper struct to store vars needed for fseek() operations
  file_loc file_location = {0};
  
  //fill start/end deliminators according to query type
  switch(search_query->type){
  case title:
    strcpy(search_delim, title_delim);
    strcpy(search_end_delim, title_end_delim);
    break;
  case text:
    strcpy(search_delim, text_delim);
    strcpy(search_end_delim, text_end_delim);
    break;
  case keyword:
    strcpy(search_delim, keyword_delim);
    strcpy(search_end_delim, keyword_end_delim);    
    break;
  case date:
    strcpy(search_delim, date_delim);
    strcpy(search_end_delim, date_end_delim);    
    break;
  default:
    break;
  }

  //if searching all files
  if(search_query->mode == all_files){
    //determine num of files and allocate/fill a list of them
    num_files = file_int_count_files();
    file_name_array = file_int_allocate_list(num_files);
    file_int_list_files(file_name_array);

    //move to journal files directory
    chdir("../journal_files/");

    //step through found files
    for(int i = 0; i < num_files; i++){
      //reset file counter and open next file
      file_location.count = 0;
      file_location.fid = fopen(file_name_array[i], "a+");

      //count results and close file
      num_results += evaluate_search(search_query,
				     results_array,
				     num_results,
				     file_name_array[i],
				     file_line,
				     &file_location,
				     search_delim,
				     search_end_delim);
      fclose(file_location.fid);
    }

    //clean up allocated list
    file_int_free_list(num_files, file_name_array);

    return num_results;
  }

  //if searching specific file
  else if(search_query->mode == specific_file){
    chdir("../journal_files/"); 
    file_location.fid = fopen(search_query->file_name, "a+");

    num_results = evaluate_search(search_query,
				  results_array,
				  0,
				  search_query->file_name,
				  file_line,
				  &file_location,
				  search_delim,
				  search_end_delim);
    fclose(file_location.fid);

    return num_results;
  }

  else return 0;
}

int file_int_count_files(){
  DIR *dirp;
  int count = 0;
  struct dirent *struct_dir = NULL;
  //  char file_name[PATH_MAX] = {0};
  
  dirp = opendir("../journal_files/");

  while((struct_dir = readdir(dirp))){
    if(strstr(struct_dir->d_name, ".jnl")) count++;
  }

  free(dirp);

  return count;
}

int file_int_list_files(char **file_list){
  int count = 0;
  DIR *dirp;
  struct dirent *struct_dir = NULL;
  
  dirp = opendir("../journal_files/");

  while((struct_dir = readdir(dirp))){
    if(strstr(struct_dir->d_name, ".jnl")){
      strcpy(file_list[count], struct_dir->d_name);
      count++;
    }
  }

  free(dirp);

  return 0;
}

char **file_int_allocate_list(int num_files)
{
  char file_name[100];
  char **file_name_array = NULL;

  file_name_array = malloc(num_files * sizeof(file_name));

  for(int i = 0; i < num_files; i++){
    file_name_array[i] = malloc(sizeof(file_name));
  }

  return file_name_array;
}

int file_int_free_list(int num_files, char **file_name_array)
{
  for(int i = 0; i < num_files; i++){
    free(file_name_array[i]);
    file_name_array[i] = NULL;
  }

  free(file_name_array);
  file_name_array = NULL;

  return 0;
}

search_result **file_int_allocate_result_list(int num_results)
{
  search_result result_struct = {0};
  search_result **search_result_array = NULL;

  search_result_array = malloc(num_results * sizeof(result_struct));

  for(int i = 0; i < num_results; i++){
    search_result_array[i] = malloc(sizeof(result_struct));
  }

  return search_result_array;
}

int file_int_free_result_list(int num_results, search_result **search_result_array)
{
  for(int i = 0; i < num_results; i++){
    free(search_result_array[i]);
    search_result_array[i] = NULL;
  }

  free(search_result_array);
  search_result_array = NULL;

  return 0;
}

FILE *file_int_open_file(char *file_name){
  FILE *fid;
  char file_path[50] = "../journal_files/";

  strcat(file_path, file_name);
  fid = fopen(file_path, "a+");

  return fid;
}

int file_int_save_entry(entry_struct *entry)
{
  FILE *fid = NULL;
  char file_name[50];
  int ret = 0;

  strcpy(file_name, entry->file_name);
  chdir("../journal_files/");
  fid = fopen(file_name, "a+");

  ret = fprintf(fid, "%s", entry_delim);

  ret = fprintf(fid, "%s", date_delim);
  ret = fprintf(fid, "%ld\n", entry->date);
  ret = fprintf(fid, "%s", date_end_delim);

  ret = fprintf(fid, "%s", title_delim);
  ret = fprintf(fid, "%s\n", entry->title);
  ret = fprintf(fid, "%s", title_end_delim);

  ret = fprintf(fid, "%s", keyword_delim);
  ret = fprintf(fid, "%s\n", entry->keywords);
  ret = fprintf(fid, "%s", keyword_end_delim);

  ret = fprintf(fid, "%s", text_delim);
  ret = fprintf(fid, "%s\n", entry->text);
  ret = fprintf(fid, "%s", text_end_delim);

  ret = fprintf(fid, "%s", entry_end_delim);

  ret = fprintf(fid, "\n");

  fclose(fid);
  
  return 0;
}

int file_int_build_entry_struct_from_file(entry_struct *entry, FILE *fid, char *file_name){
  //parses file from entry point fid until end of entry
  //fills passed entry struct fields

  char file_line[100];

  strcpy(entry->file_name, file_name);

  while(fgets(file_line, sizeof(file_line), fid)){
    
    if(!strcmp(file_line, entry_end_delim)) break;
    
    if(!strcmp(file_line, date_delim)){
      fgets(file_line, sizeof(file_line), fid);
      trim_nl(file_line, strlen(file_line));
      entry->date = atoi(file_line);
    }
    if(!strcmp(file_line, title_delim)){
      fgets(file_line, sizeof(file_line), fid);
      trim_nl(file_line, strlen(file_line));
      strcpy(entry->title, file_line);
    }
    if(!strcmp(file_line, keyword_delim)){
      fgets(file_line, sizeof(file_line), fid);
      trim_nl(file_line, strlen(file_line));
      strcpy(entry->keywords, file_line);
    }
  }

  return 0;
}

long int file_int_ret_date_int(char *date_str){
  int month = 0, day = 0, year = 0;
  struct tm time_struct = {0};
  time_t date_time_t = 0;

  sscanf(date_str, "%d/%d/%d", &month, &day, &year);

  //load tm struct with values from input string
  time_struct.tm_mon = month - 1;
  time_struct.tm_mday = day;
  time_struct.tm_year = year - 1900;

  //convert tm struct to time_t (long int seconds since 1970) value
  date_time_t = mktime(&time_struct);

  return date_time_t;
}


int file_int_verify_db(char *error_file_name, int *error_line_num){
  int count = 0, db_error_line = 0;
  DIR *dirp;
  struct dirent *struct_dir = NULL;
  
  dirp = opendir("../journal_files/");

  while((struct_dir = readdir(dirp))){
    if(strstr(struct_dir->d_name, ".jnl")){
      if((db_error_line = verify_db(struct_dir->d_name))){
	strcpy(error_file_name, struct_dir->d_name);
	*error_line_num = db_error_line;
	return -1;
      }
      count++;
    }
  }

  free(dirp);

  return 0;
}

// ////////////////////////
// Local functions/////////
// ////////////////////////

static int check_search_date(search_query_struct *search_query, char *file_line){
  int result = 0;
  long int file_line_int = 0;
  char file_line_trimmed[50];

  strcpy(file_line_trimmed, file_line);
  file_line_trimmed[strlen(file_line_trimmed) - 1] = '\0';
  file_line_int = (long int)atoi(file_line_trimmed);

  if(file_line_int >= search_query->search_date_start){
    if(file_line_int <= search_query->search_date_end){
      result = 1;
    }
  }

  return result;
}

static int trim_nl(char *str, int size){
  //removes trailing nl from string
  //passed int size should be strlen(str)
  str[size - 1] = '\0';
  return 0;
}

static int evaluate_search
(
 search_query_struct *search_query,
 search_result       *results_array,
 int                 results_so_far,
 char                *file_name,
 char                *file_line,
 file_loc            *file_location,
 char                *search_delim,
 char                *search_end_delim
)
{

  char *found_result_flag = NULL;
  char temp_result_flag = '1';
  int num_results = 0;
  int entry_offset = 0;
  time_t temp_date = 0;
  char temp_keywords[50];
  char file_line_no_case[50] , search_text_no_case[50];

  strcpy(search_text_no_case, search_query->search_text);
  str_to_lowercase(search_text_no_case);

  //start reading lines of file
  while(fgets(file_line, FILE_LINE_SIZE, file_location->fid)){
    found_result_flag = NULL;
    //if entry start is encountered
    if(!strcmp(file_line, entry_delim)){
      entry_offset = ftell(file_location->fid);
      while(fgets(file_line, FILE_LINE_SIZE, file_location->fid)){
	if(!strcmp(file_line, date_delim)){
	  fgets(file_line, FILE_LINE_SIZE, file_location->fid);
	  trim_nl(file_line, strlen(file_line));
	  temp_date = atoi(file_line);
	} else if(!strcmp(file_line, keyword_delim)) {
	  fgets(file_line, sizeof(temp_keywords), file_location->fid);
	  trim_nl(file_line, strlen(file_line));
	  strcpy(temp_keywords, file_line);
	  break;
	}
      }
      fseek(file_location->fid, entry_offset, SEEK_SET);
      //start reading lines of file again
      while(fgets(file_line, FILE_LINE_SIZE, file_location->fid)){
	if(found_result_flag) break;
	//if search delim is encountered
	if(!strcmp(file_line, search_delim)){
	  file_location->fid_offset = ftell(file_location->fid);
	  file_location->count = 0;	      
	  //read a chunk of the file
	  if(search_query->type == date){
	    fgets(file_line, FILE_LINE_SIZE, file_location->fid);
	    if(check_search_date(search_query, file_line)){
	      num_results++;
	      results_array[results_so_far + num_results - 1].date = temp_date;
	      strcpy(results_array[results_so_far + num_results - 1].keywords, temp_keywords);
	      strcpy(results_array[results_so_far + num_results - 1].file_name, file_name);
	    }
	    break;
	  }
	  else{
	    while(fread(file_line, FILE_LINE_SIZE - 1, 1, file_location->fid)){
	      //if chunk contains search_term and none have been found so far increment num_results
	      strcpy(file_line_no_case, file_line);
	      str_to_lowercase(file_line_no_case);
	      found_result_flag = strstr(file_line_no_case, search_text_no_case);
	      if(found_result_flag){
		num_results++;
		results_array[results_so_far + num_results - 1].date = temp_date;
		strcpy(results_array[results_so_far + num_results - 1].keywords, temp_keywords);		
		strcpy(results_array[results_so_far + num_results - 1].file_name, file_name);		
		file_location->count += FILE_LINE_SIZE - 1;
	      }
	      //break if chunk contains end delim or we have found the search term
	      if(strstr(file_line, search_end_delim)){
		found_result_flag = &temp_result_flag;
		break;
	      }
	      if(found_result_flag) break;
	      file_location->count++;
	      fseek(file_location->fid, file_location->fid_offset + file_location->count, SEEK_SET);
	    }
	  }
	}
      }
    }
  }

  return num_results;
}
 
static int str_to_lowercase(char *str){

  for(int i = 0; i < (int)strlen(str); i++){
    str[i] = tolower(str[i]);
  }

  return 0;
}

static int verify_db(char *file_name){
  int line_count = 0, index = 0;
  char *fgets_ret = NULL;
  FILE *fid = NULL;
  char file_line[FILE_LINE_SIZE];

  //array for tracking current expected delim
  const char *delims_array[] = {
    entry_delim,  
    date_delim,      
    date_end_delim,  
    title_delim,     
    title_end_delim, 
    keyword_delim,   
    keyword_end_delim,
    text_delim,        
    text_end_delim,  
    entry_end_delim 
  };

  int delims_array_size = ((int)sizeof(delims_array) / (int)(sizeof(delims_array[0])));

  chdir("../journal_files/");

  //open passed file
  fid = fopen(file_name, "r");

  while(1){
    //read a FILE_LINE_SIZE chars
    fgets_ret = fgets(file_line, FILE_LINE_SIZE, fid);
    //if chunk contains NL increment line count
    if(strstr(file_line, "\n")) line_count++;
    //while not EOF and does not contain <*** keep reading
    if((fgets_ret) && !strstr(file_line, "<***")) continue;

    //if EOF check to see if array has rolled over = complete set of delims
    if(!fgets_ret){
      fclose(fid);
      //if not return line num
      if(index != 0) return line_count;
      else return 0;
    }
    //if current delim does not match expected next delim break to return -1
    if(strcmp(file_line, delims_array[index])) break;
    //increment index for next delim
    index++;
    //if at end of delim array roll over index to 0
    if(index == delims_array_size) index = 0;
  }

  fclose(fid);

  return line_count;
}
