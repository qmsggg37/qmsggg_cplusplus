#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

int main(int argc, char const *argv[])
{
    const string hello("Hello, how are you?");
    
    string s(hello.cbegin(), hello.cend());
    // cout << s << endl;
    for (char c : s) {
        cout << c;
    }
    cout << endl;
    reverse (s.begin(), s.end());
    cout << "reverse:       " << s << endl;
    sort(s.begin(), s.end());
    cout << "ordered:       " << s << endl;
    s.erase(unique(s.begin(), s.end()), s.end());
    return 0;
}
