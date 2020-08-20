#include "app/App.h"

#include <iostream>
#include <unistd.h> //for sleep


int main() {
    App app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "\n\n\n Bebyak \n\n\n";

    return EXIT_SUCCESS;
}