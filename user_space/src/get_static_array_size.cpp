/*
    Array types are different from pointer types, even though they sometimes
    seem to be interchangeable. A primary contributor to this illusion is that,
    in many contexts, an array decays into a pointer to its first element.

    E.g.
        const char name[] = "J. P. Briggs"; // name's type is const char[13]
        const char * ptrToName = name;      // array decays to pointer

    These types (const char* and const char[13]) are not the same,
    but because of the array-to-pointer decay rule, the code compiles.
    (*Example is taken out from Scott Meyers - Effective Modern C++)

    This equivalence of array and pointer parameters is a bit of foliage
    springing from the C roots at the base of C++, and it fosters the illusion
    that array and pointer types are the same.

    In this example it is demonstrated how we can pass and get static array
    with a template function that will
*/
#include <iostream>

// This will decay to array*
int sum(int array[]) {
    int sum = 0;
    for (int i = 0; i < 10; ++i) {
        sum += array[i];
    }
    return sum;
}

/*
    Although functions can’t declare parameters that are truly arrays,
    they can declare parameters that are references to arrays!.
    So if we create the template func. to take its argument by reference and we pass
    the array to it, the type not will be decayed to pointer but will remain
    int (&)[10].

*/
template<typename T, int N>
constexpr int get_ArraySize(T (&)[N]) noexcept
{
    return N;
}

int main(int argc, char *argv[])
{
    int array[] = {1,2,3,4,5,6,7,8,9,10};
    std::cout << "Size of array: " << get_ArraySize(array) << std::endl;
    std::cout << "Sum is: " << sum(array) << std::endl;
    return 0;
}
