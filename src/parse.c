
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "common.h"
#include "parse.h"
#include "json.h"

void slow_skip_whitespace(char** src)
{
	assert(src != NULL);
	assert(*src != NULL);

	char* temp = *src;
	while (*temp == ' ' || *temp == '\n' || *temp == '\r' || *temp == '\t') temp++;
	*src = temp;
}

int slow_parse_literal(char** src, const char* literal)
{
	assert(src != NULL);
	assert(*src != NULL);
	assert(literal != NULL);

	char* temp = *src;
	int i = 0;
	for (; literal[i]; i++)
	{
		if (temp[i] != literal[i]) return SLOW_INVALID_VALUE;
	}
	*src += i;
	return SLOW_OK;
}

int slow_parse_null(char** src, slow_null_t* pn)
{
	assert(src != NULL);
	assert(*src != NULL);
	assert(pn != NULL);

	int ret;

	slow_skip_whitespace(src);
	if ((ret = slow_parse_literal(src, "null")) != SLOW_OK) return ret;
	pn->flag = 1;
	return SLOW_OK;
}

int slow_parse_true(char** src, slow_true_t* pt)
{
	assert(src != NULL);
	assert(*src != NULL);
	assert(pt != NULL);

	int ret;

	slow_skip_whitespace(src);
	if ((ret = slow_parse_literal(src, "true")) != SLOW_OK) return ret;
	pt->flag = 1;
	return SLOW_OK;
}

int slow_parse_false(char** src, slow_false_t* pf)
{
	assert(src != NULL);
	assert(*src != NULL);
	assert(pf != NULL);

	int ret;

	slow_skip_whitespace(src);
	if ((ret = slow_parse_literal(src, "false")) != SLOW_OK) return ret;
	pf->flag = 1;
	return SLOW_OK;
}

int slow_parse_number(char** src, slow_number_t* pn)
{
	assert(src != NULL);
	assert(*src != NULL);
	assert(pn != NULL);

	slow_skip_whitespace(src);
	
	char* temp = *src;
	if (*temp == '-') temp++;
	if (*temp == '0')
	{
		temp++;
	}
	else
	{	
		if (!ISDIGIT1TO9(*temp)) return SLOW_INVALID_VALUE;
		while (ISDIGIT(*temp)) temp++;
	}

	if (*temp == '.')
	{
		temp++;
		if (!ISDIGIT(*temp)) return SLOW_INVALID_VALUE;
		while (ISDIGIT(*temp)) temp++;
	}

	if (*temp == 'e' || *temp == 'E')
	{
		temp++;
		if (*temp == '-' || *temp == '+') temp++;
		if (!ISDIGIT(*temp)) return SLOW_INVALID_VALUE;
		while (ISDIGIT(*temp)) temp++;
	}

	errno = 0;
	double d = strtod(*src, NULL);
	if (errno == ERANGE) return SLOW_INVALID_VALUE;
	pn->d = d;
	*src = temp;
	return SLOW_OK;
}

int slow_parse_string(char** src, slow_string_t* ps)
{
	assert(src != NULL);
	assert(*src != NULL);
	assert(ps != NULL);

	slow_skip_whitespace(src);

	char* temp = *src;
	if (*temp != '\"') return SLOW_INVALID_VALUE;
	temp++;
	for (;;)
	{
		if (*temp == '\"')
		{
			temp++;
			*src = temp;
			return SLOW_OK;
		}
		else if (*temp == '\\')
		{
			temp++;
			if (*temp == '\\') slow_string_pushc(ps, '\\');
			else if (*temp == '\"') slow_string_pushc(ps, '\"');
			else if (*temp == '/') slow_string_pushc(ps, '/');
 			else if (*temp == 'r') slow_string_pushc(ps, '\r');
			else if (*temp == 'n') slow_string_pushc(ps, '\n');
			else if (*temp == 't') slow_string_pushc(ps, '\t');
			else if (*temp == 'b') slow_string_pushc(ps, '\b');
			else if (*temp == 'f') slow_string_pushc(ps, '\f');
			else if (*temp == 'u')
			{
				//todo
			}
		}
		else if (*temp == '\0') return SLOW_INVALID_VALUE;
		else slow_string_pushc(ps, *temp);
		temp++;
	}
}

