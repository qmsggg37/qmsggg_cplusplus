//
//  main.cpp
//  dynamic_cast_test
//
//  Created by qmsggg on 2018/12/11.
//  Copyright © 2018年 qmsggg. All rights reserved.
//

#include <iostream>

class Base
{
public:
    Base() {}
    virtual ~Base(){}
};

class Derived : public Base
{
public:
    Derived() {}
    virtual ~Derived() {}
};

int main(int argc, const char * argv[]) {
    Base* b;
    Derived* d = new Derived();
    b = d;
    d = dynamic_cast<Derived *>(b);
    
    std::cout << "end" << std::endl;
    return 0;
}
