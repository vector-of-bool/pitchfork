#include <iostream>

#include <simple/simple-cmake.hpp>

int main() {
    const auto value = simple::calculate_value();
    if (value == 42) {
        std::cout << "We calculated the value correctly\n";
        return 0;
    } else {
        std::cout << "The value was incorrect!\n";
        return 1;
    }
}
