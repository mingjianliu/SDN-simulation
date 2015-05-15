#include <stdio.h>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

typedef vector <int> destination;
typedef map<int, destination> traffic;
typedef map<int, destination>::iterator traffic_iter;

void test(vector< vector< int >> &temp) {
    temp.push_back({1, 2});
    temp.push_back({1, 3});
    temp.push_back({2, 2});
    temp.push_back({2, 3});
}

int main(int argc, char **argv) {

    vector<vector< vector< int >>> temp(1, vector< vector<int>>(numberofnodes, vector< int >(numberofnodes, 0)));
    for( vector<vector <int>> z : temp){
        cout << endl;
        for( vector<int> x : z){
            for( int y : x) cout << y;
            cout <<endl;
        }
    }
    return 0;
}

