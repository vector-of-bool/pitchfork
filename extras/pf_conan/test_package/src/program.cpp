#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        return 1;
    }
    if (std::string("Test string") == argv[1]) {
        return 0;
    } else {
        return 1;
    }
}