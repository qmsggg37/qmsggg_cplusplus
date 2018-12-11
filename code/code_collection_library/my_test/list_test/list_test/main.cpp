//
//  main.cpp
//  list_test
//
//  Created by qmsggg on 2018/12/12.
//  Copyright © 2018年 qmsggg. All rights reserved.
//

#include <iostream>
#include <list>
#include <vector>
using namespace std;

using test_list = std::list<int>;
using test_vector = std::vector<int>;

int main(int argc, const char * argv[]) {
    // insert code here...
    test_list t;
    test_vector v;
    for (int i = 0; i < 10; i++) {
        t.push_back(i);
        v.push_back(i);
    }
// 正确
//    for (test_list::iterator it = t.begin(); it != t.end();) {
//        t.erase(it);
//        it++;
//    }
// 错误
//    auto iter = t.begin();
//    while (iter != t.end()) {
//        iter = t.erase(iter);
//    }
// 正确
//    auto iter = v.begin();
//    while (iter != v.end()) {
//        v.erase(iter++);
//    }
        auto iter = v.begin();
        while (iter != v.end()) {
            iter = v.erase(iter);
        }
    
    return 0;
}
