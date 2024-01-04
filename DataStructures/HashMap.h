#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdlib.h>
#include "LinkedList.h"

#ifndef KEY_VALUE_PAIR
#define KEY_VALUE_PAIR
typedef struct KeyValuePair {
    void* key;
    void* value;
} KeyValuePair;
#endif //KEY_VALUE_PAIR

#ifndef HASH_FUNC_TYPEDEF
#define HASH_FUNC_TYPEDEF
typedef size_t (*HashFunc)(const void* key, const size_t capacity);
#endif //HASH_FUNC_TYPEDEF

#ifndef CMP_FUNC_TYPEDEF
#define CMP_FUNC_TYPEDEF
typedef int (*CmpFunc)(const void* key1, const void* key2);
#endif //CMP_FUNC_TYPEDEF

typedef struct HashMap {
    size_t size;
    size_t capacity; 
    LinkedList* array;
    LinkedList keyValuePairs;
    HashFunc hashFunc;
    CmpFunc cmpFunc;
} HashMap;

/*Initializes HashMap with a given initial capacity. HashFunc and CmpFunc can be set to None for default functions*/
void initHashMap(HashMap* map, size_t initialCapacity, HashFunc hashFunc, CmpFunc cmpFunc);
void insertHashMap(HashMap* map, void* key, void* value);

//returns 0 if key exists, but returns -1 if it does not.
int updateHashMap(HashMap* map, void* key, void* value); 
int containsHashMap(HashMap* map, void* key);

//returns NULL if key does not exist.
void* getHashMap(HashMap* map, void* key); 

void removeElementHashMap(HashMap* map, void* key);

void destroyHashMap(HashMap* map);

#endif //HASH_MAP_H