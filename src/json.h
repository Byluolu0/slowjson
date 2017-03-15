#ifndef _JSON_H_
#define _JSON_H_

enum slow_type
{
	ST_NONE = 0,
	ST_NULL,
	ST_FALSE,
	ST_TRUE,
	ST_NUMBER,
	ST_STRING,
	ST_KEY_VALUE,
	ST_OBJECT,
	ST_ARRAY,
	ST_DOT,
	ST_COLON,
	ST_OBJECT_END,
	ST_ARRAY_END,
};

struct slow_base_s
{
	char type;
	void* p;
};
typedef struct slow_base_s slow_base_t;

struct slow_null_s
{
	int flag;
};
typedef struct slow_null_s slow_null_t;

struct slow_false_s
{
	int flag;
};
typedef struct slow_false_s slow_false_t;

struct slow_true_s
{
	int flag;
};
typedef struct slow_true_s slow_true_t;

struct slow_number_s
{
	double d;
};
typedef struct slow_number_s slow_number_t;

struct slow_string_s
{
	int len;
	char* p;
};
typedef struct slow_string_s slow_string_t;

struct slow_kv_s
{
	slow_string_t key;
	slow_base_t value;
};
typedef struct slow_kv_s slow_kv_t;

struct slow_object_s
{
	struct slow_kv_list_s* next;
};
typedef struct slow_object_s slow_object_t;

struct slow_kv_list_s
{
	slow_kv_t node;
	struct slow_kv_list_s* next;
};
typedef struct slow_kv_list_s slow_kv_list_t;

struct slow_base_list_s
{
	slow_base_t node;
	struct slow_base_list_s* next;
};
typedef struct slow_base_list_s slow_base_list_t;

struct slow_array_s
{
	slow_base_list_t* next;
};
typedef struct slow_array_s slow_array_t;

struct slow_ret_string_s
{
	char* p;
	int size;
	int offset;
};
typedef struct slow_ret_string_s slow_ret_string_t;

int slow_init_null(slow_null_t* ptrNull);
int slow_init_false(slow_false_t* ptrFalse);
int slow_init_true(slow_true_t* ptrTrue);
int slow_init_number(slow_number_t* ptrNumber, double d);
int slow_init_string(slow_string_t* ptrString, const char* s);
int slow_init_object(slow_object_t* ptrObject);
int slow_init_array(slow_array_t* ptrArray);
int slow_init_ret_string(slow_ret_string_t* ptr);
int slow_check_ret_string_size(slow_ret_string_t* ptr, int size);

int slow_object_get_base(slow_object_t* ptrObject, const char* k, slow_base_t** b);
int slow_object_get_null(slow_object_t* ptrObject, const char* k, int* n);
int slow_object_get_bool(slow_object_t* ptrObject, const char* k, int* b);
int slow_object_get_number(slow_object_t* ptrObject, const char* k, double* d);
int slow_object_get_string(slow_object_t* ptrObject, const char* k, slow_string_t** s);
int slow_object_get_object(slow_object_t* ptrObject, const char* k, slow_object_t** o);
int slow_object_get_array(slow_object_t* ptrObject, const char* k, slow_array_t** a);

int slow_array_get_size(slow_array_t* ptrArray);
int slow_array_get_by_index(slow_array_t* ptrArray, int index, slow_base_t** b);

int slow_release_string(slow_string_t* ptrString);
int slow_release_base(slow_base_t* ptrBase);
int slow_release_key_value(slow_kv_t* ptrKeyValue);
int slow_release_object(slow_object_t* ptrObject);
int slow_release_array(slow_array_t* ptrArray);
int slow_release_ret_string(slow_ret_string_t* ptrRetString);


#endif 
