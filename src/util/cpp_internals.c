#include <debug.h>
#include <stdbool.h>

#ifdef __EZ80__

// Define some C++ internal stuff that's needed for this to link
_Noreturn __attribute((unused)) void __cxa_pure_virtual() {
    dbg_printf("Pure virtual function was called\n");
    while (true);
}

__attribute((unused)) int __cxa_atexit(void (*func) (void *), void * arg, void * dso_handle) {
    dbg_printf("__cxa_atexit called: %p %p %p\n", func, arg, dso_handle);
    return 0;
}

void*  __dso_handle = (void*) &__dso_handle;

#endif