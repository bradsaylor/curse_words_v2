#ifndef _FILE_INTERFACE_H
#define _FILE_INTERFACE_H

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#define FILE_LINE_SIZE 20

//data types
typedef enum {
  all_files,
  specific_file,
}search_mode;

typedef enum {
  title,
  text,
  keyword,
  date,
}search_type;

typedef struct{
  char file_name[50];
  time_t date;
  char title[50];
  char keywords[100];
  char *text;
}entry_struct;

typedef struct{
  char file_name[50];
  search_mode mode;
  search_type type;
  char search_text[50];
  long int search_date_start;
  long int search_date_end;    
}search_query_struct;

typedef struct {
  time_t date;
  char file_name[50];
  char keywords[50];
}search_result;

//public function prototypes
int file_int_count_files();
int file_int_list_files(char **file_list);
char **file_int_allocate_list(int num_files);
int file_int_free_list(int num_files, char **file_name_array);
FILE *file_int_open_file(char *file_name);
int file_int_save_entry(entry_struct *entry);
int file_int_build_search_results(search_mode mode, search_result **search_result_array, char *search_term, char *file_name);
search_result **file_int_allocate_result_list(int num_results);
int file_int_free_result_list(int num_results, search_result **search_result_array);
int file_int_build_entry_struct_from_file(entry_struct *entry, FILE *fid, char *file_name);
int file_int_count_search_results(search_query_struct  *search_query, search_result *results_array);
int find_substring(char *ref_str, int ref_size,  char *search_str, int search_size);
long int file_int_ret_date_int(char *date_str);
int file_int_fill_date_str(char *date_str);
int file_int_verify_db(char *error_file_name, int *error_line_num);

#endif //_FILE_INTERFACE_H
