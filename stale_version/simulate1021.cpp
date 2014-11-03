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


using namespace std;
//using namespace std::placeholders;
#define MAX_NODE 1000
#define MAX_NUM  1000
#define MULTIPLE 10
#define HOPS     3
#define MAX_ENTRY 1000
#define MEAN     10
#define DURATIONTIME 8

int selecthops;
int arrayh[MAX_NODE];
int testnumber=0;

struct node {
    //int ingress_count;
    //int egress_count;
    //int cost;
    map<int,vector <int>>               table_entry;            /* flow table entry in TCAM */
    //std::string                            pktbuffer[BUFNUM];     /* packets stored in each node*/
    int cnt=0;
    vector <int> end_time;
};

struct flow
{
    int src ;
    int dst ;
    int end_time;
    //vector <int> tmp_tag(HOPS);
};

multimap <int,vector<int>>      flow_setup;

int position;
int pathcost[MULTIPLE][MAX_NODE][MAX_NODE];
int shortestpath[MULTIPLE][MAX_NODE][MAX_NODE];
int pathlength[MULTIPLE][MAX_NODE][MAX_NODE];

//int test_cnt = 0;
vector <int> tmp_tag;
struct node switch_data[MAX_NODE];
struct flow temp_flow;

typedef vector<int>::iterator iter_vector;
typedef multimap <int,vector<int>>::iterator iter_multimap;
typedef multimap <int,vector<int>>::size_type list_size;


//////////////added parameter///////////////////
int refused=0;
int recordflow=99999;
int flowtime=0;
//struct flow test;

int demand[10000][2];
int destroyed = 0;
int temppath[MAX_NODE];
////////////////////////////////////////////////

default_random_engine generator(time(NULL));

uniform_int_distribution<int> uniform_dis(0,DURATIONTIME);
uniform_int_distribution<int> generation(1,10);

auto uni_generation = std::bind(generation,generator);
auto uni_dis = std::bind(uniform_dis,generator);


/////////////////////function here//////////////

int myPow(int x, int p) {
  if (p == 0) return 1;
  if (p == 1) return x;
  return x * myPow(x, p-1);
}

void trafficdemand(){
    FILE *file;
    
    /*read and skip the second line*/
    file=fopen("trafficdemand1.txt","r");
    if(file==NULL)
    {
        cout<<"File doesn't exist."<<endl;
        fclose(file);
    }
    else
    {
        /*initialize traffic matrix*/
        
        
        /*input path cost raw data*/
        char ch=getc(file);
        int x,y,n,count=0;
        for(int i=0; count<9609;)
        {
            if(ch =='\t') {
                i++;
                ch=getc(file);
            }
            else if(ch =='\n') {
                //                if(i!=0){
                //		}
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
                
                x=atoi(str);
                demand[count][0]=x;
                //cout<<"the source is "<<x;
            }
            else if(i==2) {
                char str[100]="";
                n=0;
                while(ch!='\t'&&ch!='\n') {
                    str[n]=ch;
                    ch=getc(file);
                    n++;
                }
                
                y=atoi(str);
                demand[count][1]=y;
                //cout<<" and the destination is "<<y<<endl;
                count++;
            }
            else {
                ch=getc(file);
            }
        }
    }
    
    fclose(file);
    
}


