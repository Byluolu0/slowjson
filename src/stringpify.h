
#ifndef _STRINGPIFY_H_
#define _STRINGPIFY_H_

#include "json.h"

// tostring
int slow_null2string(slow_null_t* ptrNull, slow_ret_string_t* jrs);
int slow_false2string(slow_false_t* ptrFalse, slow_ret_string_t* jrs);
int slow_true2string(slow_true_t* ptrTrue, slow_ret_string_t* jrs);
int slow_number2string(slow_number_t* ptrNumber, slow_ret_string_t* jrs);
int slow_string2string(slow_string_t* ptrString, slow_ret_string_t* jrs);
int slow_kv2string(slow_kv_t* ptrKeyValue, slow_ret_string_t* jrs);
int slow_base2string(slow_base_t* ptrBase, slow_ret_string_t* jrs);
int slow_add2string(const char* s, slow_ret_string_t* jrs);
int slow_object2string(slow_object_t* ptrObject, slow_ret_string_t* jrs);
int slow_array2string(slow_array_t* ptrArray, slow_ret_string_t* jrs);
int slow_end2string(slow_ret_string_t* jrs);

#endif // !_STRINGPIFY_H_

