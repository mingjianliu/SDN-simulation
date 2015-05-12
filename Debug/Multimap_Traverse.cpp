#include <stdio.h>
#include <map>
#include <iostream>
#include <vector>
#include <iterator>

using namespace std;

vector<int> try1() {
    vector<int> temp;
    temp.push_back(0);
    temp.push_back(1);
    temp.push_back(2);
    temp.push_back(3);
    temp.push_back(4);
    return temp;
}

int main(int argc, char **argv) {
    multimap<int, int> temp = {
        {1, 2},
        {3, 4},
        {1, 3},
        {3, 5}
    };
    multimap<int, int>::iterator temp1;
    for (temp1 = temp.begin(); temp1 != temp.end(); temp1++) {
        cout << " Key: \t" << temp1->first << "\t" << "Value: "<< "\t" << temp1->second << endl;
    }

    return 0;
}
