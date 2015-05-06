#include <map>
#include <stdio.h>
#include <iostream>

using namespace std;

typedef map<int, int> traffic;
typedef map<int, int>::iterator traffic_iter;

int GenerateTrafficPatter(traffic &MultiTraffic) {
    MultiTraffic.insert({1,2});
    MultiTraffic.insert({2,3});
    MultiTraffic.insert({3,4});
    MultiTraffic.insert({4,5});
    //for (traffic_iter iter = MultiTraffic.begin(); iter != MultiTraffic.end(); iter++)
    //    cout<<iter->second<<endl;

}

int main(int argc, char **argv) {

    traffic MultiTraffic;
    GenerateTrafficPatter(MultiTraffic);
    for (traffic_iter iter = MultiTraffic.begin(); iter != MultiTraffic.end(); iter++)
        cout<<iter->second<<endl;

    return 0;
}
