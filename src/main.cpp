#include <format>
#include <iostream>

#include "proxyApplication.hpp"

int main() {
    try {
        ProxyApplication proxyApplication;

        proxyApplication.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
