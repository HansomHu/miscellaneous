#include <execinfo.h> // for backtrace
#include <signal.h>
#include <unistd.h>  /* STDERR_FILENO */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <assert.h>
#include <iostream>
#include <chrono>
#include <functional>
#include <memory>
#include <queue>
#include <regex>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <vector>
#include <random>

#if 1 // #1 test
enum : uint64_t {
    Data1 = 1 << 1,
    Data2 = 1 << 2,
    Data30 = 1 << 30,
    Data31 = 1ULL << 31,
    Data32 = 1ULL << 32,
    Data63 = 1ULL << 63,
};

using RequiredDataType = uint64_t;

enum Constant {
    MAX = 1,
};

struct Person {
    std::string name;
    float weight;
    float height;
    int age;
    explicit Person() : name(""), weight(0.0), height(0.0), age(0) {}
    Person(std::string n, float w, float h, int a) : name(n), weight(w), height(h), age(a) {}
};

struct GSPlatformInfo {
    /// 平台类型
    enum class PlatformType {
        PlatformType_UNKNOWN = 0,
        PlatformType_IOS = 1,
        PlatformType_ANDROID = 2,
        PlatformType_EGL = 3,
        PlatformType_QT = 4,
        PlatformType_GLEW = 5
    } platformType = PlatformType::PlatformType_UNKNOWN;
    /// GPU分数
    int gpuScore = 0;
    /// 设备的性能分级
    enum class DevicePerformance {
        UNKNOWN = 0,
        BAD,
        LOW,
        MEDIUM,
        HIGH,
        BEST
    } gpuPerformance = DevicePerformance::UNKNOWN;
    /// reserved
    enum class ContextType {
        UNKNOWN = 0,
        ES2 = 2,
        ES3 = 3
    } contextType = ContextType::ES2;

    std::string deviceModel;   ///< reserved
    std::string deviceBrand;   ///< reserved
    std::string systemVersion; ///< reserved
};

struct FMPlatformInfo {
    /// 平台类型
    enum class PlatformType {
        PlatformType_UNKNOWN = 0,
        PlatformType_IOS = 1,
        PlatformType_ANDROID = 2,
        PlatformType_EGL = 3,
        PlatformType_QT = 4,
        PlatformType_GLEW = 5
    } platformType = PlatformType::PlatformType_UNKNOWN;
    /// GPU分数
    int gpuScore = 0;
    /// 设备的性能分级
    enum class DevicePerformance {
        UNKNOWN = 0,
        BAD,
        LOW,
        MEDIUM,
        HIGH,
        BEST
    } gpuPerformance = DevicePerformance::UNKNOWN;
    /// reserved
    enum class ContextType {
        UNKNOWN = 0,
        ES2 = 2,
        ES3 = 3
    } contextType = ContextType::ES2;

    std::string deviceModel;   ///< reserved
    std::string deviceBrand;   ///< reserved
    std::string systemVersion; ///< reserved
};

template <typename E>
constexpr auto toUType(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}

class IA {
public:
    virtual ~IA() = default;
    virtual void a(void) = 0;
};

class IB : public IA {
public:
    void b() { a(); }
};
class IC : public IA {};
class D : virtual public IB, virtual public IC {
public:
    void a(void) override { std::cout << "hahaha" << std::endl; }
};

void test_diamond_downcast() {
    auto obj = std::make_shared<D>();
    std::shared_ptr<IB> ptr_b = obj;
    ptr_b->b();
    obj->b();
}

static_assert(toUType(GSPlatformInfo::PlatformType::PlatformType_UNKNOWN) == toUType(FMPlatformInfo::PlatformType::PlatformType_UNKNOWN));

