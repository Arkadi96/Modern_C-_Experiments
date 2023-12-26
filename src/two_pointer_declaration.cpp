/*
    In the following example ilustrated how to write c compatible code blocks
    using c++ compilers. To see how compiler treated cg() function we should make 
    assembly of cpp file.
        $g++ -S src/compatible_with_c.cpp

    This will create .s file in which we can see that all functions expect cg()
    have unique names like `Zviiv1`, cg() function name remained the same.

    Keywords:
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
