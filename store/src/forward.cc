#include <iostream>
class B;
class D;

void f(B*) {
    std::cout << "B" << std::endl;
}

void f(void*) {
    std::cout << "void" << std::endl;
}


void test(D *x) {
    f(x);
}