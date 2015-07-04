#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hashmap.h"

/**
 * Internal function for hashing keys into valid array indices into the table
 * This is O (length of key)
 */
unsigned int hash_key (Hashmap * map, const char * key) {
    // I am OK with overflow for unsigned int here
    unsigned int sum = 0, i;
    for (i = 0; i < strlen(key); i++) {
        sum += key[i];
    }
    return sum % (map->max_size);
}

/**
 * This is O(1)
 */
bool hashmap_is_empty (Hashmap * map) {
    return map->size == 0;
}

/**
 * Create a new hashmap on the heap and return ptr to it
 */
Hashmap * hashmap_create (const size_t size) {
    Hashmap * map = (Hashmap *) malloc (sizeof (Hashmap));
    if (NULL == map) {
        perror("Could not malloc enough memory for map");
        exit(1);
    }
    map->max_size = size;
    map->size = 0;
    map->table = malloc (size * sizeof (ValueEntry *));
    if (NULL == map->table) {
        perror("Could not malloc enough memory for table");
        exit(1);
    }
    unsigned int slot;
    for (slot = 0; slot < size; slot++) {
        map->table[slot] = NULL;
    }
    return map;
}

/**
 * Internal function which creates ValueEntry with given fields
 */
ValueEntry * _create_value_entry (const char * key, const char* value) {
    ValueEntry * ve = malloc (sizeof (ValueEntry));
    if (ve == NULL) {
        perror("Could not malloc enough memory for entry");
        exit(1);
    }
    ve->key = (char *) key;
    ve->value = (char *) value;
    ve->next = NULL;
    ve->prev = NULL;
    return ve;
}

/**
 * Internal function.
 * Return value entry for a certain slot with the given key
 * Return NULL if not found
 */
ValueEntry * _find_value_entry (Hashmap * map, const char * key, const unsigned int slot) {
    ValueEntry * ve_parent = map->table[slot];
    while (ve_parent != NULL) {
        if (strcmp (ve_parent->key, key) == 0) {
            return ve_parent;
        } else {
            ve_parent = ve_parent->next;
        }
    }
    return NULL;
}

/**
 * Create an entry in the hashmap for the given key
 * Update if key already exists
 */
void hashmap_put (Hashmap * map, const char* key, const char* value) {
    unsigned int slot = hash_key (map, key);
    ValueEntry * ve = _find_value_entry (map, key, slot);
    if (ve == NULL) {
        // add value to head of LL
        ve = _create_value_entry(key, value);
        ve->next = map->table[slot];
        // update former head
        if (ve->next) {
            ve->next->prev = ve;
        }
        // add VE to table
        map->table[slot] = ve;
        // only increase size if new key
        map->size++;
    } else {
        // change value in correct LL node
        ve->value = (char *) value;
    }
}

/**
 * Return value for given key
 * Return NULL on failure
 */
char * hashmap_get (Hashmap * map, const char * key) {
    unsigned int slot = hash_key (map, key);
    ValueEntry * ve = _find_value_entry (map, key, slot);
    if (ve == NULL) {
        return NULL;
    } else {
        return ve->value;
    }
}

/**
 * Internal function.
 * free linked list node and update pointers
 */
void _LL_remove_node (ValueEntry * node) {
    if (node->next) {
        node->next->prev = node->prev;
    }
    if (node->prev) {
        node->prev->next = node->next;
    }
    free(node);
}

/**
 * Remove given key from hashmap. Deallocate associated data structures.
 */
void hashmap_pop (Hashmap * map, const char * key) {
    unsigned int slot = hash_key(map, key);
    ValueEntry * ve = _find_value_entry (map, key, slot);
    if (ve) {
        if (ve == map->table[slot]) {
            map->table[slot] = ve->next;
        }
        _LL_remove_node(ve);
        map->size--;
    }
}

/*
 * Destroy a hashmap. Frees all memory associated with it, but keys and values remain
 * As in - frees entry structs, but not strings
 */
void hashmap_destroy (Hashmap * map) {
    // free each slot
    unsigned int slot;
    for (slot = 0; slot < map->max_size; slot++) {
        // free all value entries in this slot
        ValueEntry * ve = map->table[slot];
        ValueEntry * tmp;
        while (ve != NULL) {
            tmp = ve->next;
            free(ve);
            ve = tmp;
        }
    }
    // free table
    free (map->table);
    // free hashmap itself
    free (map);
}

/*
 * Print given map in JSON notation to stdout
 */
void hashmap_print (Hashmap * map) {
    if (hashmap_is_empty(map)) {
        printf("{}\n");
    } else {
        printf("{\n");
        unsigned int slot;
        ValueEntry * ve;
        for (slot = 0; slot < map->max_size; slot++) {
            ve = map->table[slot];
            while (ve != NULL) {
                printf("    \"%s\" : \"%s\"\n", ve->key, ve->value);
                ve = ve->next;
            }
        }
        printf("}\n");
    }
}
