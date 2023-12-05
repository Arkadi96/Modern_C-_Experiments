/*
    Provided two different ways of evaluating the value of the function
    or variable at compile time.
    ``constexpr`` specifier (since C++11).
*/

#include <iostream>
#include <chrono>

using namespace std::chrono;
using namespace std;

template<int n>
struct Fibonacci_struct
{
    enum {value = Fibonacci_struct<n-1>::value + Fibonacci_struct<n-2>::value};
};

template<>
struct Fibonacci_struct<1>
{
    enum {value = 1};
};

template<>
struct Fibonacci_struct<2>
{
    enum {value = 1};
};

constexpr int Fibonacci_expr(int id)
{
    if (id == 1 || id == 2) {
        return 1;
    }

    return Fibonacci_expr(id - 1) + Fibonacci_expr(id - 2);
}

int main(int argv, char** argc) {
    const int v = 6;
    [[maybe_unused]] auto beg = high_resolution_clock::now();
    const int res1 = Fibonacci_struct<v>::value;
    cout << "Fibonacci template struct (" << v << "): is " << res1 << endl;
    auto step1 = high_resolution_clock::now();
    const int res2 = Fibonacci_expr(v);
    cout << "Fibonacci const expression (" << v << "): is " << res2 << std::endl;
    auto end = high_resolution_clock::now();

    auto duration1 = duration_cast<microseconds>(step1 - beg);
    auto duration2 = duration_cast<microseconds>(end - step1);

    cout << "\nElapsed Time first case: " << duration1.count() << "(μs)";
    cout << "\nElapsed Time second case: " << duration2.count() << "(μs)";

    return 1;
}
