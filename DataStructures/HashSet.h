#ifndef HASH_SET_H
#define HASH_SET_H

#include <stdlib.h>
#include "LinkedList.h"

#ifndef HASH_FUNC_TYPEDEF
#define HASH_FUNC_TYPEDEF
typedef size_t (*HashFunc)(const void* key, const size_t capacity);
#endif //HASH_FUNC_TYPEDEF

#ifndef CMP_FUNC_TYPEDEF
#define CMP_FUNC_TYPEDEF
typedef int (*CmpFunc)(const void* key1, const void* key2);
#endif //CMP_FUNC_TYPEDEF

typedef struct HashSet {
    size_t size;
    size_t capacity; 
    LinkedList* array;
    LinkedList keys;
    HashFunc hashFunc;
    CmpFunc cmpFunc;
} HashSet;

/*Initializes HashSet with a given initial capacity. HashFunc and CmpFunc can be set to None for default functions*/
void initHashSet(HashSet* set, size_t initialCapacity, HashFunc hashFunc, CmpFunc cmpFunc);
void insertHashSet(HashSet* set, void* key);
int containsHashSet(HashSet* set, void* key);
void removeElementHashSet(HashSet* set, void* key);
void destroyHashSet(HashSet* set);

void copyHashSet(HashSet* srcSet, HashSet* dstSet);

void getSetFromList(LinkedList* list, HashSet* set);

#endif