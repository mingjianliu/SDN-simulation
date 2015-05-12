#include <map>
#include <vector>

using namespace std;

typedef vector <int> destination;
typedef map<int, destination> traffic;
typedef map<int, destination>::iterator traffic_iter;

void GenerateMulti(traffic &MultiTraffic, vector<vector<int>> &demand) {
    for(vector<int> TempTraffic: demand) {
        traffic_iter temp = MultiTraffic.find(TempTraffic[0]);
        if(temp==MultiTraffic.end()) {
            vector<int> tempVector;
            tempVector.push_back(TempTraffic[1]);
            MultiTraffic.insert({TempTraffic[0],tempVector});
        }
        else {
            temp->second.push_back(TempTraffic[1]);
        }
    }
    return;
}
