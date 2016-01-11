#ifndef MJS_JSOPCODE_H
#define MJS_JSOPCODE_H

#include <stddef.h>

typedef enum{
    OP_NEWARRAY,
    OP_NEWOBJECT,
    OP_UNDEFINED,
    OP_NULL,
    OP_TRUE,
    OP_FALSE,
    OP_THIS,
    OP_CALL,
    OP_TYPEOF,
    OP_ADD,
    OP_DIV,
    OP_MUL,
    OP_SUB,
    OP_THROW,
    OP_TRY,
    OP_CATCH,
    OP_RETURN
} js_opcode;

struct js_function{
    char* name;
    size_t args_size;
    size_t params_size;

    js_opcode* code;
    size_t code_size;
};

#endif