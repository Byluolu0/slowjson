#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "parser.h"
#include "json.h"

// ÒÆ³ýÎÞÓÃ×Ö·û
int slow_remove_useless(const char* src, char** dst)
{
	if (src == NULL) return -1;

	if (*dst != NULL) return -1;

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
	return 0;
}

int slow_parse_null(char** src, slow_null_t* objNull)
{
	if (src == NULL || *src == NULL) return -1;

	if (slow_valid_null(*src) != 0) return -1;
	
	objNull->flag = 1;
	*src += 4;
	return 0;
}

int slow_valid_null(char* src)
{
	if (src == NULL) return -1;

	int srcLen = strlen(src);
	if (srcLen < 4) return -1;
	
	if (src[0] != 'n' || src[1] != 'u' || src[2] != 'l' || src[3] != 'l') return -1;

	return 0;
}


int slow_parse_true(char** src, slow_true_t* objTrue)
{
	if (src == NULL || *src == NULL) return -1;

	if (slow_valid_true(*src) != 0) return -1;

	objTrue->flag = 1;
	*src += 4;
	return 0;
}

int slow_valid_true(char* src) 
{
	if (src == NULL) return -1;

	int srcLen = strlen(src);
	if (srcLen < 4) return -1;

	if (src[0] != 't' || src[1] != 'r' || src[2] != 'u' || src[3] != 'e') return -1;

	return 0;
}

int slow_parse_false(char** src, slow_false_t* objFalse)
{
	if (src == NULL || *src == NULL) return -1;

	if (slow_valid_false(*src) != 0) return -1;

	objFalse->flag = 1;
	*src += 5;
	return 0;
}

int slow_valid_false(char* src)
{
	if (src == NULL) return -1;

	int srcLen = strlen(src);
	if (srcLen < 5) return -1;

	if (src[0] != 'f' || src[1] != 'a' || src[2] != 'l' || src[3] != 's' || src[4] != 'e') return -1;

	return 0;
}

int slow_parse_number(char** src, slow_number_t* objNumber)
{
	if (src == NULL || *src == NULL) return -1;
	int count = 0, hasDot = 0;
	if (slow_valid_number(*src, &count, &hasDot) != 0) return -1;

	if (hasDot == 0) hasDot = count;

	hasDot--;

	double result = 0;
	int i;
	for (i = 0; i < count; ++i)
	{
		if ((*src)[i] == '.')
		{
			hasDot++;
			continue;
		}
		int tempDigit = (*src)[i] - '0';
		result += tempDigit * pow(10, hasDot - i);
	}
	objNumber->d = result;
	*src += count;

	return 0;
}

int slow_valid_number(char* src, int* count, int* hasDot)
{
	if (src == NULL) return -1;

	char* temp = src;
	int tempHasDot = 0;
	int tempCount = 0;
	while (*temp)
	{
		if (isdigit(*temp))
		{
			tempCount++;
		}
		else if (*temp == '.' && tempHasDot == 0 && tempCount > 0)
		{
			tempHasDot = tempCount;
			tempCount++;
		}
		else
		{
			break;
		}
		temp++;
	}
	if (tempCount == 0) return -1;

	if (src[tempCount - 1] == '.') return -1;

	*count = tempCount;
	*hasDot = tempHasDot;

	return 0;
}

int slow_parse_string(char** src, slow_string_t* objString, int isKey)
{
	if (src == NULL || *src == NULL) return -1;

	int count = 0;
	if (slow_valid_string(*src, &count, isKey) != 0) return -1;

	objString->p = (char*)malloc(count + 1);
	memcpy(objString->p, (*src) + 1, count);
	objString->p[count] = '\0';
	objString->len = count;
	*src += (2 + count);

	return 0;
}

int slow_valid_string(char* src, int* count, int isKey)
{
	if (src == NULL) return -1;

	if (*src != '"') return -1;

	char* temp = src;
	temp++;
	int tempCount = 0;
	while(*temp) 
	{
		if (*temp == '"') break;
		else tempCount++;

		temp++;
	}

	if (!isKey && tempCount == 0 && src[1 + tempCount] == '"') return 0;

	if (tempCount == 0) return -1;

	if (src[1 + tempCount] != '"') return -1;

	*count = tempCount;

	return 0;
}

int slow_parse_key_value(char** src, slow_kv_t* objKeyValue)
{
	if (src == NULL || *src == NULL) return -1;

	if (slow_check_type(*src) != ST_STRING) return -1;
	if (slow_parse_string(src, &(objKeyValue->key), 1) != 0) return -1;

	if (slow_check_type(*src) != ST_COLON) return -1;
	*src += 1;

	if (slow_parse_base(src, &(objKeyValue->value)) != 0) return -1;

	return 0;
}

int slow_parse_object(char** src, slow_object_t* ptrObject)
{
	if (src == NULL || *src == NULL) return -1;

	if (slow_check_type(*src) != ST_OBJECT) return -1;
	*src += 1;

	if (slow_check_type(*src) == ST_OBJECT_END)
	{
		*src += 1;
		return 0;
	}

	while (1)
	{
		slow_kv_list_t* node = (slow_kv_list_t*)malloc(sizeof(slow_kv_list_t));
		if (node == NULL)
		{
			slow_release_object(ptrObject);
			return -1;
		}

		if (slow_parse_key_value(src, &(node->node)) != 0)
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
	return 0;
}

int slow_parse_array(char** src, slow_array_t* ptrArray)
{
	if (src == NULL || *src == NULL) return -1;

	if (slow_check_type(*src) != ST_ARRAY) return -1;
	*src += 1;

	if (slow_check_type(*src) == ST_ARRAY_END)
	{
		*src += 1;
		return 0;
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
	return 0;
}

int slow_check_type(char* src)
{
	if (src == NULL) return -1;

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
	if (src == NULL || *src == NULL) return -1;

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
	return 0;
}

int slow_cmp_string(slow_string_t* s, const char* str)
{
	if (s == NULL || str == NULL) return -1;

	int len = strlen(str);
	if (s->len != len) return -1;

	if (strcmp(s->p, str) != 0) return -1;

	return 0;
}

int slow_cmp_number(slow_number_t* n, double d)
{
	if (n == NULL) return -1;

	if (fabs(n->d - d) > ACCURACY) return -1;

	return 0;
}

