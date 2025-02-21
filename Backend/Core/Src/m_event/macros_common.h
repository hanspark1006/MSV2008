/*
 * macros_common.h
 *
 *  Created on: Feb 6, 2024
 *      Author: catsa
 */

#ifndef SRC_M_EVENT_MACROS_COMMON_H_
#define SRC_M_EVENT_MACROS_COMMON_H_

#define CEIL_DIV(a, b)  ((((a) - 1) / (b)) + 1)

#define ZERO_OR_COMPILE_ERROR(cond) ((int) sizeof(char[1 - 2 * !(cond)]) - 1)

#define case_str(name) case name : return #name;

#define BREAK_IF_ERROR(err_code) \
if ((err_code) != 0) \
{ \
	LOG_ERR(__FILE__); \
    LOG_ERR(" - ERROR. line: %d, with error code %d \r\n", __LINE__, err_code); \
    break; \
}

#define CONTINUE_IF_ERROR(err_code) \
if ((err_code) != 0) \
{ \
	LOG_ERR(__FILE__); \
    LOG_ERR(" - ERROR. line: %d, with error code %d \r\n", __LINE__, err_code); \
    continue; \
}

#define RETURN_IF_ERROR(err_code) \
if ((err_code) != 0) \
{ \
	LOG_ERR(__FILE__); \
    LOG_ERR(" - ERROR. line: %d, with error code %d \r\n", __LINE__, err_code); \
    return (err_code); \
}

#define REPORT_IF_ERROR(err_code) \
if ((err_code) != 0) \
{ \
	LOG_ERR(__FILE__); \
    LOG_ERR(" - ERROR. line: %d, with error code %d \r\n", __LINE__, err_code); \
}

#define NULL_PARAM_CHECK(param)					\
if ((param) == NULL)							\
{												\
	LOG_ERR(__FILE__);							\
	LOG_ERR(" - NULL. line: %d\r\n", __LINE__);	\
	return -EINVAL;								\
}

#define IS_ARRAY(array) \
	ZERO_OR_COMPILE_ERROR( \
		!__builtin_types_compatible_p(__typeof__(array), \
					      __typeof__(&(array)[0])))

#define ARRAY_SIZE(array) \
	((size_t) (IS_ARRAY(array) + (sizeof(array) / sizeof((array)[0]))))
#endif /* SRC_M_EVENT_MACROS_COMMON_H_ */
