/*
    This example answers to a one of the famous interview questions.
        Given console output inside main function, try to print something else
        out of main function that will be called before main.

    So we have at lease two ways solving the problem:
        1. init_array() - this function is used by compiler and is called
            prior main() to initialize and allocate all necessary data
            and memories in order to set necessary environment for main
            to be called next.
            (0 - 100 constructors are reserved by the system, 101 to 65535 are free).
        2. Globals, static objects.
            Unlike C, in C++ we can be sure that all static and global objects will be
            constructed before main function call.: In C there is no specific rule for globals
            and variable initializations will be done at the point of their declarations.

    The result:
        When compiling to assembly (g++ -S -o <file.s> <file.cpp>) we can see 4 function
        implementations that take place before main function initialization.
            1. init1() is .init_array with priority 101
            2. init2() is .init_array with priority 102
            3. init2() is .init_array with priority (2^16 - 1)
            4. init4() is static global object has no priority
            4. main() is entry point.

*/

#include <iostream>

// g++ compiler attribute
__attribute__((constructor(101))) static void init1(){
    printf("%s\n", __FUNCTION__);
}

__attribute__((constructor(102))) static void init2(){
    printf("%s\n", __FUNCTION__);
}

__attribute__((constructor)) static void init3(){
    printf("%s\n", __FUNCTION__);
}

struct init4
{
    init4() {
        printf("%s\n", __FUNCTION__);
    }
} init4;

int main(int argc, char *argv[])
{
    printf("%s\n", __FUNCTION__);
    return 0;
}