void test_regex(void) {
    // test regex
    if (std::regex_match("softwareTesting", std::regex("(soft)(.*)"))) {
        std::cout << "string:literal => matched\n";
    }
    const char mystr[] = "SoftwareTestingHelp";
    std::string str(" software abc software");
    std::regex str_expr("( soft)(.*)");

    if (std::regex_match(str, str_expr))
        std::cout << "string:object => matched\n";

    if (std::regex_match(str.begin(), str.end(), str_expr))
        std::cout << "string:range(begin-end)=> matched\n";

    std::cmatch cm;
    std::regex_match(mystr, cm, str_expr);

    std::smatch sm;
    std::regex_match(str, sm, str_expr);

    // std::regex_match(str.cbegin(), str.cend(), sm, str_expr);
    std::cout << "String:range, size:" << sm.size() << " matches\n";

    std::regex_match(mystr, cm, str_expr, std::regex_constants::match_default);

    std::cout << "the matches are: ";
    for (unsigned i = 0; i < sm.size(); ++i) {
        std::cout << "[" << sm[i] << "] ";
    }
    std::cout << std::endl;

    {
        using namespace std;
        string mystr = "This is software testing Help portal \n";

        cout << "Input string: " << mystr << endl;

        // regex to match string beginning with 'p'
        regex regexp("p[a-zA-Z]+");
        cout << "Replace the word 'portal' with word 'website' : ";
        // regex_replace() for replacing the match with the word 'website'
        string result = regex_replace(mystr, regexp, "website");
        cout << result;

        cout << "Replace the word 'website' back to 'portal': ";
        // regex_replace( ) for replacing the match back with 'portal'
        regexp = ("we[a-zA-Z]+");

        string res;
        regex_replace(back_inserter(res), result.begin(), result.end(),
                        regexp, "portal");

        cout << res;
        cout << result;
    }

    // check date format
    {
        std::regex r("(\\d{4})/(0?[1-9]|1[0-2])/(0?[1-9]|[1-2][0-9]|3[0-1])");
        while(true) {
            std::cout << "Enter a date (year/month/day) (q=quit): ";
            std::string str;
            if (!std::getline(std::cin, str) || str == "q") {
                break;
            }
            std::smatch m;
            if (std::regex_match(str, m, r)) {
                int year = std::stoi(m[1]);
                int month = std::stoi(m[2]);
                int day = std::stoi(m[3]);
                std::cout << " Valid date: Year = " << year
                    << ", month = " << month << ", day = " << day << std::endl;
            } else {
                std::cout << " Invalid date!" << std::endl;
            }
        }
    }
}

void test_chrono(void) {
    // test std::chrono::steady_clock::now(), it represents an integer in ns
    auto t1 = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto t2 = std::chrono::steady_clock::now();
    auto elapsed = (t2 - t1).count(); // ns
    std::cout << "elapsed = " << elapsed << " ns, t1.time_since_epoch().count() = " << t1.time_since_epoch().count() << "\n";

    // test std::chrono::high_resolution_clock::now(), it represents an integer in ns
    auto tt1 = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto tt2 = std::chrono::high_resolution_clock::now();
    auto elapsed1 = (tt2 - tt1).count(); // ns
    std::cout << "elapsed1 = " << elapsed1 << " ns\n";
    std::chrono::duration<double, std::milli> elapsed2 = tt2 - tt1;
    std::cout << "elapsed2 = " << elapsed2.count() << " ms\n";

    auto ts_ms = std::chrono::steady_clock::now();
    std::cout << "ts_ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(ts_ms.time_since_epoch()).count() << " ms" << std::endl;
}

void test_struct_cast(void) {
    FMPlatformInfo fmInfo;
    GSPlatformInfo gsInfo;
    fmInfo.platformType = FMPlatformInfo::PlatformType::PlatformType_IOS;
    fmInfo.gpuPerformance = FMPlatformInfo::DevicePerformance::HIGH;
    fmInfo.deviceBrand = "Apple";
    gsInfo.platformType = static_cast<GSPlatformInfo::PlatformType>(fmInfo.platformType);
    std::cout << "gsInfo.platformType = " << toUType(gsInfo.platformType) << "\n";
    gsInfo = reinterpret_cast<GSPlatformInfo&>(fmInfo);
    std::cout << "gsInfo.deviceBrand = " << gsInfo.deviceBrand << "\n";
}

