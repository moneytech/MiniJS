cmake_minimum_required(VERSION 3.2)
project(MJS)

include_directories(include)

add_executable(MJS main.c include/mjs.h include/mjs/common.h include/mjs/jsstate.h include/mjs/jsvalue.h src/jsstate.c src/jsproperty.c src/jsvalue.c src/jsruntime.c include/mjs/jsopcode.h include/mjs/jsfunction.h)