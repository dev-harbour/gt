#ifndef _LINUX_H_
#define _LINUX_H_

#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define GET_CURRENT_DIR  getcwd
#define PATH_MAX         4096  /* # chars in a path name including nul */

#define _POSIX_C_SOURCE  200809L

#endif /* End _LINUX_H_ */
