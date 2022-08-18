/*
 * Copyright 2022 Yonggang Luo
 * SPDX-License-Identifier: MIT
 */

#ifndef KIT_DISABLE_SYSTEM_THREADS
#  if defined(_WIN32) && !defined(__CYGWIN__)

#    include "threads.win32.h"

struct tls_callback {
  tls_callback() { }
  ~tls_callback() {
    __threads_win32_tls_callback();
  }
};

static thread_local tls_callback tls_callback_instance;

#  endif
#endif
