#ifndef MJS_JSVALUE_H
#define MJS_JSVALUE_H

#include <stddef.h>
#include "common.h"
#include "jsstate.h"

typedef enum{
    JS_HNONE,
    JS_HSTRING,
    JS_HNUMBER
} js_hint;

typedef enum{
    JS_COBJECT,
    JS_CARRAY,
    JS_CFUNCTION,
    JS_CCFUNCTION,
    JS_CSCRIPT,
    JS_CERROR,
    JS_CBOOLEAN,
    JS_CNUMBER,
    JS_CSTRING,
    JS_CREGEXP,
    JS_CDATE,
    JS_CMATH,
    JS_CJSON,
    JS_CITERATOR,
    JS_CUSERDATA
} js_class;

typedef enum{
    JS_TUNDEFINED,
    JS_TSTRING,
    JS_TNULL,
    JS_TBOOLEAN,
    JS_TNUMBER,
    JS_TOBJECT,
    JS_TLITERAL
} js_type;

typedef struct{
    char* value;
    size_t length;
} js_string;

struct js_value{
    union{
        bool boolean;
        double number;
        js_string* string;
        js_object* object;
        char* literal;
    } as;
    js_type type;
};

typedef struct js_property{
    struct js_property* next;
    struct js_property* prev;
    js_symbol key;
    char* name;
    int level;
    int attrs;
    js_value value;
    js_object* getter;
    js_object* setter;
} js_property;

struct js_object{
    js_class type;
    bool final;
    js_property* properties;
    js_object* prototype;
    union{
        bool boolean;
        double number;
        js_string string;
        struct{
            size_t length;
        } array;
        struct{
            js_function* call;
            js_environment* scope;
        } function;
        struct{
            char* name;
            js_cfunction call;
            js_cfunction constructor;
            size_t length;
        } cfunction;
    } as;
};

js_object* js_object_new(js_state* J, js_class type, js_object* prototype);
js_object* js_string_new(js_state* J, char* str);
js_object* js_undefined();
js_object* js_boolean_new(js_state* J, bool v);
js_object* js_number_new(js_state* J, double v);
js_property* js_property_getown(js_state* J, js_object* obj, char* name);
js_property* js_property_get(js_state* J, js_object* obj, char* name);
js_property* js_property_set(js_state* J, js_object* obj, char* name);
void js_property_delete(js_state* J, js_object* obj, char* name);

#endif