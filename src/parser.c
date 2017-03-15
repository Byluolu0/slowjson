#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "parser.h"
#include "json.h"

// ÒÆ³ýÎÞÓÃ×Ö·û
int removeUseless(const char* src, char** dst)
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

int parseNull(char** src, JsonNull* objNull)
{
	if (src == NULL || *src == NULL) return -1;

	if (validNull(*src) != 0) return -1;
	
	objNull->flag = 1;
	*src += 4;
	return 0;
}

int validNull(char* src)
{
	if (src == NULL) return -1;

	int srcLen = strlen(src);
	if (srcLen < 4) return -1;
	
	if (src[0] != 'n' || src[1] != 'u' || src[2] != 'l' || src[3] != 'l') return -1;

	return 0;
}


int parseTrue(char** src, JsonTrue* objTrue)
{
	if (src == NULL || *src == NULL) return -1;

	if (validTrue(*src) != 0) return -1;

	objTrue->flag = 1;
	*src += 4;
	return 0;
}

int validTrue(char* src) 
{
	if (src == NULL) return -1;

	int srcLen = strlen(src);
	if (srcLen < 4) return -1;

	if (src[0] != 't' || src[1] != 'r' || src[2] != 'u' || src[3] != 'e') return -1;

	return 0;
}

int parseFalse(char** src, JsonFalse* objFalse)
{
	if (src == NULL || *src == NULL) return -1;

	if (validFalse(*src) != 0) return -1;

	objFalse->flag = 1;
	*src += 5;
	return 0;
}

int validFalse(char* src)
{
	if (src == NULL) return -1;

	int srcLen = strlen(src);
	if (srcLen < 5) return -1;

	if (src[0] != 'f' || src[1] != 'a' || src[2] != 'l' || src[3] != 's' || src[4] != 'e') return -1;

	return 0;
}

