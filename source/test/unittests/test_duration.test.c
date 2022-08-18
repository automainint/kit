#define KIT_TEST_FILE test_duration
#include "../../kit_test/test.h"

#if defined(_WIN32) && !defined(__CYGWIN__)
__declspec(dllimport) void __stdcall Sleep(unsigned long timeout);
static void kit_sleep(int ms) {
  Sleep(ms);
}
#else
#  include <unistd.h>
static void kit_sleep(int ms) {
  usleep(ms * 1000);
}
#endif

TEST("test duration") {
  kit_sleep(100);
}
