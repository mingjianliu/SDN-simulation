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
            for(auto iter : temp2[i]) {
                if (iter.first > time) {
                    break;
                }
                number += iter.second;
                count[i] -= iter.second;
                temp2[i].erase (iter.first);
            }
        }
        total -= number;
    }
    void Insert( int time, vector <int> nodes ) {
        total += nodes.size();
        for(int node : nodes) {
            count[node]++;
            auto iter = temp2[node].find(time);
            if( iter == temp2[node].end() ) {
                temp2[node].insert({time, 1});
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

traffic_node* init_treenode(int number) {
    traffic_node *ret = (traffic_node *)malloc(sizeof(traffic_node));
    ret->label = number;
    return ret;
}

/* for source random number generator*/
default_random_engine generator(0);
uniform_int_distribution<int> uniform_dis(0,DURATION);
uniform_int_distribution<int> generation(1,40);
auto uni_generation = bind(generation,generator);
auto uni_dis = bind(uniform_dis,generator);

/* for destination random number generator*/
uniform_int_distribution<int> dstGeneration(1,50);
uniform_int_distribution<int> durationGeneration(1,DURATION);
auto uniRandn = bind(dstGeneration, generator);
auto durationRandn = bind(durationGeneration, generator);

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
/*------------------------------------------------------------------------

	This part record the node to node cost, save it in Pathcost

	Input	:	topology.txt, numberofnodes, Pathcost[policy][][]

	Output	:	0 or -1

------------------------------------------------------------------------*/
int Pathcost_Input(const char* filename,const int policy, const int number, vector <vector <vector <int>>> &Pathcost) {

    FILE *file;
    int h=number;/*read the first line node and edge numbers*/
    /*read and skip the second line*/
    file=fopen(filename,"r");

    if(file==NULL) {
        cout<<"File doesn't exist."<<endl;
        fclose(file);
        return -1;
    }

    else {
        /*initialize traffic matrix*/

        for(int i=0; i<h; i++) {
            for(int j=0; j<h; j++) {
                if(i==j) {
                    Pathcost[policy][i][j]=0;
                }
                else {
                    Pathcost[policy][i][j]=99999;
                }
            }
        }
        /*input path cost raw data*/

        char ch=getc(file);
        int x,y,n;

        for(int i=0; ch!=EOF;)
        {
            if(ch =='\t') {
                i++;
                ch=getc(file);
            }
            else if(ch =='\n') {
                i=0;
                ch=getc(file);
            }
            else if(i==1) {
                char str[MAX_NUM]="";
                /*read the string*/
                n=0;
                while(ch!='\t') {
                    str[n]=ch;
                    ch=getc(file);
                    n++;
                }
                x=atoi(str);

            }
            else if(i==2) {
                char str[MAX_NUM]="";
                /*read the string*/
                n=0;
                while(ch!='\t') {
                    str[n]=ch;
                    ch=getc(file);
                    n++;
                }
                y=atoi(str);
            }

            else if(i==3) {
                char str[MAX_NUM]="";
                /*read the string*/
                n=0;
                while(ch!='\t') {
                    str[n]=ch;
                    ch=getc(file);
                    n++;
                }
                Pathcost[policy][x][y]=atoi(str);
            }
            else {
                ch=getc(file);
            }
        }
    }
    fclose(file);
    return 0;
}

/*------------------------------------------------------------------------

	This part calculate the shortest path in the node matrix and calculate the PathLength and also Pathcost

	Input	:	policy, numberofnodes, ShortestPath, Pathcost, PathLength

	Output	:

------------------------------------------------------------------------*/
void calculate_path(int policy, int number, vector <vector <vector <int>>> &Pathcost, vector <vector <vector <int>>> &PathLength , vector <vector <vector <int>>> &ShortestPath) {
    int tmp[MAX_NODE][MAX_NODE];
    int tmp2[MAX_NODE][MAX_NODE];
    for (int i=0; i<number; i++) {
        for (int j=0; j<number; j++) {
            if(i==j) {
                ShortestPath[policy][i][j]=-1;
            }
            else    {
                ShortestPath[policy][i][j]=i;
            }
        }
    }

    for (int i=0; i<number; i++) {
        for (int j=0; j<number; j++) {
            if(i==j) {
                PathLength[policy][i][j]=0;
            }
            else    {
                PathLength[policy][i][j]=1;
            }
        }
    }

    for (int k=0; k<number; k++) {
        for (int i=0; i<number; i++) {
            for (int j=0; j<number; j++) {
                if(Pathcost[policy][i][j]>Pathcost[policy][i][k]+Pathcost[policy][k][j]) {
                    /*path length*/tmp2[i][j]=PathLength[policy][i][k]+PathLength[policy][k][j];
                    /*path cost  */Pathcost[policy][i][j]=Pathcost[policy][i][k]+Pathcost[policy][k][j];
                    /*last hop   */tmp[i][j]=ShortestPath[policy][k][j];
                }
                else  {
                    tmp[i][j]=-1;
                    tmp2[i][j]=-1;
                }
            }
        }

        //copy matrix
        for (int i=0; i<number; i++) {
            for (int j=0; j<number; j++) {
                if (tmp[i][j]!=-1) {
                    ShortestPath[policy][i][j]=tmp[i][j];
                }
                if (tmp2[i][j]!=-1) {
                    PathLength[policy][i][j]=tmp2[i][j];
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------

/*------------------------------------------------------------------------
	This part take the input trafficdemand1.txt as input and get the demand[] as E for all edges
	Input	:	trafficdemand1.txt
	Output	:	demand[9609][0:src,1:dst]
------------------------------------------------------------------------*/
int trafficdemand(vector<vector<int>> &demand) {
    FILE *file;

    /*read and skip the second line*/
    file=fopen("trafficdemand1.txt","r");
    if(file==NULL)
    {
        cout<<"File doesn't exist."<<endl;
        fclose(file);
        return -1;
    }
    else
    {
        /*initialize traffic matrix*/
        vector<int> temp;

        /*input path cost raw data*/
        char ch=getc(file);
        int n,i=0;
        for(int count=0; count<9609;)
        {   
            if(ch == EOF) break;
            if(ch =='\t') {
                i++;
                ch=getc(file);
            }
            else if(ch =='\n') {
                i=0;
                ch=getc(file);
            }
            else if(i==1) {
                char str[100]="";
                /*read the string*/
                n=0;
                while(ch!='\t'&&ch!='\n') {
                    str[n]=ch;
                    ch=getc(file);
                    n++;
                }
                temp.push_back(atoi(str));
            }
            else if(i==2) {
                char str[100]="";
                n=0;
                while(ch!='\t'&&ch!='\n') {
                    str[n]=ch;
                    ch=getc(file);
                    n++;
                }
                temp.push_back(atoi(str));
                demand.push_back(temp);
                temp.clear();
                count++;
            }
            else {
                ch=getc(file);
            }
        }
    }

    fclose(file);
    return 0;
}

//-------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------
void GenerateMulti_Tree(traffic &MultiTraffic, map<int, traffic_node*> &traffic_tree, vector <vector <vector <int>>> ShortestPath, int policy) {
    int src;
    for (traffic_iter iter = MultiTraffic.begin(); iter != MultiTraffic.end(); iter++) {

        traffic_node* root = new traffic_node();
        root -> label = iter->first;
        traffic_tree.insert({iter->first, root});
	src = iter->first;

        for(int destination: iter->second) {
            //Use DFS to insert every nodes from root
            traffic_node* node = root;
            //1. Get the shortest path
            vector<int> path;

            while(destination != src ) {
                path.insert(path.begin(), destination);
                destination = ShortestPath[policy][src][destination];
            }

            //2. Using vector path, traverse the tree
            for(int cur : path) {
                auto temp = node->traffic_output.find(cur);
                if( temp == node->traffic_output.end() ) {

		    traffic_node* temp_node = new traffic_node();
		    temp_node -> label = cur;
                    node->traffic_output.insert({cur, temp_node});
                    temp = node->traffic_output.find(cur);
                }
                node = temp->second;
            }
        }
    }
    return;
}

//-------------------------------the new objective function-------------------------------
double path_cost2(vector<int> const &path, int positions, vector <int> const &usage) {

    double costx=0.0;
    int length = path.size();
    for (int i = 0; i<length; ++i ) {
        int test = positions & 1;
        positions = positions >> 1;
        if ( ! (test) )
            continue;

        int node_usage = usage[ path[length - 1 - i] ];

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
vector <int> Findpath(vector <int> const & path, int const schematic, vector <int> const &usage) {
    vector <int> result;
    if( schematic == 0 ) { //Maxhop
        for( unsigned i = 0; i < path.size(); ++i) {
            if( i % HOPS == 0 || i == path.size() - 1 ) {
		    if( usage [ path.at(i) ] == 2000 ) {
			result.clear();
			return result;
		    }
		    result.push_back( path.at(i) );
            }
        }
    }
    if( schematic == 1 ) {
        double cost = 999999.99, temp_cost=999999.99;
        int positions = 1;
        int length = path.size();
        int range = myPow( 2, length );
        for( int i=myPow( 2, length-1 ); i< range; ++i ) {
            if( ! (i%2) ) continue;

            int temp_number = i;
            int count = 0;
            while(temp_number && count < HOPS) {
                if ( temp_number & 1 ) count = 0;
                else ++count;
                temp_number = temp_number >> 1;
            }
            if( count >= HOPS ) continue;

            temp_cost=path_cost2(path, i, usage);
            if( cost>temp_cost ) {
                cost=temp_cost;
                positions = i;
            }
        }
        for( int i=1; i <= path.size(); ++i) {
            if(positions>>i) result.push_back( path.at(i) );
        }
    }//Jumpflow
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

//-------------------------------------------------------------------------------
int poisson_dis(float criteria) {
    float temp = float(uni_generation());
    if (temp/40.0 < criteria) {
        return 1;
    }
    else
        return 0;
}

//-------------------------------------------------------------------------------
vector <vector <flows>> Rand_Generation_Multicast(traffic &MultiTraffic, int flownumber, int numberofnodes, float criteria, float criteria2) {
        vector <vector <flows>> result;
        int flow_num = 0;
        int time = 0;
        while (flow_num < flownumber){
            vector <flows> temp;
            for(auto traffic_iter : MultiTraffic){ 
		    if(poisson_dis(criteria)){
			flows currentFlow;
			int src = traffic_iter.first;
			destination tempDst;
			destination dstPattern = traffic_iter.second;
			for(vector<int>::iterator iter = dstPattern.begin(); iter != dstPattern.end();++iter){
				if(float(uniRandn()/50.0) < criteria2)
					tempDst.push_back(*iter);
			}  
			int tempEndTime = durationRandn();
			currentFlow.src = src;
			currentFlow.dst = tempDst;
			currentFlow.endtime = tempEndTime;
			temp.push_back(currentFlow);
		    }
		    ++flow_num;
		    if(flow_num >= flownumber)  break;
            }
            ++time;
            result.push_back(temp);
        }
        return result;
}

//-------------------------------------------------------------------------------
int main(int argc, char **argv) {
    int number=0;
    int overhead_counter = 0;
    int route_request = 0;
    int flownumber = atoi(argv[1]);	  	//
    int numberofnodes=atoi(argv[2]);    	//how many nodes in the network
    int loop_time=atoi(argv[3]);	  	//the density of flow
    float criteria_generate = 0.1;
    float criteria_dest = 0.5; 
    int schematic=atoi(argv[4]);		//four method choose from input
    traffic MultiTraffic;
    map<int, traffic_node*> traffic_tree;
    vector<vector<int>> demand;
    int refused = 0;
    int refuseTime = 999999;
    int cur_flow = 0;
    string methodName[4];
    methodName[0]="Max_Hop_";
    methodName[1]="Jump_Flow_";
    methodName[2]="Load_Balance_";
    methodName[3]="Alpha_beta_";
    string total_flow = itos(flownumber * loop_time);
    vector<vector< vector< int >>> Pathcost(1, vector< vector<int>>(numberofnodes, vector< int >(numberofnodes, 0)));
    vector<vector< vector< int >>> PathLength(1, vector< vector<int>>(numberofnodes, vector< int >(numberofnodes, 0)));
    vector<vector< vector< int >>> ShortestPath(1, vector< vector<int>>(numberofnodes, vector< int >(numberofnodes, 0)));
    
    //string filename = methodName[schematic] + "_flow_number_" + total_flow + ".txt";
    //ofstream fout(filename.c_str(),ofstream::out | ofstream::app);

//----------------------------build up topology and traffic pattern-----------------------------
    number=Pathcost_Input("topology.txt", 0, numberofnodes, Pathcost);
    if(number==-1) {
        cout<<"Error reading topology"<<endl;
        return -1;
    }

    calculate_path(0, numberofnodes, Pathcost, PathLength, ShortestPath);

    if(trafficdemand(demand)) {
        cout<<"Error reading trafficdemand"<<endl;
        return -1;
    }
    GenerateMulti(MultiTraffic,demand);
    GenerateMulti_Tree(MultiTraffic,traffic_tree, ShortestPath, 0);

    //Handleflow
    vector <vector <flows>> total_flows;
//-----------------------------generate flows---------------------
    for (int time = 0; time < loop_time; time++) {
        vector <vector <flows>> temp = Rand_Generation_Multicast(MultiTraffic, flownumber, numberofnodes, criteria_generate, criteria_dest);
        if ( temp.size() > total_flows.size() ){
            total_flows.resize( temp.size() );
        }
        for ( int i = 0; i < temp.size(); ++i ){
            total_flows[i].insert( total_flows[i].end(), temp[i].begin(), temp[i].end() );
        }
    }
    //Try to print out flows numbers
    int testnumber = 0;
    cout << "Totally " << total_flows.size() << " time slots" <<endl;
    for ( auto each : total_flows) testnumber += each.size();
    cout << "Totally generated " << testnumber << " of flows" <<endl;
 
// Handle all flows and collect the result
    Switches total_nodes (numberofnodes);
    vector <int> usage = total_nodes.Usage();    
    int temp_time = 0;   
    for ( auto each_time : total_flows ) {
        for (auto each_flow : each_time) {
            ++cur_flow;
            vector <int> temp_path = Handle_Flow ( each_flow, traffic_tree, schematic, usage); 
            if ( temp_path.empty() ) {
                ++refused;
                if ( refuseTime == 999999 )  refuseTime = temp_time;
            } else {
                total_nodes.Insert(temp_time, temp_path);
                usage = total_nodes.Usage();
                
            }
        }
        if ( !atoi(argv[5]) ) total_nodes.Destroy(temp_time);
        cout << " Time " << temp_time << " finished" << endl;
        cout << "In time " << temp_time << ":\t" << cur_flow << " flows created\t" << cur_flow - refused << " flows accepted\t" << refused << " flows refused" << endl;
        ++temp_time;
    }
// Output the result
    return 0;
}





