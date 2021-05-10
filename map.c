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



// Creates a map node
static Map_Node createMapNode(Map map, MapKeyElement in_key, MapDataElement in_data)
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
    out_node->next_map_node = NULL;

    return out_node;
}

// Destroys a map node
static void destroyMapNode(Map map, Map_Node in_node)
{
    map->free_key_func(in_node->key);
    map->free_data_func(in_node->data);
    free(in_node);
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





// Handles edge cases for the mapPut function (list is empty new node should be placed first, etc...)
static bool mapPutEdgeCase (Map map, MapKeyElement keyElement, MapDataElement dataElement, MapResult *status)
{
    compareMapKeyElements compareKeys = map->compare_key_func;

    // Allocates new node
    Map_Node new_node  = createMapNode(map, keyElement, dataElement);
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
        map->map_size++;
        return true;
    }

    // New node should be placed before first node
    if (compareKeys(map->first_node->key, keyElement) > 0)
    {
        new_node->next_map_node = map->first_node;
        map->first_node = new_node;
        *status = MAP_SUCCESS;
        map->map_size++;
        return true;
    }

    // New node shares the same key as the first node - de-allocate the new node and update the first node
    if (compareKeys(map->first_node->key, keyElement) == 0)
    {
        destroyMapNode(map, new_node);
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
    if (iterator->next_map_node == NULL)
    {
        iterator->next_map_node = new_node;
        *status = MAP_SUCCESS;
        map->map_size++;
        return true;
    }

    // Key already exists, update data
    if (map->compare_key_func(iterator->next_map_node->key, new_node->key) == 0)
    {
        // Keep the updated data and destroy the new node
        MapDataElement new_data = map->copy_data_func(new_node->data);
        destroyMapNode(map, new_node);
        new_node = NULL;

        // free current data and update it to the new one
        map->free_data_func(iterator->next_map_node->data);
        iterator->next_map_node->data = new_data;

        *status = MAP_ITEM_ALREADY_EXISTS;
        return true;
    }

    // New node should go between current node and next node
    if (map->compare_key_func(iterator->next_map_node->key, new_node->key) > 0)
    {
        new_node->next_map_node = iterator->next_map_node;
        iterator->next_map_node = new_node;
        *status = MAP_SUCCESS;
        map->map_size++;
        return true;
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

    Map_Node new_node  = createMapNode(map, keyElement, dataElement);

    // Scan for the right spot for the new node
    Map_Node iterator = map->first_node;
    
    // Scanning the list to find the right place to put the new_node
    bool is_new_node_placed = false;
    while(iterator != NULL && is_new_node_placed == false) // change conditions?
    {
        is_new_node_placed = mapPutIteration(map, iterator, new_node, &return_value);
        // move iterator forward
        iterator = iterator->next_map_node;
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
        iter = iter->next_map_node;
    }
}