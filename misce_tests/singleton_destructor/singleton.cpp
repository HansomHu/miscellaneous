#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class Singleton {
public:
    static std::shared_ptr<Singleton>& getInstance() {
        // none thread safe but can be constructed for constructed for multi times
        // if you have destructed the instance
        if (!m_instance) {
            m_instance.reset(new Singleton());
        }

        // thread safe but can only be constructed for just once time
        /* static std::once_flag flag;
        if (!m_instance) {
            std::call_once(flag, [](){ m_instance.reset(new Singleton()); });
        }
        */
        return m_instance;
    }
    ~Singleton() {  std::cout << "In the Singleton destructor" << std::endl; }
    void setName(const std::string& name) { m_name = name; }
    std::string getName(void) { return m_name; }

private:
    Singleton() { std::cout << "In the Singleton constructor" << std::endl; }

private:
    static std::shared_ptr<Singleton> m_instance;
    std::string m_name;
};

std::shared_ptr<Singleton> Singleton::m_instance = nullptr;

int main(int argc, char** argv) {
    std::cout << "Start test: \nCall getInstance() first time: " << std::endl;
    auto&& instance = Singleton::getInstance();
    instance->setName("John");
    std::cout << "The name: " << instance->getName() << std::endl;
    std::cout << "First time done" << std::endl;
    {
        std::cout << "Call getInstance() for second time: " << std::endl;
        auto&& instance1 = Singleton::getInstance();
        std::cout << "Second time done" << std::endl;
    }
    std::cout << "Try to destroy the singleton instance by reset the instance: " << std::endl;
    // instance.reset();
    Singleton::getInstance().reset();
    std::cout << "Rest the instance done\nCall getInstance() for third time: " << std::endl;
    // auto&& instance2 = Singleton::getInstance();
    instance = Singleton::getInstance();
    std::cout << "Third time done" << std::endl;
    std::vector<std::string> peoples = {"John", "James", "White", "Jack"};
    std::cout << "The first people is : " << *peoples.cbegin() << std::endl;
    return 0;
}