void test_unique_ptr_and_deque(void) {
    auto ptr1 = std::make_unique<Person>("Jack", 80, 170, 20);
    auto ptr2 = std::make_unique<Person>("John", 75, 175, 21);
    auto ptr3 = std::make_unique<Person>("Li Lei", 70, 176, 22);

    std::deque<std::unique_ptr<Person>> my_queue;
    my_queue.push_back(std::move(ptr1));
    my_queue.push_back(std::move(ptr2));
    my_queue.push_back(std::move(ptr3));
    std::cout << ptr1.get() << "\n" << ptr2.get() << "\n" << ptr3.get() << my_queue.size() << std::endl;

    while (!my_queue.empty()) {
        auto ptr = std::move(my_queue.front());
        std::cout << ptr->name << ", " << ptr->age << ", " << ptr->height << ", " << ptr->weight << std::endl;
        my_queue.pop_front();
    }
}

class Base {
public:
    Base() { std::cout << "Construct Base" << std::endl; }
    virtual ~Base() { std::cout << "Destruct Base" << std::endl; }
    void print() { std::cout << "Base print()" << std::endl; }
    virtual void print1() { std::cout << "Base print1()" << std::endl; print(); }
    virtual void print2() { std::cout << "Base print2()" << std::endl; }
    int get_x(void) { return m_x; }
    void set_x(int d) { m_x = d; }

protected:
    int m_x = 0;
};

class Derived : public Base {
public:
    Derived() { std::cout << "Construct Derived" << std::endl; }
    ~Derived() { std::cout << "Destruct Derived" << std::endl; }
    void print() { std::cout << "Derived print()" << std::endl; print1(); }
    virtual void print1() override { std::cout << "Derived print1()" << std::endl; Base::print1(); }
    virtual void print2() override { std::cout << "Derived print2()" << std::endl; print(); }
    int get_x_derived(void) { return m_x; }
    void set_x_derived(int d) { m_x = d; }
    int get_x_base(void) { return Base::m_x; }
    void set_x_base(int d) { Base::m_x = d; }

protected:
    int m_x = 1;
};

std::string str_modify(std::string&& to_change) {
    std::cout << "source string: " << to_change << std::endl;
    to_change += "<WORDS ADDED>";
    std::cout << "after modification: " << to_change << std::endl;
    return std::move(to_change);
}

void test_string_rvalue(void) {
    const char* sentence = "This is my world!";
    std::string sentence1 = "This sentence is wrapped by std::string";
    std::cout << sentence << " | " << str_modify(sentence) << " | " << std::string(sentence) << std::endl;
    std::cout << sentence1 << " | " << str_modify(std::move(sentence1)) << " | " << std::string(sentence1) << std::endl;
    std::cout << str_modify("This is a literal") << std::endl;
}

void test_vector_erase(void) {
    struct Integer {
        Integer() { std::cout << "Integer()" << std::endl; }
        Integer(const Integer& rhs) { x = rhs.x; std::cout << "Integer(const Integer& rhs)" << std::endl; }
        Integer(int m) : x(m) { std::cout << "Integer(m): " << x << std::endl; }
        Integer(Integer&& rhs) { std::cout << "Integer(Integer&& rhs) " << x << ", " << rhs.x << std::endl; x = rhs.x; }
        Integer& operator=(Integer& rhs) { std::cout << "operator= " << x << ", " << rhs.x << std::endl; x = rhs.x; return *this; }
        Integer& operator=(Integer&& rhs) { std::cout << "operator=&& " << x << ", " << rhs.x << std::endl; x = rhs.x; return *this; }
        ~Integer() { std::cout << "~Integer(): " << x << std::endl; }
        int x = 0;
    };
    // std::vector<Integer> v = {1, 2, 3, 4, 5};
    std::vector<Integer> v;
    v.reserve(10);
    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);
    v.emplace_back(4);
    v.emplace_back(5);
    std::cout << "========" << std::endl;
    auto m = v.begin();
    v.erase(m);
    m = v.begin();
    for (; m != v.end();) {
        std::cout << "Erasing " << m->x << ", " << &m << std::endl;
        m = v.erase(m);
    }
    std::cout << "========" << std::endl;
}

struct MyStruct {
    double d;
    int i;
    unsigned short s;
    std::string str;
};