int slow_parse_kv(char** src, slow_kv_t* pkv)
{
	assert(src != NULL);
	assert(*src != NULL);
	assert(pkv != NULL);

	int ret;

	slow_skip_whitespace(src);
	if (slow_check_type(*src) != ST_STRING) return SLOW_INVALID_VALUE;
	if ((ret = slow_parse_string(src, &pkv->key)) != SLOW_OK) return ret;
	slow_skip_whitespace(src);
	if (slow_check_type(*src) != ST_COLON) return SLOW_INVALID_VALUE;
	*src += 1;
	slow_skip_whitespace(src);
	if ((ret = slow_parse_base(src, &pkv->value)) != SLOW_OK) return ret;

	return SLOW_OK;
}

int slow_parse_object(char** src, slow_object_t* po)
{
	assert(src != NULL);
	assert(*src != NULL);
	assert(po != NULL);

	int ret;

	slow_skip_whitespace(src);
	if (slow_check_type(*src) != ST_OBJECT) return SLOW_INVALID_VALUE;
	*src += 1;

	slow_skip_whitespace(src);
	if (slow_check_type(*src) == ST_OBJECT_END)
	{
		*src += 1;
		return SLOW_OK;
	}

	while (1)
	{
		slow_kv_list_t* node = (slow_kv_list_t*)malloc(sizeof(slow_kv_list_t));
		slow_init_kv_list(node);
		slow_skip_whitespace(src);
		if ((ret = slow_parse_kv(src, &node->node)) != SLOW_OK)
		{
			free(node);
			slow_release_object(po);
			return ret;
		}

		node->next = po->next;
		po->next = node;
		slow_skip_whitespace(src);
		if (slow_check_type(*src) == ST_DOT)
		{
			*src += 1;
			continue;
		}
		else if (slow_check_type(*src) == ST_OBJECT_END) 
		{
			*src += 1;
			break;
		}
		else
		{
			slow_release_object(po);
			return SLOW_INVALID_VALUE;
		}
	}
	return SLOW_OK;
}

int slow_parse_array(char** src, slow_array_t* pa)
{
	assert(src != NULL);
	assert(*src != NULL);
	assert(pa != NULL);

	int ret;

	slow_skip_whitespace(src);
	if (slow_check_type(*src) != ST_ARRAY) return SLOW_INVALID_VALUE;
	*src += 1;

	slow_skip_whitespace(src);
	if (slow_check_type(*src) == ST_ARRAY_END)
	{
		*src += 1;
		return SLOW_OK;
	}

	while (1)
	{
		slow_base_list_t* jbl = (slow_base_list_t*)malloc(sizeof(slow_base_list_t));
		slow_init_base_list(jbl);
		slow_skip_whitespace(src);
		if ((ret = slow_parse_base(src, &jbl->node)) != SLOW_OK) return ret;
		jbl->next = pa->next;

		pa->next = jbl;
		slow_skip_whitespace(src);
		if (slow_check_type(*src) == ST_DOT)
		{
			*src += 1;
			continue;
		}
		else if (slow_check_type(*src) == ST_ARRAY_END)
		{
			*src += 1;
			break;
		}
		else
		{
			slow_release_array(pa);
			return SLOW_INVALID_VALUE;
		}
	}
	return SLOW_OK;
}

