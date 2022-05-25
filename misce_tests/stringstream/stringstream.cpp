#include <unistd.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std::chrono;

struct MyInfo {
    int people;
    float score;
    std::vector<float> colors;
    std::vector<int> numbers;
};

void printToStringStream(const std::shared_ptr<MyInfo>& info, std::stringstream& ss) {
    ss << "info: " << std::endl << info->people << std::endl << info->score << std::endl;
    ss <<  "colors: " << info->colors.size() << " ";
    for (int i = 0; i < info->colors.size(); ++i) {
        ss << info->colors[i] << " ";
    }
    ss << std::endl << "numbers: " << info->numbers.size() << " ";
    for (int i = 0; i < info->numbers.size(); ++i) {
        ss << info->numbers[i] << " ";
    }
    ss << std::endl;
}

bool writeToFile(const std::string& file, const std::shared_ptr<MyInfo>& info) {
    std::ofstream ofs(file);
    if (!ofs.is_open()) {
        return false;
    }

    if (info == nullptr) {
        return false;
    }
    std::stringstream ss;
    printToStringStream(info, ss);
    ofs << ss.str();
    ofs.close();
    return true;
}

bool readFromFile(const std::string& file, std::shared_ptr<MyInfo>& info) {
    std::ifstream ifs(file);
    if (!ifs.is_open()) {
        return false;
    }

    auto data = std::make_shared<MyInfo>();
    std::string line;
    std::getline(ifs, line);
    std::stringstream ss(line);
    if (line.find("info: ") != 0) {
        std::cout << "Cannot find \"info: \" at the very beginning, format is wrong!" << std::endl;
        return false;
    }

    std::getline(ifs, line);
    ss.clear();
    ss.str(line);
    ss >> data->people;

    std::getline(ifs, line);
    ss.clear();
    ss.str(line);
    ss >> data->score;

    std::getline(ifs, line);
    if (line.find("colors: ") != 0) {
        std::cout << "Cannot find \"colors: \" at the beginning of line, format is wrong!" << std::endl;
        return false;
    } else {
        ss.clear();
        ss.str(line);
        std::string label;
        ss >> label;
        std::cout << "Find label: " << label << std::endl;
        int x;
        ss >> x;
    }

    for (;;) {
        float x;
        ss >> x;
        if (ss.fail()) break;
        data->colors.push_back(x);
    }

    std::getline(ifs, line);
    if (line.find("numbers: ") != 0) {
        std::cout << "Cannot find \"numbers: \" at the beginning of line, format is wrong!" << std::endl;
        return false;
    } else {
        ss.clear();
        ss.str(line);
        std::string label;
        ss >> label;
        std::cout << "Find label: " << label << std::endl;
        int x;
        ss >> x;
    }

    for (;;) {
        int x;
        ss >> x;
        if (ss.fail()) break;
        data->numbers.push_back(x);
    }

    info = data;
    return true;
}

bool test_empty_string(const std::string& str) {
    return str.empty();
}

int main(int argc, char** argv) {
    #if 0
    std::ofstream ofs("output.txt");
    std::stringstream ss;
    std::vector<float> colors = {1.111, 2.222, 3.333, 0.05};
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    ss << "colors: ";
    for (auto& c : colors) {
        ss << c << " ";
    }
    ss << std::endl << "numbers; ";
    for (auto& n : numbers) {
        ss << n << " ";
    }
    ss << std::endl;
    ofs << ss.str();
    ofs.close();
    #else
    const std::string file = "output.txt";
    auto info1 = std::make_shared<MyInfo>();
    std::vector<float> colors = {1.111, 2.222, 3.333, 0.05};
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    info1->people = 20;
    info1->score = 99.99;
    info1->colors = colors;
    info1->numbers = numbers;

    if (writeToFile(file, info1)) {
        std::cout << "Write to file success: " << file << std::endl;
        std::shared_ptr<MyInfo> info2;
        if (readFromFile(file, info2)) {
            std::cout << "Read from file success: " << file << std::endl;
            std::stringstream ss;
            printToStringStream(info2, ss);
            std::cout << "The data read from the file is: " << std::endl << ss.str() << std::endl;
            writeToFile("output2.txt", info2);
        }
    }
    #endif

    // test for std::chrono::high_resolution_clock
    auto start_ts = high_resolution_clock::now();
    std::cout << "sleep for 5000ms" << std::endl;
    usleep(5000 * 1000);
    auto end_ts = high_resolution_clock::now();
    auto interval = duration_cast<milliseconds>(end_ts - start_ts).count();
    std::cout << "high_resolution_clock interval: " << interval << std::endl;
    printf("printf() high_resolution_clock interval: %lld\n", interval);
    
    // test the const std::string& syntax
    const std::string empty = "";
    const std::string non_empty = "John";
    std::cout << std::boolalpha << test_empty_string(empty) << std::endl;
    std::cout << std::boolalpha << test_empty_string(non_empty) << std::endl;
    std::cout << std::boolalpha << test_empty_string("") << std::endl;

    return 0;
}
