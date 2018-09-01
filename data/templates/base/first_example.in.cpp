#include <iostream>

#include <{{ns_path}}/{{first_stem}}.hpp>

int main() {
    std::cout << "I am an example executable\n";
    std::cout << "Let's calculate the value...\n";
    const auto value = {{root_ns}}::calculate_value();
    std::cout << "The value we got is " << value << '\n';
}
