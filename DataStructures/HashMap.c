#include "HashMap.h"

static size_t defaultHashFunc(const void* key, const size_t capacity) {
    return (size_t) key % capacity;
}

static int defaultCmpFunc(const void* key1, const void* key2) {
    return key1 == key2;
}

void initHashMap(HashMap* map, size_t initialCapacity, HashFunc hashFunc, CmpFunc cmpFunc) {
    map->size = 0;
    map->capacity = initialCapacity;
    map->array = (LinkedList*) malloc(initialCapacity*sizeof(LinkedList));
    for (size_t i = 0; i < initialCapacity; i++) {
        initList(&(map->array[i]));
    }
    initList(&map->keyValuePairs);

    map->hashFunc = hashFunc != NULL ? hashFunc : &defaultHashFunc;
    map->cmpFunc = cmpFunc != NULL ? cmpFunc : &defaultCmpFunc;
}

void insertHashMap(HashMap* map, void* key, void* value) {
    if (map->array == NULL) {
        return;
    }
    if (containsHashMap(map, key)) {
        updateHashMap(map, key, value);
        return;
    }
    double load_factor = (double) map->size/map->capacity;
    if (load_factor > 0.7) {
        size_t newCapacity = map->capacity*2;
        LinkedList* newArray = (LinkedList*) malloc(newCapacity*sizeof(LinkedList));
        for (size_t i = 0; i < newCapacity; i++) {
            initList(&newArray[i]);
        }

        for (DoublyNode* currentNode = map->keyValuePairs.head; currentNode != NULL; currentNode = currentNode->next) {
            KeyValuePair* currentKeyValuePair = (KeyValuePair*) currentNode->value;
            size_t index = map->hashFunc((const void*) currentKeyValuePair->key, (const size_t) newCapacity);
            pushBackList(&newArray[index], (void*) currentNode);
        }

        for (size_t i = 0; i < map->capacity; i++) {
            destroyList(&map->array[i]);
        }
        free(map->array);

        map->array = newArray;
        map->capacity = newCapacity;
    }

    KeyValuePair * keyValuePair = (KeyValuePair*) malloc(sizeof(KeyValuePair));
    keyValuePair->key = key;
    keyValuePair->value = value;
    pushBackList(&map->keyValuePairs, (void*) keyValuePair);
    DoublyNode* keyNode = map->keyValuePairs.tail;

    size_t index = map->hashFunc((const void*) key, (const size_t) map->capacity);
    pushBackList(&(map->array[index]), (void*) keyNode);

    map->size++;
}

int updateHashMap(HashMap* map, void* key, void* value) {
    if (map->array == NULL) {
        return -1;
    }
    size_t index = map->hashFunc((const void*) key, (const size_t) map->capacity);
    for (DoublyNode* currentNode = map->array[index].head; currentNode != NULL; currentNode = currentNode->next) {
        const void* currentKey = (const void*) ((KeyValuePair*) ((DoublyNode*) currentNode->value)->value)->key;
        if (map->cmpFunc(currentKey, (const void*) key)) {
            ((KeyValuePair*) ((DoublyNode*) currentNode->value)->value)->value = value;
            return 0;
        }
    }
    return -1;
}

int containsHashMap(HashMap* map, void* key) {
    if (map->array == NULL) {
        return 0;
    }
    size_t index = map->hashFunc((const void*) key, (const size_t) map->capacity);
    for (DoublyNode* currentNode = map->array[index].head; currentNode != NULL; currentNode = currentNode->next) {
        const void* currentKey = (const void*) ((KeyValuePair*) ((DoublyNode*) currentNode->value)->value)->key;
        if (map->cmpFunc(currentKey, (const void*) key)) {
            return 1;
        }
    }
    return 0;
}

void* getHashMap(HashMap* map, void* key) {
    if (map->array == NULL) {
        return NULL;
    }
    size_t index = map->hashFunc((const void*) key, (const size_t) map->capacity);
    for (DoublyNode* currentNode = map->array[index].head; currentNode != NULL; currentNode = currentNode->next) {
        const void* currentKey = (const void*) ((KeyValuePair*) ((DoublyNode*) currentNode->value)->value)->key;
        if (map->cmpFunc(currentKey, (const void*) key)) {
            return ((KeyValuePair*) ((DoublyNode*) currentNode->value)->value)->value; 
        }
    }
    return NULL;
}

void removeElementHashMap(HashMap* map, void* key) {
    if (!containsHashMap(map, key)) {
        return;
    }
    size_t index = map->hashFunc((const void*) key, (const size_t) map->capacity);
    for (DoublyNode* currentNode = map->array[index].head; currentNode != NULL; currentNode = currentNode->next) {
        const void* currentKey = (const void*) ((KeyValuePair*) ((DoublyNode*) currentNode->value)->value)->key;
        if (map->cmpFunc(currentKey, (const void*) key)) {
            free((KeyValuePair*) ((DoublyNode*) currentNode->value)->value);
            removeNodeFromList(&map->keyValuePairs, (DoublyNode*) currentNode->value);
            removeNodeFromList(&map->array[index], currentNode);
            map->size--;
            break;
        }
    }
}

void destroyHashMap(HashMap* map) {
    for (size_t i = 0; i < map->capacity; i++) {
        destroyList(&map->array[i]);
    }
    free(map->array);
    for (DoublyNode* currentNode = map->keyValuePairs.head; currentNode != NULL; currentNode = currentNode->next) {
        free((KeyValuePair*) currentNode->value);
    }
    map->array = NULL;
    destroyList(&map->keyValuePairs);
    map->capacity = 0;
    map->size = 0;
    map->hashFunc = &defaultHashFunc;
    map->cmpFunc = &defaultCmpFunc;
}


