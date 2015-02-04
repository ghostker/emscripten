#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <emscripten.h>

#define DB "THE_DB"

int expected;
int result;

void ok(void* arg)
{
  assert(expected == (int)arg);
  REPORT_RESULT();
}

void onerror(void* arg)
{
  assert(expected == (int)arg);
  result = 999;
  REPORT_RESULT();
}

void onload(void* arg, void* ptr, int num)
{
  assert(expected == (int)arg);
  printf("loaded %s\n", ptr);
  assert(num == strlen(SECRET)+1);
  assert(strcmp(ptr, SECRET) == 0);
  result = 1;
  REPORT_RESULT();
}

void onbadload(void* arg, void* ptr, int num)
{
  printf("load failed, surprising\n");
  result = 999;
  REPORT_RESULT();
}

void test() {
  result = STAGE;
#if STAGE == 0
  expected = 12;
  emscripten_idb_async_store(DB, "the_secret", SECRET, strlen(SECRET)+1, (void*)expected, ok, onerror);
  printf("storing %s\n", SECRET);
#elif STAGE == 1
  expected = 31;
  emscripten_idb_async_load(DB, "the_secret", (void*)expected, onload, onerror);
#elif STAGE == 2
  expected = 44;
  emscripten_idb_async_delete(DB, "the_secret", (void*)expected, ok, onerror);
  printf("deleting the_secret\n");
#elif STAGE == 3
  expected = 55;
  emscripten_idb_async_load(DB, "the_secret", (void*)expected, onbadload, ok);
  printf("loading, should fail as we deleted\n");
#else
  assert(0);
#endif
}

int main() {
  test();
  emscripten_exit_with_live_runtime();
  return 0;
}

