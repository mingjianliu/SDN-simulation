#include <map>
#include <vector>

/*
   This part contains insert, delete, usage and total_usage methods
*/
class Switches {
    unsigned int total;
    vector <int> count;
public:
    Switches(unsigned int size) : total(0), count( size, 0 ), temp2( new map < int, int>[size] ), switch_size(size) {} ;
    unsigned int Total_Usage() {
        return total;
    }
    vector <int> Usage() {
        return count;
    }
    void Destroy( int time ) {
        unsigned int number = 0;
        for(int i = 0; i < switch_size; ++i) {
            auto iter = temp2[i].find(time);
            if ( iter == temp2[i].end() )  continue;
            number += iter->second;
            count[i] -= iter->second;
            temp2[i].erase(iter);
        }
        total -= number;
    }
    void Insert( int time, vector <int> nodes, int endtime ) {
        total += nodes.size();
        for(int node : nodes) {
            count[node]++;
            auto iter = temp2[node].find(endtime);
            if( iter == temp2[node].end() ) {
                temp2[node].insert({endtime, 1});
            } else {
                iter->second++;
            }
        }
        return;
    }
private:
    unsigned int switch_size;
    map < int, int >* temp2; //key = time, value = entry number
};
