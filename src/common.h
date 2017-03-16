
#ifndef _COMMON_H_
#define _COMMON_H_

const static double ACCURACY = 0.000001;
const static int RET_STRING_INIT_SIZE = 1024;
const static int NUMBER_BIT_SIZE = 22;
const static double MAX_NUMBER = 10000000000;

enum
{
	SLOW_OK = 0,
	SLOW_NULL_PTR,
	SLOW_ELEMENT_NOT_FOUND,
	SLOW_TYPE_NOT_FOUND,
};

#endif // !_UTIL_H_
