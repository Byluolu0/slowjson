
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "json.h"
#include "common.h"
#include "stringpify.h"


int slow_null2string(slow_null_t* pn, slow_string_t* ps)
{
	assert(pn != NULL);
	assert(ps != NULL);
	slow_string_pushs(ps, "null");

	return SLOW_OK;
}

int slow_false2string(slow_false_t* pf, slow_string_t* ps)
{
	assert(pf != NULL);
	assert(ps != NULL);
	slow_string_pushs(ps, "false");

	return SLOW_OK;
}

int slow_true2string(slow_true_t* pt, slow_string_t* ps)
{
	assert(pt != NULL);
	assert(ps != NULL);
	slow_string_pushs(ps, "true");
	return SLOW_OK;
}

int slow_number2string(slow_number_t* pn, slow_string_t* ps)
{
	assert(pn != NULL);
	assert(ps != NULL);

	char buffer[32];

	int len = sprintf_s(buffer, 32, "%.17g", pn->d);
	slow_string_pushs_len(ps, buffer, len);
	return SLOW_OK;
}

int slow_string2string(slow_string_t* psrcs, slow_string_t* ps)
{
	assert(psrcs != NULL);
	assert(ps != NULL);

	slow_string_pushc(ps, '\"');
	slow_string_pushs_len(ps, psrcs->p, psrcs->offset);
	slow_string_pushc(ps, '\"');

	return SLOW_OK;
}

int slow_kv2string(slow_kv_t* pkv, slow_string_t* ps)
{
	assert(pkv != NULL);
	assert(ps != NULL);

	int ret;

	if ((ret = slow_string2string(&pkv->key, ps)) != SLOW_OK) return ret;
	slow_string_pushc(ps, ':');
	if ((ret = slow_base2string(&pkv->value, ps)) != SLOW_OK) return ret;

	return SLOW_OK;
}

int slow_base2string(slow_base_t* pb, slow_string_t* ps)
{
	assert(pb != NULL);
	assert(ps != NULL);

	int jsonType = pb->type;
	if (jsonType == ST_NULL)
	{
		slow_null_t* jn = (slow_null_t*)pb->p;
		return slow_null2string(jn, ps);
	}
	else if (jsonType == ST_FALSE)
	{
		slow_false_t* jf = (slow_false_t*)pb->p;
		return slow_false2string(jf, ps);
	}
	else if (jsonType == ST_TRUE)
	{
		slow_true_t* jt = (slow_true_t*)pb->p;
		return slow_true2string(jt, ps);
	}
	else if (jsonType == ST_NUMBER)
	{
		slow_number_t* jn = (slow_number_t*)pb->p;
		return slow_number2string(jn, ps);
	}
	else if (jsonType == ST_STRING)
	{
		slow_string_t* psrcs = (slow_string_t*)pb->p;
		return slow_string2string(psrcs, ps);
	}
	else if (jsonType == ST_OBJECT)
	{
		slow_object_t* jo = (slow_object_t*)ps->p;
		return slow_object2string(jo, ps);
	}
	else if (jsonType == ST_ARRAY)
	{
		slow_array_t* ja = (slow_array_t*)pb->p;
		return slow_array2string(ja, ps);
	}

	return SLOW_UNKNOWN_TYPE;
}

int slow_object2string(slow_object_t* po, slow_string_t* ps)
{
	assert(po != NULL);
	assert(ps != NULL);

	int ret;

	slow_string_pushc(ps, '{');

	slow_kv_list_t* temp = po->next;
	while (temp)
	{
		if ((ret = slow_kv2string(&temp->node, ps)) != SLOW_OK) return ret;
		temp = temp->next;
		if (temp) slow_string_pushc(ps, ',');
	}

	slow_string_pushc(ps, '}');
	return SLOW_OK;
}

int slow_array2string(slow_array_t* pa, slow_string_t* ps)
{
	assert(pa != NULL);
	assert(ps != NULL);

	int ret;

	slow_string_pushc(ps, '[');
	slow_base_list_t* temp = pa->next;
	while (temp)
	{
		if ((ret = slow_base2string(&temp->node, ps)) != SLOW_OK) return ret;
		temp = temp->next;
		if (temp) slow_string_pushc(ps, ',');
	}

	slow_string_pushc(ps, ']');
	return SLOW_OK;
}
