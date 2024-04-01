/*
    This example shows float data type architecture causes accuracy problems.
*/
#include <iostream>
#include <iomanip>

int main () {
    // Adding highest values first
    float a = 0.f;
    a += 5.743658066;
    a += 1.741842628;
    a += 1.683252454;
    a += 1.00594759;
    std::cout << "a: " << std::setprecision(10) << a << std::endl;
    // 10.17470169
    // Adding highest values last
    float b = 0.f;
    b += 1.683252454;
    b += 1.00594759;
    b += 5.743658066;
    b += 1.741842628;
    std::cout << "b: " << std::setprecision(10) << b << std::endl;
    // 10.17470074
    return 1;
}