int slow_check_type(char* src)
{
	assert(src != NULL);

	if (*src == 'n') return ST_NULL;
	else if (*src == 'f') return ST_FALSE;
	else if (*src == 't') return ST_TRUE;
	else if (isdigit(*src)) return ST_NUMBER;
	else if (*src == '"') return ST_STRING;
	else if (*src == '{') return ST_OBJECT;
	else if (*src == '[') return ST_ARRAY;
	else if (*src == ',') return ST_DOT;
	else if (*src == ':') return ST_COLON;
	else if (*src == '}') return ST_OBJECT_END;
	else if (*src == ']') return ST_ARRAY_END;
	else return ST_NONE;
}

int slow_parse_base(char** src, slow_base_t* pb)
{
	assert(src != NULL);
	assert(*src != NULL);

	int ret;

	slow_skip_whitespace(src);
	int jsonType = slow_check_type(*src);
	if (jsonType == ST_NULL)
	{
		slow_null_t *jn = (slow_null_t*)malloc(sizeof(slow_null_t));
		slow_init_null(jn);
		if ((ret = slow_parse_null(src, jn)) != SLOW_OK) return ret;
		pb->type = ST_NULL;
		pb->p = (void*)jn;
	}
	else if (jsonType == ST_FALSE)
	{
		slow_false_t *jf = (slow_false_t*)malloc(sizeof(slow_false_t));
		slow_init_false(jf);
		if ((ret = slow_parse_false(src, jf)) != SLOW_OK) return ret;
		pb->type = ST_FALSE;
		pb->p = (void*)jf;
	}
	else if (jsonType == ST_TRUE)
	{
		slow_true_t *jt = (slow_true_t*)malloc(sizeof(slow_true_t));
		slow_init_true(jt);
		if ((ret = slow_parse_true(src, jt)) != SLOW_OK) return ret;
		pb->type = ST_TRUE;
		pb->p = (void*)jt;
	}
	else if (jsonType == ST_NUMBER)
	{
		slow_number_t *jn = (slow_number_t*)malloc(sizeof(slow_number_t));
		slow_init_number(jn, 0);
		if ((ret = slow_parse_number(src, jn)) != SLOW_OK) return ret;
		pb->type = ST_NUMBER;
		pb->p = (void*)jn;
	}
	else if (jsonType == ST_STRING)
	{
		slow_string_t *ps = (slow_string_t*)malloc(sizeof(slow_string_t));
		slow_init_string(ps);
		if ((ret = slow_parse_string(src, ps)) != SLOW_OK)
		{
			slow_release_string(ps);
			return ret;
		}

		pb->type = ST_STRING;
		pb->p = (void*)ps;
	}
	else if (jsonType == ST_OBJECT)
	{
		slow_object_t *jo = (slow_object_t*)malloc(sizeof(slow_object_t));
		slow_init_object(jo);
		if ((ret = slow_parse_object(src, jo)) != SLOW_OK) return ret;
		pb->type = ST_OBJECT;
		pb->p = (void*)jo;
	}
	else if (jsonType == ST_ARRAY)
	{
		slow_array_t *ja = (slow_array_t*)malloc(sizeof(slow_array_t));
		slow_init_array(ja);
		if ((ret = slow_parse_array(src, ja)) != SLOW_OK) return ret;
		pb->type = ST_ARRAY;
		pb->p = (void*)ja;
	}
	else
	{
		return SLOW_UNKNOWN_TYPE;
	}
	return SLOW_OK;
}

int slow_cmp_string(slow_string_t* s, const char* str)
{
	assert(s != NULL);
	assert(str != NULL);

	return strcmp(s->p, str);
}

int slow_cmp_string_len(slow_string_t* s, const char* str, int len)
{
	assert(s != NULL);
	assert(str != NULL);
	assert(len > 0);
	return memcmp(s->p, str, len);
}

int slow_cmp_number(slow_number_t* n, double d)
{
	assert(n != NULL);
	if (fabs(n->d - d) > ACCURACY) return SLOW_UNEQUAL;
	return SLOW_OK;
}

