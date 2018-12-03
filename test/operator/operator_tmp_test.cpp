/*
 * https://blog.csdn.net/qq_37058442/article/details/78118644
 * int operator *() 和operator int*()的区别 
 */

#include <iostream>

using namespace std;

class Base
{
    int mem;
public:
    Base() : mem(0) {}
    int operator*() { return 1; } // 重载 * 
    operator int*() { return 0; } // 类型转换运算符重载
};

int main()
{
    Base b;
    cout << "*b: " << *b << endl; // output: 1;
    cout << "b: " << b << endl;   // output: 0x0;
    return 0;
}