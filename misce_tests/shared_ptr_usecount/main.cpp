#include <functional>
#include <iostream>
#include <memory>
#include <string>

class Base {
public:
    Base() { std::cout << "In Base()" << std::endl; }
    virtual ~Base() { std::cout << "In ~Base()" << std::endl; }
};

class Derived : public Base {
public:
    Derived() { std::cout << "In Derived()" << std::endl; }
    virtual ~Derived() { std::cout << "In ~Derived()" << std::endl; }
};

struct Functions {
    using Func = std::function<void(int a, int b)>;
    std::string a = "aaa";
    Func lambda;
    ~Functions() { std::cout << "In Functions destruct functions" << std::endl; }
};

int main(int argc, char** argv) {
    {
        auto funcs = std::make_shared<Functions>();
        std::cout << "funcs.use_count() is: " << funcs.use_count() << std::endl;
        funcs->a = "bbb";
        // capture funcs itself leads to a circle reference
        funcs->lambda = [funcs](int a, int b) {
            std::cout << "In funcs lambda: " << a << ", " << b << std::endl;
        };
        std::cout << "After capture funcs itself by value, funcs.use_count() is: " << funcs.use_count() << std::endl;
    }
    {
        auto ptr1 = std::shared_ptr<Base>(new Derived());
        std::cout << "1. make base class object, current ptr1.use_count() is: " << ptr1.use_count() << std::endl;
        std::cout << "2. dynamic_pointer_cast ptr1(base class) to ptr2(derived class)" << std::endl;
        auto ptr2 = std::dynamic_pointer_cast<Derived>(ptr1);
        if (!ptr2){
            std::cout << "Cast failed! Exits!" << std::endl;
            return 0;
        }
        std::cout << "3. after down cast, the ptr1.use_count() is: " << ptr1.use_count()
            << " and ptr2.use_count() is: " << ptr2.use_count() << std::endl;
    }
    return 0;
}
