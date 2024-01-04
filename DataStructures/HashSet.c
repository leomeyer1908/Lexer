#include "HashSet.h"

static size_t defaultHashFunc(const void* key, const size_t capacity) {
    return (size_t) key % capacity;
}

static int defaultCmpFunc(const void* key1, const void* key2) {
    return key1 == key2;
}

void initHashSet(HashSet* set, size_t initialCapacity, HashFunc hashFunc, CmpFunc cmpFunc) {
    set->size = 0;
    set->capacity = initialCapacity;
    set->array = (LinkedList*) malloc(initialCapacity*sizeof(LinkedList));
    for (size_t i = 0; i < initialCapacity; i++) {
        initList(&(set->array[i]));
    }
    initList(&set->keys);

    set->hashFunc = hashFunc != NULL ? hashFunc : &defaultHashFunc;
    set->cmpFunc = cmpFunc != NULL ? cmpFunc : &defaultCmpFunc;
}

void insertHashSet(HashSet* set, void* key) {
    if (containsHashSet(set, key)) {
        return;
    }
    double load_factor = (double) set->size/set->capacity;
    if (load_factor > 0.7) {
        size_t newCapacity = set->capacity*2;
        LinkedList* newArray = (LinkedList*) malloc(newCapacity*sizeof(LinkedList));
        for (size_t i = 0; i < newCapacity; i++) {
            initList(&newArray[i]);
        }

        for (DoublyNode* currentNode = set->keys.head; currentNode != NULL; currentNode = currentNode->next) {
            size_t index = set->hashFunc((const void*) currentNode->value, (const size_t) newCapacity);
            pushBackList(&newArray[index], (void*) currentNode);
        }

        for (size_t i = 0; i < set->capacity; i++) {
            destroyList(&set->array[i]);
        }
        free(set->array);

        set->array = newArray;
        set->capacity = newCapacity;
    }

    pushBackList(&set->keys, key);
    DoublyNode* keyNode = set->keys.tail;
    size_t index = set->hashFunc((const void*) key, (const size_t) set->capacity);
    pushBackList(&(set->array[index]), (void*) keyNode);
    set->size++;
}

int containsHashSet(HashSet* set, void* key) {
    size_t index = set->hashFunc((const void*) key, (const size_t) set->capacity);
    for (DoublyNode* currentNode = set->array[index].head; currentNode != NULL; currentNode = currentNode->next) {
        if (set->cmpFunc((const void*) ((DoublyNode*) currentNode->value)->value, (const void*) key)) {
            return 1;
        }
    }
    return 0;
}

void removeElementHashSet(HashSet* set, void* key) {
    if (!containsHashSet(set, key)) {
        return;
    }
    size_t index = set->hashFunc((const void*) key, (const size_t) set->capacity);
    for (DoublyNode* currentNode = set->array[index].head; currentNode != NULL; currentNode = currentNode->next) {
        if (set->cmpFunc((const void*) ((DoublyNode*) currentNode->value)->value, (const void*) key)) {
            removeNodeFromList(&set->keys, (DoublyNode*) currentNode->value);
            removeNodeFromList(&set->array[index], currentNode);
            set->size--;
            break;
        }
    }
}

void destroyHashSet(HashSet* set) {
    for (size_t i = 0; i < set->capacity; i++) {
        destroyList(&set->array[i]);
    }
    free(set->array);
    destroyList(&set->keys);
    set->capacity = 0;
    set->size = 0;
    set->hashFunc = &defaultHashFunc;
    set->cmpFunc = &defaultCmpFunc;
}

void copyHashSet(HashSet* srcSet, HashSet* dstSet) {
	dstSet->size = srcSet->size;
	dstSet->capacity = srcSet->capacity;
	dstSet->array = (LinkedList*) malloc(srcSet->capacity*sizeof(LinkedList));
	for (size_t i = 0; i < srcSet->capacity; i++) {
		copyList(&srcSet->array[i], &dstSet->array[i]);
	}
	copyList(&srcSet->keys, &dstSet->keys);
    dstSet->hashFunc = srcSet->hashFunc;
    dstSet->cmpFunc = srcSet->cmpFunc;
}

void getSetFromList(LinkedList* list, HashSet* set) {
	for (DoublyNode* node = list->head; node != NULL; node = node->next) {
		insertHashSet(set, node->value);
	}
}