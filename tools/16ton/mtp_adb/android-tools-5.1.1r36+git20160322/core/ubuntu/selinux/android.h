#ifndef _SELINUX_ANDROID_H_
#define _SELINUX_ANDROID_H_

#include <unistd.h>

/**
 * This is just stub header for missing functions
 */
int selinux_android_restorecon(const char *file, unsigned int flags);

#endif
