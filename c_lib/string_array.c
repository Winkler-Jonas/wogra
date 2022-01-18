#include <malloc.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include "def.h"

#define ERROR_CODE -1 //Error-code for this module

void *__stringArray_init__(size_t length);

str __stringArray_str__(StringArray *stringArray);

void __stringArray_del__(StringArray *stringArray);

void *__stringArray_add_wrapper(StringArray *stringArray, str string);

void *__stringArray_add__(StringArray *stringArray, str string);

void *__stringArray_split__(StringArray *stringArray, size_t chunk_size, List *param_list);

/*int main(){
    size_t length_of_array = 5;

    StringArray *new_stringArray = stringArray_new(length_of_array);
    stringArray_add(new_stringArray, "Ein neuer Satz im StringArray");
    stringArray_add(new_stringArray, "Ein weiterer STring im StringArray");
    fprintf(stdout, "Jetzt wurde ein Satz zum StringArray hinzugefügt\n");
    stringArray_print(new_stringArray, stdout);




    stringArray_del(new_stringArray);
}*/


void *__stringArray_split__(StringArray *stringArray, size_t chunk_size, List *param_list) {
    /*
     * Function: __stringArray_split__
     * -------------------------------
     * Create list with chunks of provided StringArray
     * List must be freed!
     *
     * stringArray:         The stringArray to be split (Caller must free!)
     * type stringArray:    *StringArray
     * chunk_size:          The length of the individual chunks
     * type chunk_size:     size_t
     *
     * throws:              Several errors in case of unsuccessful execution
     *
     * returns:             NULL if successful else Error
     * rtype:               Optional[NULL|Error]
     */
    void *retValue = NULL;
    if (!stringArray || chunk_size <= 0) {
        return error_new("Provided arguments illegal!", ERROR_CODE);
    }
    size_t amount_chunks = stringArray->length / chunk_size;
    size_t left_over = stringArray->length % chunk_size;
    if (left_over)
        amount_chunks++;
    size_t last_element = amount_chunks - 1;
    for (size_t i = 0; i < amount_chunks; i++) {
        size_t tmp = (left_over != 0 && i == last_element) ? left_over : chunk_size;
        StringArray *tmp_strArray = stringArray_new(tmp);
        for (size_t j = 0; j < tmp; j++) {
            if (NULL != (retValue = stringArray_add(tmp_strArray, stringArray->array[i * chunk_size + j]))) {
                return retValue;
            }
        }
        if(NULL != (retValue = list_add_str_array(param_list, tmp_strArray))){
            return retValue;
        }
    }
    return NULL;
}

void *stringArray_split(StringArray *stringArray, size_t chunk_size, List *empty_list) {
    return __stringArray_split__(stringArray, chunk_size, empty_list);
}

void *__stringArray_init__(size_t length) {
    /*
     * Function: ___StringArray__init__
     * --------------------------------
     * Allocate and initiate StringArray struct
     * with necessary data.
     *
     * length:          The max amount of strings the Array is supposed to store
     * type length:     size_t
     *
     * returns:         Pointer to new stringArray struct
     */
    StringArray *new_string_array = malloc(sizeof(StringArray));
    if (new_string_array == NULL) {
        return NULL;
    }
    new_string_array->array = malloc(length * sizeof(char *));
    if (new_string_array->array == NULL) {
        return NULL;
    }
    new_string_array->length = 0;
    new_string_array->max_length = length;
    new_string_array->toString = &__stringArray_str__;
    return new_string_array;
}

str __stringArray_str__(StringArray *stringArray) {
    /*
     * Function: toString
     * ------------------
     * Representing String of StringArray
     * Call stringArray_print for pretty printing!
     *
     * returns:     Pretty String describing StringArray
     * rtype:       char* (Must be freed)
     */
    if(!stringArray){
        return NULL;
    }
    str retValue = "";

    register size_t characters = 0;
    for (size_t i = 0; i < stringArray->length; i++) {
        //printf("%s", stringArray->array[i]);
        characters += strlen(stringArray->array[i]);
    }
    // If the array is empty 3 characters are needed "[]\0" else for each entry a Comma and a leading and ending bracket
    characters += stringArray->length > 0 ? stringArray->length * 2 + 1 : 3;
    retValue = malloc(characters * sizeof(char));
    retValue[0] = '\0';
    strcat(retValue, "[");
    for(size_t i = 0; i < stringArray->length; i++){
        if(i != 0){
            strcat(retValue, ", ");
        }
        strcat(retValue, stringArray->array[i]);
    }
    strcat(retValue, "]");
    return retValue;
}

