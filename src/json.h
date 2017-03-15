#ifndef _JSON_H_
#define _JSON_H_

enum JsonType
{
	JT_NONE = 0,
	JT_NULL,
	JT_FALSE,
	JT_TRUE,
	JT_NUMBER,
	JT_STRING,
	JT_KEY_VALUE,
	JT_OBJECT,
	JT_ARRAY,
	JT_DOT,
	JT_COLON,
	JT_OBJECT_END,
	JT_ARRAY_END,
};

struct JsonBaseS
{
	char type;
	void* p;
};
typedef struct JsonBaseS JsonBase;

struct JsonNullS
{
	int flag;
};
typedef struct JsonNullS JsonNull;

struct JsonFalseS
{
	int flag;
};
typedef struct JsonFalseS JsonFalse;

struct JsonTrueS
{
	int flag;
};
typedef struct JsonTrueS JsonTrue;

struct JsonNumberS
{
	double d;
};
typedef struct JsonNumberS JsonNumber;

struct JsonStringS
{
	int len;
	char* p;
};
typedef struct JsonStringS JsonString;

struct JsonKeyValueS
{
	JsonString key;
	JsonBase value;
};
typedef struct JsonKeyValueS JsonKeyValue;

struct JsonObjectS
{
	struct JsonKVListS* next;
};
typedef struct JsonObjectS JsonObject;

struct JsonKVListS
{
	JsonKeyValue node;
	struct JsonKVListS* next;
};
typedef struct JsonKVListS JsonKVList;

struct JsonBaseListS
{
	JsonBase node;
	struct JsonBaseListS* next;
};
typedef struct JsonBaseListS JsonBaseList;

struct JsonArrayS
{
	JsonBaseList* next;
};
typedef struct JsonArrayS JsonArray;

struct JsonRetStringS
{
	char* p;
	int size;
	int offset;
};
typedef struct JsonRetStringS JsonRetString;

int initJsonNull(JsonNull* ptrNull);
int initJsonFalse(JsonFalse* ptrFalse);
int initJsonTrue(JsonTrue* ptrTrue);
int initJsonNumber(JsonNumber* ptrNumber, double d);
int initJsonString(JsonString* ptrString, const char* s);
int initJsonObject(JsonObject* ptrObject);
int initJsonArray(JsonArray* ptrArray);
int initJsonRetString(JsonRetString* ptr);
int checkJsonRetStringSize(JsonRetString* ptr, int size);

int ObjectGetBase(JsonObject* ptrObject, const char* k, JsonBase** b);
int ObjectGetNull(JsonObject* ptrObject, const char* k, int* n);
int ObjectGetBool(JsonObject* ptrObject, const char* k, int* b);
int ObjectGetNumber(JsonObject* ptrObject, const char* k, double* d);
int ObjectGetString(JsonObject* ptrObject, const char* k, JsonString** s);
int ObjectGetObject(JsonObject* ptrObject, const char* k, JsonObject** o);
int ObjectGetArray(JsonObject* ptrObject, const char* k, JsonArray** a);

int ArraySize(JsonArray* ptrArray);
int ArrayGetByIndex(JsonArray* ptrArray, int index, JsonBase** b);

int releaseJsonString(JsonString* ptrString);
int releaseJsonBase(JsonBase* ptrBase);
int releaseJsonKeyValue(JsonKeyValue* ptrKeyValue);
int releaseJsonObject(JsonObject* ptrObject);
int releaseJsonArray(JsonArray* ptrArray);
int releaseJsonRetString(JsonRetString* ptrRetString);


#endif 
