#include <stdio.h>
#include "map.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define KEY_INT -1
//#define KEY_DOUBLE -1.0
//#define KEY_STRING

#define DATA_INT -1
//#define DATA_DOUBLE -1.0
//#define DATA_STRING


#define MAX_INPUT_LENGTH


void printInstructions(char* word)
{
    printf("Enter %s type: [0=int,  1=double,  2=char,  3=string\n", word); // to update more values
}

void printInput(char* word)
{
    printf("Enter input for %s\n", word);
}


bool getMapInput (Map my_map)
{
    printInput("key");
    #ifdef KEY_INT
    int key_in;
    scanf("%d", &key_in);
    #endif

    #ifdef KEY_DOUBLE
    double key_in;
    scanf("%lf", &key_in);
    #endif

    if (!key_in)
    {
        return false;
    }


    printInput("data");
    #ifdef DATA_INT
    int data_in;
    scanf("%d", &data_in);
    #endif

    #ifdef DATA_DOUBLE
    double data_in;
    scanf("%lf", &key_in);
    #endif


    // MAP PUT
    MapResult res = mapPut(my_map, &key_in, &data_in);
    
    if (res == MAP_SUCCESS)
    {
        return true;
    }

    printf("MAP FAILED - %d\n", res);
    return false;
}









int intCompare (void *x1, void *x2)
{
    int x1_new = *(int*)x1;
    int x2_new = *(int*)(x2);
    return x1_new - x2_new;
}

void intFree (void* x)
{
    free(x);
}

void* intCopy (void *x)
{
    int *y = malloc(sizeof(int));
    if (y == NULL)
    {
        exit(1);
    }
    *y = *(int*)x;
    return y;
}



int doubleCompare (void *x1, void *x2)
{
    double x1_new = *(double*)x1;
    double x2_new = *(double*)(x2);

    if (x1_new == x2_new)
    {
        return 0;
    }

    return x1_new > x2_new ? 1 : -1;
}

void doubleFree (void* x)
{
    free(x);
}

void *doubleCopy (void *x)
{
    double *y = malloc(sizeof(double));
    if (y == NULL)
    {
        exit(1);
    }
    *y = *(double*)x;
    return y;
}



int main ()
{
    #ifdef KEY_INT
    copyMapKeyElements    copy_key    = intCopy;
    freeMapKeyElements    free_key    = intFree;
    compareMapKeyElements compare_key = intCompare;

    #endif

    #ifdef KEY_DOUBLE
    copyMapKeyElements    copy_key  = doubleCopy;
    freeMapKeyElements    free_key  = doubleFree;
    compareMapKeyElements compare_key = doubleCompare;

    double key_in = KEY_DOUBLE;
    #endif

    #ifdef KEY_STRING
    copyMapKeyElements    copy_key    = intCopy; //tbd
    freeMapKeyElements    free_key    = intFree; //tbd
    compareMapKeyElements compare_key = intCompare; //tbd
    #endif




    #ifdef DATA_INT
    copyMapKeyElements    copy_data    = intCopy;
    freeMapKeyElements    free_data    = intFree;

    #endif

    #ifdef DATA_DOUBLE
    copyMapKeyElements    copy_data  = doubleCopy;
    freeMapKeyElements    free_data  = doubleFree;

    double data_in = DATA_DOUBLE
    #endif

    #ifdef DATA_STRING
    copyMapKeyElements    copy_data    = intCopy; //tbd
    freeMapKeyElements    free_data    = intFree; //tbd
    #endif



    Map test_map = mapCreate(copy_data, copy_key, free_data, free_key, compare_key);
    bool put_success = true;

    do
    {
        put_success = getMapInput(test_map);

    } while (put_success);
    

    printIntMapContent(test_map);
    // free

}


/*int main ()
{
    int key_type  = -1;
    int data_type = -1;

    copyMapDataElements copy_data = NULL;
    copyMapKeyElements  copy_key  = NULL;

    freeMapDataElements free_data = NULL;
    freeMapKeyElements  free_key  = NULL;

    compareMapKeyElements compare_key = NULL;

    printf("\n\n\n\n=================MTM HW1 Q3.1 Tester=================\n");
    printInstructions("key");
    scanf("%d", &key_type);

    switch (key_type)
    {
    case 0:
        compare_key = intCompare;
        copy_key    = intCopy;
        free_key    = intFree;
        break;
    case 1:
        compare_key = doubleCompare;
        copy_key    = doubleCopy;
        free_key    = doubleFree;
        break;

    default:
        printf("Invalid input\n");
        exit(1);
        break;
    }

    printInstructions("data");
    scanf("%d", &data_type);

    switch (data_type)
    {
    case 0:
        copy_data    = intCopy;
        free_data    = intFree;
        break;
    case 1:
        copy_data    = doubleCopy;
        free_data    = doubleFree;
        break;

    default:
        printf("Invalid input\n");
        exit(1);
        break;
    }

    Map test_map = mapCreate(copy_data, copy_key, free_data, free_key, compare_key);


    // get input until getting the string "quit"
    
    char input[MAX_INPUT_LENGTH] = "temp";
    do
    {
        void* in_key  = malloc(MAX_INPUT_LENGTH);
        void* in_data = malloc(MAX_INPUT_LENGTH);
        printInput("key");
        scanf("%s", input);

        switch (key_type)
        {
        case 0:
            break;
        
        default:
            break;
        }

        printInput("data");
        scanf("%s", input);

    } while (strcmp(input, "quit"));



    free(test_map);
    return 0;
}*/