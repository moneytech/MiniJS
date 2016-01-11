#include <mjs.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mjs/jsvalue.h>

#define STACK (J->stack)
#define TOP (J->top)
#define BOTTOM (J->bottom)
#define CHECK_STACK(n) if(TOP + n >= JS_STACKSIZE) js_stackoverflow(J)

static js_value undefined = { {0}, JS_TUNDEFINED };

static js_value*
stackidx(js_state* J, int idx){
    idx = idx < 0 ? TOP + idx : BOTTOM + idx;
    if(idx < 0 || idx >= TOP){
        return &undefined;
    }
    return STACK + idx;
}

static void
js_stackoverflow(js_state* J){
    STACK[TOP].type = JS_TSTRING;
    STACK[TOP].as.literal = "stack overflow";
    TOP++;
    // TODO: Throw error js_throw(J);
}

void
js_pushundefined(js_state* J){
    CHECK_STACK(1);
    STACK[TOP].type = JS_TUNDEFINED;
    TOP++;
}

void
js_pop(js_state* J, int idx){
    TOP -= idx;
    if(TOP < BOTTOM){
        TOP = BOTTOM;
        // TODO: Throw error
    }
}

void
js_copy(js_state* J, int idx){
    CHECK_STACK(1);
    STACK[TOP] = *stackidx(J, idx);
    TOP++;
}

void
js_pushnull(js_state* J){
    CHECK_STACK(1);
    STACK[TOP].type = JS_TNULL;
    TOP++;
}

void
js_pushboolean(js_state* J, bool v){
    CHECK_STACK(1);
    STACK[TOP].type = JS_TBOOLEAN;
    STACK[TOP].as.boolean = v;
    TOP++;
}

void
js_pushnumber(js_state* J, double v){
    CHECK_STACK(1);
    STACK[TOP].type = JS_TNUMBER;
    STACK[TOP].as.number = v;
    TOP++;
}

void
js_pushobject(js_state* J, js_object* obj){
    CHECK_STACK(1);
    STACK[TOP].type = JS_TOBJECT;
    STACK[TOP].as.object = obj;
    TOP++;
}

void
js_pushvalue(js_state* J, js_value v){
    CHECK_STACK(1);
    STACK[TOP] = v;
    TOP++;
}

void
js_pushstring(js_state* J, char* str){
    CHECK_STACK(1);
    STACK[TOP].type = JS_TSTRING;
    STACK[TOP].as.string = malloc(sizeof(js_string));
    STACK[TOP].as.string->value = strdup(str);
    STACK[TOP].as.string->length = strlen(str);
    TOP++;
}

bool
js_iscallable(js_state* J, int idx){
    js_value* val = stackidx(J, idx);
    if(val->type == JS_TOBJECT){
        return (bool) (val->as.object->type == JS_CFUNCTION ||
                       val->as.object->type == JS_CCFUNCTION ||
                       val->as.object->type == JS_CSCRIPT);
    }
    return false;
}

static void
js_prop_def(js_state* J, js_object* obj, char* name, int attrs, js_value* value, js_object* getter, js_object* setter){
    js_property* ref;

    if(obj->type == JS_CARRAY){
        if(!strcmp(name, "length")){
            goto readonly;
        }
    }

    if(obj->type == JS_CSTRING){
        if(!strcmp(name, "length")){
            goto readonly;
        }
    }

    ref = js_property_set(J, obj, name);
    if(ref){
        if(value){
            if(!(ref->attrs & JS_READONLY)){
                ref->value = *value;
            } else if(J->strict){
                // TODO: Throw error
            }
        }

        if(getter){
            if(!(ref->attrs & JS_DONTCONF)){
                ref->getter = getter;
            } else if(J->strict){
                // TODO: Throw error
            }
        }

        if(setter){
            if(!(ref->attrs & JS_DONTCONF)){
                ref->setter = setter;
            } else if(J->strict){
                // TODO: Throw error
            }
        }

        ref->attrs |= attrs;
    }

    return;

    readonly:
        if(J->strict){
            // TODO: Throw error
        }
}

