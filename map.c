#include <stdio.h>
#include <stdlib.h>
#include "map.h"


// helper struct - List of keys & values
typedef struct map_node_t {

    MapKeyElement key;
    MapDataElement data;
    struct map_node_t* next;

} *Map_Node;




struct Map_t {
    
    Map_Node first_node;
    Map_Node iterator;
    int size;
    copyMapDataElements   copy_data_func;
    copyMapKeyElements    copy_key_func;
    freeMapDataElements   free_data_func;
    freeMapKeyElements    free_key_func;
    compareMapKeyElements compare_key_func;
};



// Creates a map node
static Map_Node mapNodeCreate(Map map, MapKeyElement in_key, MapDataElement in_data)
{
    Map_Node out_node = malloc(sizeof(*out_node));
    
    if (out_node == NULL)
    {
        return NULL;
    }

    MapKeyElement  new_key  = map->copy_key_func(in_key);
    MapDataElement new_data = map->copy_data_func(in_data);
    out_node->key  = new_key;
    out_node->data = new_data;
    out_node->next = NULL;

    return out_node;
}

// Destroys a map node
static void mapNodeDestroy(Map_Node current_node, freeMapDataElements free_data_func, 
                           freeMapKeyElements free_key_func)
{
    free_key_func(current_node->key);
    free_data_func(current_node->data);
    free(current_node);
}


// Destroys an entire map list
static void mapListDestroy(Map_Node current_node, freeMapDataElements free_data_func, 
                           freeMapKeyElements free_key_func)
{
    if (current_node == NULL)
    {
        return;
    }

    Map_Node next_node = current_node->next;
    mapNodeDestroy(current_node, free_data_func, free_key_func);
    mapListDestroy(next_node, free_data_func, free_key_func);

}


// Copies a list of Map_Nodes
static Map_Node mapListCopy (Map_Node first_node, copyMapDataElements copy_data_function,
                             copyMapKeyElements copy_key_function, freeMapDataElements free_data_func, 
                             freeMapKeyElements free_key_func)
{
    // Got empty list
    if (first_node == NULL)
    {
        return NULL;
    }

    // Allocate first node
    Map_Node duplicated_node = malloc(sizeof(*duplicated_node));
    if (duplicated_node == NULL)
    {
        return NULL;
    }

    // Copy keys & data, allocate new nodes
    Map_Node iterator = duplicated_node;
    do
    {
        // Copy key & data
        iterator->data = copy_data_function(first_node->data);
        iterator->key  = copy_key_function(first_node->key);

        first_node = first_node->next;
        if (first_node == NULL)
        {
            break;
        }

        // Allocate next node
        Map_Node new_node = malloc(sizeof(*new_node));
        if (new_node == NULL)
        {
            mapListDestroy(new_node, free_data_func, free_key_func);
            return NULL;
        }

        iterator->next = new_node;
        iterator = new_node;

    } while (first_node != NULL);
    
    return duplicated_node;
}


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
        return NULL;
    }
    

    new_map -> size = 0;
    new_map -> first_node = NULL;
    new_map -> iterator   = NULL;

    new_map -> compare_key_func = compareKeyElements;
    new_map -> copy_data_func   = copyDataElement;
    new_map -> copy_key_func    = copyKeyElement;
    new_map -> free_data_func   = freeDataElement;
    new_map -> free_key_func    = freeKeyElement;
    
    return new_map;
}





// Handles edge cases for the mapPut function (list is empty new node should be placed first, etc...)
static bool mapPutEdgeCase (Map map, MapKeyElement keyElement, MapDataElement dataElement, MapResult *status)
{
    compareMapKeyElements compareKeys = map->compare_key_func;

    // Allocates new node
    Map_Node new_node  = mapNodeCreate(map, keyElement, dataElement);
    if (new_node == NULL)
    {
        *status = MAP_OUT_OF_MEMORY;
        return true;
    }

    // The map is empty
    if (map->first_node == NULL)
    {
        map->first_node = new_node;
        *status = MAP_SUCCESS;
        map->size++;
        return true;
    }

    // New node should be placed before first node
    if (compareKeys(map->first_node->key, keyElement) > 0)
    {
        new_node->next = map->first_node;
        map->first_node = new_node;
        *status = MAP_SUCCESS;
        map->size++;
        return true;
    }

    // New node shares the same key as the first node - de-allocate the new node and update the first node
    if (compareKeys(map->first_node->key, keyElement) == 0)
    {
        mapNodeDestroy(new_node, map->free_data_func, map->free_key_func);
        map->first_node->data = map->copy_data_func(dataElement);
        *status = MAP_ITEM_ALREADY_EXISTS;
        return true;
    }

    return false;
}

