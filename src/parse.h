#ifndef _PARSER_H_
#define _PARSER_H_

#include "json.h"

void slow_skip_whitespace(char** src);

int slow_parse_null(char** src, slow_null_t* pn);

int slow_parse_true(char** src, slow_true_t* pt);

int slow_parse_false(char** src, slow_false_t* pf);

int slow_parse_number(char** src, slow_number_t* pn);

int slow_parse_string(char** src, slow_string_t* ps);

int slow_parse_kv(char** src, slow_kv_t* pkv);

int slow_parse_object(char** src, slow_object_t* po);

int slow_parse_base(char** src, slow_base_t* pb);

int slow_parse_array(char** src, slow_array_t* pa);

int slow_check_type(char* src);

int slow_cmp_string(slow_string_t* s, const char* str);
int slow_cmp_string_len(slow_string_t* s, const char* str, int len);
int slow_cmp_number(slow_number_t* n, double d);
#endif 

