#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include <assert.h>

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
static Map_Node mapNodeCreate(MapKeyElement in_key, MapDataElement in_data,
                              copyMapDataElements copy_data_function, copyMapKeyElements copy_key_function)
{
    Map_Node out_node = malloc(sizeof(*out_node));
    
    if (out_node == NULL)
    {
        return NULL;
    }

    MapKeyElement  new_key  = copy_key_function(in_key);
    MapDataElement new_data = copy_data_function(in_data);
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


// Destroys an entire map list (Node and all next nodes in the list)
static void mapNodeListDestroy(Map_Node current_node, freeMapDataElements free_data_func, 
                           freeMapKeyElements free_key_func)
{
    if (current_node == NULL)
    {
        return;
    }
    Map_Node next_node = current_node->next;
    mapNodeDestroy(current_node, free_data_func, free_key_func);
    
    if (next_node != NULL)
    {
        mapNodeListDestroy(next_node, free_data_func, free_key_func);
    }
}


// Copies a list of Map_Nodes
static Map_Node mapNodeListCopy (Map_Node first_node, copyMapDataElements copy_data_function,
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
        iterator->next = NULL;

        first_node = first_node->next;
        if (first_node == NULL)
        {
            break;
        }

        // Allocate next node
        Map_Node new_node = malloc(sizeof(*new_node));
        if (new_node == NULL)
        {
            mapNodeListDestroy(new_node, free_data_func, free_key_func);
            return NULL;
        }

        iterator->next = new_node;
        iterator = new_node;

    } while (first_node != NULL);
    
    return duplicated_node;
}


// Finds a placement for a new node with a specified key
// If a node with that key exists, it returns it.
// Otherwise, returns the node that should be placed BEFORE the new node after new nnde insertion
// If the node should be placed before the first node / first node is NULL, returns NULL
static Map_Node mapNodeFindPlacement(Map_Node node, MapKeyElement key, compareMapKeyElements compare_func)
{
    if (node == NULL || compare_func(key, node->key) < 0)
    {
        return NULL;
    }

    // Scan node list to find correct node
    while (node->next != NULL)
    {
        // Comparing, if the next node's key is the same as the wanted key - return the next node
        int comparison =  compare_func(key, node->next->key);
        if (comparison == 0)
        {
            return node->next;
        }

        // If the next node's key is LOWER than the key, return current node
        if (compare_func(key, node->next->key) < 0)
        {
            return node;
        }

        node = node->next;
    }

    // Should be inserted after the last node
    return node;
}


// ASSUMING NODE IS IN FIRST_NODE'S LIST
static Map_Node mapNodeFindPrev(Map_Node first_node, Map_Node node, compareMapKeyElements compare_func)
{
    if (first_node == NULL || node == NULL || first_node->key == node->key)
    {
        return NULL;
    }

    while (compare_func(first_node->next->key, node->key) != 0)
    {
        first_node = first_node->next;
    }

    return first_node;
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




void mapDestroy(Map map)
{
    if (map == NULL)
    {
        return;
    }
    mapNodeListDestroy(map->first_node, map->free_data_func, map->free_key_func); /////////
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

    Map_Node new_node = mapNodeListCopy(map->first_node, map->copy_data_func, map->copy_key_func,
                                    map->free_data_func, map->free_key_func);
    if (new_node == NULL)
    {
        free(new_map);
        return NULL;
    }

    // Reset source map iterator, update new map size
    map->iterator     = NULL;
    new_map->iterator = NULL;
    new_map->size = map->size;
    new_map->first_node = new_node;
    return new_map;

}


int mapGetSize(Map map)
{
    if (map == NULL)
    {
        return -1;
    }

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
        if (map->compare_key_func(element, node->key) == 0)   // <<<<<<<<<<<<<<<<<<< ADD 
        {
            return true;
        }
        
        node = node->next;
    }
    map->iterator = NULL;
    return false;
}



MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    // Verify the input items aren't NULL
    if (map == NULL || keyElement == NULL || dataElement == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    // Get the node with specified key / the one that should come before it
    Map_Node prev_node = mapNodeFindPlacement(map->first_node, keyElement, map->compare_key_func);

    // If the key exists, update the data
    if (prev_node != NULL && (map->compare_key_func(prev_node->key, keyElement) == 0))
    {
        map->free_data_func(prev_node->data);
        prev_node->data = map->copy_data_func(dataElement);
        return MAP_SUCCESS;
    }

    // Otherwise, create a new node that should be placed after prev_node
    Map_Node new_node = mapNodeCreate(keyElement, dataElement, map->copy_data_func, map->copy_key_func);

    // If new_node should be first
    if (prev_node == NULL)
    {
        new_node->next = map->first_node;
        map->first_node = new_node;
        (map->size)++;
        return MAP_SUCCESS;
    }

    // If it shouldn't be first
    Map_Node next_node = prev_node->next;
    prev_node->next = new_node;
    new_node ->next = next_node;
    (map->size)++;
    return MAP_SUCCESS;
    
}


MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
    // Verify input
    if (map == NULL || keyElement == NULL)
    {
        return NULL;
    }

    map->iterator = NULL; // Now or later?

    // Return data if key exists
    if (mapContains(map, keyElement))
    {
        Map_Node node = mapNodeFindPlacement(map->first_node, keyElement, map->compare_key_func);
        assert(map->compare_key_func(node->key, keyElement) == 0);
        return node->data;
    }

    // Key doesn't exist
    return NULL;
}


MapResult mapRemove(Map map, MapKeyElement keyElement)
{
    // Verify input
    if (map == NULL || keyElement == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    map->iterator = NULL; // Now or later?

    // Item not found
    if (!mapContains(map, keyElement))
    {
        return MAP_ITEM_DOES_NOT_EXIST;
    }

    // Find node to remove and it's previous node
    Map_Node node_to_remove = mapNodeFindPlacement(map->first_node, keyElement, map->compare_key_func);
    Map_Node prev_node      = mapNodeFindPrev(map->first_node, node_to_remove, map->compare_key_func);

    // First to be removed
    if (prev_node == NULL)
    {
        map->first_node = map->first_node->next;
        mapNodeDestroy(node_to_remove, map->free_data_func, map->free_key_func);
        (map->size)--;
        return MAP_SUCCESS;
    }

    // Case - node to remove is not the first node
    prev_node->next = node_to_remove->next;
    mapNodeDestroy(node_to_remove, map->free_data_func, map->free_key_func);
    (map->size)--;
    return MAP_SUCCESS;
}


MapKeyElement mapGetFirst(Map map)
{
    // Verify input
    if (map == NULL || map->first_node == NULL)
    {
        return NULL;
    }

    map->iterator = map->first_node;
    return map->copy_key_func(map->iterator->key);
}


MapKeyElement mapGetNext(Map map)
{
    // Verify input
    if (map == NULL || map->iterator == NULL || map->iterator->next == NULL)
    {
        return NULL;
    }

    map->iterator = map->iterator->next;
    return map->copy_key_func(map->iterator->key);
}


MapResult mapClear(Map map)
{
    if (map == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    map->size = 0;
    map->iterator = NULL;
    mapNodeListDestroy(map->first_node, map->free_data_func, map->free_key_func);
    map->first_node = NULL;
    return MAP_SUCCESS;
}

