#include <malloc.h>
#include <string.h>
#include "def.h"

#define ERROR_CODE -2 // Error-Code for this module

void  *__list_init__();
str    __list_str__(List *list);
void   __list_del__(List *list);
void  *__list_add__(List *list, StringArray *stringArray);



//int main(){
//    List *list = list_new();
//
//    StringArray *stringArray = stringArray_new(5);
//    stringArray_add(stringArray, "1. String in StringArray");
//    stringArray_add(stringArray, "2. String in StringArray");
//
//    list_add_str_array(list, stringArray);
//
//    StringArray *stringArray_two = stringArray_new(5);
//    stringArray_add(stringArray_two, "1. String in 2. StringArray");
//    stringArray_add(stringArray_two, "2. String in 2. StringArray");
//
//    list_add_str_array(list, stringArray_two);
//
//    list_print(list, stdout);
//    list_del(list);
//    return 0;
//}

void *__list_init__(){
    List *list = malloc(sizeof(List));
    if(!list){
        return NULL;
    }
    list->list = NULL;
    list->length = 0;
    list->toString = &__list_str__;
    return list;
}

str __list_str__(List *list){
    /*
     * Function: __list_str__
     * ----------------------
     * Pretty print list (use for debugging)
     */
    if(!list){
        return NULL;
    }
    str retValue = "";
    register size_t characters = 0;

    // Init Array with 3 characters to at least fit "[]\0" or if list not empty allocate space for commas too
    retValue = malloc((list->length == 0 ? 3 : list->length * 2 + 1) * sizeof(char));
    retValue[0] = '\0';
    strcat(retValue, "[");

    for(size_t i = 0; i < list->length; i++){
        str new_string = NULL;
        if (i != 0) {
            strcat(retValue, ", ");
        }
        str current_str = (str) (&list->list[i])->toString(&list->list[i]);
        characters += strlen(current_str) + 2;
        if (NULL == (new_string = realloc(retValue, characters))){
            // reallocation failed
            return NULL;
        }
        strcat(new_string, current_str);
        free(current_str);
        retValue = new_string;
    }
    strcat(retValue, "]");
    return retValue;
}

void  __list_del__(List *list){
    for (size_t i = 0; i < list->length; i++){
        if (!&list->list[i]){
            stringArray_del(&list->list[i]);
        }
    }
    if(list->list){
        free(list->list);
    }
    free(list);
}

void  *__list_add__(List *list, StringArray *stringArray){
    /*
     * Function: __list_add__
     * ----------------------
     * Enlarge list of StringArray and add new stringArray to list
     *
     * list:                The list to be edited
     * type list:           &List
     * stringArray:         The StringArray to be added to the List
     * type stringArray:    &StringArray
     *
     * returns:             Null if successful, else Error
     * rtype:               Optional[NULL, Error]
     */
    pthread_mutex_lock(&mutex);
    StringArray *new_array_list = NULL;
    void *retValue = NULL;
    if(NULL == (new_array_list = realloc(list->list, (list->length+1) * sizeof(StringArray)))){
        retValue = error_new("Could not reallocate storage for list", ERROR_CODE);
    }else{
        new_array_list[list->length++] = *stringArray;
        list->list = new_array_list;
    }
    pthread_mutex_unlock(&mutex);
    return retValue;
}

List *list_new(){
    /*
     * Function: list_new
     * ------------------
     * Create a new empty List
     * Call list_del to deallocate List structure
     *
     * returns: new List reference or NULL pointer if not successful
     * rtype:   Optional[*List, Null]
     */
    return __list_init__();
}

void list_del(List *list){
    /*
     * Function: list_del
     * ------------------
     * Deallocate storage allocated by List structure
     *
     * list:        List structure to be deallocated
     * type list:   *List
     */
    __list_del__(list);
}

void *list_add_str_array(List *list, StringArray *stringArray){
    /*
     * Function: list_add_str_array
     * ----------------------------
     * Append new StringArray structure to List
     *
     * list:                List reference to be altered
     * type list:           *List
     * stringArray:         The StringArray to be added to the list
     * type stringArray:    *StringArray
     *
     * returns:             Null if successful, else Error
     * rtype:               Optional[NULL, Error]
     */
    return __list_add__(list, stringArray);
}

void list_print(List *list, FILE *file){
    /*
     * Function: list_print
     * --------------------
     * Print provided List with all it's parts (StringArrays)
     * Useful for debugging
     */
    str list_str = list->toString(list);
    if(list_str){
        fprintf(file, "%s", list_str);
    }else{
        fprintf(file, "%s", "Error occurred printing List");
    }
    free(list_str);
}