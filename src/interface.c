#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

int toJsonObject(char* s, JsonObject *jo)
{
	if (s == NULL || jo == NULL) return -1;

	char* temp = NULL;
	if (removeUseless(s, &temp) != 0) return -1;
	char* freeTemp = temp;
	if (parseObject(&temp, jo) != 0) return -1;
	
	free(freeTemp);

	return 0;
}

int toJsonArray(char* s, JsonArray* ja)
{
	if (s == NULL || ja == NULL) return -1;

	if (parseArray(&s, ja) != 0) return -1;
	
	return 0;
}

int OtoString(char** s, JsonObject* jo)
{
	if (jo == NULL) return -1;

	JsonRetString jrs;
	initJsonRetString(&jrs);
	if (objectToString(jo, &jrs) != 0)
	{
		releaseJsonRetString(&jrs);
		return -1;
	}
	if (endToString(&jrs) != 0)
	{
		releaseJsonRetString(&jrs);
		return -1;
	}
	char* temp = (char*)malloc(jrs.offset);
	if (temp == NULL)
	{
		releaseJsonRetString(&jrs);
		return -1;
	}
	memcpy(temp, jrs.p, jrs.offset);
	*s = temp;
	releaseJsonRetString(&jrs);
	return 0;
}

int AtoString(char** s, JsonArray* ja)
{
	if (ja == NULL) return -1;

	JsonRetString jrs;
	initJsonRetString(&jrs);
	if (arrayToString(ja, &jrs) != 0)
	{
		releaseJsonRetString(&jrs);
		return -1;
	}
	if (endToString(&jrs) != 0)
	{
		releaseJsonRetString(&jrs);
		return -1;
	}
	char* temp = (char*)malloc(jrs.offset);
	if (temp == NULL)
	{
		releaseJsonRetString(&jrs);
		return -1;
	}
	memcpy(temp, jrs.p, jrs.offset);
	*s = temp;
	releaseJsonRetString(&jrs);
	return 0;
}


int printJsonBase(JsonBase* jb)
{
	if (jb == NULL) return -1;


}