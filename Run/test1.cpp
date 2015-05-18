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

#define MAX_NUM  1000
#define MULTIPLE 3
#define HOPS     3
#define MAX_ENTRY 2000
#define DURATION 10

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

/*------------------------------------------------------------------------

	This part calculate x to the power of p

	Input	:	x, p

	Output	:	myPow(x,p)

------------------------------------------------------------------------*/

int myPow(int x, int p) {
    if (p == 0) return 1;
    if (p == 1) return x;
    return x * myPow(x, p-1);
}

double path_cost2(vector<int> const &path, int positions, vector <int> const &usage) {

    double costx=0.0;
    int length = path.size();
    for (int i = 0; i<length; ++i ) {
        int test = positions & 1;
        positions = positions >> 1;
        if ( ! (test) )
            continue;

        int node_usage = usage[ path[i] ];

        if(node_usage<333*2) {
            costx+=0.01;
        } else if(node_usage<666*2) {
            costx+=0.03;
        } else if(node_usage<900*2) {
            costx+=0.1;
        } else if(node_usage<1000*2) {
            costx+=0.7;
        } else {
            return 999999.99;
        }

    }
    return costx;
}

//-------------------------------------------------------------------------------
vector <int> Findpath(vector <int> const & path, int const schematic, vector <int> &usage) {
    vector <int> result;
    vector <int> temp_usage = usage;
    if( schematic == 0 ) { //Maxhop
        for( unsigned i = 0; i < path.size(); ++i) {
            if( i % HOPS == 0 || i == path.size() - 1 ) {
                if( usage [ path[i] ] >= MAX_ENTRY ) {
                    result.clear();
                    return result;
                }
                result.push_back( path.at(i) );
                ++temp_usage[ path[i] ];
            }
        }
    }
    if( schematic == 1 ) {
        double cost = 999999.99, temp_cost=999999.99;
        int positions = 0;
        int length = path.size();
        int range = myPow( 2, length );
        for( int i=myPow( 2, length-1 ); i< range; ++i ) {
            cout << "Finding " << i <<endl;
            if( ! (i&1) ) {
                cout << i << " failed" <<endl;
                continue;
            }
            int temp_number = i;
            int count = 0;
            while(temp_number && count < HOPS) {
                if ( temp_number & 1 ) count = 0;
                else ++count;
                temp_number = temp_number >> 1;
            }
            if( count >= HOPS ) {
                cout << i << " failed" <<endl;
                continue;
            }
            temp_cost=path_cost2(path, i, usage);
            if( cost>temp_cost ) {
                cost=temp_cost;
                cout << "Cost is " << cost << endl;
                positions = i;
            }
        }
        if (cost == 999999.99) {
            cout << " failed" <<endl;
            return result;
        }
        cout << "Position is " << positions << endl;
        for( int i=1; i <= path.size(); ++i) {
            if(positions & 1) result.push_back( path.at(i-1) );
            positions = positions >> 1;
        }
    }//Jumpflow
    usage = temp_usage;
    return result;
}

//-------------------------------------------------------------------------------
vector <int> Assign_Path(vector <int> &destination, traffic_node* root, int schematic, vector <int> &usage) {

    vector <int> result;
    vector <int> path;
    map < traffic_node*, vector <int> > flow_setup;
    bool loop = true;
    do {
        loop = true;
        flow_setup.clear();
        vector <int> temp_dest = destination;
        destination.clear();
        for(int temp : temp_dest) {
            auto temp_node = (root->traffic_output).find(temp);
            if ( temp_node == ( root->traffic_output ).end() ) { //if the traffic stop here, skip it
                loop = false;
                continue;
            }
            destination.push_back(temp);
            //Here we insert the key=node pointer, value=next node's label to map
            auto iter = flow_setup.find(temp_node->second);
            if ( iter == flow_setup.end() ) {
                vector <int> temp1 { temp };
                flow_setup.insert({ temp_node->second, temp1 });
            } else {
                iter->second.push_back( temp );
            }
        }
        path.push_back( root->label );
    } while( flow_setup.size() == 1 && destination.size() > 0 && loop ); //the loop is used to see whether there is any node stop in the middle
    //After quit it, we have 0 or multiple traffic nodes

    //find path by vector path, save it into result
    vector <int> temp = Findpath(path, schematic, usage);
    if( temp.empty() ) {
        result.clear();
        return result;
    } else {
        result.insert( result.begin(), temp.begin(), temp.end() );
    }

    if( flow_setup.size() >1 ) { //Recursively run it for all nodes
        for( auto iter_flow : flow_setup ) {
            vector < int > temp = Assign_Path( iter_flow.second, iter_flow.first, schematic, usage );
            if ( temp.empty() ) {
                result.clear();
                return result;
            }
            result.insert( result.end(), temp.begin(), temp.end() );
        }
    }

    return result;
}

//-------------------------------------------------------------------------------
vector<int> Handle_Flow (flows &flow, map<int, traffic_node*> &traffic_tree, int schematic, vector <int> &usage) {
//We need to add endtime to all context switches later
    map<int, traffic_node*>::iterator temp = traffic_tree.find(flow.src);
    if( temp == traffic_tree.end() ) {
        vector<int> temp;
        return temp;
    } else {
        return Assign_Path(flow.dst,temp->second, schematic, usage);
    }
}

int main(int argc, char **argv) {
    vector <int> usage {2, 2000, 0};
    int positions = 7;
    vector <int> path {0,1,2};
    vector <int> result = Findpath(path, 1, usage);
    for ( int temp : result ) {
        cout << temp << endl;
    }
    return 0;
}