js_object*
js_toobject(js_state* J, int idx){
    js_value* v = stackidx(J, idx);
    switch(v->type){
        default:
        case JS_TUNDEFINED:
        case JS_TNULL:{
            // TODO: Throw error
            fprintf(stderr, "undefined\n");
            abort();
        }
        case JS_TBOOLEAN:{
            return js_boolean_new(J, v->as.boolean);
        };
        case JS_TNUMBER:{
            return js_number_new(J, v->as.number);
        };
        case JS_TOBJECT:{
            return v->as.object;
        };
        case JS_TSTRING:{
            return js_string_new(J, v->as.string->value);
        };
    }
}

static char*
js_to_string(js_state* J, js_value* v){
    switch(v->type){
        default:
        case JS_TUNDEFINED: return "undefined";
        case JS_TNULL: return "null";
        case JS_TBOOLEAN: return v->as.boolean ? "true" : "false";
        case JS_TSTRING: return v->as.string->value;
        case JS_TNUMBER:{
            char* str = malloc(sizeof(char) * 32);
            snprintf(str, 32, "%lf", v->as.number);
            return str;
        };
        case JS_TLITERAL: return v->as.literal;
        case JS_TOBJECT:{
            js_toprimitive(J, v, JS_HSTRING);
            return js_to_string(J, v);
        };
    }
}

char*
js_tostring(js_state* J, int idx){
    return js_to_string(J, stackidx(J, idx));
}

char*
js_typeof(js_state* J, int idx){
    js_value* val = stackidx(J, idx);
    switch(val->type){
        case JS_TSTRING: return "string";
        case JS_TNULL: return "object";
        case JS_TUNDEFINED: return "undefined";
        case JS_TBOOLEAN: return "boolean";
        case JS_TNUMBER: return "number";
        case JS_TOBJECT:{
            if(val->as.object->type == JS_CFUNCTION || val->as.object->type == JS_CCFUNCTION){
                return "function";
            }

            return "object";
        }
        default:{
            return "<unknown>";
        }
    }
}

void
js_defproperty(js_state* J, int idx, char* name, int attrs){
    js_prop_def(J, js_toobject(J, idx), name, attrs, stackidx(J, -1), NULL, NULL);
    js_pop(J, 1);
}

static bool
has_property(js_state* J, js_object* obj, char* name){
    if(obj->type == JS_CARRAY){
        if(!strcmp(name, "length")){
            js_pushnumber(J, obj->as.array.length);
            return true;
        }
    }

    if(obj->type == JS_CSTRING){
        if(!strcmp(name, "length")){
            js_pushnumber(J, obj->as.string.length);
            return true;
        }
    }

    js_property* ref = js_property_get(J, obj, name);
    if(ref){
        if(ref->getter){
            js_pushobject(J, ref->getter);
            js_pushobject(J, obj);

            // TODO: Call getter
        } else{
            js_pushvalue(J, ref->value);
        }
    }

    return false;
}

void
js_getproperty(js_state* J, int idx, char* name){
    js_object* obj = js_toobject(J, idx);
    if(!has_property(J, obj, name)){
        js_pushundefined(J);
    }
}

static void
js_callcfunction(js_state* J, int idx, size_t length, js_cfunction func){
    js_value val;
    for(int i = idx; i < length; i++){
        js_pushundefined(J);
    }
    func(J);
    val = *stackidx(J, -1);
    TOP = --BOTTOM;
    js_pushvalue(J, val);
}

void
js_call(js_state* J, int idx){
    if(!js_iscallable(J, -idx-2)){
        //TODO: Throw Exception
    }

    js_object* this = js_toobject(J, -idx-2);
    int save = BOTTOM;
    BOTTOM = TOP - idx - 1;

    if(this->type == JS_CCFUNCTION){
        js_callcfunction(J, idx, this->as.cfunction.length, this->as.cfunction.call);
    }

    BOTTOM = save;
}