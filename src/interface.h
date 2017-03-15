#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "json.h"
#include "parser.h"
#include "stringpify.h"

int slow_raw2object(char* s, slow_object_t* jo);
int slow_raw2array(char* s, slow_array_t* ja);

int slow_object2raw(char** s, slow_object_t* jo);
int slow_array2raw(char** s, slow_array_t* ja);

#endif