#ifndef _PARSER_H_
#define _PARSER_H_

#include "json.h"

// ½âÎö
int slow_remove_useless(const char* src, char** dst);

int slow_parse_null(char** src, slow_null_t* objNull);
int slow_valid_null(char* src);

int slow_parse_true(char** src, slow_true_t* objTrue);
int slow_valid_true(char* src);

int slow_parse_false(char** src, slow_false_t* objFalse);
int slow_valid_false(char* src);

int slow_parse_number(char** src, slow_number_t* objNumber);
int slow_valid_number(char* src, int* count, int* hasDot);

int slow_parse_string(char** src, slow_string_t* objString, int isKey);
int slow_valid_string(char* src, int* count, int isKey);

int slow_parse_key_value(char** src, slow_kv_t* objKeyValue);

int slow_parse_object(char** src, slow_object_t* objObject);

int slow_parse_base(char** src, slow_base_t* objBase);

int slow_parse_array(char** src, slow_array_t* ptrArray);


// ¸¨Öúº¯Êý
int slow_check_type(char* src);

int slow_cmp_string(slow_string_t* s, const char* str);
int slow_cmp_number(slow_number_t* n, double d);
#endif 

