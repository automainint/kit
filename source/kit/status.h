#ifndef KIT_STATUS_H
#define KIT_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
  KIT_OK,
  KIT_ERROR_BAD_ALLOC,
  KIT_ERROR_MKDIR_FAILED,
  KIT_ERROR_RMDIR_FAILED,
  KIT_ERROR_UNLINK_FAILED,
  KIT_ERROR_FILE_ALREADY_EXISTS,
  KIT_ERROR_FILE_DO_NOT_EXISTS,
  KIT_ERROR_PATH_TOO_LONG
};

typedef int kit_status_t;

#ifdef __cplusplus
}
#endif

#endif
