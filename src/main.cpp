#include "magma/app/App.h"

#include <iostream>


int main() {
    App app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "\n\n Bebyak \n\n";

    return EXIT_SUCCESS;
}
