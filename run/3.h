#include <map>
#include <vector>

using namespace std;

typedef vector <int> destination;
typedef map<int, destination> traffic;
typedef map<int, destination>::iterator traffic_iter;

int GenerateTrafficPatter(traffic &MultiTraffic) {
    MultiTraffic.insert({1,2});
    MultiTraffic.insert({2,3});
    MultiTraffic.insert({3,4});
    MultiTraffic.insert({4,5});
}
