#include <iostream>
#include <vector>

template <typename T>
class PriorityQueue {
public:
    // 获得最大值
    T& top() {
        return m_heap[0];
    }

    // 插入任意值：把新的数字放在最后一位，然后上浮
    void push(T&& k) {
        m_heap.push_back(std::forward<T>(k));
        swim(m_heap.size() - 1);
    }

    // 删除最大值：把最后一个数字挪到开头，然后下沉
    void pop() {
        m_heap[0] = m_heap.back();
        m_heap.pop_back();
        sink(0);
    }
    bool empty() const { return m_heap.empty(); }

private:
    // 上浮
    void swim(int pos) {
        while (pos > 0 && m_heap[(pos - 1) / 2] < m_heap[pos]) {
                std::swap(m_heap[(pos - 1) / 2], m_heap[pos]);
                pos = (pos - 1) / 2;
            }
    }
    // 下沉
    void sink(int pos) {
        while (2 * pos + 1 < m_heap.size()) {
            int i = 2 * pos + 1;
            if (i < m_heap.size() - 1 && m_heap[i] < m_heap[i + 1])
                ++i;
            if (m_heap[pos] >= m_heap[i])
                break;
            std::swap(m_heap[pos], m_heap[i]);
            pos = i;
        }
    }

private:
    std::vector<T> m_heap;
};

int main(int argc, char** argv) {
    PriorityQueue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);
    while (!queue.empty()) {
        std::cout << queue.top() << " ";
        queue.pop();
    }
    std::cout << std::endl;
    queue.push(2);
    queue.push(3);
    queue.push(4);
    std::cout << queue.top() << " ";
    queue.pop();
    queue.push(5);
    queue.push(1);
    while (!queue.empty()) {
        std::cout << queue.top() << " ";
        queue.pop();
    }
    return 0;
}