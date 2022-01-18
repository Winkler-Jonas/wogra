//
// Created by edu on 12/14/21.
//


#include <pthread.h>
#include <bits/pthreadtypes.h>
#include <stdio.h>
#include <regex.h>
#include <Python.h>
#include <stdbool.h>

#ifndef UNTITLED_DEF_H
#define UNTITLED_DEF_H

#define MAXPATTERN 10       // The max amount of patterns (important because length is static!!) (Threadsafety)
#define LOCKADD 0           // Defines whether adding a string must be thread safe or not

PyThreadState *mainstate;

typedef char *str;

str slice_string();

typedef struct {
    int err_code;
    str err_msg;
}Error;

Error *__init_error_caller(const char *c_fun, str err_msg, int error_code);
void error_del(Error *error);
#define error_new(error_msg, error_code) __init_error_caller(__func__, error_msg, error_code)

// errorcodes:  StringArray:    -1
//              List:           -2
//              regex:          -3
//              string:         -4

typedef struct {
    size_t start;
    size_t end;
} Span;

typedef struct {
    Span *span;
    int len;
    str group;
} Match;

typedef struct {
    regex_t obj;
    str pattern;
    Match *match;
    Error *err;
} Regex;

Regex *regex_compile(const char *pattern);
void   regex_search(Regex *regex, str string);
void   regex_del(Regex *regex);


// todo mutex must be initialized before threads are started (pthread_mutex_init(&mutex, NULL))
// todo mutex must be destroyed before exiting (after joining the threads)the program (pthread_mutex_destroy(&mutex))
pthread_mutex_t mutex;

/*
 * Structure StringArray
 */
typedef struct {
    char **array;
    size_t max_length;
    size_t length;
    str (*toString)();
}StringArray;

/*
 * Structure List
 */
typedef struct {
    StringArray *list;
    size_t length;
    str (*toString)();
}List;

StringArray *stringArray_new(size_t length);
void *stringArray_add(StringArray *stringArray, str string);
void stringArray_print(StringArray *stringArray, FILE *file);
void stringArray_del(StringArray *stringArray);
void *stringArray_split(StringArray *stringArray, size_t chunk_size, List *empty_list);
bool stringArray_empty(StringArray *stringArray);

List *list_new();
void list_del(List *list);
void *list_add_str_array(List *list, StringArray *stringArray);
void list_print(List *list, FILE *file);

#endif //UNTITLED_DEF_H
