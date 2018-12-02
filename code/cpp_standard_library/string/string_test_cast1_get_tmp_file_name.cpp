/*
  input: string1 prog.dat mydir hello. oops.tmp end.dat
  output: 
        prog.bat =>pro.tmp
        mydir => mydir.tmp
        hello. => hello.tmp
        oops.tmp => oops.xxx
        end.dat => end.tmp
 */

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    string file_name;
    string base_name;
    string ext_name;
    string tmp_name;
    const string suffix("tmp");

    for (int i = 1; i < argc; ++i) {
        file_name = argv[i];

        string::size_type idx = file_name.find('.');
        if (idx == string::npos) {
            tmp_name = file_name + '.' + suffix;
        } else {
            base_name = file_name.substr(0, idx);
            ext_name = file_name.substr(idx + 1);
            if (ext_name.empty()) {
                tmp_name = file_name;
                tmp_name += suffix;
            } else if (ext_name == suffix) {
                tmp_name = file_name;
                tmp_name.replace(idx + 1, ext_name.size(), "xxx");
            } else {
                tmp_name = file_name;
                tmp_name.replace(idx + 1, string::npos, suffix);
            }
        }
        
        cout << file_name << " => " << tmp_name << endl;
    }

    return 0;
}
