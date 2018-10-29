#include <iostream>

#include <simple/simple-cmake.hpp>

int main() {
    std::cout << "I am an example executable\n";
    std::cout << "Let's calculate the value...\n";
    const auto value = simple::calculate_value();
    std::cout << "The value we got is " << value << '\n';
}
