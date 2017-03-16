
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "parse.h"
#include "json.h"

// ÒÆ³ýÎÞÓÃ×Ö·û
int slow_remove_useless(const char* src, char** dst)
{
	if (src == NULL) return SLOW_NULL_PTR;

	int srcLen = strlen(src);
	char* temp = (char*)malloc(srcLen + 1);
	if (temp == NULL) return -1;
	*dst = temp;
	while (*src)
	{
		if ((isprint(*src) != 0) && (*src != ' '))
		{
			*temp = *src;
			temp++;
		}
		src++;
	}
	*temp = '\0';
	return SLOW_OK;
}

int slow_parse_null(char** src, slow_null_t* objNull)
{
	if (src == NULL || *src == NULL) return SLOW_NULL_PTR;

	if (slow_valid_null(*src) != 0) return -1;
	
	objNull->flag = 1;
	*src += 4;
	return SLOW_OK;
}

int slow_valid_null(char* src)
{
	if (src == NULL) return SLOW_NULL_PTR;

	int srcLen = strlen(src);
	if (srcLen < 4) return -1;
	
	if (src[0] != 'n' || src[1] != 'u' || src[2] != 'l' || src[3] != 'l') return -1;

	return SLOW_OK;
}


int slow_parse_true(char** src, slow_true_t* objTrue)
{
	if (src == NULL || *src == NULL) return SLOW_NULL_PTR;

	if (slow_valid_true(*src) != 0) return -1;

	objTrue->flag = 1;
	*src += 4;
	return SLOW_OK;
}

int slow_valid_true(char* src) 
{
	if (src == NULL) return SLOW_NULL_PTR;

	int srcLen = strlen(src);
	if (srcLen < 4) return -1;

	if (src[0] != 't' || src[1] != 'r' || src[2] != 'u' || src[3] != 'e') return -1;

	return SLOW_OK;
}

int slow_parse_false(char** src, slow_false_t* objFalse)
{
	if (src == NULL || *src == NULL) return SLOW_NULL_PTR;

	if (slow_valid_false(*src) != 0) return -1;

	objFalse->flag = 1;
	*src += 5;
	return SLOW_OK;
}

int slow_valid_false(char* src)
{
	if (src == NULL) return SLOW_NULL_PTR;

	int srcLen = strlen(src);
	if (srcLen < 5) return -1;

	if (src[0] != 'f' || src[1] != 'a' || src[2] != 'l' || src[3] != 's' || src[4] != 'e') return -1;

	return SLOW_OK;
}

int slow_parse_number(char** src, slow_number_t* psn)
{
	if (*src == NULL) return SLOW_NULL_PTR;

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
	psn->d = d;
	*src = temp;
	return SLOW_OK;
}

int slow_parse_string(char** src, slow_string_t* ps)
{
	if (src == NULL || *src == NULL || ps == NULL) return SLOW_NULL_PTR;

	if (*src != '\"') return SLOW_INVALID_VALUE;

	int ret = SLOW_UNKNOWN;
	char* temp = *src;
	for (;;)
	{
		if (*temp == '\"')
		{
			slow_string_push(ps, *temp);
			temp++;
			*src = temp;
			return SLOW_OK;
		}
		else if (*temp == '\\')
		{
			temp++;
		}
	}
}

int slow_parse_kv(char** src, slow_kv_t* objKeyValue)
{
	if (src == NULL || *src == NULL) return SLOW_NULL_PTR;

	if (slow_check_type(*src) != ST_STRING) return -1;
	if (slow_parse_string(src, &(objKeyValue->key), 1) != 0) return -1;

	if (slow_check_type(*src) != ST_COLON) return -1;
	*src += 1;

	if (slow_parse_base(src, &(objKeyValue->value)) != 0) return -1;

	return SLOW_OK;
}

int slow_parse_object(char** src, slow_object_t* ptrObject)
{
	if (src == NULL || *src == NULL) return SLOW_NULL_PTR;

	if (slow_check_type(*src) != ST_OBJECT) return -1;
	*src += 1;

	if (slow_check_type(*src) == ST_OBJECT_END)
	{
		*src += 1;
		return SLOW_OK;
	}

	while (1)
	{
		slow_kv_list_t* node = (slow_kv_list_t*)malloc(sizeof(slow_kv_list_t));
		if (node == NULL)
		{
			slow_release_object(ptrObject);
			return -1;
		}

		if (slow_parse_kv(src, &(node->node)) != 0)
		{
			free(node);
			slow_release_object(ptrObject);
			return -1;
		}

		node->next = ptrObject->next;
		ptrObject->next = node;

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
			slow_release_object(ptrObject);
			return -1;
		}
	}
	return SLOW_OK;
}

