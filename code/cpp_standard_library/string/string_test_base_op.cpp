#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv) 
{
    // string s('x');  error
    // string s1(1, 'x');
    // string s2({'x'});
    string s("12345");
    const char * cstr = s.c_str();
    s += "ext";
    cout << cstr << endl;
    cout << atoi(s.c_str()) << endl;
    char buffer[100];
    s.copy(buffer, 100);
    cout << buffer << endl;
    s.copy(buffer, 100, 2);
    cout << buffer << endl;

    cout << "size: " << s.size() << "length: " << s.length() << "empty: " << s.empty() << "max_size: " << s.max_size() << "capacity: " << s.capacity() << endl;
    s.reserve(50);
    cout << "size: " << s.size() << "length: " << s.length() << "empty: " << s.empty() << "max_size: " << s.max_size() << "capacity: " << s.capacity() << endl;

    const string aString("age");
    string p("p");
    p.insert(1, aString);
    cout << p << endl;
    p.insert(1, "ersifl");
    cout << p << endl;
    // p.insert(1, ' ');
    cout << p << endl;
    p.insert(1, " ");
    cout << p << endl;
    p.insert((string::size_type)0, 2, ' ');
    cout << p << endl;

    string s_replace = "il8n";
    s_replace.replace(1, 2, "nternat");
    cout << s_replace << endl;
    s_replace.erase(4);
    cout << s_replace << endl;
    s_replace.erase(2, 3);
    cout << s_replace << endl;
    s_replace.pop_back();
    cout << s_replace << endl;
    s_replace.reserve(20);
    // s_replace.replace(0, string::npos, 'ee');
    s_replace.replace(0, string::npos, "ex");
    cout << s_replace << endl;

    string s_replace_move = move(s_replace);
    cout << s_replace_move << endl;
    s_replace.replace(0, string::npos, "ee");
    cout << s_replace_move << endl;

    // string op;
    // cin >> op;
    // cout << op << endl;
    const string quit("quit");
    string str_cin;
    while (getline(cin, str_cin, ':')) {
        if (str_cin == quit) {
            break;
        }
        cout << str_cin << endl;
    }
    str_cin.clear();
    return 0;
}