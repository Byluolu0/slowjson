
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "common.h"
#include "parse.h"
#include "json.h"

void slow_init_src(slow_src_t* pss, const char* src)
{
	pss->json = src;
	pss->offset = 0;
}

void slow_skip_whitespace(slow_src_t* pss)
{
	assert(pss != NULL);

	const char* json = pss->json;
	int i = pss->offset;
	while (json[i] == ' ' || json[i] == '\n' || json[i] == '\r' || json[i] == '\t') i++;
	pss->offset = i;
}

int slow_parse(const char* src, slow_base_t* psb)
{
	assert(src != NULL);
	assert(psb != NULL);

	slow_src_t ss;
	slow_init_src(&ss, src);

	return slow_parse_base(&src, psb);
}

int slow_parse_literal(slow_src_t* pss, const char* literal)
{
	assert(pss != NULL);
	assert(literal != NULL);

	const char* json = pss->json;
	int offset = pss->offset;
	int i = 0;
	for (; literal[i]; i++)
	{
		if (json[offset + i] != literal[i]) return SLOW_INVALID_VALUE;
	}
	pss->offset += i;
	return SLOW_OK;
}

int slow_parse_null(slow_src_t* pss, slow_null_t* psn)
{
	assert(pss != NULL);
	assert(psn != NULL);

	int ret;

	slow_skip_whitespace(pss);
	if ((ret = slow_parse_literal(pss, "null")) != SLOW_OK) return ret;
	psn->flag = 1;
	return SLOW_OK;
}

int slow_parse_true(slow_src_t* pss, slow_true_t* pst)
{
	assert(pss != NULL);
	assert(pst != NULL);

	int ret;

	slow_skip_whitespace(pss);
	if ((ret = slow_parse_literal(pss, "true")) != SLOW_OK) return ret;
	pst->flag = 1;
	return SLOW_OK;
}

int slow_parse_false(slow_src_t* pss, slow_false_t* psf)
{
	assert(pss != NULL);
	assert(psf != NULL);

	int ret;

	slow_skip_whitespace(pss);
	if ((ret = slow_parse_literal(pss, "false")) != SLOW_OK) return ret;
	psf->flag = 1;
	return SLOW_OK;
}

int slow_parse_number(slow_src_t* pss, slow_number_t* psn)
{
	assert(pss != NULL);
	assert(psn != NULL);

	slow_skip_whitespace(pss);
	
	const char* json = pss->json;
	int offset = pss->offset;

	if (json[offset] == '-') offset++;
	if (json[offset] == '0') offset++;
	else
	{	
		if (!ISDIGIT1TO9(json[offset])) return SLOW_INVALID_VALUE;
		while (ISDIGIT(json[offset])) offset++;
	}

	if (json[offset] == '.')
	{
		offset++;
		if (!ISDIGIT(json[offset])) return SLOW_INVALID_VALUE;
		while (ISDIGIT(json[offset])) offset++;
	}

	if (json[offset] == 'e' || json[offset] == 'E')
	{
		offset++;
		if (json[offset] == '-' || json[offset] == '+') offset++;
		if (!ISDIGIT(json[offset])) return SLOW_INVALID_VALUE;
		while (ISDIGIT(json[offset])) offset++;
	}

	errno = 0;
	double d = strtod(pss->json[pss->offset], NULL);
	if (errno == ERANGE) return SLOW_INVALID_VALUE;
	psn->d = d;
	pss->offset = offset;
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

int slow_check_type(slow_src_t* pss)
{
	assert(pss != NULL);

	const char* json = pss->json;
	if (*json == 'n') return ST_NULL;
	else if (*json == 'f') return ST_FALSE;
	else if (*json == 't') return ST_TRUE;
	else if (ISDIGIT(*json)) return ST_NUMBER;
	else if (*json == '"') return ST_STRING;
	else if (*json == '{') return ST_OBJECT;
	else if (*json == '[') return ST_ARRAY;
	else if (*json == ',') return ST_DOT;
	else if (*json == ':') return ST_COLON;
	else if (*json == '}') return ST_OBJECT_END;
	else if (*json == ']') return ST_ARRAY_END;
	else return ST_NONE;
}

int slow_parse_base(slow_src_t* pss, slow_base_t* psb)
{
	assert(pss != NULL);
	assert(psb != NULL);

	int ret;

	slow_skip_whitespace(pss);
	int jsonType = slow_check_type(pss);
	if (jsonType == ST_NULL)
	{
		slow_null_t *psn = (slow_null_t*)malloc(sizeof(slow_null_t));
		slow_init_null(psn);
		if ((ret = slow_parse_null(pss, psn)) != SLOW_OK) return ret;
		psb->type = ST_NULL;
		psb->p = (void*)psn;
	}
	else if (jsonType == ST_FALSE)
	{
		slow_false_t *psf = (slow_false_t*)malloc(sizeof(slow_false_t));
		slow_init_false(psf);
		if ((ret = slow_parse_false(pss, psf)) != SLOW_OK) return ret;
		psb->type = ST_FALSE;
		psb->p = (void*)psf;
	}
	else if (jsonType == ST_TRUE)
	{
		slow_true_t *pst = (slow_true_t*)malloc(sizeof(slow_true_t));
		slow_init_true(pst);
		if ((ret = slow_parse_true(pss, pst)) != SLOW_OK) return ret;
		psb->type = ST_TRUE;
		psb->p = (void*)pst;
	}
	else if (jsonType == ST_NUMBER)
	{
		slow_number_t *psn = (slow_number_t*)malloc(sizeof(slow_number_t));
		slow_init_number(psn, 0);
		if ((ret = slow_parse_number(pss, psn)) != SLOW_OK) return ret;
		psb->type = ST_NUMBER;
		psb->p = (void*)psn;
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

