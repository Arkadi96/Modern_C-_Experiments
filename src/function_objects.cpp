/*
    std::function is a template in the C++11 Standard Library that generalizes
    the idea of a function pointer. Whereas function pointers can point
    only to functions, however, std::function objects can refer to any callable
    object, i.e., to anything that can be invoked like a function.

    The mission in the following example is to multiply 3 with 4 with a function
    that looks like this:
        ``mult(3)(4);``
*/

#include <iostream>
#include <functional>

using namespace std;

// f() that returns int and takes two int input arguments
std::function<int(int, int)> get_mult1()
{
    return [](int a, int b){ return a * b; };
}

// f(int) that returns int and takes one int input arguments
std::function<int(int)> mult2(int y)
{
    return [y](int x) { return y * x; };
}

int main(int argv, char** argc)
{
    // mult1 is object with type of std::function<int(int, int)>
    auto mult1 = get_mult1();
    cout << "Mult(3,4) = " << mult1(3, 4);
    cout << endl;
    cout << "Mult(3)(4) = " << mult2(3)(4);
    cout << endl;
    return 1;
}
