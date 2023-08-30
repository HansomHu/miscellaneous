#include <cassert>
#include <iostream>

// the following codes are from PLOG
class NonCopyable {
protected:
    // the derived class can call this proteced constructor
    NonCopyable() { std::cout << "==== NonCopyable() called\n"; }

private:
    NonCopyable(const NonCopyable &);
    NonCopyable &operator=(const NonCopyable &);
};

template <class T>
class Singleton : NonCopyable {
public:
    Singleton() {
        std::cout << "==== Singleton() called\n";
        assert(!m_instance);
        m_instance = static_cast<T *>(this);
    }

    ~Singleton() {
        std::cout << "==== ~Singleton() called\n";
        assert(m_instance);
        m_instance = 0;
    }

    static T *getInstance() {
        return m_instance;
    }

private:
    static T *m_instance;
};

template <class T>
T *Singleton<T>::m_instance = NULL;

template <int instanceId>
class Logger : public Singleton<Logger<instanceId>> {
public:
    // 
    Logger(){
        std::cout << "==== Logger() called\n";
    }
};

int main(int argc, char** argv) {
    {
        Logger<1> logger1;
    }
    Logger<1> logger2;
    return 0;
}