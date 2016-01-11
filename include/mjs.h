#ifndef MJS_MJS_H
#define MJS_MJS_H

#include "mjs/common.h"
#include "mjs/jsstate.h"
#include "mjs/jsvalue.h"

HEADER_BEGIN

#define JS_STACKSIZE 256

enum{
    JS_READONLY = 1,
    JS_DONTENUM = 2,
    JS_DONTCONF = 4
};

void js_pop(js_state* J, int idx);
void js_copy(js_state* J, int idx);

void js_toprimitive(js_state* J, js_value* val, int pref);
char* js_typeof(js_state* J, int idx);
char* js_tostring(js_state* J, int idx);
js_object* js_toobject(js_state* J, int idx);

void js_pushundefined(js_state* J);
void js_pushnull(js_state* J);
void js_pushboolean(js_state* J, bool v);
void js_pushnumber(js_state* J, double v);
void js_pushobject(js_state* J, js_object* obj);
void js_pushvalue(js_state* J, js_value val);
void js_pushstring(js_state* J, char* str);

void js_newobject(js_state* J);
void js_newarray(js_state* J);
void js_newboolean(js_state* J, bool v);
void js_newnumber(js_state* J, double v);
void js_newstring(js_state* J, char* v);
void js_newcfunction(js_state* J, js_cfunction func, char* name, size_t length);

void js_getproperty(js_state* J, int idx, char* name);
void js_defproperty(js_state* J, int idx, char* name, int attrs);
void js_setproperty(js_state* J, int idx, char* name);
void js_delproperty(js_state* J, int idx, char* name);

bool js_iscallable(js_state* J, int idx);

void js_call(js_state* J, int idx);

void js_throw(js_state* J);

HEADER_END

#endif