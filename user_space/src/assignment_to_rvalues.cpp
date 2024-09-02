/*
    In the following example difference between built-in and custom types
    are introduced. Mainly the way compiler behaves when assignment is done.
    For built in types it takes all responsibilities and prevents any attempts
    that meant to modify rvalue references meanwhile it let programmers
    to handle that scenario without even issuing warnings or errors when
    there is no implemented explicit solutions. Instead in this example we
    can see that compiler creates a default assignment operator for custom type
    that will copy all data wisely and return *this.
*/
#include <iostream>

struct Muchacha {
    int* ptr;
    Muchacha()
    {
        std::cout << "default ctor" << std::endl;
        ptr = nullptr;
    }
    Muchacha(const Muchacha &m)
    {
        std::cout << "copy ctor" << std::endl;
        this->ptr = m.ptr;
    }
    Muchacha& operator=(Muchacha& m) // add & to prevent func. call for rvalues
    {
        std::cout << "assignment oper" << std::endl;
        this->ptr = m.ptr;
        return *this;
    }
};

Muchacha ma()
{
    return Muchacha(); // creating new temp object
}

char mu()
{
    return 'c';
}

int main() {
    Muchacha mo;
    // Calling func. that will create tmp and copy object
    // This works even without assignment operator function.
    // In that case compiler creates default one.
    ma() = mo; // No error (custom type)
    // Here copy constructor is called.
    Muchacha mn = mo;
    // Here compiler prevents this type of assignments
    char mc = 'm';
    // mu() = 'm'; // Error (built in type)
    return 0;
}
