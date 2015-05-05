#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <random>
#include <unistd.h>
#include <functional>
#include <cmath>
#include <sstream>
#include <algorithm>

using namespace std;

#define MAX_NODE 512
#define MAX_NUM  1000
#define MULTIPLE 3
#define HOPS     2
#define MAX_ENTRY 2000
#define MEAN     10
#define DURATIONTIME 10

struct traffic_node{
	int label;
	map<int, traffic_node*> traffic_output; 	
};

typedef vector <int> destination;
typedef map<int, destination> traffic;
typedef map<int, destination>::iterator traffic_iter;
typedef map<

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

	This part record the node to node cost, save it in pathcost

	Input	:	topology.txt, numberofnodes, pathcost[policy][][]

	Output	:	0 or -1

------------------------------------------------------------------------*/

int PathCost_Input(const char* filename,const int policy, const int number, int[][][] &pathcost){

    FILE *file;
    int h=number;/*read the first line node and edge numbers*/
    /*read and skip the second line*/
    file=fopen(filename,"r");

    if(file==NULL){
        cout<<"File doesn't exist."<<endl;
        fclose(file);
	return -1;
    }

    else{
        /*initialize traffic matrix*/

        for(int i=0; i<h; i++) {
            for(int j=0; j<h; j++) {
                if(i==j) {
                    pathcost[policy][i][j]=0;
                }
                else {
                    pathcost[policy][i][j]=99999;
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
                pathcost[policy][x][y]=atoi(str);
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

	This part calculate the shortest path in the node matrix and calculate the pathlength and also pathcost

	Input	:	policy, numberofnodes, shortestpath, pathcost, pathlength

	Output	:	

------------------------------------------------------------------------*/
void calculate_path(int policy, int number, int[][][] &PathCost, int[][][] &PathLength , int[][][] &ShortestPath){
    int tmp[MAX_NODE][MAX_NODE];
    int tmp2[MAX_NODE][MAX_NODE];
    for (int i=0; i<number; i++) {
        for (int j=0; j<number; j++) {
            if(i==j) {
                shortestpath[policy][i][j]=-1;
            }
            else    {
                shortestpath[policy][i][j]=i;
            }
        }
    }

    for (int i=0; i<number; i++) {
        for (int j=0; j<number; j++) {
            if(i==j) {
                pathlength[policy][i][j]=0;
            }
            else    {
                pathlength[policy][i][j]=1;
            }
        }
    }

    for (int k=0; k<number; k++) {
        for (int i=0; i<number; i++) {
            for (int j=0; j<number; j++) {
                if(pathcost[policy][i][j]>pathcost[policy][i][k]+pathcost[policy][k][j]) {
                    /*path length*/tmp2[i][j]=pathlength[policy][i][k]+pathlength[policy][k][j];
                    /*path cost  */pathcost[policy][i][j]=pathcost[policy][i][k]+pathcost[policy][k][j];
                    /*last hop   */tmp[i][j]=shortestpath[policy][k][j];
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
                    shortestpath[policy][i][j]=tmp[i][j];
                }
                if (tmp2[i][j]!=-1) {
                    pathlength[policy][i][j]=tmp2[i][j];
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

void GenerateMulti(traffic &MultiTraffic, vector<vector<int>> &demand){
    for(vector<int> TempTraffic: demand){
	traffic_iter temp = MultiTraffic.find(TempTraffic[0]);
	if(temp==MultiTraffic.end()){
	    vector<int> tempVector;
	    tempVector.push_back(TempTraffic[1]);
	    MultiTraffic.insert({TempTraffic[0],tempVector});
	}
	else{
	    temp->second.push_back(TempTraffic[1]);
	}
    } 
}
//-------------------------------------------------------------------------------


void GenerateMulti_Tree(traffic &MultiTraffic, map<int, traffic_node*> &traffic_tree){


}
//-------------------------------------------------------------------------------

int Rand_Generation_Multicast(traffic &MultiTraffic){

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
    int schematic=atoi(argv[4]);		//four method choose from input
    traffic MultiTraffic;
    map<int, traffic_node*> traffic_tree;
    vector<vector<int>> demand;
    int refused = 0;
    int refuseTime = 999999;
    string methodName[4];
    methodName[0]="Max_Hop_";
    methodName[1]="Jump_Flow_";
    methodName[2]="Load_Balance_";
    methodName[3]="Alpha_beta_";
    string total_flow = itos(flownumber * loop_time);
    int PathCost[MULTIPLE][MAX_NODE][MAX_NODE];
    int PathLength[MULTIPLE][MAX_NODE][MAX_NODE];
    int ShortestPath[MULTIPLE][MAX_NODE][MAX_NODE];


    string filename = methodName[schematic] + "_flow_number_" + total_flow + ".txt";
    ofstream fout(filename.c_str(),ofstream::out | ofstream::app);

//----------------------------build up topology-----------------------------
    number=PathCost_Input("topology.txt", 0, numberofnodes, PathCost);
    if(number==-1) {
        cout<<"Error reading topology"<<endl;
        return -1;
    }

    calculate_path(0, numberofnodes, PathCost, PathLength, ShortestPath);

    if(trafficdemand(demand)){
	cout<<"Error reading trafficdemand"<<endl;
	return -1;
    }
		
    GenerateMulti(MultiTraffic,demand); 
    GenerateMulti_Tree(MultiTraffic,traffic_tree);

//-----------------------------generate flows---------------------
    for (int time = 0; time < loop_time; time++) {
        switch(argv[6]) {
        case 'uni':
            Rand_Generation_Unicast(flownumber,numberofnodes,criteria_generate);
        case 'multi':
            Rand_Generation_Multicast(flownumber,numberofnodes,criteria_generate);
        case 'hybird':
            //not setup up yet
        }
    }
//--------------------------processing loop per time step--------------------
    for (iter_multimap iter = flow_setup.begin(); iter != flow_setup.end();) {

        //iter_multimap iter_per_entry = flow_setup.find(iter->first);
        list_size entry_num = flow_setup.count(iter->first);

        //check switch time table
        if (!atoi(argv[5]))  //argv[5] indicates whether the flow ends
        {
            entry_destroy(iter->first,numberofnodes);
        }
        flowtime = iter->first;


        for (list_size entry_cnt = 0; entry_cnt != entry_num; ++entry_cnt,++iter) {

            //flow parameter setting up, preparing for processing per flow
            iter_vector iter_vector_per_entry = (iter->second).begin();
            temp_flow.src = *iter_vector_per_entry;


            iter_vector_per_entry++;
            temp_flow.dst = *iter_vector_per_entry;

            iter_vector_per_entry++;

            temp_flow.end_time = *iter_vector_per_entry;
            temp_flow.over_head = 0;
            //processing per flow
            route_request++; //here is the final result


            /*here we want to calculate overhead_counter in another way*/
            overhead_counter += temp_flow.over_head;
        }

        /***this part is reserved for future use
                for (int i = 0; i<numberofnodes; i++)
                {
                    distribution[flowtime][i] = switch_data[i].cnt;

                    if (switch_data[i].cnt == 0) {
                        continue;
                    }

                    collection_usage_stage1(switch_data[i].cnt);
                    collection_usage_stage2(switch_data[i].cnt);

                    //k+=switch_data[i].cnt;

                }
        */


        for (int i = 0; i < 105; i++) {
            fout<<"arrival time is "<<i<<endl;
            for (int j = 0; j < numberofnodes; j++) {
                fout<<distribution[i][j]<<'\t';
            }
            fout<<endl;
        }

        fout.close();

        cout<<"totally "<<flowentry_used<<" entries used"<<endl;
        cout<<"entry destroyed "<<destroyed<<endl;
        cout<<"refuse time"<<recordflow<<endl;
        cout<<"control message overhead is "<<overhead_counter<<endl;
        cout<<"Accepted flow num is "<<flownumber*loop_time - refused<<endl;
        cout<<"Total route request is "<<route_request<<endl;
    }

