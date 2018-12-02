#include <iostream>
#include <string>

using namespace std;

int main(int argc, char ** argv)
{
    const string delims(" \t,.;");
    string line;

    while (getline(cin, line)) {
        string::size_type beg_idx, end_idx;
        beg_idx = line.find_first_not_of(delims);

        while (beg_idx != string::npos) {
            end_idx = line.find_first_of(delims, beg_idx);
            if (end_idx == string::npos) {
                end_idx = line.length();
            }

            for (int i = end_idx - 1; i >= static_cast<int>(beg_idx); --i) {
                cout << line.at(i);
            }
            cout << ' ';
            beg_idx = line.find_first_not_of(delims, end_idx);
        }
        cout << endl;
    }
}