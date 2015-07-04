#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hashmap.h"

typedef enum {
    OP,
    PRE_KEY,
    KEY,
    PRE_VAL,
    VALUE
} State;

/**
 * Note that processCommand has memory leaks due to use of strdup
 */
void processCommand (Hashmap * map, const char op, const char* key, const char* value) {
    char* newKey;
    char* newVal;
    switch (op) {
        case 'P':
        case 'p':
            {
                newKey = strdup(key);
                newVal = strdup(value);
                printf("PUT [%s] = %s\n", newKey, newVal);
                hashmap_put(map, newKey, newVal);
            }
            break;
        case 'D':
        case 'd':
            printf("DELETE [%s]\n", key);
            hashmap_pop(map, key);
            break;
        case 'G':
        case 'g':
            printf("GET [%s]\n", key);
            printf("---> %s\n", hashmap_get(map, key));
            break;
        default:
            fprintf(stderr, "Invalid command: '%c'\n", op);
    }
    // print the map @ end of op
    hashmap_print(map);
}

void readTrace (const char* filename) {
    const size_t MAP_SIZE = 100;
    Hashmap * map = hashmap_create(MAP_SIZE);

    FILE * fp = fopen(filename, "r");
    char c;
    int keyPos = 0, valuePos = 0;
    State state = OP;
    char currentOp;
    const size_t MAX_KEY_SIZE = 200;
    const size_t MAX_VAL_SIZE = 1000;

    char * key = (char*) malloc (MAX_KEY_SIZE);
    char * value = (char*) malloc (MAX_VAL_SIZE);
    if (key == NULL || value == NULL)  {
        perror ("Failed to malloc for key and value");
        exit(1);
    }
    memset(key, '\0', MAX_KEY_SIZE);
    memset(value, '\0', MAX_KEY_SIZE);

    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            processCommand(map, currentOp, key, value);
            keyPos = 0;
            valuePos = 0;
            state = OP;
            memset(key, '\0', MAX_KEY_SIZE);
            memset(value, '\0', MAX_KEY_SIZE);
            continue;
        }

        // state machine
        switch (state) {
            case OP:
                if (c == '\n') {
                    // skip
                } else {
                    currentOp = c;
                    state = PRE_KEY;
                }
                break;
            case PRE_KEY:
                if (c == '"') {
                    state = KEY;
                }
                break;
            case KEY:
                if (c == '"') {
                    state = PRE_VAL;
                } else {
                    key[keyPos] = c;
                    keyPos++;
                }
                break;
            case PRE_VAL:
                if (c == '"') {
                    state = VALUE;
                }
                break;
            case VALUE:
                if (c == '"') {
                    // do nothing for closing quote
                } else {
                    value[valuePos] = c;
                    valuePos++;
                }
                break;
        }
    }

    // detect if key was given
    if (keyPos != 0) {
        processCommand(map, currentOp, key, value);
    }

    free (key);
    free (value);
    fclose(fp);

    hashmap_destroy(map);
}

int main () {
    readTrace ("trace.txt");
    return 0;
}
