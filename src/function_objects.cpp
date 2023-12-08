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
using f_type = std::function<int(int)>;

std::function<int(int, int)> get_mult1()
{
    return [](int a, int b){ return a * b; };
}

f_type get_mult2_0()
{
    return [](int a){ return 1 * a; };
}


std::function<int(int), (int)> get_mult2()
{
    auto f = get_mult2_0();
    return [&f](f_type a, int b){ return f(a) * b; };
}

int main(int argv, char** argc)
{
    auto mult1 = get_mult1();
    auto mult2 = get_mult2();
    cout << "Mult(3,4) = " << mult1(3, 4);
    cout << endl;
    cout << "Mult(3)(4) = " << mult2(3)(4);
    cout << endl;
    return 1;
}
