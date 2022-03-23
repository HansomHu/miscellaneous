// 任意给定一个没有重复数字的数字序列，输出它的所有排列形式，不限制输出顺序，例如：输入[1, 2, 3]
// 输出：[[1, 2, 3], [1, 3, 2], [2, 1, 3], [2, 3, 1], [3, 1, 2], [3, 2, 1]]
#include <iostream>
#include <vector>

void perm(int list[], int low, int high) {
    if (low == high) { //当low==high时,此时list就是其中一个排列,输出list
        for (int i = 0; i <= low; i++) {
            std::cout << list[i];
        }
        std::cout << std::endl;
    } else {
        for (int i = low; i <= high; i++) { //每个元素与第一个元素交换
            std::swap(list[i], list[low]);
            perm(list, low + 1, high); //交换后,得到子序列,用函数perm得到子序列的全排列
            std::swap(list[i], list[low]); //最后,将元素交换回来,复原,然后交换另一个元素
        }
    }
}

void printPermutation(int* input, int left, int right, std::vector<int>& output) {
    if (left == right) {
        for (int i = 0; i <= left; ++i) {
            std::cout << input[i] << " ";
            output.push_back(input[i]);
        }
        std::cout << std::endl;
        output.push_back(-999); // delimiter
        return;
    }
    for (int i = left; i <= right; ++i) {
        std::swap(input[left], input[i]);
        printPermutation(input, left + 1, right, output);
        std::swap(input[left], input[i]);
    }
}

std::vector<std::vector<int>> permutation(int* input, int left, int right) {
    std::vector<std::vector<int>> res;
    std::vector<int> oneLine;
    printPermutation(input, left, right, oneLine);
    std::vector<int> onePiece;
    for (int i = 0; i < oneLine.size(); ++i) {
        if (oneLine[i] == -999) {
            res.push_back(std::move(onePiece));
            onePiece.clear();
        } else {
            onePiece.push_back(oneLine[i]);
        }
    }
    return res;
}

int main(int argc, char** argv) {
    int test[4] = { 1, 2, 3, 4 };
    auto arrays = permutation(test, 0, 3);
    for (int i = 0; i < arrays.size(); ++i) {
        for (int j = 0; j < arrays[i].size(); ++j) {
            std::cout << arrays[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "======" << std::endl;
    perm(test, 0, 3);
    return 0;
}