
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "common.h"

int slow_raw2object(char* s, slow_object_t *jo)
{
	if (s == NULL || jo == NULL) return SLOW_NULL_PTR;

	char* temp = NULL;
	if (slow_remove_useless(s, &temp) != 0) return -1;
	char* freeTemp = temp;
	if (slow_parse_object(&temp, jo) != 0) return -1;
	
	free(freeTemp);

	return SLOW_OK;
}

int slow_raw2array(char* s, slow_array_t* ja)
{
	if (s == NULL || ja == NULL) return SLOW_NULL_PTR;

	if (slow_parse_array(&s, ja) != 0) return -1;
	
	return SLOW_OK;
}

int slow_object2raw(char** s, slow_object_t* jo)
{
	if (jo == NULL) return SLOW_NULL_PTR;

	slow_ret_string_t jrs;
	slow_init_ret_string(&jrs);
	if (slow_object2string(jo, &jrs) != 0)
	{
		slow_release_ret_string(&jrs);
		return -1;
	}
	if (slow_end2string(&jrs) != 0)
	{
		slow_release_ret_string(&jrs);
		return -1;
	}
	char* temp = (char*)malloc(jrs.offset);
	if (temp == NULL)
	{
		slow_release_ret_string(&jrs);
		return -1;
	}
	memcpy(temp, jrs.p, jrs.offset);
	*s = temp;
	slow_release_ret_string(&jrs);
	return SLOW_OK;
}

int slow_array2raw(char** s, slow_array_t* ja)
{
	if (ja == NULL) return SLOW_NULL_PTR;

	slow_ret_string_t jrs;
	slow_init_ret_string(&jrs);
	if (slow_array2string(ja, &jrs) != 0)
	{
		slow_release_ret_string(&jrs);
		return -1;
	}
	if (slow_end2string(&jrs) != 0)
	{
		slow_release_ret_string(&jrs);
		return -1;
	}
	char* temp = (char*)malloc(jrs.offset);
	if (temp == NULL)
	{
		slow_release_ret_string(&jrs);
		return -1;
	}
	memcpy(temp, jrs.p, jrs.offset);
	*s = temp;
	slow_release_ret_string(&jrs);
	return SLOW_OK;
}
