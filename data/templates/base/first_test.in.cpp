#include <iostream>

#include <{{ns_path}}/{{first_stem}}.hpp>

int main() {
    const auto value = {{root_ns}}::calculate_value();
    if (value == 42) {
        std::cout << "We calculated the value correctly\n";
        return 0;
    } else {
        std::cout << "The value was incorrect!\n";
        return 1;
    }
}
