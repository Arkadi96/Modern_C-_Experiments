/*
    This example shows how compiler align data in memory.

    Popular question is when a class with several built-in data members
    is given and asked what is the size of object of that class.

    The main rule that c++ compilers follow when saving data in the ram is that
    each size must be aligned in the blocks of memory which addresses are aligned
    with the size of that specific data. The structure itself must be aligned with its
    largest member.

    Formula: (pseudo code of finding suitable address for T type of object)
        next_address = 0x0;
        for (i = 0, i < RAM_SIZE / BLOCK_SIZE; ++i) {
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
                   short d;
                   int i;
                   char g;
               };
*/

#include <iostream>

using namespace std;

/*
   if 64 bit cpu architecture the size of struct
   must be aligned with int (its highest member) size that is 32
   The size of struct must be 4 * 32 = 128 bit (16 byte)
*/
struct S0_nOpt {
    char a;     // 1 byte [X,O,O,O] 0
    char b;     // 1 byte [X,X,O,O]
    char c;     // 1 byte [X,X,X,O]
    short d;    // 2 byte [X,X,O,O] 1
    int i;      // 4 byte [X,X,X,X] 2
    char g;     // 1 byte [X,O,O,O] 3
};

/*
   if 64 bit cpu architecture the size of struct
   must be aligned with int (its highest member) size that is 32
   The size of struct must be 3 * 32 = 96 bit (12 byte)
*/
struct S0_opt {
    int i;      // 4 byte [X,X,X,X] 0
    short d;    // 2 byte [X,X,O,O] 1
    char a;     // 1 byte [X,X,X,O]
    char b;     // 1 byte [X,X,X,X]
    char c;     // 1 byte [X,O,O,O] 2
    char g;     // 1 byte [X,X,O,O]
};

int main(int argv, char** argc)
{
    S0_nOpt snopt;
    S0_opt sopt;

    cout << "not optimized struct size: " << sizeof(snopt) << " byte\n";
    cout << "optimized struct size: " << sizeof(sopt) << " byte\n";

    return 1;
}
