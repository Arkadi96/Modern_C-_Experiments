/*
    In this example one of the common mismatches is introduced.
    Declaration of pointers in the same line.

    Introduced typdef as more convenient way to create types
    that are compile time resolved (cant use for templates).

    in C++ 11 using keyboard is first introduced, than in c++14 it is widely
    recommended and adopted as a main type alias because of:
        1. flexibility within templates.
        2. Consistency with other language features.
        3. Readability and clarity of intent.

    Golden rules as conclusion:
        1. To have const value prefer const over #define
        2. To have several values prefer enum over #define
        3. To define type prefer typdef over #define
        4. To create macros prefer inline or constexpr functions over #define
        5. To have conditional compiling. its #define 100%

    Keywords:
        1. typdef
        2. using
        3. error: int* to int
*/

#include <iostream>

using namespace std;

void error_init()
{
    int x = 1;
    int* p, pp; // Compiler binds pointer int only to p and int to p
    if (typeid(pp) != typeid(int*))
        throw runtime_error("Invalid initialization: type mismatch");
    //p = &x; pp = &x;
    /* Using define wont do this declaration work as define will just swap
       int_ptr to int* at preprocessor time. pp will remain as type of int.
    int x = 1;
    #define int_ptr int*
    int_ptr p, pp;
    p = &x; pp = &x;
    */
}

void best_practice_init()
{
    int x = 1;
    int *p;  // Could be in one line as well
    int *pp; // int *p, *pp; but its ugly code as good programmers used to see pointer
    // declaration with its specific data type right to it. (*pp) does not make sense.
    p = &x; pp = &x;
}

void using_typedef_init()
{
    typedef int* int_ptr;
    int x = 1;
    int_ptr p, pp;
    if (typeid(pp) != typeid(int*))
        throw runtime_error("Invalid initialization: type mismatch");
    p = &x; pp = &x;
}

int main(int argv, char** argc)
{
    try {
        error_init();
        cout << "Error case :) initialization is done successfully" << endl;
    } catch (const std::runtime_error& err) {
        cerr << "Error: " << err.what() << endl;
    }

    try {
        best_practice_init();
        cout << "Best practice initialization is done successfully" << endl;
    } catch (const std::runtime_error& err) {
        cerr << "Error: " << err.what() << endl;
    }

    try {
        using_typedef_init();
        cout << "Typdef initialization is done successfully" << endl;
    } catch (const std::runtime_error& err) {
        cerr << "Error: " << err.what() << endl;
    }

    return 1;
}
