// tostring

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "json.h"
#include "common.h"
#include "stringpify.h"


int slow_null2string(slow_null_t* ptrNull, slow_ret_string_t* jrs)
{
	if (ptrNull == NULL || jrs == NULL) return -1;

	if (slow_check_ret_string_size(jrs, 4) != 0) return -1;

	memcpy(jrs->p + jrs->offset, "null", 4);
	jrs->offset += 4;
	return 0;
}

int slow_false2string(slow_false_t* ptrFalse, slow_ret_string_t* jrs)
{
	if (ptrFalse == NULL || jrs == NULL) return -1;

	if (slow_check_ret_string_size(jrs, 5) != 0) return -1;

	memcpy(jrs->p + jrs->offset, "false", 5);
	jrs->offset += 5;
	return 0;
}

int slow_true2string(slow_true_t* ptrTrue, slow_ret_string_t* jrs)
{
	if (ptrTrue == NULL || jrs == NULL) return -1;

	if (slow_check_ret_string_size(jrs, 4) != 0) return -1;

	memcpy(jrs->p + jrs->offset, "true", 4);
	jrs->offset += 4;
	return 0;
}

int slow_number2string(slow_number_t* ptrNumber, slow_ret_string_t* jrs)
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

	if (slow_check_ret_string_size(jrs, temp) != 0) return -1;

	memcpy(jrs->p + jrs->offset, buffer, temp);
	jrs->offset += temp;
	return 0;
}

int slow_string2string(slow_string_t* ptrString, slow_ret_string_t* jrs)
{
	if (ptrString == NULL || jrs == NULL) return -1;

	if (ptrString->p == NULL || ptrString->len <= 0) return -1;

	if (slow_check_ret_string_size(jrs, ptrString->len) != 0) return -1;
	if (slow_add2string("\"", jrs) != 0)  return -1;
	memcpy(jrs->p + jrs->offset, ptrString->p, ptrString->len);
	jrs->offset += ptrString->len;
	if (slow_add2string("\"", jrs) != 0)  return -1;
	return 0;
}

int slow_kv2string(slow_kv_t* ptrKeyValue, slow_ret_string_t* jrs)
{
	if (ptrKeyValue == NULL || jrs == NULL) return -1;

	if (slow_string2string(&ptrKeyValue->key, jrs) != 0) return -1;

	if (slow_add2string(":", jrs) != 0) return -1;

	if (slow_base2string(&ptrKeyValue->value, jrs) != 0) return -1;

	return 0;
}

int slow_base2string(slow_base_t* ptrBase, slow_ret_string_t* jrs)
{
	if (ptrBase == NULL || jrs == NULL) return -1;

	int jsonType = ptrBase->type;
	if (jsonType == ST_NULL)
	{
		slow_null_t* jn = (slow_null_t*)ptrBase->p;
		if (slow_null2string(jn, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == ST_FALSE)
	{
		slow_false_t* jf = (slow_false_t*)ptrBase->p;
		if (slow_false2string(jf, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == ST_TRUE)
	{
		slow_true_t* jt = (slow_true_t*)ptrBase->p;
		if (slow_true2string(jt, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == ST_NUMBER)
	{
		slow_number_t* jn = (slow_number_t*)ptrBase->p;
		if (slow_number2string(jn, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == ST_STRING)
	{
		slow_string_t* js = (slow_string_t*)ptrBase->p;
		if (slow_string2string(js, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == ST_OBJECT)
	{
		slow_object_t* jo = (slow_object_t*)ptrBase->p;
		if (slow_object2string(jo, jrs) != 0) return -1;
		return 0;
	}
	else if (jsonType == ST_ARRAY)
	{
		slow_array_t* ja = (slow_array_t*)ptrBase->p;
		if (slow_array2string(ja, jrs) != 0) return -1;
		return 0;
	}

	return -1;
}

int slow_add2string(const char* s, slow_ret_string_t* jrs)
{
	if (jrs == NULL || s == NULL) return -1;

	int len = strlen(s);
	if (slow_check_ret_string_size(jrs, len) != 0) return -1;

	memcpy(jrs->p + jrs->offset, s, len);
	jrs->offset += len;
	return 0;
}

int slow_object2string(slow_object_t* ptrObject, slow_ret_string_t* jrs)
{
	if (ptrObject == NULL || jrs == NULL) return -1;

	if (slow_add2string("{", jrs) != 0) return -1;

	slow_kv_list_t* temp = ptrObject->next;
	while (temp)
	{
		if (slow_kv2string(&temp->node, jrs) != 0) return -1;
		temp = temp->next;
		if (temp)
		{
			if (slow_add2string(",", jrs) != 0) return -1;
		}
	}

	if (slow_add2string("}", jrs) != 0) return -1;
	return 0;
}

int slow_array2string(slow_array_t* ptrArray, slow_ret_string_t* jrs)
{
	if (ptrArray == NULL || jrs == NULL) return -1;

	if (slow_add2string("[", jrs) != 0) return -1;

	slow_base_list_t* temp = ptrArray->next;
	while (temp)
	{
		if (slow_base2string(&temp->node, jrs) != 0) return -1;
		temp = temp->next;
		if (temp)
		{
			if (slow_add2string(",", jrs) != 0) return -1;
		}
	}

	if (slow_add2string("]", jrs) != 0) return -1;
	return 0;
}

int slow_end2string(slow_ret_string_t* jrs)
{
	if (jrs == NULL) return -1;
	if (slow_check_ret_string_size(jrs, 1) != 0) return -1;

	jrs->p[jrs->offset] = '\0';
	jrs->offset += 1;
	return 0;
}