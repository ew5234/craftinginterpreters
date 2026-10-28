//> Hash Tables table-c
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

//> max-load
#define TABLE_MAX_LOAD 0.75

//< max-load
void initTable(Table* table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

static uint32_t hashValue(Value value) {
  if (IS_NIL(value)) {
    return 2;
  }

  if (IS_BOOL(value)) {
    return AS_BOOL(value) ? 1 : 0;
  }

  if (IS_NUMBER(value)) {
    double number = AS_NUMBER(value);
    if (number == 0) number = 0;  // Normalize negative zero.

    uint64_t bits;
    memcpy(&bits, &number, sizeof(double));
    return (uint32_t)(bits ^ (bits >> 32));
  }

  if (IS_OBJ(value)) {
    Obj* object = AS_OBJ(value);

    if (object->type == OBJ_STRING) {
      return ((ObjString*)object)->hash;
    }
  }

  return 0;
}

//> free-table
void freeTable(Table* table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}
//< free-table
//> find-entry
//> omit
// NOTE: The "Optimization" chapter has a manual copy of this function.
// If you change it here, make sure to update that copy.
//< omit
static Entry* findEntry(Entry* entries, int capacity, Value key) {
  uint32_t index = hashValue(key) & (capacity - 1);
  Entry* tombstone = NULL;

  for (;;) {
    Entry* entry = &entries[index];

    if (!entry->isOccupied) {
      if (!entry->isTombstone) {
        return tombstone != NULL ? tombstone : entry;
      }

      if (tombstone == NULL) tombstone = entry;
    } else if (valuesEqual(entry->key, key)) {
      return entry;
    }

    index = (index + 1) & (capacity - 1);
  }
}
//< find-entry
//> table-get
bool tableGet(Table* table, Value key, Value* value) {
  if (table->count == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (!entry->isOccupied) return false;

  *value = entry->value;
  return true;
}
//< table-get
//> table-adjust-capacity
static void adjustCapacity(Table* table, int capacity) {
  Entry* entries = ALLOCATE(Entry, capacity);

  for (int i = 0; i < capacity; i++) {
    entries[i].key = NIL_VAL;
    entries[i].value = NIL_VAL;
    entries[i].isOccupied = false;
    entries[i].isTombstone = false;
  }

  int oldCapacity = table->capacity;
  Entry* oldEntries = table->entries;

  table->count = 0;
  table->entries = entries;
  table->capacity = capacity;

  for (int i = 0; i < oldCapacity; i++) {
    Entry* oldEntry = &oldEntries[i];

    if (!oldEntry->isOccupied) continue;

    Entry* dest = findEntry(entries, capacity, oldEntry->key);
    dest->key = oldEntry->key;
    dest->value = oldEntry->value;
    dest->isOccupied = true;
    dest->isTombstone = false;

    table->count++;
  }

  FREE_ARRAY(Entry, oldEntries, oldCapacity);
}
//< table-adjust-capacity
//> table-set
bool tableSet(Table* table, Value key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }

  Entry* entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = !entry->isOccupied;

  if (isNewKey && !entry->isTombstone) table->count++;

  entry->key = key;
  entry->value = value;
  entry->isOccupied = true;
  entry->isTombstone = false;

  return isNewKey;
}
//< table-set
//> table-delete
bool tableDelete(Table* table, Value key) {
  if (table->count == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (!entry->isOccupied) return false;

  entry->isOccupied = false;
  entry->isTombstone = true;
  entry->value = BOOL_VAL(true);

  return true;
}
//< table-delete
//> table-add-all
void tableAddAll(Table* from, Table* to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];

    if (entry->isOccupied) {
      tableSet(to, entry->key, entry->value);
    }
  }
}
//< table-add-all
//> table-find-string
ObjString* tableFindString(Table* table, const char* chars,
                           int length, uint32_t hash) {
  if (table->count == 0) return NULL;

  uint32_t index = hash & (table->capacity - 1);

  for (;;) {
    Entry* entry = &table->entries[index];

    if (!entry->isOccupied) {
      if (!entry->isTombstone) return NULL;
    } else if (IS_STRING(entry->key)) {
      ObjString* string = AS_STRING(entry->key);

      if (string->length == length &&
          string->hash == hash &&
          memcmp(string->chars, chars, length) == 0) {
        return string;
      }
    }

    index = (index + 1) & (table->capacity - 1);
  }
}
//< table-find-string
//> Garbage Collection table-remove-white
void tableRemoveWhite(Table* table) {
  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (entry->isOccupied &&
      IS_STRING(entry->key) &&
      !AS_STRING(entry->key)->obj.isMarked) {
    tableDelete(table, entry->key);
  }
  }
}
//< Garbage Collection table-remove-white
//> Garbage Collection mark-table
void markTable(Table* table) {
  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];

    if (entry->isOccupied) {
      markValue(entry->key);
      markValue(entry->value);
    }
  }
}
//< Garbage Collection mark-table