int PathCost_input2(const char* filename,const int policy, const int number)
{
    FILE *file;
    //here comes the hard code, we should improve it later
    int h=number;/*read the first line node and edge numbers*/
    cout<<"total number of nodes are "<<h<<endl;
    /*read and skip the second line*/
    file=fopen(filename,"r");
    if(file==NULL)
    {
        cout<<"File doesn't exist."<<endl;
        fclose(file);
    }
    else
    {
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
    //cout<<"the traffic matrix is:"<<endl;
    //for(int j=0; j<h; j++)
    //{
    //    for(int k=0; k<h; k++)
    //    {
    //        cout<<pathcost[0][j][k]<<"\t";
    //    }
    //    cout<<endl;
    //}
    fclose(file);
    return h;
}

void calculate_path(int policy, int number)
{

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

    //cout<<endl<<"the shortest path is"<<endl;
    //for (int i=0; i<number; i++) {
    //    for (int j=0; j<number; j++) {
    //        cout<<shortestpath[policy][i][j]<<"\t";
    //    }
    //    cout<<endl;
    //}

}


int findpath(int x, int y, int z,int w) {
    if(x==0) {
        //cout<<(pathlength-x)<<" the end"<<endl;
        return 0;
    }
    /*when in the middle, we try to find out the minimum of maximum path recursively*/
    else {
        int min_max=MAX_ENTRY;
        int number;
        for(int i=0; i<y+1; ++i)
        {
            int n;
            if(x-HOPS+i>0)/*segmentation fault without it*/
            {
                //n=min(n,findpath(x-maxhop+i,y-i,z+1));
                number=findpath(x-HOPS+i,y-i,z+1,w);
                /*return the maximum of the current and left recursive numbers*/
                n=max(switch_data[(arrayh[w-x-1])].cnt,number);
            }
            else if(x-HOPS+i==0)
            {
                /*return the last and current node's TCAM table entry*/
                n=switch_data[(arrayh[w-x-1])].cnt;
            }
            if(min_max>n) {
                min_max=n;
                if(z==0) {
                    selecthops=HOPS-i;
                }
            }
        }
        return min_max;
    }

}



/*Demo for shortest path calculation*/
void path_statistic(int policy, int number)
{
    std::map<int,int> counters;
    for (int i=0; i<number; i++) {
        for (int j=0; j<number; j++) {
            if( counters.find(pathlength[policy][i][j]) == counters.end() ) {
                counters[pathlength[policy][i][j]] = 1;
            }
            else {
                counters[pathlength[policy][i][j]] ++;
            }
        }
    }
    int k=0;
    for( std::map<int,int>::iterator cit=counters.begin() ; cit != counters.end() ; ++cit) {
        printf("Path length %-5d: %-10d paths\n", cit->first, cit->second);
        k+=cit->second;
    }
    cout<<"total numbers of path is "<<k<<endl;
}


/*------------------max hop--------------------*/
void path_request1(int policy, int dst) {
    int  temp[MAX_NODE];
    int  label;
    
    //switch_data[position].cnt++;
    for(int i=0; i<MAX_NODE; i++) {
        temp[i]=-1;
    }
    //vector<int> tcam_hops;
    temp[0]=dst;
    int src=position;
    for(int i=1; i<MAX_NODE; i++) {
        temp[i]=shortestpath[policy][src][dst];
        if (shortestpath[policy][src][dst]==position) {
            label=i;
            //cout<<"here is the label "<<label<<endl;
            break;
        }
        dst=temp [i];
        //cout<<"now src is "<<src<<endl;
        //cout<<i+1<<"th output switch "<<temp[i]<<endl;
    }
    tmp_tag.clear();
    for(int i=0; (label-i)>0; i++) {
        arrayh[i]=temp[label-1-i];
        //cout<<"label is "<<temp[i]<<endl;
    }
    tmp_tag.assign(arrayh, arrayh+ HOPS);
    
}



/*------------------new objective function--------------------*/
int path_firsthop(int number){
    int number1;
    for(int i=1;myPow(2,i)<number;++i)
    {
	if ((number%(myPow(2,i))-number%(myPow(2,(i-1)))))
        {return i;}
        number1=i;
    }
    return number1;
}

double path_cost1(double average, int length, int number)
{
    double cost=0.0;
    int temp=0;
    int tempnumber;
    for(int i=1;i<=length;++i){
	tempnumber=number%(myPow(2,i))-number%(myPow(2,(i-1)));  //Get the bit in ith hop
               

	if(tempnumber)
	{temp++;}
	else{temp=0;}   //Count the consecutive '0's

        //if(temp==HOPS){return 9999.99;}   //If no less than the max number of hops are 0, then it is not valid, return big value as invalid
        
        if(temppath[i]>average)
        {cost+=2;}
    }
    return cost;
}


void path_request2(int policy, int dst) 
{  
//the objective function 1
    
    int length=0;
    int temp_position=dst;
    temppath[0]=dst;
    int src=position;
    int hops;
    double average,cost=9999.99,temp_cost=9999.99;
    
    for(int i=0; i<MAX_NODE; i++) {
        temppath[i]=-1;
    }
    
    for(int i=1; i<MAX_NODE; i++) {
        temppath[i]=shortestpath[policy][src][dst];
        if (shortestpath[policy][src][dst]==position) {
            length=i;
            break;
        }
        dst=temppath[i];
    }


    for(int i=0;i<=length;++i)   //substitute all nodes with their TCAM entry number
    { 
    		temppath[i]= switch_data[temppath[i]].cnt;
                average+=temppath[i];
    }

    average/=(length+1);

    for(int i=0;i<myPow(2,length);++i)
    {
    	temp_cost=path_cost1(average,length,i);
    	if(cost>temp_cost)   //here we start the array from destination to the next switch to source switch
    	{
		cost=temp_cost;
		hops=path_firsthop(i);	        
	}        
    }


    tmp_tag.clear();
    for(int i=0; (length-i)>0; i++) {
        arrayh[i]=temppath[length-1-i];
    }
    tmp_tag.assign(arrayh, arrayh+ hops);
   
}




void entry_destroy(int time,int numberofnodes)
{
    for (int i = 0; i < numberofnodes; i++)
    {
        for (iter_vector iter = switch_data[i].end_time.begin(); iter != switch_data[i].end_time.end(); ++iter)
        {
            if (time >= *iter)
            {
                switch_data[i].cnt--;
                iter = switch_data[i].end_time.erase(iter);
                iter--;
                destroyed++;
            }
        }
    }
    return;
}


void tcam_lookup(int dst,int endtime)
{
    if(switch_data[position].cnt>=MAX_ENTRY) {
        if (refused==0){recordflow=endtime;}
        refused++;
        tmp_tag.clear();
        position=temp_flow.dst;
        return;
    }
    else {
        switch_data[position].cnt++;
        switch_data[position].end_time.push_back(endtime);
    }
    
    if ((switch_data[position].table_entry).find(dst) == (switch_data[position].table_entry).end())
    {
        path_request2(0,dst);
        (switch_data[position].table_entry)[dst] = tmp_tag;
        //cout<<"calling path request......"<<endl<<endl;
        //test_cnt++;
    }
    else {
        path_request2(0,dst);
        (switch_data[position].table_entry)[dst] = tmp_tag;
        
    }
    
    return;
}


void express_handle(int dst,int endtime)
{
    vector <int> pass_tag;
    vector <int>::iterator tag_iter = tmp_tag.begin();
    if(tmp_tag.empty() == true)
    {
        tcam_lookup(dst,endtime);
        //cout<<"call tcam_table......."<<endl<<endl;
    }
    else if(position != dst)
    {
        tag_iter = tmp_tag.begin();
        position = *tag_iter;

        tag_iter++;
        if(tag_iter != tmp_tag.end())
        {
            for(; tag_iter != tmp_tag.end(); ++tag_iter)
            {
                pass_tag.push_back(*tag_iter);
            }
        }
        tmp_tag = pass_tag;
    }
}

int poisson_dis(float criteria){
    float temp = float(uni_generation());
    if (temp/10.0 < criteria) {
        return 1;
    }
    else
        return 0;
}

void rand_generation(int flownumber,int numberofnodes,float criteria_generate){
    
//    int cnt = 0;
//    
//    for (int i = 0; i < flownumber; i++) {
//        vector <int> flow_in;
//        
//        int flow_src = uni_dis();
//        int flow_dst = uni_dis();
//        int flow_duration = poisson_dis();
//        
//        int flow_arrival = poisson_dis();
//        
//        flow_in.push_back(flow_src);
//        flow_in.push_back(flow_dst);
//        flow_in.push_back(flow_duration + flow_arrival);
//        
//        flow_setup.insert(make_pair(flow_arrival,flow_in));
//        //cout<<"key "<<flow_arrival<<endl;
//        flow_in.clear();
//        cnt++;
//    }
//    cout<<"running time "<<cnt<<endl;
//    //    for (iter_multimap iter = flow_setup.begin(); iter != flow_setup.end(); ++iter) {
//    //
//    //        cout << "stating time " << (*iter).first<< endl;
//    //
    //    }
    int flow_num = 0;
    for (int time_stamp = 0; flow_num < flownumber; time_stamp++)
    {
        for (int tmp_rule = 0; tmp_rule < 9600; tmp_rule++)
        {
            if(poisson_dis(criteria_generate) && (flow_num < flownumber))
            {
                vector <int> flow_in;
                flow_in.push_back(demand[tmp_rule][0]);
                flow_in.push_back(demand[tmp_rule][1]);
                flow_in.push_back(time_stamp + uni_dis());
                
                
                flow_setup.insert(make_pair(time_stamp,flow_in));
                flow_in.clear();
                flow_num++;
                //cout<<"generation completed!!"<<endl;


            }
        }
        
    }
}
int main(int argc, char **argv
         ) {
    cout<<"start to process"<<endl;

    
    int  number;
    int flownumber =atoi(argv[1]);
    int numberofnodes=atoi(argv[2]);
    float criteria_generate = 0.5;
    
    vector<vector<int>> p(1000);
    for(auto &v : p) {
        v.resize(1000,0);
    }
    
    string filename="method2";
    //cout<<"Enter your file name here"<<endl;
    //cin>>filename;
    ofstream fout(filename.c_str(),ofstream::out | ofstream::app);

    
    
    fout<<endl<<endl<<"-------------------------------------------------------------------------------"<<endl<<flownumber<<"\ttotally flow number"<<endl;
    fout<<endl;
//----------------------------build up topology-----------------------------
    number=PathCost_input2("topology.txt",0,numberofnodes);
    calculate_path(0,number);
    trafficdemand();
    cout<<"-----------------------------------------------------------------------------"<<endl<<endl;
//----------------------------flow handle MAX-HOP old_version----------------------------
//    for(int i=0; i<flownumber; ++i) {
//        tmp_tag.clear();
//        temp_flow.src = rand() % numberofnodes;
//        temp_flow.dst = rand() % numberofnodes;
//        ++p[temp_flow.src][temp_flow.dst];
//
//        position=temp_flow.src;
//        while(position != temp_flow.dst) {
//            express_handle(temp_flow.dst);
//        }
//    }

//---------------------------flow handling 1013 version---------------------
    
    rand_generation(flownumber,numberofnodes,criteria_generate);
    
    int k= 0;
    
    for (iter_multimap iter = flow_setup.begin(); iter != flow_setup.end();) {
        
        //iter_multimap iter_per_entry = flow_setup.find(iter->first);
        list_size entry_num = flow_setup.count(iter->first);
        
        //check switch time table
        entry_destroy(iter->first,numberofnodes);
        flowtime = iter->first;
        
        for (list_size entry_cnt = 0; entry_cnt != entry_num; ++entry_cnt,++iter) {
            
            //flow parameter setting up, preparing for processing per flow
            iter_vector iter_vector_per_entry = (iter->second).begin();
            temp_flow.src = *iter_vector_per_entry;
            
            iter_vector_per_entry++;
            temp_flow.dst = *iter_vector_per_entry;
            
            iter_vector_per_entry++;
            //int duration = *iter_vector_per_entry;
//            cout<<dst<<endl;
//            cout<<src<<endl;
//            cout<<duration<<endl;
            //int arrival = iter->first;
            
            temp_flow.end_time = *iter_vector_per_entry;
            
            //processing per flow
            position = temp_flow.src;
            while (position != temp_flow.dst) {
                express_handle(temp_flow.dst,temp_flow.end_time);
            }
        }
        for (int i = 0; i<numberofnodes; i++)
        {
            fout<<switch_data[i].cnt<<endl;
            k+=switch_data[i].cnt;
            
        }
        fout<<"-------------------------------------------------------------------------------"<<endl<<k<<"\totally flow entry"<<endl;
        //cout<<test_cnt<<" testing counter"<<endl;
        
        fout<<"testnumber is "<<testnumber<<endl;
        fout<<"arrival time is "<<iter->first<<endl;
        fout<<"number of refused flow's is "<<refused<<endl;
        cout<<"number of refused flow's is "<<refused<<endl;
        fout<<"refused times is "<<recordflow<<endl;

        fout<<"-------------------------------------------------------------------------------"<<endl;
        
        
    }



    fout.close();
    cout<<"entry destroyed "<<destroyed<<endl;
}