template <typename T>
void func(T&& a) {
    std::cout << "func(" << a << ")" << std::endl;
    return;
}
void test_template(void) {
    func(std::string("hhhh"));
    func(27);
}

// judge whether T1 and T2 are exactly same
// is_same_type<int, int>() == true
// is_same_type<int, float>() == false
template<typename T1, typename T2>
struct is_same_type
{
    constexpr operator bool()
    {
        return false;
    }
};
template<typename T1>
struct is_same_type<T1, T1>
{
    constexpr operator bool()
    {
        return true;
    }
};
struct Type1 {
    int a = 0;
};
struct Type2 {
    int a = 0;
};

using FuncType1 = std::function<void(int)>;
using FuncType2 = std::function<void(int)>;
using FuncType3 = std::function<void(void)>;

void test_cast_function_type(void) {
    int a = 1;
    int b = 2;
    FuncType1 func1 = [=](int d) { std::cout << "In FuncType1, get a = " << a << ", input d = " << d << std::endl; };
    auto cast_func = [](void* ptr) {
        auto func = *reinterpret_cast<FuncType2*>(ptr);
        func(25);
        std::cout << "cast_func() ends" << std::endl;
    };

    cast_func(&func1);

    std::cout << "FuncType1 & FuncType2 are same: " << is_same_type<FuncType1, FuncType2>() << std::endl;
    std::cout << "FuncType1 & FuncType3 are same: " << is_same_type<FuncType1, FuncType3>() << std::endl;
    static_assert(is_same_type<FuncType1, FuncType2>(), "FuncType1 & FuncType2 aren't same");
    auto my_lambda = [](int d) {};
    FuncType1 my_functype1 = [](int d) {};
    // static_assert(is_same_type<FuncType1, decltype(my_lambda)>(), "FuncType1 & FuncType2 aren't same");
    std::cout << "================= using std::is_same instead" << std::endl;
    std::cout << "FuncType1 & FuncType2 are same: " << std::is_same<FuncType1, FuncType2>::value << std::endl;
    std::cout << "FuncType1 & FuncType3 are same: " << std::is_same<FuncType1, FuncType3>::value << std::endl;
    // static_assert(std::is_same<FuncType1, FuncType2>::value, "FuncType1 & FuncType2 aren't same");
    // static_assert(std::is_same<FuncType1, decltype(my_lambda)>::value, "FuncType1 & my_lambda aren't same");
    static_assert(std::is_same<FuncType1, decltype(my_functype1)>::value, "FuncType1 & my_functype1 aren't same");
    assert((std::is_same<int, int>::value == true));
    // static_assert(std::is_same<FuncType1, FuncType3>::value, "FuncType1 & FuncType3 aren't same");
    // static_assert(std::is_same<FuncType1, void(int)>::value, "FuncType1 & FuncType3 aren't same");
}

// test variadic templates
template <class T>
void printarg(T t)
{
   std::cout << t << std::endl;
}

template <class ...Args>
void expand(Args... args)
{
   // int arr[] = {(printarg(args), args)...};
   int arr[] = {args...};
   std::cout << "expand() print: ";
   for (auto i : arr) {
       std::cout << i << ", ";
   }
   std::cout << std::endl;
}

void test_variadic_templates() {
    expand(1, 2, 3, 4, 5);
}

void test_find_if() {
    std::deque<int> my_queue;
    auto&& it = std::find_if(my_queue.begin(), my_queue.end(), [&](const auto& e) { return e == 999; });
    if (it == my_queue.end())
    {
        std::cout << "Not found in empty dequeue" << std::endl;
    }
    my_queue.push_back(111);
    auto&& it1 = std::find_if(my_queue.begin(), my_queue.end(), [&](const auto& e) { return e == 999; });
    if (it1 == my_queue.end())
    {
        std::cout << "Not found in none-empty dequeue" << std::endl;
    }
}


void test_if_temp_varaible() {
    if (auto ptr = std::make_shared<int>(10)) {
        std::cout << *ptr << std::endl;
    } else {
        std::cout << __func__ << ": ptr not available!" << std::endl;
    }
}

