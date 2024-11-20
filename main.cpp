#include <iostream>
#include <vector>

using namespace std;

int main() {
    vector<string> result;
    string str;
    while (getline(cin, str)) {
        if (str.empty()) {
            break;
        }
        result.push_back(str);
    }

    for (auto i : result) {
        cout << i << endl;
    }


    return 0;
}

