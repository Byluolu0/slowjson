#include "json.h"
#include "common.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int initJsonNull(JsonNull* ptrNull)
{
	if (ptrNull == NULL) return -1;
	ptrNull->flag = 1;
	return 0;
}

int initJsonFalse(JsonFalse* ptrFalse)
{
	if (ptrFalse == NULL) return -1;
	ptrFalse->flag = 1;
	return 0;
}

int initJsonTrue(JsonTrue* ptrTrue)
{
	if (ptrTrue == NULL) return -1;
	ptrTrue->flag = 1;
	return 0;
}

int initJsonNumber(JsonNumber* ptrNumber, double d)
{
	if (ptrNumber == NULL) return -1;
	ptrNumber->d = d;
	return 0;
}

int initJsonString(JsonString* ptrString, const char* s)
{
	if (ptrString == NULL || s == NULL) return -1;
	int len = strlen(s);
	ptrString->p = (char*)malloc(len + 1);
	if (ptrString->p == NULL) return -1;
	memcpy(ptrString->p, s, len);
	ptrString->p[len] = '\0';
	ptrString->len = len;
	return 0;
}

int initJsonObject(JsonObject* ptrObject)
{
	if (ptrObject == NULL) return -1;
	ptrObject->next = NULL;
	return 0;
}

int initJsonArray(JsonArray* ptrArray)
{
	if (ptrArray == NULL) return -1;
	ptrArray->next = NULL;
	return 0;
}

int initJsonRetString(JsonRetString* ptr)
{
	if (ptr == NULL) return -1;
	ptr->offset = 0;
	ptr->size = RET_STRING_INIT_SIZE;
	ptr->p = (char*)malloc(ptr->size);
	if (ptr->p == NULL) return -1;
	return 0;
}

int checkJsonRetStringSize(JsonRetString* ptr, int size)
{
	if (ptr == NULL || size < 0) return -1;
	if (ptr->offset + size <= ptr->size) return 0;
	char* temp = (char*)realloc(ptr->p, ptr->size * 2);
	if (temp == NULL) return -1;
	ptr->p = temp;
	ptr->size = ptr->size * 2;
	return 0;
}

