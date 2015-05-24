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
#define HOPS    3 
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
vector <int> Assign_Path(vector <int> &destination, traffic_node* root, int schematic, vector <int> &usage, vector <vector <vector <int>>> ShortestPath) {
    int src = root -> label;
    vector <int> result;
    vector <int> path;
    map < traffic_node*, vector <int> > flow_setup;
    bool loop = true;
    traffic_node* cur_src = root;
    do {

        cout << "------------------ A new iteration--------------- " << endl;
        src = cur_src->label;
        loop = true;
        flow_setup.clear();
        vector <int> temp_dest = destination;
        destination.clear();
        path.push_back( cur_src->label );

        for(int temp : temp_dest) {
            cout << endl<< "Handling node " << temp << endl;
            auto temp_node = (cur_src->traffic_output).find(ShortestPath[0][src][temp]);
            cout << "Find from src " << src << " to dest " << temp << ", the next hop is " << ShortestPath[0][src][temp] << endl;
            if ( temp == cur_src->label ) { //if the traffic stop here, skip it
                loop = false;
                cout << "The destination is just here: node " << temp << endl;
                //if there is only one destination, we need to push temp into 
                continue;
            }

            destination.push_back ( temp );
            //Here we insert the key=node pointer, value=next node's label to map
            auto iter = flow_setup.find(temp_node->second);
            if ( iter == flow_setup.end() ) {
                vector <int> temp1 { temp };
                flow_setup.insert({ temp_node->second, temp1 });
                cout << "Inserting " << temp_node->second->label << "to " << temp << " into the map 1" << endl;
            } else {
                iter->second.push_back( temp );
                cout << "Inserting " << temp << " into the map 2" << endl;
            }

        }
        
        cout << "test"<<endl;
        if(flow_setup.size() == 0) break;
        cur_src = (flow_setup.begin())->first; 

        cout << "Now the src is changed to " << src << endl;
        cout << "Flow_setup size is " << flow_setup.size() << endl;
        cout << "Destination size is " << destination.size() << endl;


    } while( flow_setup.size() == 1 && destination.size() > 0 && loop ); //the loop is used to see whether there is any node stop in the middle
    //After quit it, we have 0 or multiple traffic nodes

    //find path by vector path, save it into result
    cout << "Finished the loop. Now find the path: \t";
    for (int i : path)  cout << i << "\t"; 
    cout << endl;
    vector <int> temp = Findpath(path, schematic, usage);
    if( temp.empty() ) {
        result.clear();
        cout << "Find path failed" << endl;
        return result;
    } else {
        result.insert( result.begin(), temp.begin(), temp.end() );
    }
    //for (int x : result)  cout << x <<endl;

    if( flow_setup.size() >1 || !loop ) { //Recursively run it for all nodes
        for( auto iter_flow : flow_setup ) {
            vector < int > temp = Assign_Path( iter_flow.second, iter_flow.first, schematic, usage, ShortestPath );
            if ( temp.empty() ) {
                result.clear();
		cout << "Find other path failed" << endl;
                return result;
            }
            for( int x : temp) cout << "Other branch " << x;
            cout << endl;
            result.insert( result.begin(), temp.begin(), temp.end() );
        }
    }
    // for (int x : result)  cout << x <<endl;
    return result;
}

//-------------------------------------------------------------------------------
vector<int> Handle_Flow (flows &flow, map<int, traffic_node*> &traffic_tree, int schematic, vector <int> &usage, vector <vector <vector <int>>> ShortestPath) {
//We need to add endtime to all context switches later
    map<int, traffic_node*>::iterator temp = traffic_tree.find(flow.src);
    if( temp == traffic_tree.end() ) {
        vector<int> temp;
        return temp;
    } else {
        return Assign_Path(flow.dst,temp->second, schematic, usage, ShortestPath);
    }
}

int main(int argc, char **argv) {
    map<int, traffic_node*> tree;
    vector <vector <int>> ShortestPath(3, vector< int >(3, 0));
    ShortestPath[0][2] = 1;
    ShortestPath[1][2] = 2;
    ShortestPath[2][2] = 2;
    ShortestPath[0][1] = 1;
    vector <vector < vector <int>>> xxx;
    xxx.push_back(ShortestPath);
    traffic_node* temp = new traffic_node();
    temp->label = 0;    
    tree.insert({0, temp});

    traffic_node* temp1 = new traffic_node();
    temp1->label = 1;
    temp->traffic_output.insert({1, temp1});
    
    traffic_node* temp2 = new traffic_node();
    temp2->label = 2;
    temp1->traffic_output.insert({2, temp2});

    vector <int> usage {2, 2, 0};
    //int positions = 7;
    vector <int> path {0,1,2};
    //vector <int> result = Findpath(path, 1, usage);
    vector <int> destination{2, 1};
    vector <int> result = Assign_Path(destination, temp, 0, usage, xxx);
    cout << "Final result length is " << result.size() << endl;
    for ( int x : result ) {
        cout << x << "\t";
    }
    cout << endl;
    return 0;
}

/*
        //cout << "Find all maps here "<< endl;
        //cout << "Node is " << root->label << endl; 
        //traffic_node* root1;
        //for( auto iter : root->traffic_output ){ cout << iter.first << "\t"  << endl;
        //cout << "--------------------------"<< endl;
        //root1 = iter.second;}

        //cout << "Find all maps here "<< endl;
        //cout << "Node is " << root1->label << endl; 
        //for( auto iter : root1->traffic_output ) cout << iter.first << "\t"  << endl;
        //cout << "--------------------------"<< endl;
*/
