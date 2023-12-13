#ifndef __FLEXPIPE_UTILS__
#define __FLEXPIPE_UTILS__


#define debug_print(...) do { \
                              fprintf(stderr, "\033[1;31m[DEBUG] \033[0;32m[FUNC] %s \033[0m", __PRETTY_FUNCTION__); \
                              fprintf(stderr, __VA_ARGS__); \
                              fprintf(stderr, "\n"); \
                            } while (0)

#include <chrono>
#define getNow() std::chrono::high_resolution_clock::now()
#define getTsUs() (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count())
#define getTsMs() ( ((double)getTsUs()) / 1000)
#define getTsNow() getTsMs()

#endif

