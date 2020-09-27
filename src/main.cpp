#include "app/App.h"

#include <iostream>

void test_config();

int main() {
    App app;
    // TODO: make infrastructure for tests
    test_config();
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "\n\n Bebyak \n\n";

    return EXIT_SUCCESS;
}