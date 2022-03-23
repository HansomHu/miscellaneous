#include <iostream>
#include <string>
#include <stack>
using namespace std;

// 获取运算符的优先级
int prior(char c) {
    switch (c) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        default:
            return 0;
    }
}

// 判断是否是运算符
bool isOperator(char c) {
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
            return true;
        default:
            return false;
    }
}

// 中缀转后缀，从左到右遍历中缀表达式字符串，用一个运算符栈来临时存储遇到的运算符，并且，栈顶运算符优先级
// 高于栈低，不允许存在相同优先级的运算符
// 1. 遇到操作数直接输出
// 2. 遇到运算符：
//    2.1 如果是(，则无条件直接入栈
//    2.2 如果是)，则要求把运算符出栈，放到输出队列，直到遇到(，(应当被丢弃
//    2.3 如果是其他运算符：
//        2.3.1 如果该运算符优先级高于栈顶运算符，则直接入栈
//        2.3.2
//        如果该运算符优先级小于等于栈顶运算福，则将栈顶出栈放到输出队列，直到栈顶运算符优先级低于该运算符，再将运算符入栈
//    2.4 重复上述过程，直到遍历完
string getPostfix(const string& expr) {
    string output; // 输出
    stack<char> op_stack; // 操作符栈
    for (int i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (isOperator(c)) {
            while (!op_stack.empty() && isOperator(op_stack.top()) && prior(op_stack.top()) >= prior(c)) {
                output.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.push(c);
        } else if (c == '(') {
            op_stack.push(c);
        } else if (c == ')') {
            while (op_stack.top() != '(') {
                output.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.pop();
        } else {
            output.push_back(c);
        }
    }
    while (!op_stack.empty()) {
        output.push_back(op_stack.top());
        op_stack.pop();
    }
    return output;
}

// 从栈中连续弹出两个操作数
void popTwoNumbers(stack<int>& s, int& first, int& second) {
    first = s.top();
    s.pop();
    second = s.top();
    s.pop();
}

// 计算后缀表达式的值
int expCalculate(const string& postfix) {
    int first, second;
    stack<int> s;
    for (int i = 0; i < postfix.size(); ++i) {
        char c = postfix[i];
        switch (c) {
            case '+':
                popTwoNumbers(s, first, second);
                s.push(second + first);
                break;
            case '-':
                popTwoNumbers(s, first, second);
                s.push(second - first);
                break;
            case '*':
                popTwoNumbers(s, first, second);
                s.push(second * first);
                break;
            case '/':
                popTwoNumbers(s, first, second);
                s.push(second / first);
                break;
            default:
                s.push(c - '0');
                break;
        }
    }
    int result = s.top();
    s.pop();
    return result;
}

int main() {
    string expr = "a+b*c+(d*e+f)*g";
    string postfix = getPostfix(expr);
    cout << expr << endl << postfix << endl;

    string expr1 = "5+2*(6-3)-4/2";
    string postfix1 = getPostfix(expr1);
    cout << expr1 << " = " << expCalculate(postfix1) << endl;
    return 0;
}