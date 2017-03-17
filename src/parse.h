#ifndef _PARSER_H_
#define _PARSER_H_

#include "json.h"

struct slow_src_s
{
	const char* json;
	int offset;
};
typedef struct slow_src_s slow_src_t;

void slow_init_src(slow_src_t* pss, const char* src);

void slow_skip_whitespace(slow_src_t* pss);

int slow_parse(const char* src, slow_base_t* psb);

int slow_parse_null(slow_src_t* pss, slow_null_t* psn);

int slow_parse_true(slow_src_t* pss, slow_true_t* pst);

int slow_parse_false(slow_src_t* pss, slow_false_t* psf);

int slow_parse_number(slow_src_t* pss, slow_number_t* psn);

int slow_parse_string(char** src, slow_string_t* ps);

int slow_parse_kv(char** src, slow_kv_t* pkv);

int slow_parse_object(char** src, slow_object_t* po);

int slow_parse_base(slow_src_t* pss, slow_base_t* psb);

int slow_parse_array(char** src, slow_array_t* pa);

int slow_check_type(slow_src_t* pss);

int slow_cmp_string(slow_string_t* s, const char* str);
int slow_cmp_string_len(slow_string_t* s, const char* str, int len);
int slow_cmp_number(slow_number_t* n, double d);
#endif 

