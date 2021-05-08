#include <stdio.h>
#include <stdlib.h>
#include "map.h"


// helper struct - List of keys & values
typedef struct map_node_t {

    MapKeyElement key;
    MapDataElement data;
    struct map_node_t* next_map_node;

} *Map_Node;


struct Map_t {
    
    Map_Node first_node;
    Map_Node iterator;
    int map_size;
    copyMapDataElements   copy_data_func;
    copyMapKeyElements    copy_key_func;
    freeMapDataElements   free_data_func;
    freeMapKeyElements    free_key_func;
    compareMapKeyElements compare_key_func;
};



Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements)

{
    // allocate map
    Map new_map = malloc(sizeof(*new_map));
    if (new_map == NULL)
    {
        ////////////////////////////////////////////////////// MapResult?
        return NULL;
    }
    

    new_map -> map_size = 0;
    new_map -> first_node = NULL;
    new_map -> iterator   = NULL;

    new_map -> compare_key_func = compareKeyElements;
    new_map -> copy_data_func   = copyDataElement;
    new_map -> copy_key_func    = copyKeyElement;
    new_map -> free_data_func   = freeDataElement;
    new_map -> free_key_func    = freeKeyElement;
    
    return new_map;
}