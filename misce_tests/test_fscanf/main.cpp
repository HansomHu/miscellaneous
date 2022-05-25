#include <iostream>
#include <stdio.h>

bool read_from_file(const std::string& filename, unsigned short* data, bool flag_unsigned = false) {
    FILE* file;

    file = fopen(filename.c_str(), "r");
    if (file == nullptr) {
        std::cout << "Read file failed" << std::endl;
        return false;
    }
    char line_header[128] = {0};
    int res = fscanf(file, "%s", line_header);
    if (res == EOF) {
        std::cout << "EOF" << std::endl;
        fclose(file);
        return false;
    }
    if (strcmp(line_header, "f") == 0) {
        if (flag_unsigned) {
            fscanf(file, "%hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu\n",
                &data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6], &data[7], &data[8]);
        } else {
            fscanf(file, "%hd/%hd/%hd %hd/%hd/%hd %hd/%hd/%hd\n",
                &data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6], &data[7], &data[8]);
        }
    }
    fclose(file);
    return true;
}

int main(int argc, char** argv) {
    unsigned short data1[9] = {0};
    unsigned short data2[9] = {1};

    if (read_from_file("obj.txt", data1, true)) {
        printf("data1: %d/%d/%d %d/%d/%d %d/%d/%d\n",
                data1[0], data1[1], data1[2], data1[3], data1[4], data1[5], data1[6], data1[7], data1[8]);
    }

    if (read_from_file("obj.txt", data2, false)) {
        printf("data2: %d/%d/%d %d/%d/%d %d/%d/%d\n",
                data2[0], data2[1], data2[2], data2[3], data2[4], data2[5], data2[6], data2[7], data2[8]);
    }

    if (!memcmp(data1, data2, sizeof(unsigned short) * 9)) {
        std::cout << "data1[9] & data2[9] is same" << std::endl;
    }
    std::cout << "exit" << std::endl;
    return 0;
}
