/*
    This example shows how compiler align data in memory.

    Popular question is when a class with several built-in data members
    is given and asked what is the size of object of that class.

    The main rule that c++ compilers follow when saving data in the ram is that
    each size must be aligned in the blocks of memory which addresses are aligned
    with the size of that specific data.

    Formula: (pseudo code of finding suitable address for T type of object)
        next_address = 0x0;
        for (i = 0, i < ram_size / block_size; ++i) {
            next_address = sizeof(T) * i;
            if (is_address_free(next_address)) break;
        }
        return next_address;

    Question:
        What will be the size of this class object?.
            `` class C {
                   char a;
                   char b;
                   char c;
                   shot d;
                   int i;
                   char g;
               };
*/

#include <iostream>

using namespace std;

int main(int argv, char** argc)
{
    return 1;
}
