#ifndef _PARSER_H_
#define _PARSER_H_

#include "json.h"

// ½âÎö
int removeUseless(const char* src, char** dst);

int parseNull(char** src, JsonNull* objNull);
int validNull(char* src);

int parseTrue(char** src, JsonTrue* objTrue);
int validTrue(char* src);

int parseFalse(char** src, JsonFalse* objFalse);
int validFalse(char* src);

int parseNumber(char** src, JsonNumber* objNumber);
int validNumber(char* src, int* count, int* hasDot);

int parseString(char** src, JsonString* objString, int isKey);
int validString(char* src, int* count, int isKey);

int parseKeyValue(char** src, JsonKeyValue* objKeyValue);

int parseObject(char** src, JsonObject* objObject);

int parseBase(char** src, JsonBase* objBase);

int parseArray(char** src, JsonArray* ptrArray);


// ¸¨Öúº¯Êý
int checkJsonType(char* src);

int cmpJsonString(JsonString* s, const char* str);
int cmpJsonNumber(JsonNumber* n, double d);
#endif 

