/*
    This example demonstrates best practises for scoped (or Enum class) and unscoped Enums.
*/

#include <iostream>
#include <string>
#include <tuple>

using UserInfo = std::tuple<std::string, std::string, int>;

// Definition of unscoped enums (can't be forward declared)
// Unscoped enums have no default underlying type. But we can specify it.
enum UserInfoFields {name, surname, age};
// Forward declaration of scoped enums with size_t underlying type.
enum class TrafficColor: std::size_t;

// Creating tuple object example
UserInfo ui0 {"Amanda", "Power", 19};

// Definition of scoped enum
enum class TrafficColor: std::size_t {Red, Green, Yellow};

int main(int argv, char** argc)
{
    // Best practise for unscoped (98's enums) is to use with std::tuples
    // Since they are "polluting" the namespaces in which they are we can use "enumerators".
    // name is implicitly converts to int as required by std::get.
    std::cout << "name: "     << std::get<name>(ui0)    // instead of std::get<0>
              << " surname: " << std::get<surname>(ui0) // instead of std::get<1>
              << " age: "     << std::get<age>(ui0)     // instead of std::get<2>
              << std::endl;

    // The same with scoped enums will create unnecessary mess in the code
    // std::get<static_cast<std::size_t>(UserInfoFields_2::name)>(ui0);
    //  -- the default underlying type is int
    //  -- using static_cast we are converting explicitly int to std::size_t;

    // Using scoped enums
    auto Color = TrafficColor::Red;

    return 1;
}