void test_mt19937_random() {
    std::random_device seeder;
    const auto seed = seeder.entropy() ? seeder() : time(nullptr);
    std::mt19937 eng(static_cast<std::mt19937::result_type>(1));
    std::uniform_int_distribution<int> dist(1, 99);

    auto gen = std::bind(dist, eng);

    std::mt19937 eng1;
    // eng1 = std::mt19937(static_cast<std::mt19937::result_type>(seed));
    eng1 = eng;
    auto gen1 = std::bind(dist, eng1);
    std::vector<int> vec(12);
    std::generate(vec.begin(), vec.end(), gen1);
    std::cout << "random numbers: \n";
    for (auto i : vec) {
        std::cout << " - " << i << "\n";
    }
}

void test_lambda_and_static_variable() {
    static int x = 5;
    std::cout << "x = " << x << std::endl;
    auto f_copy = [] { x = 6; };
    f_copy();
    std::cout << "x = " << x << std::endl;
    auto f_reference = [&] { x = 7; };
    f_reference();
    std::cout << "x = " << x << std::endl;
}

class Singleton {
public:
    static Singleton* getInstance() {
        static Singleton& instance = *new Singleton(); // this statement is executed when getInstance() is called
        return &instance;
    }

    virtual ~Singleton() { std::cout << "Singleton Destructor" << std::endl; }

private:
    Singleton() { std::cout << "Singleton constructor" << std::endl; }
};

template <int isGLorCV = 0>
class MyQueue {
public:
    using FBOPointer = typename std::conditional_t<isGLorCV, std::shared_ptr<Person>, std::shared_ptr<MyStruct>>;
    struct QueueInfo {
        int x;
        int y;
    };
public:
    ~MyQueue() = default;
protected:
    std::deque<FBOPointer> m_my_queue;
};

class MyQueueManagerGL : public MyQueue<0> {
public:
    ~MyQueueManagerGL() = default;
protected:
    std::string m_name = "xxxGL";
};

class MyQueueManagerCV : public MyQueue<1> {
public:
    ~MyQueueManagerCV() = default;
protected:
    std::string m_name = "xxxCV";
};

static_assert(!std::is_same<MyQueueManagerGL::QueueInfo, MyQueueManagerCV::QueueInfo>::value, "QueueInfo is not same!");

constexpr bool strings_equal(char const * a, char const * b) {
    return std::string_view(a)==b;
}

static_assert(strings_equal("abc", "abc" ), "strings are equal");
// static_assert(strings_equal("abc", "abcd"), "strings are not equal");

class Base1 {
public:
    void func1() { std::cout << "func1" << std::endl; }
protected:
    int a = 1;
};

class Base2 {
public:
    void func2() { std::cout << "func2" << std::endl; }
protected:
    int b = 2;
};

class Derived1 : public Base1, public Base2 {
public:
    void func3() { std::cout << "func3" << std::endl; }
    void handle(Base* obj) {
        std::cout << "In handle(Base* obj) function" << std::endl;
        obj->print();
    }
    void handle(Derived* obj) {
        std::cout << "In handle(Derived* obj) function" << std::endl;
        obj->print();
    }
protected:
    int c = 3;
};

void test_pointer() {
    Derived1 obj;
    Base1* ptr1 = &obj;
    Base2* ptr2 = &obj;
    std::cout << "Address of obj: " << &obj << ", ptr1: " << ptr1 << ", ptr2: " << ptr2 << std::endl;
}

void test_parameter_with_overload() {
    Base base_obj;
    Derived derived_obj;
    Derived1 caller;
    caller.handle(&base_obj);
    caller.handle(&derived_obj);
    Base* ptr = &derived_obj;
    caller.handle(ptr);
}

void signal_handler(int sig) {
    void *array[10];
    size_t size=backtrace(array, 10);
    int fd = open("crash.txt", O_WRONLY | O_CREAT | O_APPEND | O_SYNC, 0666);
    write(fd, "====\n", 5);

    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, fd);

    /* additional instructions */
    if (sig == SIGILL){
        fprintf(stderr, "This error may raised when integer overflowing.\n");
    }
    exit(1);
}