int ObjectGetBase(JsonObject* ptrObject, const char* k, JsonBase** b)
{
	if (ptrObject == NULL || k == NULL) return -1;

	JsonKVList* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		JsonKeyValue* node = &temp->node;
		if (cmpJsonString(&node->key, k) == 0)
		{
			*b = &node->value;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int ObjectGetNull(JsonObject* ptrObject, const char* k, int* n)
{
	if (ptrObject == NULL || k == NULL) return -1;

	JsonKVList* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		JsonKeyValue* node = &temp->node;
		if (cmpJsonString(&node->key, k) == 0 && node->value.type == JT_NULL)
		{
			*n = 1;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int ObjectGetBool(JsonObject* ptrObject, const char* k, int* b)
{
	if (ptrObject == NULL || k == NULL) return -1;

	JsonKVList* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		JsonKeyValue* node = &temp->node;
		if (cmpJsonString(&node->key, k) == 0 && node->value.type == JT_FALSE)
		{
			*b = 0;
			return 0;
		}
		if (cmpJsonString(&node->key, k) == 0 && node->value.type == JT_TRUE)
		{
			*b = 1;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int ObjectGetNumber(JsonObject* ptrObject, const char* k, double* d)
{
	if (ptrObject == NULL || k == NULL) return -1;

	JsonKVList* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		JsonKeyValue* node = &temp->node;
		if (cmpJsonString(&node->key, k) == 0 && node->value.type == JT_NUMBER)
		{
			*d = ((JsonNumber*)node->value.p)->d;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int ObjectGetString(JsonObject* ptrObject, const char* k, JsonString** s)
{
	if (ptrObject == NULL || k == NULL) return -1;

	JsonKVList* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		JsonKeyValue* node = &temp->node;
		if (cmpJsonString(&node->key, k) == 0 && node->value.type == JT_STRING)
		{
			*s = (JsonString*)node->value.p;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int ObjectGetObject(JsonObject* ptrObject, const char* k, JsonObject** o)
{
	if (ptrObject == NULL || k == NULL) return -1;

	JsonKVList* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		JsonKeyValue* node = &temp->node;
		if (cmpJsonString(&node->key, k) == 0 && node->value.type == JT_OBJECT)
		{
			*o = (JsonObject*)node->value.p;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int ObjectGetArray(JsonObject* ptrObject, const char* k, JsonArray** a)
{
	if (ptrObject == NULL || k == NULL) return -1;

	JsonKVList* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		JsonKeyValue* node = &temp->node;
		if (cmpJsonString(&node->key, k) == 0 && node->value.type == JT_ARRAY)
		{
			*a = (JsonArray*)node->value.p;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int ArraySize(JsonArray* ptrArray)
{
	if (ptrArray == NULL) return 0;

	JsonBaseList* temp = ptrArray->next;
	int count = 0;
	while (temp)
	{
		count++;
		temp = temp->next;
	}
	return count;
}

int ArrayGetByIndex(JsonArray* ptrArray, int index, JsonBase** b)
{
	if (ptrArray == NULL) return -1;

	JsonBaseList* temp = ptrArray->next;
	int count = 0;
	while (temp)
	{
		if (index == count)
		{
			*b = &(temp->node);
			return 0;
		}
		temp = temp->next;
		count++;
	}
	return -1;
}

int releaseJsonBase(JsonBase *ptrBase)
{
	if (ptrBase == NULL) return -1;

	if (ptrBase->type == JT_NULL)
	{
		free((JsonNull*)ptrBase->p);
	}
	else if (ptrBase->type == JT_FALSE)
	{
		free((JsonFalse*)ptrBase->p);
	}
	else if (ptrBase->type == JT_TRUE)
	{
		free((JsonTrue*)ptrBase->p);
	}
	else if (ptrBase->type == JT_NUMBER)
	{
		free((JsonNumber*)ptrBase->p);
	}
	else if (ptrBase->type == JT_STRING)
	{
		free((JsonString*)ptrBase->p);
	}
	else if (ptrBase->type == JT_KEY_VALUE)
	{
		JsonKeyValue* jkv = (JsonKeyValue*)ptrBase->p;
		releaseJsonKeyValue(jkv);
		free(jkv);
	}
	else if (ptrBase->type == JT_OBJECT)
	{
		JsonObject* jo = (JsonObject*)ptrBase->p;
		releaseJsonObject(jo);
		free(jo);
	}
	else if (ptrBase->type == JT_ARRAY)
	{
		JsonArray* ja = (JsonArray*)ptrBase->p;
		releaseJsonArray(ja);
		free(ja);
	}
	else
	{
		return -1;
	}
	return 0;
}

int releaseJsonString(JsonString* ptrString)
{
	if (ptrString == NULL) return -1;

	if (ptrString->len > 0 && ptrString->p != NULL)
	{
		free(ptrString->p);
		ptrString->len = 0;
	}
	return 0;
}

int releaseJsonKeyValue(JsonKeyValue* ptrKeyValue)
{
	if (ptrKeyValue == NULL) return -1;

	releaseJsonBase(&(ptrKeyValue->value));

	return 0;
}

int releaseJsonObject(JsonObject *ptrObject)
{
	if (ptrObject == NULL) return -1;

	while (ptrObject->next)
	{
		JsonKVList* jkvl = ptrObject->next;
		ptrObject->next = jkvl->next;
		releaseJsonKeyValue(&(jkvl->node));
		free(jkvl);
	}
	return 0;
}

int releaseJsonArray(JsonArray* ptrArray)
{
	if (ptrArray == NULL) return -1;

	while (ptrArray->next)
	{
		JsonBaseList* jbl = ptrArray->next;
		ptrArray->next = jbl->next;
		releaseJsonBase(&(jbl->node));
		free(jbl);
	}
	return 0;
}

int releaseJsonRetString(JsonRetString* ptrRetString)
{
	if (ptrRetString == NULL) return -1;

	if (ptrRetString->p == NULL) return 0;

	free(ptrRetString->p);
	ptrRetString->offset = 0;
	ptrRetString->size = 0;
	return 0;
}


