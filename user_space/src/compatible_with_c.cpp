/*
    In the following example ilustrated how to write c compatible code blocks
    using c++ compilers. To see how compiler treated cg() function we should make 
    assembly of cpp file.
        $g++ -S src/compatible_with_c.cpp

    This will create .s file in which we can see that all functions expect cg()
    have unique names like `Zviiv1`, cg() function name remained the same.

    Keywords:
        extern "c"
        #define __cplusplus

    Compatibilities:
        There are many compatible data structures we can use in C++ that will run also in C/
        1. Class and structures that don't modify memory layouts
            (eg. virtual functions, inheritances). They add C++ features in memory
            layouts such as pointer to a virtual table or base class sub-objects.
        2. C functions from standard library
            (eg. strdup, strcpy etc). We must ensure to treat them the same way
            as we would do in C program, In instance, using free() to deallocate
            the used memory instead of delete().
*/

#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

using namespace std;

int f(int v)
{
    cout << "f - function" << endl;
    return v * 2;
}

void g()
{
    cout << "g - function" << endl;
}

// If __cplusplus is defined that means we are in c++ compiler
// This is the standard and all compatible c++ compilers have defined this macro.
#ifdef __cplusplus
extern "C"
#endif
void cg()
{
    cout << "cg - function" << endl;
}

int main(int argv, char** argc)
{
    // Here we use typeid().name() to see what will be mangled name of our functions
    cout << "type name of f() : " << typeid(f).name() << endl; // mangled name of int (int)
    cout << "type name of g() : " << typeid(g).name() << endl; // mangled name of void (void)
    cout << "type name of cg() : " << typeid(cg).name() << endl; // mangled name of void (void)
    return 1;
}