void test_backtrace() {
    char* str = new char [5];
    int i = 6;
    delete[] str;
    str = nullptr;
    str[i] = 'd';
}

void test_const_shared_ptr() {
    Person a;
    a.name = "a";
    auto func1 = [](const Person& a) {
        // a.name = "a++"; // illegal
    };
    auto func2 = [](const Person* a) {
        // a->name = "b++"; // illegal
    };
    auto func3 = [](const std::shared_ptr<Person>& a) {
        a->name = "c++";
        (*a).name = "d++";
        // a.reset(); // illegal
        // a = nullptr; // illegal
        // a = std::make_shared<Person>(); // illegal
    };
    auto func4 = [](const std::shared_ptr<const Person>& a) {
        // a->name = "e++"; // illegal
    };
    auto func5 = [](const std::shared_ptr<Person>& a) {
        a->name = "g++";
    };
    auto func6 = [](const std::shared_ptr<Person>* const a) {
        (*a)->name = "g++";
    };
}

void test_base_class_with_no_destructor() {
    class BaseNoVirtualDestructor {
    public:
        ~BaseNoVirtualDestructor() {  std::cout << "BaseNoVirtualDestructor()" << std::endl;  }
        Base a;
    };
    class Child : public BaseNoVirtualDestructor {
    public:
        Child() = default;
        virtual ~Child() = default;
    };

    Child child;
}

void test_sizeof() {
    char str[] = "abcdefg";
    std::cout << "sizeof(str): " << sizeof(str) << std::endl;
    int array[] = { 1, 2, 3, 4 };
    std::cout << "sizeof(array): " << sizeof(array) << std::endl;

    auto f = [](auto* ptr) { std::cout << "sizeof(ptr): " << sizeof(ptr) << std::endl; };
    auto f1 = [&] {
        std::cout << "sizeof(&str): " << sizeof(str) << std::endl;
        std::cout << "sizeof(&array): " << sizeof(array) << std::endl;
    };
    f(str);
    f(array);
    f1();
}

void test_dynamic_array(int n) {
    int array[n];
    std::cout << "sizeof(array) " << sizeof(array) << std::endl;
}

struct AlignmentTest {
    int a;
    double b;
    char c;
    void printAligment() {
        std::cout << "a: " << sizeof(a) << ", b: " << sizeof(b) << ", c: " << sizeof(c)
            << ", AlignmentTest: " << sizeof(AlignmentTest) << std::endl;
    }
    void printElements() {
        std::cout << "a: " << a << ", b: " << b << ", c: " << c << std::endl;
    }
};

void test_cast_array_to_struct_pointer() {
    int size = sizeof(AlignmentTest);
    int nr = 2;
    AlignmentTest e1 = {123, 567, 'A'};
    AlignmentTest e2 = {321, 765, 'B'};
    e1.printAligment();
    uint8_t* arr = new uint8_t [nr * size];
    memcpy(arr, &e1, size);
    memcpy(arr + size, &e2, size);
    AlignmentTest* ptr = reinterpret_cast<AlignmentTest*>(arr);
    ptr->printElements();
    ptr = reinterpret_cast<AlignmentTest*>(arr + size);
    ptr->printElements();
    delete [] arr;
}

void test_priority_queue() {
    std::priority_queue<int, std::vector<int>, std::less<int>> que1;
    que1.push(3);
    que1.push(2);
    que1.push(5);
    std::cout << "operator std::less: ";
    while (!que1.empty()) {
        std::cout << que1.top() << " ";
        que1.pop();
    }
    std::cout << "\noperator std::greater: ";
    std::priority_queue<int, std::vector<int>, std::greater<int>> que2;
    que2.push(3);
    que2.push(2);
    que2.push(5);
    while (!que2.empty()) {
        std::cout << que2.top() << " ";
        que2.pop();
    }
    std::cout << "\noperator < ";
    auto less = [] (const auto& a, const auto& b) { return a < b; };
    std::priority_queue<int, std::vector<int>, decltype(less)> que3{less};
    que3.push(3);
    que3.push(2);
    que3.push(5);
    while (!que3.empty()) {
        std::cout << que3.top() << " ";
        que3.pop();
    }
}

