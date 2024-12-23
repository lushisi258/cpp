#include "../include/router.h"

int main() {
    Router router;
    router.start();
    char x;
    std::cin >> x;
    router.stop();
    return 0;
}