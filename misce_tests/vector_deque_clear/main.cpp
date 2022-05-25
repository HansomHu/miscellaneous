#include <unistd.h>
#include <iostream>
#include <deque>
#include <vector>

#define TEST_VECTOR_RESERVE 1
#define TEST_HOW_TO_FREE_VECTOR_MEMORY 0

struct Person {
    std::string name;
    float weight;
    float height;
    int age;
};

#if (TEST_VECTOR_RESERVE == 1)
#include <cstddef>
#include <new>
#include <vector>
#include <iostream>

// minimal C++11 allocator with debug output
template <class Tp>
struct NAlloc {
    typedef Tp value_type;
    NAlloc() = default;
    template <class T> NAlloc(const NAlloc<T>&) {}
 
    Tp* allocate(std::size_t n)
    {
        n *= sizeof(Tp);
        std::cout << "allocating " << n << " bytes\n";
        return static_cast<Tp*>(::operator new(n));
    }
 
    void deallocate(Tp* p, std::size_t n) 
    {
        std::cout << "deallocating " << n*sizeof*p << " bytes\n";
        ::operator delete(p);
    }
};
template <class T, class U>
bool operator==(const NAlloc<T>&, const NAlloc<U>&) { return true; }
template <class T, class U>
bool operator!=(const NAlloc<T>&, const NAlloc<U>&) { return false; }
 
int main()
{
    int sz = 1U << 27;
    std::cout << "using reserve: \n";
    {
        std::vector<int, NAlloc<int>> v1;
        sleep(10);
        v1.reserve(sz);
        std::cout << "After reserve, the capacity is: " << v1.capacity() << std::endl;
        sleep(10);
        std::cout << "Start to push back..." << std::endl;
        for(int n = 0; n < sz; ++n)
            v1.push_back(n);
    }
    std::cout << "not using reserve: \n";
    {
        std::vector<int, NAlloc<int>> v1;
        for(int n = 0; n < sz; ++n)
            v1.push_back(n);
    }
    std::vector<Person> persons;
    persons.reserve(10);
}
#endif // TEST_VECTOR_RESERVE

#if (TEST_HOW_TO_FREE_VECTOR_MEMORY == 1)
using namespace std;
int main(int argc, char** argv) {
    // how to deallocate the memory of std::vector manually:
#if 0
    // method 1, use shrink_to_fit()
    {
        vector<int> v(1U << 27);
        cout<<"step into stage one..."<<endl;
        sleep(10);
        v.clear();
        cout<<"step into stage two..."<<endl;
        sleep(10);
        v.shrink_to_fit();
        cout << "step into stage three..." << endl;
        sleep(10);
    }
#endif

    // method 2, use swap()
    {
        // vector<int> v(1U << 27);
        vector<int> v;
        v.reserve(1U << 27);
        cout<<"step into stage one..."<<endl;
        cout << "the current capacity is: " << v.capacity() << endl;
        sleep(10);
        v.resize(v.capacity());
        cout << "after resize ..." << endl;
        sleep(10);
        v.clear();
        cout<<"step into stage two..."<<endl;
        sleep(10);
        vector<int>().swap(v);
        cout << "step into stage three..." << endl;
        sleep(10);
    }
    return 0;
}
#endif // TEST_HOW_TO_FREE_VECTOR_MEMORY
