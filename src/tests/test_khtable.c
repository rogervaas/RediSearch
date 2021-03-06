#include "test_util.h"
#include "../util/khtable.h"
#include "../util/fnv.h"
#include <string.h>

typedef struct {
  KHTableEntry base;
  char *key;
  uint32_t hash;
  uint32_t value;
} MyEntry;

static int myEntryCompare(const KHTableEntry *e, const void *k, size_t n, uint32_t h) {
  const MyEntry *ent = (const MyEntry *)e;
  return !(ent->hash == h && strcmp(k, ent->key) == 0);
}

static uint32_t myHash(const KHTableEntry *e) {
  return ((const MyEntry *)e)->hash;
}

static KHTableEntry *myAlloc() {
  return calloc(1, sizeof(MyEntry));
}

static uint32_t calcHash(const char *s) {
  return fnv_32a_buf((char *)s, strlen(s), 0);
}

static KHTableProcs myProcs = {.Alloc = myAlloc, .Hash = myHash, .Compare = myEntryCompare};

int testKhTable() {
  KHTable kht;
  KHTable_Init(&kht, &myProcs, NULL, 4);

  MyEntry *ent = NULL;
  ent = (void *)KHTable_GetEntry(&kht, "key", 0, calcHash("key"), NULL);
  ASSERT(ent == NULL);  // Not found, and no isNew pointer

  int isNew = 0;
  ent = (void *)KHTable_GetEntry(&kht, "key", 0, calcHash("key"), &isNew);
  ASSERT(ent != NULL);
  ASSERT(isNew != 0);
  ent->key = "key";
  ent->hash = calcHash("key");
  ent->value = 42;

  isNew = 0;
  MyEntry *ent2 = (void *)KHTable_GetEntry(&kht, "key", 0, calcHash("key"), NULL);
  ASSERT(ent2 == ent);

  // Try it again, but with isNew
  ent2 = (void *)KHTable_GetEntry(&kht, "key", 0, calcHash("key"), &isNew);
  ASSERT(ent2 == ent);
  ASSERT(isNew == 0);
  return 0;
}

TEST_MAIN({ TESTFUNC(testKhTable); })