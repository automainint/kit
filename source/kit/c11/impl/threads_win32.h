/*
 * Copyright 2022 Yonggang Luo
 * SPDX-License-Identifier: MIT
 */

#ifndef KIT_C11_IMPL_THREADS_WIN32_H_
#define KIT_C11_IMPL_THREADS_WIN32_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef KIT_HAVE_WINDOWS
void __threads_win32_tls_callback(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
