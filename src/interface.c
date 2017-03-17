
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "interface.h"
#include "common.h"

int slow_raw2object(char* s, slow_object_t *jo)
{
	assert(s != NULL);
	assert(jo != NULL);

	int ret;
	if ((ret = slow_parse_object(&s, jo)) != SLOW_OK) return ret;

	return SLOW_OK;
}

int slow_raw2array(char* s, slow_array_t* ja)
{
	assert(s != NULL);
	assert(ja != NULL);

	int ret;

	if ((ret = slow_parse_array(&s, ja)) != SLOW_OK) return ret;
	
	return SLOW_OK;
}

int slow_object2raw(char** s, slow_object_t* jo)
{
	assert(jo != NULL);

	int ret;

	slow_string_t ps;
	slow_init_string(&ps);
	if ((ret = slow_object2string(jo, &ps)) != SLOW_OK)
	{
		slow_release_string(&ps);
		return ret;
	}
	slow_string_pushc(&ps, '\0');
	char* temp = (char*)malloc(ps.offset);
	memcpy(temp, ps.p, ps.offset);
	*s = temp;
	slow_release_string(&ps);
	return SLOW_OK;
}

int slow_array2raw(char** s, slow_array_t* ja)
{
	assert(ja != NULL);

	int ret;

	slow_string_t ps;
	slow_init_string(&ps);
	if ((ret = slow_array2string(ja, &ps)) != SLOW_OK)
	{
		slow_release_string(&ps);
		return ret;
	}
	slow_string_pushc(&ps, '\0');
	char* temp = (char*)malloc(ps.offset);
	memcpy(temp, ps.p, ps.offset);
	*s = temp;
	slow_release_string(&ps);
	return SLOW_OK;
}
