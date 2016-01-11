#ifndef MJS_COMMON_H
#define MJS_COMMON_H

#include <stdint.h>

#ifdef __cplusplus
#define HEADER_BEGIN extern "C"{
#define HEADER_END };
#else
#define HEADER_BEGIN
#define HEADER_END
#endif

typedef unsigned char bool;
typedef uint32_t js_symbol;

typedef struct js_value js_value;
typedef struct js_object js_object;
typedef struct js_function js_function;
typedef struct js_environment js_environment;

js_symbol murmur(char* str);

#define true 1
#define false 0

#endif