// One iteration in the while loop - when scanning for the right place for new_node.
// Return true if placed new_node.
static bool mapPutIteration (Map map, Map_Node iterator, Map_Node new_node, MapResult *status)
{

    // Reached the last node and still no match, put the new node at the end
    if (iterator->next == NULL)
    {
        iterator->next = new_node;
        *status = MAP_SUCCESS;
        map->size++;
        return true;
    }

    // Key already exists, update data
    if (map->compare_key_func(iterator->next->key, new_node->key) == 0)
    {
        // Keep the updated data and destroy the new node
        MapDataElement new_data = map->copy_data_func(new_node->data);
        mapNodeDestroy(new_node, map->free_data_func, map->free_key_func);
        new_node = NULL;

        // free current data and update it to the new one
        map->free_data_func(iterator->next->data);
        iterator->next->data = new_data;

        *status = MAP_ITEM_ALREADY_EXISTS;
        return true;
    }

    // New node should go between current node and next node
    if (map->compare_key_func(iterator->next->key, new_node->key) > 0)
    {
        new_node->next = iterator->next;
        iterator->next = new_node;
        *status = MAP_SUCCESS;
        map->size++;
        return true;
    }

    return false;
}


void mapDestroy(Map map)
{
    mapListDestroy(map->first_node, map->free_data_func, map->free_key_func); /////////
    free(map);
}


Map mapCopy(Map map)
{
    if (map == NULL)
    {
        return NULL;
    }

    // Create new map
    Map new_map = mapCreate(map->copy_data_func, map->copy_key_func,
                            map->free_data_func, map->free_key_func,
                            map->compare_key_func);


    if (new_map == NULL)
    {
        return NULL;
    }

    // If source map is empty (no Map_Nodes)
    if (map->first_node == NULL)
    {
        new_map->first_node = NULL;
        new_map->size       = 0;
        return new_map;
    }

    Map_Node new_node = mapListCopy(map->first_node, map->copy_data_func, map->copy_key_func,
                                    map->free_data_func, map->free_key_func);
    if (new_node == NULL)
    {
        free(new_map);
        return NULL;
    }

    // Reset source map iterator, update new map size
    map->iterator = NULL;
    new_map->size = map->size;
    
    return new_map;

}


int mapGetSize(Map map)
{
    // Size will be updated when adding/removing items from the map
    return map->size;
}


bool mapContains(Map map, MapKeyElement element)
{
    // Invalid input, return false
    if (map == NULL || element == NULL)
    {
        return false;
    }

    // Initialize iterator node and scan for the key
    Map_Node node = map->first_node;
    while (node != NULL)
    {
        if (map->compare_key_func(element, node->key) == 0)
        {
            return true;
        }

        node = node->next;
    }

    return false;
}





MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{

    // if map == NULL ?
    // reset internal iterator??????????
    MapResult return_value = MAP_SUCCESS;
    bool is_edge_case = mapPutEdgeCase(map, keyElement, dataElement, &return_value);

    // If there was a handled edge case, return the result
    if (is_edge_case)
    {
        return return_value;
    }

    Map_Node new_node  = mapNodeCreate(map, keyElement, dataElement);

    // Scan for the right spot for the new node
    Map_Node iterator = map->first_node;
    
    // Scanning the list to find the right place to put the new_node
    bool is_new_node_placed = false;
    while(iterator != NULL && is_new_node_placed == false) // change conditions?
    {
        is_new_node_placed = mapPutIteration(map, iterator, new_node, &return_value);
        // move iterator forward
        iterator = iterator->next;
    }
    
    return return_value;

}











// helper function
void printIntMapContent (Map my_map)
{
    Map_Node iter = my_map->first_node;
    int i = 0;
    while (iter)
    {
        i++;
        printf("item #%d\n",i);
        printf("key:  %d\n", *(int*)(iter->key));
        printf("dat:  %d\n\n", *(int*)(iter->data));
        iter = iter->next;
    }
}