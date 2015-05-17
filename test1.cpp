#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <cmath>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <functional>
#include <sstream>

using namespace std;

#define MAX_NODE 512
#define MAX_NUM  1000
#define MULTIPLE 3
#define HOPS     2
#define MAX_ENTRY 2000
#define MEAN     10
#define DURATIONTIME 10

struct traffic_node {
    int label;
    map<int, traffic_node*> traffic_output;
};

struct flows {
    int src;
    vector<int> dst;
    int endtime;
};

typedef vector <int> destination;
typedef map<int, destination> traffic;
typedef map<int, destination>::iterator traffic_iter;

string itos(int value) {
    stringstream stream;
    stream<<value;
    return stream.str();
}


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
            if ( iter == temp2[i].end() )  break;
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

int main(int argc, char **argv) {
    Switches temp(10);
    temp.Insert(0,{0,1,2,3,4,5,6,7,8,9}, 1);
    temp.Insert(0,{0,1,2,3,4,5,6,7,8,9}, 2);
    temp.Insert(0,{0,1,2,3,4,5,6,7,8,9}, 3);
    
    vector<int> temp1 = temp.Usage();
    cout << "Time 0, total usage is " << temp.Total_Usage() << "\t" << endl << "All nodes are\t";
    for ( int x : temp1 )  cout << x << "\t";
    cout << endl;

    temp.Destroy (1);
    //temp.Destroy (2);
    temp.Destroy (3);
    temp1 = temp.Usage();
    cout << "Time 3, total usage is " << temp.Total_Usage() << "\t" << endl << "All nodes are\t";
    for ( int x : temp1 )  cout << x << "\t";
    cout << endl;

    return 0;
}


