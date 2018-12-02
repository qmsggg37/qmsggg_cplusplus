#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <regex>

using namespace std;

int main(int argc, char** argv) 
{
    string s("The zip code of Braunschweig in Germany in 38100");
    cout << "original:" << s << endl;
    transform(s.cbegin(), s.cend(), s.begin(), [](char c) { return tolower(c); });
    cout << "lowered: " << s << endl;

    transform(s.cbegin(), s.cend(), s.begin(), [](char c) { return toupper(c); });
    cout << "uppered: " << s << endl;

    string g("Germany");
    string::const_iterator pos;
    pos = search(s.cbegin(), s.cend(), g.cbegin(), g.cend(), [] (char c1, char c2) { return toupper(c1) == toupper(c2); });
    if (pos != s.cend()) {
        cout << "substring \"" << g << "\" found at index " << pos - s.cbegin() << endl;
    }
    string gg(g.cbegin(), g.cend());
    cout << gg << endl;

    regex pat ("Germany", regex_constants::icase);
    smatch m;
    if (regex_search(s, m, pat)) {
        cout << "substring \"Germany\" found at index " << m.position() << endl;
    }
    return 0;
}
