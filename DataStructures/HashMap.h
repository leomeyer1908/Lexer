#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdlib.h>
#include "LinkedList.h"

typedef struct KeyValuePair {
    int key;
    void* value;
} KeyValuePair;

typedef struct HashMap {
    size_t size;
    size_t capacity; 
    LinkedList* array;
} HashMap;

void initHashMap(HashMap* map, size_t initialCapacity);
void insertHashMap(HashMap* map, int key, void* value);

//returns 0 if key exists, but returns -1 if it does not.
int updateHashMap(HashMap* map, int key, void* value); 
int containsHashMap(HashMap* map, int key);

//returns NULL if key does not exist.
void* getHashMap(HashMap* map, int key); 

void destroyHashMap(HashMap* map);

#endif