void __stringArray_del__(StringArray *stringArray) {
    /*
     * Function: __stringArray_del__
     * ------------------------------
     * Free up storage allocated by provided StringArray
     *
     * stringArray: Pointer to the StringArray to be freed
     */
    if (stringArray) {
        for (size_t i = 0; i < stringArray->length; i++) {
            if (stringArray->array[i]) {
//                free(stringArray->array[i]);
            }
        }
        if (stringArray->array) {
//            free(stringArray->array);
        }
        free(stringArray);
    }


}

void *__stringArray_add__(StringArray *stringArray, str string) {
    /*
     * Function: __stringArray_add__
     * -----------------
     * Internal function to add a string to preexisting stringArray
     *
     * stringArray:         The stringArray the string to be added to
     * type stringArray:    &StringArray
     * string:              The string to be added
     * type string:         &char*
     *
     * throws: Error, containing err_msg and err_code (must be freed)
     *
     * returns: NULL if successful
     */
    // Error Handling
    if (!stringArray || !string) {
        return error_new("Provided string or array not accessible!", ERROR_CODE);
    }
    if (stringArray->length >= stringArray->max_length) {
        return error_new("Exceeded max amount of strings for StringArray", ERROR_CODE);
    }
    // String appending

    //fprintf(stdout, "StringArray: ");
    //stringArray_print(stringArray, stdout);
    //fprintf(stdout, " is trying to append -> ");
    //str tmp = strdup(string);
    //fprintf(stdout, "%s\n", tmp);
    stringArray->array[stringArray->length] = strdup(string);
    //fprintf(stdout, "Element '%s' now stored in StringArray at position %lu\n", stringArray->array[stringArray->length], stringArray->length);
    stringArray->length += 1;
    //stringArray_print(stringArray, stdout);
    //fprintf(stdout, "\n");
    return NULL;
}

void *__stringArray_add_wrapper(StringArray *stringArray, str string) {
    /*
     * Function: __stringArray_add_wrapper
     * ------------------------------
     * Adds new string to provided StringArray
     * Function can be set to thread safe with mutex if required - see def.h
     *
     * returns: NULL if successful
     *
     * throws: Error, in case something goes wrong (must be freed)
     */
    void *error;
    if (LOCKADD == 1) {
        pthread_mutex_lock(&mutex);
        return __stringArray_add__(stringArray, string);
        pthread_mutex_unlock(&mutex);
    } else if (LOCKADD == 0) {
        return __stringArray_add__(stringArray, string);
    }
}

StringArray *stringArray_new(size_t length) {
    /*
     * Function: stringArray_new
     * -------------------------
     * Create a new StringArray
     *
     * length:      The maximal length of the StringArray
     * type length: size_t
     *
     * returns:     StringArray struct or NULL if not successful.
     */
    return __stringArray_init__(length);
}

void *stringArray_add(StringArray *stringArray, str string) {
    /*
     * Function: stringArray_add
     * -------------------------
     * Adds new string to StringArray
     *
     * stringArray:         Reference to StringArray struct to be edited
     * type stringArray:    &StringArray
     * string:              The string to be added to StringArray
     * type string:         char *
     *
     * returns:             Null if successful, else Error (Error must be freed!)
     */
    return __stringArray_add_wrapper(stringArray, string);
}

void stringArray_print(StringArray *stringArray, FILE *file) {
    /*
     * Function: stringArray_print
     * ---------------------------
     * Print content of StringArray to StdOut
     */
    if (stringArray) {
        str stringArray_str = stringArray->toString(stringArray);
        fprintf(file, "%s", stringArray_str);
        free(stringArray_str);
    }
}

void stringArray_del(StringArray *stringArray) {
    /*
     * Function: stringArray_del
     * -------------------------
     * Free up storage allocated by stringArray
     *
     * stringArray:         Reference to the StringArray to be freed
     * type stringArray:    &StringArray
     */
    __stringArray_del__(stringArray);
}

bool empty_array(StringArray *stringArray){
    if(stringArray == NULL){
        return true;
    }else{
        false;
    }
}