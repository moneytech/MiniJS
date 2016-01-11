#include <stdlib.h>
#include <mjs.h>

struct js_environment_entry{
    struct js_environment_entry* next;
    struct js_environment_entry* prev;
    char* key;
    js_value* value;
};

js_state*
mjs_new(bool strict){
    js_state* state = malloc(sizeof(js_state));
    state->scope = malloc(sizeof(js_environment));
    state->scope->outer = NULL;
    state->scope->root = NULL;
    state->strict = strict;
    state->object_proto = js_object_new(state, JS_COBJECT, NULL);
    state->array_proto = js_object_new(state, JS_CARRAY, state->object_proto);
    state->boolean_proto = js_object_new(state, JS_CBOOLEAN, state->object_proto);
    state->number_proto = js_object_new(state, JS_CNUMBER, state->object_proto);
    state->function_proto = js_object_new(state, JS_CFUNCTION, state->object_proto);
    state->string_proto = js_object_new(state, JS_CSTRING, state->object_proto);
    state->stack = malloc(sizeof(js_value) * JS_STACKSIZE);
    state->top = 0;
    state->bottom = 0;
    return state;
}