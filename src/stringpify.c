
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "json.h"
#include "common.h"
#include "stringpify.h"


int slow_null2string(slow_null_t* pn, slow_string_t* ps)
{
	assert(pn != NULL);
	assert(ps != NULL);
	slow_string_push(ps, "null");

	return SLOW_OK;
}

int slow_false2string(slow_false_t* pf, slow_string_t* ps)
{
	assert(pf != NULL);
	assert(ps != NULL);
	slow_string_push(ps, "false");

	return SLOW_OK;
}

int slow_true2string(slow_true_t* pt, slow_string_t* ps)
{
	assert(pt != NULL);
	assert(ps != NULL);
	slow_string_push(ps, "true");
	return SLOW_OK;
}

int slow_number2string(slow_number_t* pn, slow_string_t* ps)
{
	/*
	assert(pn != NULL);
	assert(ps != NULL);

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
	*/
	return SLOW_OK;
}

void slow_string2string(slow_string_t* psrcs, slow_string_t* ps)
{
	assert(psrcs != NULL);
	assert(ps != NULL);

	slow_string_push(psrcs, "\0");
	slow_string_push(ps, psrcs->p);
}

int slow_kv2string(slow_kv_t* pkv, slow_string_t* ps)
{
	assert(pkv != NULL);
	assert(ps != NULL);

	slow_string2string(&pkv->key, ps);
	slow_string_push(ps, ":");

	if (slow_base2string(&pkv->value, ps) != 0) return -1;

	return 0;
}

int slow_base2string(slow_base_t* pb, slow_string_t* ps)
{
	assert(pb != NULL);
	assert(ps != NULL);

	int jsonType = pb->type;
	if (jsonType == ST_NULL)
	{
		slow_null_t* jn = (slow_null_t*)pb->p;
		slow_null2string(jn, ps);
		return SLOW_OK;
	}
	else if (jsonType == ST_FALSE)
	{
		slow_false_t* jf = (slow_false_t*)pb->p;
		slow_false2string(jf, ps);
		return SLOW_OK;
	}
	else if (jsonType == ST_TRUE)
	{
		slow_true_t* jt = (slow_true_t*)pb->p;
		slow_true2string(jt, ps);
		return SLOW_OK;
	}
	else if (jsonType == ST_NUMBER)
	{
		slow_number_t* jn = (slow_number_t*)pb->p;
		slow_number2string(jn, ps);
		return SLOW_OK;
	}
	else if (jsonType == ST_STRING)
	{
		slow_string_t* psrcs = (slow_string_t*)pb->p;
		slow_string2string(psrcs, ps);
		return SLOW_OK;
	}
	else if (jsonType == ST_OBJECT)
	{
		slow_object_t* jo = (slow_object_t*)ps->p;
		if (slow_object2string(jo, ps) != 0) return -1;
		return 0;
	}
	else if (jsonType == ST_ARRAY)
	{
		slow_array_t* ja = (slow_array_t*)pb->p;
		if (slow_array2string(ja, ps) != 0) return -1;
		return 0;
	}

	return -1;
}

int slow_add2string(const char* s, slow_ret_string_t* jrs)
{
	if (jrs == NULL || s == NULL) return SLOW_NULL_PTR;

	int len = strlen(s);
	if (slow_check_ret_string_size(jrs, len) != 0) return -1;

	memcpy(jrs->p + jrs->offset, s, len);
	jrs->offset += len;
	return 0;
}

int slow_object2string(slow_object_t* ptrObject, slow_ret_string_t* jrs)
{
	if (ptrObject == NULL || jrs == NULL) return SLOW_NULL_PTR;

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
	if (ptrArray == NULL || jrs == NULL) return SLOW_NULL_PTR;

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
	if (jrs == NULL) return SLOW_NULL_PTR;
	if (slow_check_ret_string_size(jrs, 1) != 0) return -1;

	jrs->p[jrs->offset] = '\0';
	jrs->offset += 1;
	return 0;
}