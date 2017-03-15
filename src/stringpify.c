// tostring

#include <stdio.h>
#include "json.h"
#include "common.h"


int nullToString(JsonNull* ptrNull, JsonRetString* jrs)
{
	if (ptrNull == NULL || jrs == NULL) return -1;

	if (checkJsonRetStringSize(jrs, 4) != 0) return -1;

	memcpy(jrs->p + jrs->offset, "null", 4);
	jrs->offset += 4;
	return 0;
}

int falseToString(JsonFalse* ptrFalse, JsonRetString* jrs)
{
	if (ptrFalse == NULL || jrs == NULL) return -1;

	if (checkJsonRetStringSize(jrs, 5) != 0) return -1;

	memcpy(jrs->p + jrs->offset, "false", 5);
	jrs->offset += 5;
	return 0;
}

int trueToString(JsonTrue* ptrTrue, JsonRetString* jrs)
{
	if (ptrTrue == NULL || jrs == NULL) return -1;

	if (checkJsonRetStringSize(jrs, 4) != 0) return -1;

	memcpy(jrs->p + jrs->offset, "true", 4);
	jrs->offset += 4;
	return 0;
}

int numberToString(JsonNumber* ptrNumber, JsonRetString* jrs)
{
	if (ptrNumber == NULL || jrs == NULL) return -1;

	if (ptrNumber->d > MAX_NUMBER) return -1;

	char* buffer = (char*)malloc(NUMBER_BIT_SIZE);
	if (buffer == NULL) return -1;
	memset(buffer, 0, NUMBER_BIT_SIZE);

	sprintf_s(buffer, NUMBER_BIT_SIZE, "%.10lf", ptrNumber->d);
	int len = strlen(buffer);
	int temp = len;
	while (temp--)
	{
		char aa = buffer[temp];
		if (buffer[temp] == '0')
		{
			buffer[temp] = '\0';
			continue;
		}
		else if (buffer[temp] == '.')
		{
			buffer[temp] = '\0';
			break;
		}
		else if (buffer[temp] == '\0')
		{
			continue;
		}
		else
		{
			temp++;
			break;
		}
	}

	//len = temp + 1;

	if (checkJsonRetStringSize(jrs, temp) != 0) return -1;

	memcpy(jrs->p + jrs->offset, buffer, temp);
	jrs->offset += temp;
	return 0;
}

int stringToString(JsonString* ptrString, JsonRetString* jrs)
{
	if (ptrString == NULL || jrs == NULL) return -1;

	if (ptrString->p == NULL || ptrString->len <= 0) return -1;

	if (checkJsonRetStringSize(jrs, ptrString->len) != 0) return -1;
	if (addToString("\"", jrs) != 0)  return -1;
	memcpy(jrs->p + jrs->offset, ptrString->p, ptrString->len);
	jrs->offset += ptrString->len;
	if (addToString("\"", jrs) != 0)  return -1;
	return 0;
}

int keyValueToString(JsonKeyValue* ptrKeyValue, JsonRetString* jrs)
{
	if (ptrKeyValue == NULL || jrs == NULL) return -1;

	if (stringToString(&ptrKeyValue->key, jrs) != 0) return -1;

	if (addToString(":", jrs) != 0) return -1;

	if (baseToString(&ptrKeyValue->value, jrs) != 0) return -1;

	return 0;
}

int baseToString(JsonBase* ptrBase, JsonRetString* jrs)
{
	if (ptrBase == NULL || jrs == NULL) return -1;

	int jsonType = ptrBase->type;
	if (jsonType == JT_NULL)
	{
		JsonNull* jn = (JsonNull*)ptrBase->p;
		if (nullToString(jn, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == JT_FALSE)
	{
		JsonFalse* jf = (JsonFalse*)ptrBase->p;
		if (falseToString(jf, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == JT_TRUE)
	{
		JsonTrue* jt = (JsonTrue*)ptrBase->p;
		if (trueToString(jt, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == JT_NUMBER)
	{
		JsonNumber* jn = (JsonNumber*)ptrBase->p;
		if (numberToString(jn, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == JT_STRING)
	{
		JsonString* js = (JsonString*)ptrBase->p;
		if (stringToString(js, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == JT_OBJECT)
	{
		JsonObject* jo = (JsonObject*)ptrBase->p;
		if (objectToString(jo, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == JT_ARRAY)
	{
		JsonArray* ja = (JsonArray*)ptrBase->p;
		if (arrayToString(ja, jrs) != 0) return -1;
		return 0;
	}

	return -1;
}

int addToString(const char* s, JsonRetString* jrs)
{
	if (jrs == NULL || s == NULL) return -1;

	int len = strlen(s);
	if (checkJsonRetStringSize(jrs, len) != 0) return -1;

	memcpy(jrs->p + jrs->offset, s, len);
	jrs->offset += len;
	return 0;
}

int objectToString(JsonObject* ptrObject, JsonRetString* jrs)
{
	if (ptrObject == NULL || jrs == NULL) return -1;

	if (addToString("{", jrs) != 0) return -1;

	JsonKVList* temp = ptrObject->next;
	while (temp)
	{
		if (keyValueToString(&temp->node, jrs) != 0) return -1;
		temp = temp->next;
		if (temp)
		{
			if (addToString(",", jrs) != 0) return -1;
		}
	}

	if (addToString("}", jrs) != 0) return -1;
	return 0;
}

int arrayToString(JsonArray* ptrArray, JsonRetString* jrs)
{
	if (ptrArray == NULL || jrs == NULL) return -1;

	if (addToString("[", jrs) != 0) return -1;

	JsonBaseList* temp = ptrArray->next;
	while (temp)
	{
		if (baseToString(&temp->node, jrs) != 0) return -1;
		temp = temp->next;
		if (temp)
		{
			if (addToString(",", jrs) != 0) return -1;
		}
	}

	if (addToString("]", jrs) != 0) return -1;
	return 0;
}

int endToString(JsonRetString* jrs)
{
	if (jrs == NULL) return -1;
	if (checkJsonRetStringSize(jrs, 1) != 0) return -1;

	jrs->p[jrs->offset] = '\0';
	jrs->offset += 1;
	return 0;
}