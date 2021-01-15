#include <debug.h>
#include "../graphics/gui.h"

#ifdef __EZ80__

// Define some C++ internal stuff that's needed for this to link
extern "C" _Noreturn __attribute((unused)) void __cxa_pure_virtual() {
    ERROR("Pure virtual function called");
}

extern "C" __attribute((unused)) int __cxa_atexit(__attribute__((unused)) void (*func) (void *),
                                                  __attribute__((unused)) void * arg,
                                                  __attribute__((unused)) void * dso_handle) {
    dbg_printf("__cxa_atexit called: %p %p %p\n", func, arg, dso_handle);
    return 0;
}

void*  __dso_handle = (void*) &__dso_handle;

#endif