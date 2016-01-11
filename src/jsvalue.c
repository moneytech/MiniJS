#include <mjs.h>
#include <string.h>
#include <mjs/jsvalue.h>
#include <stdio.h>

void
js_newobject(js_state* J){
    js_pushobject(J, js_object_new(J, JS_COBJECT, J->object_proto));
}

void
js_newarray(js_state* J){
    js_pushobject(J, js_object_new(J, JS_CARRAY, J->array_proto));
}

void
js_newboolean(js_state* J, bool v){
    js_object* obj = js_object_new(J, JS_CBOOLEAN, J->boolean_proto);
    obj->as.boolean = v;
    js_pushobject(J, obj);
}

void
js_newnumber(js_state* J, double v){
    js_object* obj = js_object_new(J, JS_CNUMBER, J->number_proto);
    obj->as.number = v;
    js_pushobject(J, obj);
}

void
js_newstring(js_state* J, char* v){
    js_object* obj = js_object_new(J, JS_CSTRING, J->string_proto);
    obj->as.string.length = strlen(v);
    obj->as.string.value = strdup(v);
    js_pushobject(J, obj);
}

void
js_newcfunction(js_state* J, js_cfunction cfun, char* name, size_t length){
    js_object* obj = js_object_new(J, JS_CCFUNCTION, J->function_proto);
    obj->as.cfunction.name = strdup(name);
    obj->as.cfunction.call = cfun;
    obj->as.cfunction.constructor = NULL;
    obj->as.cfunction.length = length;
    js_pushobject(J, obj);
    {
        js_pushnumber(J, length);
        js_defproperty(J, -2, "length", JS_READONLY|JS_DONTENUM|JS_DONTCONF);
        js_newobject(J);
        {
            js_copy(J, -2);
            js_defproperty(J, -2, "constructor", JS_DONTENUM);
        }
        js_defproperty(J, -2, "prototype", JS_DONTCONF);
    }
}

static js_object js_undef = {
        JS_COBJECT,
        true,
        NULL,
        NULL,
        { 0 }
};

js_object*
js_string_new(js_state* J, char* str){
    js_object* s = js_object_new(J, JS_CSTRING, J->string_proto);
    s->as.string.length = strlen(str);
    s->as.string.value = strdup(str);
    return s;
}

js_object*
js_undefined(){
    return &js_undef;
}

js_object*
js_boolean_new(js_state* J, bool v){
    js_object* b = js_object_new(J, JS_CBOOLEAN, J->boolean_proto);
    b->as.boolean = v;
    return b;
}

js_object*
js_number_new(js_state* J, double v){
    js_object* n = js_object_new(J, JS_CNUMBER, J->number_proto);
    n->as.number = v;
    return n;
}

static int
js_toString(js_state* J, js_object* obj){
    js_pushobject(J, obj);
    js_getproperty(J, -1, "toString");
    if(js_iscallable(J, -1)){
        // TODO: Call toString
    }
    js_pop(J, 2);
    return 0;
}

static int
js_valueOf(js_state* J, js_object* obj){
    // TODO: Call valueOf
    return 0;
}

void
js_toprimitive(js_state* J, js_value* val, int pref){
    js_object* obj;
    if(val->type != JS_TOBJECT){
        return;
    }

    obj = val->as.object;
    if(pref == JS_HNONE){
        pref = obj->type == JS_CDATE ? JS_HSTRING : JS_HNUMBER;
    }

    if(pref == JS_HSTRING){
        if(js_toString(J, obj) || js_valueOf(J, obj)){

        }
    }

    val->type = JS_TLITERAL;
    val->as.literal = "[object]";
}