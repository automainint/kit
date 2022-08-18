/*
 * Copyright 2022 Yonggang Luo
 * SPDX-License-Identifier: MIT
 */

#ifndef KIT_THREADS_WIN32_H
#define KIT_THREADS_WIN32_H

#ifndef KIT_DISABLE_SYSTEM_THREADS

#  ifdef __cplusplus
extern "C" {
#  endif

#  if defined(_WIN32) && !defined(__CYGWIN__)
void __threads_win32_tls_callback(void);
#  endif

#  ifdef __cplusplus
}
#  endif

#endif

#endif
