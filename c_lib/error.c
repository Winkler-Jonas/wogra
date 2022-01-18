
#include <malloc.h>
#include <string.h>
#include "def.h"


void *__error_init__(str message);
void *__error_deconstruct__();
void *new_error();

Error *init_error(const char *c_fun, str err_msg, int error_code){
    Error *err = malloc(sizeof(Error));
    err->err_code = error_code;
    err->err_msg = malloc(1024 * sizeof(char));
    strcat(err->err_msg, "Error: ");
    strcat(err->err_msg, c_fun);
    strcat(err->err_msg, "\n");
    strcat(err->err_msg, err_msg);
    return err;
}

Error *__init_error_caller(const char *c_fun, str err_msg, int error_code){
    return init_error(c_fun, err_msg, error_code);
}

void error_del(Error *error){
    if(error){
        free(error->err_msg);
        free(error);
    }
}

