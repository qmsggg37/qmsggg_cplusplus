//
//  main.cpp
//  test_strlen
//
//  Created by qmsggg on 2018/12/2.
//  Copyright © 2018年 qmsggg. All rights reserved.
//

#include <iostream>

using namespace std;

inline unsigned int qmsgg_strlen(const char *str)
{
    const char * s;
    for (s = str; *s; ++s);
    return static_cast<unsigned int>(s - str);
}

int main(int argc, const char * argv[]) {
    // insert code here...
    auto s = static_cast<const char *>("Hello, World!");
    cout << qmsgg_strlen(s);
    return 0;
}
