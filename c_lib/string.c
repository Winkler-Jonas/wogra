


#include <string.h>
#include "def.h"




str concat_string(str first_str, str second_str){
    if(!first_str || !second_str){
        return NULL;
    }
    register size_t fst_str_len = strlen(first_str);
    register size_t scd_str_len = strlen(second_str);
    str retValue = "";
    retValue = malloc(fst_str_len + scd_str_len + 1);
    retValue[0] = '\0';
    strcat(retValue, first_str);
    strcat(retValue, second_str);
    free(first_str);
    free(second_str);
    return retValue;
}


str slice_string(const char *str_from, str str_to, size_t start, size_t end){
    /*
 * Function: slice_str
 * -------------------
 * Slice string and create new instance
 *
 * str_from: The string to slice and copy from
 * str_to: The string / buffer used to store the result
 * start: Starting index of the slice
 * end: Ending position of the slice (exclusive)
 *
 */
    size_t j = 0;
    for (size_t i = start; i < end; ++i) {
        str_to[j++] = str_from[i];
    }
    str_to[j] = 0;
}