#ifndef TANKS_ERROR_H
#define TANKS_ERROR_H

#include <cstdint>
#include <string.h>

#define ERROR(msg) error_screen(msg, __FILE_NAME__, __LINE__)

[[noreturn]] void error_screen(const char *error, const char *file, uint24_t line);

#endif //TANKS_ERROR_H