int parseNumber(char** src, JsonNumber* objNumber)
{
	if (src == NULL || *src == NULL) return -1;
	int count = 0, hasDot = 0;
	if (validNumber(*src, &count, &hasDot) != 0) return -1;

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

int validNumber(char* src, int* count, int* hasDot)
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

int parseString(char** src, JsonString* objString, int isKey)
{
	if (src == NULL || *src == NULL) return -1;

	int count = 0;
	if (validString(*src, &count, isKey) != 0) return -1;

	objString->p = (char*)malloc(count + 1);
	memcpy(objString->p, (*src) + 1, count);
	objString->p[count] = '\0';
	objString->len = count;
	*src += (2 + count);

	return 0;
}

int validString(char* src, int* count, int isKey)
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

int parseKeyValue(char** src, JsonKeyValue* objKeyValue)
{
	if (src == NULL || *src == NULL) return -1;

	if (checkJsonType(*src) != JT_STRING) return -1;
	if (parseString(src, &(objKeyValue->key), 1) != 0) return -1;

	if (checkJsonType(*src) != JT_COLON) return -1;
	*src += 1;

	if (parseBase(src, &(objKeyValue->value)) != 0) return -1;

	return 0;
}

int parseObject(char** src, JsonObject* ptrObject)
{
	if (src == NULL || *src == NULL) return -1;

	if (checkJsonType(*src) != JT_OBJECT) return -1;
	*src += 1;

	if (checkJsonType(*src) == JT_OBJECT_END)
	{
		*src += 1;
		return 0;
	}

	while (1)
	{
		JsonKVList* node = (JsonKVList*)malloc(sizeof(JsonKVList));
		if (node == NULL)
		{
			releaseJsonObject(ptrObject);
			return -1;
		}

		if (parseKeyValue(src, &(node->node)) != 0)
		{
			free(node);
			releaseJsonObject(ptrObject);
			return -1;
		}

		node->next = ptrObject->next;
		ptrObject->next = node;

		if (checkJsonType(*src) == JT_DOT)
		{
			*src += 1;
			continue;
		}
		else if (checkJsonType(*src) == JT_OBJECT_END) 
		{
			*src += 1;
			break;
		}
		else
		{
			releaseJsonObject(ptrObject);
			return -1;
		}
	}
	return 0;
}

int parseArray(char** src, JsonArray* ptrArray)
{
	if (src == NULL || *src == NULL) return -1;

	if (checkJsonType(*src) != JT_ARRAY) return -1;
	*src += 1;

	if (checkJsonType(*src) == JT_ARRAY_END)
	{
		*src += 1;
		return 0;
	}

	while (1)
	{
		JsonBaseList* jbl = (JsonBaseList*)malloc(sizeof(JsonBaseList));
		if (jbl == NULL) return -1;

		if (parseBase(src, &(jbl->node)) != 0) return -1;
		jbl->next = ptrArray->next;
		ptrArray->next = jbl;

		if (checkJsonType(*src) == JT_DOT)
		{
			*src += 1;
			continue;
		}
		else if (checkJsonType(*src) == JT_ARRAY_END)
		{
			*src += 1;
			break;
		}
		else
		{
			releaseJsonArray(ptrArray);
			return -1;
		}
	}
	return 0;
}

int checkJsonType(char* src)
{
	if (src == NULL) return -1;

	if (*src == 'n') return JT_NULL;
	else if (*src == 'f') return JT_FALSE;
	else if (*src == 't') return JT_TRUE;
	else if (isdigit(*src)) return JT_NUMBER;
	else if (*src == '"') return JT_STRING;
	else if (*src == '{') return JT_OBJECT;
	else if (*src == '[') return JT_ARRAY;
	else if (*src == ',') return JT_DOT;
	else if (*src == ':') return JT_COLON;
	else if (*src == '}') return JT_OBJECT_END;
	else if (*src == ']') return JT_ARRAY_END;
	else return JT_NONE;
}

int parseBase(char** src, JsonBase* objBase)
{
	if (src == NULL || *src == NULL) return -1;

	int jsonType = checkJsonType(*src);
	if (jsonType == JT_NULL)
	{
		JsonNull *jn = (JsonNull*)malloc(sizeof(JsonNull));
		if (jn == NULL) return -1;
		if (parseNull(src, jn) != 0) return -1;
		objBase->type = JT_NULL;
		objBase->p = (void*)jn;
	}
	else if (jsonType == JT_FALSE)
	{
		JsonFalse *jf = (JsonFalse*)malloc(sizeof(JsonFalse));
		if (jf == NULL) return -1;
		if (parseFalse(src, jf) != 0) return -1;
		objBase->type = JT_FALSE;
		objBase->p = (void*)jf;
	}
	else if (jsonType == JT_TRUE)
	{
		JsonTrue *jt = (JsonTrue*)malloc(sizeof(JsonTrue));
		if (jt == NULL) return -1;
		if (parseTrue(src, jt) != 0) return -1;
		objBase->type = JT_TRUE;
		objBase->p = (void*)jt;
	}
	else if (jsonType == JT_NUMBER)
	{
		JsonNumber *jn = (JsonNumber*)malloc(sizeof(JsonNumber));
		if (jn == NULL) return -1;
		if (parseNumber(src, jn) != 0) return -1;
		objBase->type = JT_NUMBER;
		objBase->p = (void*)jn;
	}
	else if (jsonType == JT_STRING)
	{
		JsonString *js = (JsonString*)malloc(sizeof(JsonString));
		if (js == NULL) return -1;
		if (parseString(src, js, 0) != 0) return -1;
		objBase->type = JT_STRING;
		objBase->p = (void*)js;
	}
	else if (jsonType == JT_OBJECT)
	{
		JsonObject *jo = (JsonObject*)malloc(sizeof(JsonObject));
		if (jo == NULL) return -1;
		initJsonObject(jo);
		if (parseObject(src, jo) != 0) return -1;
		objBase->type = JT_OBJECT;
		objBase->p = (void*)jo;
	}
	else if (jsonType == JT_ARRAY)
	{
		JsonArray *ja = (JsonArray*)malloc(sizeof(JsonArray));
		if (ja == NULL) return -1;
		initJsonArray(ja);
		if (parseArray(src, ja) != 0) return -1;
		objBase->type = JT_ARRAY;
		objBase->p = (void*)ja;
	}
	else
	{
		return -1;
	}
	return 0;
}

int cmpJsonString(JsonString* s, const char* str)
{
	if (s == NULL || str == NULL) return -1;

	int len = strlen(str);
	if (s->len != len) return -1;

	if (strcmp(s->p, str) != 0) return -1;

	return 0;
}

int cmpJsonNumber(JsonNumber* n, double d)
{
	if (n == NULL) return -1;

	if (fabs(n->d - d) > ACCURACY) return -1;

	return 0;
}

