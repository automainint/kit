#define KIT_TEST_FILE test_duration
#include "../../kit_test/test.h"

#ifdef _WIN32
#  include <windows.h>
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