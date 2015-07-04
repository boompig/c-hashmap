
/**
 * Doubly-linked list, keeps track of key and value
 * Only meant to be used internally.
 */
typedef struct ValueEntry {
    char * key;
    char * value;
    struct ValueEntry * next;
    struct ValueEntry * prev;
} ValueEntry;

/**
 * Has a fixed-size array, and keeps track of number of entries
 */
typedef struct hashmap {
    size_t max_size;
    size_t size;
    ValueEntry ** table;
} Hashmap;

Hashmap * hashmap_create (const size_t size);
void hashmap_put (Hashmap * map, const char* key, const char* value);
char* hashmap_get (Hashmap * map, const char* key);
void hashmap_pop (Hashmap * map, const char* key);
void hashmap_destroy (Hashmap * map);
void hashmap_print (Hashmap * map);
