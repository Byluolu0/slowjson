#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "json.h"
#include "parser.h"
#include "stringpify.h"

int toJsonObject(char* s, JsonObject* jo);
int toJsonArray(char* s, JsonArray* ja);

int OtoString(char** s, JsonObject* jo);
int AtoString(char** s, JsonArray* ja);

int printJsonBase(JsonBase* jb);

#endif