#include <stdio.h>
#include <time.h>
#include <stdint.h>

#include "table.h"
#include "value.h"
#include "memory.h"
#include "vm.h"

#define entries 100000


static double elapsedSeconds(clock_t start, clock_t end) {
  return (double)(end - start) / CLOCKS_PER_SEC;
}

//test for table growing, resizing, and lookups
static void benchmarkSequential(void) {
  Table table;
  initTable(&table);

  clock_t start = clock();

  //setting table which includes growing and resizing 
  for (int i = 0; i < entries; i++) {
    tableSet(&table, NUMBER_VAL(i), NUMBER_VAL(i * 2));
  }

  //looking up table
  for (int i = 0; i < entries; i++) {
    Value result;
    tableGet(&table, NUMBER_VAL(i), &result);
  }

  clock_t end = clock();

  printf("Sequential insert + successful lookup: %.6f seconds\n",
         elapsedSeconds(start, end));

  freeTable(&table);
}

//updating same key
static void benchmarkRepeatedKey(void) {
  Table table;
  initTable(&table);

  clock_t start = clock();

  //create entries of the same key
  for (int i = 0; i < entries ; i++) {
    tableSet(&table, NUMBER_VAL(1), NUMBER_VAL(i));
  }

  Value result;
  for (int i = 0; i < entries; i++) {
    tableGet(&table, NUMBER_VAL(1), &result);
  }

  clock_t end = clock();

  printf("Repeated updates + lookups: %.6f seconds\n",
         elapsedSeconds(start, end));

  freeTable(&table);
}

//delete and reinsert keys
static void benchmarkDeleteAndReinsert(void) {
  Table table;
  initTable(&table);

  for (int i = 0; i < entries; i++) {
    tableSet(&table, NUMBER_VAL(i), NUMBER_VAL(i));
  }

  clock_t start = clock();

  for (int i = 0; i < entries; i += 2) {
    tableDelete(&table, NUMBER_VAL(i));
  }

  for (int i = 0; i < entries; i += 2) {
    tableSet(&table, NUMBER_VAL(i), NUMBER_VAL(i * 3));
  }

  for (int i = 0; i < entries; i++) {
    Value result;
    tableGet(&table, NUMBER_VAL(i), &result);
  }

  clock_t end = clock();

  printf("Delete + reinsert + lookup: %.6f seconds\n",
         elapsedSeconds(start, end));

  freeTable(&table);
}

int main(void) {
  initVM();

  benchmarkSequential();
  benchmarkRepeatedKey();
  benchmarkDeleteAndReinsert();

  freeVM();
  return 0;
}