int main(int argc, char** argv) {
#if 0
    std::cout << "Hello, world! MAX is: " << MAX << "\n";
    RequiredDataType dataType = Data30 | Data63;
    std::cout << "dataType is: " << std::hex << dataType << std::dec << std::endl;
    std::vector<Person> v(10);
    // test_regex();

    // test virtual member function in inheritence
    {
        Derived obj;
        std::cout << obj.get_x() << ", " << obj.get_x_base() << ", " << obj.get_x_derived() << std::endl;
        obj.set_x_base(2);
        obj.set_x_derived(3);
        std::cout << obj.get_x() << ", " << obj.get_x_base() << ", " << obj.get_x_derived() << std::endl;
    }

    {
        // auto ptr = std::make_shared<Derived>();
        std::shared_ptr<Base> ptr = std::shared_ptr<Derived>(new Derived());
        ptr->print();
        ptr->print2();
        // ptr->print1();
        auto ptr1 = std::dynamic_pointer_cast<Derived>(ptr);
        if (ptr1) {
            ptr1->print();
            // ptr1->print1();
        }
    }
    std::cout << "sizeof(MyStruct) = " << sizeof(MyStruct) << ", sizeof(std::string) = " << sizeof(std::string) << std::endl;
    std::cout << "sizeof(std::string(\"I am hyx\")) = " << sizeof(std::string("I am hyx")) << std::endl;
#endif

    // test_variadic_templates();
    // test_find_if();
    // test_string_rvalue();
    // test_vector_erase();
    // test_cast_function_type();
    // test_chrono();
    // test_struct_cast();
    // test_unique_ptr_and_deque();
    // test_if_temp_varaible();
    // test_mt19937_random();
    // auto ptr = std::shared_ptr<Singleton>(Singleton::getInstance());
    // test_lambda_and_static_variable();
    // test_diamond_downcast();
    // test_pointer();
    // test_parameter_with_overload();
    // signal(SIGSEGV, signal_handler);
    // test_backtrace();
    // test_regex();
    // test_base_class_with_no_destructor();
    // test_sizeof();
    // int n;
    // std::cin >> n;
    // test_dynamic_array(n);
    // test_cast_array_to_struct_pointer();
    test_priority_queue();

    return 0;
}
#endif // #1 test

#if 0 // #2 test
struct Base
{
    Base() { std::cout << "  Base::Base()\n"; }
    // Note: non-virtual destructor is OK here
    ~Base() { std::cout << "  Base::~Base()\n"; }
};
 
struct Derived: public Base
{
    Derived() { std::cout << "  Derived::Derived()\n"; }
    ~Derived() { std::cout << "  Derived::~Derived()\n"; }
};
 
void thr(std::shared_ptr<Base> p, int num)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::shared_ptr<Base> lp = p; // thread-safe, even though the
                                  // shared use_count is incremented
    int count = 2;
    while (count--) {
        static std::mutex io_mutex;
        std::lock_guard<std::mutex> lk(io_mutex);
        std::cout << "thread #" << num << " local pointer in a thread:\n"
                  << "  lp.get() = " << lp.get()
                  << ", lp.use_count() = " << lp.use_count() << '\n';
    }
    std::cout << "thread #" << num << " exits\n";
}

int main()
{
    std::shared_ptr<Base> p = std::make_shared<Derived>();
 
    std::cout << "Created a shared Derived (as a pointer to Base)\n"
              << "  p.get() = " << p.get()
              << ", p.use_count() = " << p.use_count() << '\n';
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back(thr, p, i);
    }
    p.reset(); // release ownership from main
    std::cout << "Shared ownership between 3 threads and released\n"
              << "ownership from main:\n"
              << "  p.get() = " << p.get()
              << ", p.use_count() = " << p.use_count() << '\n';
    for (auto& t : threads) {
        t.join();
    }
    std::cout << "All threads completed, the last one deleted Derived\n";
}
#endif // #2 test
