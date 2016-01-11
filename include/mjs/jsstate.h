#ifndef MJS_JSSTATE_H
#define MJS_JSSTATE_H

#include <stddef.h>
#include "common.h"

typedef struct{
    // State Modifiers
    bool strict;

    // Scoping
    js_environment* scope;

    // Execution Stack
    int top;
    int bottom;
    js_value* stack;

    // Prototypes Cache
    js_object* object_proto;
    js_object* array_proto;
    js_object* function_proto;
    js_object* boolean_proto;
    js_object* number_proto;
    js_object* string_proto;
} js_state;

typedef void (*js_cfunction)(js_state* vm);

js_state* mjs_new(bool strict);

struct js_environment{
    js_environment* outer;
    struct js_environemnt_entry* root;
};

#endif