
#ifndef _COMMON_H_
#define _COMMON_H_

const static double ACCURACY = 0.0000000000000001;
const static int STRING_INIT_SIZE = 256;
const static int NUMBER_BIT_SIZE = 22;

enum
{
	SLOW_OK = 0,
	SLOW_UNKNOWN,
	SLOW_NULL_PTR,
	SLOW_ELEMENT_NOT_FOUND,
	SLOW_TYPE_NOT_FOUND,
	SLOW_INVALID_VALUE,
	SLOW_MOMERY_ERROR,
	SLOW_PARAM_ERROR,
};

#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')

#endif // !_UTIL_H_
