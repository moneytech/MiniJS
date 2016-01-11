#include <stdio.h>
#include <mjs.h>

static void
add(js_state* J){
    js_object* a = js_toobject(J, -1);
    js_object* b = js_toobject(J, -2);
    js_pushnumber(J, a->as.number + b->as.number);
}

int main(int argc, char** argv){
    js_state* J = mjs_new(true);
    js_newcfunction(J, &add, "add", 1);
    js_pushnumber(J, 10);
    js_pushnumber(J, 10);
    js_call(J, -2);
    printf("%s\n", js_tostring(J, -1));
    return 0;
}