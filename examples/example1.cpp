#include <iostream>

#include <pf/pitchfork.hpp>

int main() {
    std::cout << "I am an example executable\n";
    std::cout << "Let's calculate the value...\n";
    const auto value = pf::pitchfork();
    std::cout << "The value we got is " << value << '\n';
}
