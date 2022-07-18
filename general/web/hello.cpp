#include <iostream>
#include <emscripten/emscripten.h>

#ifdef __cplusplus
extern "C"
{
#endif

    EMSCRIPTEN_KEEPALIVE
    int fib(int a)
    {
        if (a == 0 || a == 1)
            return a;
        return fib(a - 2) + fib(a - 1);
    }

#ifdef __cplusplus
}
#endif
