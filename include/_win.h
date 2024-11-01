#ifndef _WIN_H_
#define _WIN_H_

#include <direct.h>
#include <windows.h>

#define SDL_MAIN_HANDLED
#define GET_CURRENT_DIR   _getcwd
#define PATH_MAX          260  /* Windows standard path limit */

#endif /* End _WIN_H_ */
