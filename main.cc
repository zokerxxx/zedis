#include <iostream>
#include <string.h>
using namespace std;

class A {
 public:
    A() {
        cout << "A cons" << endl;
        this->buffer = (char*)malloc(10);
        this->buffer = "zhengxin";
    }
//  protected:
    ~A() {
        cout << "A des" << endl;
        // free(buffer);
    }

    A(const A& a) {
        cout << "copy cons" << endl;
    }

    const A& operator=(const A& a) {
        cout << "== cons" << endl;
        this->buffer = a.buffer;
    }

 public:
    char *buffer = nullptr;
};

class B {
 public:
    B() = default;
    B(A &a) : a(a) {
        // this->a = a;
        // printf("a address in B %p\n", this->a.buffer);
    }
 public:
    A a;
};

int main() {


    return 0;
}
