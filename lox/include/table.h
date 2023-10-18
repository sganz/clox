#ifndef clox_table_h
#define clox_table_h

#include "common.h"
#include "value.h"

typedef struct {
    ObjString* key; // sjg - NQQ has this as a Value type, may not work for clox, but will see
    Value value;
    // bool empty; // sjg - NQQ Only, this looks like a helper of sorts, see if needed due to NQQ
} Entry;

typedef struct {
    int count;
    int capacity;
    Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);

// sjg - anything with ObjString* type in NQQ has Value as the type, likely still looking
// at the same data type, but might be less flexible as no Objects in NQQ
// In looking at NQQ it may be that the hash can do more then just a 


bool tableGet(Table* table, ObjString* key, Value* value);
bool tableSet(Table* table, ObjString* key, Value value);
bool tableDelete(Table* table, ObjString* key);
void tableAddAll(Table* from, Table* to);
ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash);

void tableRemoveWhite(Table* table);
void markTable(Table* table);

#endif