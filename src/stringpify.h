
#ifndef _STRINGPIFY_H_
#define _STRINGPIFY_H_

#include "json.h"

// tostring
int slow_null2string(slow_null_t* pn, slow_string_t* ps);
int slow_false2string(slow_false_t* pf, slow_string_t* ps);
int slow_true2string(slow_true_t* pt, slow_string_t* ps);
int slow_number2string(slow_number_t* pn, slow_string_t* ps);
int slow_string2string(slow_string_t* psrcs, slow_string_t* ps);
int slow_kv2string(slow_kv_t* pkv, slow_string_t* ps);
int slow_base2string(slow_base_t* pb, slow_string_t* ps);
int slow_object2string(slow_object_t* po, slow_string_t* ps);
int slow_array2string(slow_array_t* pa, slow_string_t* ps);

#endif // !_STRINGPIFY_H_

