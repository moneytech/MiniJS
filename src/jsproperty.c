#include <mjs/jsvalue.h>
#include <stdlib.h>
#include <string.h>
#include <mjs/jsstate.h>
#include <stdio.h>

js_symbol
murmur(char* key){
    size_t len = strlen(key);
    uint32_t c1 = 0xCC9E2D51;
    uint32_t c2 = 0x1B873593;
    uint32_t r1 = 15;
    uint32_t r2 = 13;
    uint32_t m = 5;
    uint32_t n = 0xE6546B64;
    uint32_t h = 0xA;
    uint32_t k = 0;
    uint8_t* d = (uint8_t*) key;
    int l = (int) (len / 4);
    uint32_t* chunks = (uint32_t*) (d + l * 4);
    uint8_t* tail = d + l * 4;
    for(int i = -l; i != 0; i++){
        k = chunks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        h ^= k;
        h = (h << r2) | (h >> (32 - r2));
        h = h * m + n;
    }
    k = 0;
    switch(len & 3){
        case 3: k ^= (tail[2] << 16);
        case 2: k ^= (tail[1] << 8);
        case 1:{
            k ^= tail[0];
            k *= c1;
            k = (k << r1) | (k >> (32 - r1));
            k *= c2;
            h ^= k;
        }
        default:{
            break;
        }
    }

    h ^= len;
    h ^= (h >> 16);
    h *= 0x85EBCA6B;
    h ^= (h >> 13);
    h *= 0xC2B2AE35;
    h ^= (h >> 16);
    return h;
}

js_object*
js_object_new(js_state* J, js_class type, js_object* proto){
    js_object* obj = malloc(sizeof(js_object));
    obj->type = type;
    obj->final = false;
    obj->prototype = proto;
    obj->properties = NULL;
    return obj;
}

js_property*
js_property_getown(js_state* J, js_object* obj, char* name){
    js_symbol sym = murmur(name);
    js_property* prop = obj->properties;
    while(prop != NULL){
        if(prop->key == sym && (strcmp(prop->name, name) == 0)){
            return prop;
        }
        prop = prop->next;
    }
    return NULL;
}

js_property*
js_property_get(js_state* J, js_object* obj, char* name){
    js_object* object = obj;
    js_property* prop = NULL;
    while(object != NULL){
        if((prop = js_property_getown(J, obj, name)) != NULL){
            break;
        }
        object = object->prototype;
    }

    return prop;
}

static js_property*
newproperty(js_state* J, char* name){
    js_property* prop = malloc(sizeof(js_property));
    prop->key = murmur(name);
    prop->name = strdup(name);
    prop->attrs = 0;
    prop->getter = NULL;
    prop->setter = NULL;
    prop->value.type = JS_TUNDEFINED;
    prop->value.as.number = 0;
    prop->level = 1;
    return prop;
}

js_property*
js_property_set(js_state* J, js_object* obj, char* name){
    js_property* res = NULL;
    if(obj->properties != NULL){
        if(obj->final){
            res = js_property_get(J, obj, name);
            if(J->strict && !res){
                // Throw error
            }
            return res;
        }
    }
    res = newproperty(J, name);
    res->next = obj->properties;
    if(obj->properties){
        obj->properties->prev = res;
    }
    obj->properties = res;
    return res;
}

void
js_property_delete(js_state* J, js_object* obj, char* name){
    js_property* prop = js_property_get(J, obj, name);
    if(prop != NULL){
        if(prop->next){
            prop->next->prev = prop->prev;
        }
        if(prop->prev){
            prop->prev->next = prop->next;
        }
    }
}