#ifndef _STRINGPIFY_H_
#define _STRINGPIFY_H_

#include "json.h"

// tostring
int nullToString(JsonNull* ptrNull, JsonRetString* jrs);
int falseToString(JsonFalse* ptrFalse, JsonRetString* jrs);
int trueToString(JsonTrue* ptrTrue, JsonRetString* jrs);
int numberToString(JsonNumber* ptrNumber, JsonRetString* jrs);
int stringToString(JsonString* ptrString, JsonRetString* jrs);
int keyValueToString(JsonKeyValue* ptrKeyValue, JsonRetString* jrs);
int baseToString(JsonBase* ptrBase, JsonRetString* jrs);
int addToString(const char* s, JsonRetString* jrs);
int objectToString(JsonObject* ptrObject, JsonRetString* jrs);
int arrayToString(JsonArray* ptrArray, JsonRetString* jrs);
int endToString(JsonRetString* jrs);

#endif // !_STRINGPIFY_H_

