//
//  main.cpp
//  cpp_kaoshi_test
//
//  Created by qmsggg on 2018/12/11.
//  Copyright © 2018年 qmsggg. All rights reserved.
//

#include <iostream>

class Base {
public:
    Base()
    {}
};

class Sub {
public:
    Sub()
    {}
};

int main(int argc, const char * argv[]) {
    // insert code here...
    Sub* sub = reinterpret_cast<Sub *>(new Base());
    const int a = 9;
    const int* cc = &a;
    int* b = const_cast<int *>(cc);
    *b = 4;
    std::cout << a << " " << *cc << " " << *b << std::endl;
    std::cout << &a << " " << cc << " " << b << std::endl;
    return 0;
}