int slow_parse_array(char** src, slow_array_t* ptrArray)
{
	if (src == NULL || *src == NULL) return SLOW_NULL_PTR;

	if (slow_check_type(*src) != ST_ARRAY) return -1;
	*src += 1;

	if (slow_check_type(*src) == ST_ARRAY_END)
	{
		*src += 1;
		return SLOW_OK;
	}

	while (1)
	{
		slow_base_list_t* jbl = (slow_base_list_t*)malloc(sizeof(slow_base_list_t));
		if (jbl == NULL) return -1;

		if (slow_parse_base(src, &(jbl->node)) != 0) return -1;
		jbl->next = ptrArray->next;
		ptrArray->next = jbl;

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
			slow_release_array(ptrArray);
			return -1;
		}
	}
	return SLOW_OK;
}

int slow_check_type(char* src)
{
	if (src == NULL) return SLOW_NULL_PTR;

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

int slow_parse_base(char** src, slow_base_t* objBase)
{
	if (src == NULL || *src == NULL) return SLOW_NULL_PTR;

	int jsonType = slow_check_type(*src);
	if (jsonType == ST_NULL)
	{
		slow_null_t *jn = (slow_null_t*)malloc(sizeof(slow_null_t));
		if (jn == NULL) return -1;
		if (slow_parse_null(src, jn) != 0) return -1;
		objBase->type = ST_NULL;
		objBase->p = (void*)jn;
	}
	else if (jsonType == ST_FALSE)
	{
		slow_false_t *jf = (slow_false_t*)malloc(sizeof(slow_false_t));
		if (jf == NULL) return -1;
		if (slow_parse_false(src, jf) != 0) return -1;
		objBase->type = ST_FALSE;
		objBase->p = (void*)jf;
	}
	else if (jsonType == ST_TRUE)
	{
		slow_true_t *jt = (slow_true_t*)malloc(sizeof(slow_true_t));
		if (jt == NULL) return -1;
		if (slow_parse_true(src, jt) != 0) return -1;
		objBase->type = ST_TRUE;
		objBase->p = (void*)jt;
	}
	else if (jsonType == ST_NUMBER)
	{
		slow_number_t *jn = (slow_number_t*)malloc(sizeof(slow_number_t));
		if (jn == NULL) return -1;
		if (slow_parse_number(src, jn) != 0) return -1;
		objBase->type = ST_NUMBER;
		objBase->p = (void*)jn;
	}
	else if (jsonType == ST_STRING)
	{
		slow_string_t *js = (slow_string_t*)malloc(sizeof(slow_string_t));
		if (js == NULL) return -1;
		if (slow_parse_string(src, js, 0) != 0) return -1;
		objBase->type = ST_STRING;
		objBase->p = (void*)js;
	}
	else if (jsonType == ST_OBJECT)
	{
		slow_object_t *jo = (slow_object_t*)malloc(sizeof(slow_object_t));
		if (jo == NULL) return -1;
		slow_init_object(jo);
		if (slow_parse_object(src, jo) != 0) return -1;
		objBase->type = ST_OBJECT;
		objBase->p = (void*)jo;
	}
	else if (jsonType == ST_ARRAY)
	{
		slow_array_t *ja = (slow_array_t*)malloc(sizeof(slow_array_t));
		if (ja == NULL) return -1;
		slow_init_array(ja);
		if (slow_parse_array(src, ja) != 0) return -1;
		objBase->type = ST_ARRAY;
		objBase->p = (void*)ja;
	}
	else
	{
		return -1;
	}
	return SLOW_OK;
}

int slow_cmp_string(slow_string_t* s, const char* str)
{
	if (s == NULL || str == NULL) return SLOW_NULL_PTR;

	int len = strlen(str);
	if (s->len != len) return -1;

	if (strcmp(s->p, str) != 0) return -1;

	return SLOW_OK;
}

int slow_cmp_number(slow_number_t* n, double d)
{
	if (n == NULL) return SLOW_NULL_PTR;

	if (fabs(n->d - d) > ACCURACY) return -1;

	return SLOW_OK;
}

