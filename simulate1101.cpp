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

using namespace std;

#define MAX_NODE 1000
#define MAX_NUM  1000
#define MULTIPLE 10
#define HOPS     3
#define MAX_ENTRY 1000
#define MEAN     10
#define DURATIONTIME 10

int selecthops;
int arrayh[MAX_NODE];
int testnumber=0;
int loop_time;
int key;


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

int distribution[200][512];
////////////////////////////////////////////////

default_random_engine generator(time(NULL));

uniform_int_distribution<int> uniform_dis(0,DURATIONTIME);
uniform_int_distribution<int> generation(1,10);

auto uni_generation = bind(generation,generator);
auto uni_dis = bind(uniform_dis,generator);

int temppath[MAX_NODE];
int tempUsage[MAX_NODE];
int k= 0;
/////////////////////add 1103//////////////////////
int schematic;

///////////////////////////////////////////////////

int myPow(int x, int p) {
    if (p == 0) return 1;
    if (p == 1) return x;
    return x * myPow(x, p-1);
}

string itos(int value) {
    stringstream stream;
    stream<<value;
    return stream.str();
}

float average_func(int numberofnodes) {
    int sum = 0;
    for (int node_id = 0; node_id < numberofnodes; node_id++) {
        sum += switch_data[node_id].cnt;
    }
    float average = float(sum)/357.0;
    return average;
}

float RMSE_func(float average, int numberofnodes) {
    float RMSE = 0.0;
    int node_id = 0;
    while (node_id < numberofnodes) {
        RMSE += pow((float(switch_data[node_id].cnt)-average)/1000.0,2.0);
        node_id+=1;
    }
    //RMSE -= 155*(pow((0.0-average)/1000.0,2.0));
    RMSE = sqrt(RMSE/float(512.0));
    return RMSE;
}

void trafficdemand() {
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

/*------------------new objective function help function, to get the first context switch's position--------------------*/
int path_firsthop(int number,int length) {
    int number1;
    for(int i=length; i>0; --i)
    {
        if ((number%(myPow(2,i))-number%(myPow(2,(i-1)))))
        {
            return i;
        }
        number1=i;
    }
    return number1;
}

//-------------------------------the new objective function-------------------------------
double path_cost2(double average, int length, int number)
{
    
    double cost=0.0;
    int temp=0;
    int tempnumber;
    int temp_count;
    for(int i=1; i<=length; ++i) {
        tempnumber=number%(myPow(2,i))-number%(myPow(2,(i-1)));  //Get the bit in ith hop
        
        temp_count=switch_data[temppath[i]].cnt;
        
        if(tempnumber)
        {
            temp++;
            if(temp>=HOPS) {
                return 9999.99;   //If no less than the max number of hops are 0, then it is not valid, return big value as invalid
            }
        }
        else
        {
            temp=0;
            if(temp_count<333)
            {
                cost+=0.01*temp_count;
            }
            else if(temp_count<666)
            {
                cost+=0.03*temp_count;
            }
            else if(temp_count<900)
            {
                cost+=0.1*temp_count;
            }
            else if(temp_count<=1000)
            {
                cost+=0.7*temp_count;
            }
            else {return 9999.99;}
        }   //Count the consecutive '0's
        
    }
    return cost;
}

double path_cost1(double average, int length, int number)
{
    double cost=0.0;
    int temp=0;
    int tempnumber;
    for(int i=1; i<=length; ++i) {
        tempnumber=number%(myPow(2,i))-number%(myPow(2,(i-1)));  //Get the bit in ith hop
        
        
        if(tempnumber)
        {
            temp++;
            if(temppath[i]>average)
            {
                cost+=2;
            }
        }
        else {
            temp=0;
            if(temp>=HOPS) {
                return 9999.99;   //If no less than the max number of hops are 0, then it is not valid, return big value as invalid
            }
            cost+=0.4;
        }   //Count the consecutive '0's
    }
    return cost;
}

void path_request2(int policy, int dst)
{
    //the objective function 1
    int length=0;
    int temp_position=dst;
    int src=position;
    int hops;
    double average,cost=9999.99,temp_cost=9999.99;
    
    for(int i=0; i<MAX_NODE; i++) {
        temppath[i]=-1;
    }
    
    temppath[0]=dst;
    
    for(int i=1; i<MAX_NODE; i++) {
        temppath[i]=shortestpath[policy][src][dst];
        if (shortestpath[policy][src][dst]==position) {
            length=i;
            break;
        }
        dst=temppath[i];
    }
    
    
    for(int i=0; i<=length; ++i) //substitute all nodes with their TCAM entry number
    {
        average+=switch_data[temppath[i]].cnt;
    }
    
    average/=(length+1);
    
    for(int i=0; i<myPow(2,length); ++i)
    {
        temp_cost=path_cost2(average,length,i);
        if(cost>temp_cost)   //here we start the array from destination to the next switch to source switch
        {
            cost=temp_cost;
            hops=path_firsthop(i,length);
        }
    }
    
    tmp_tag.clear();
    for(int i=0; (length-i)>0; i++) {
        arrayh[i]=temppath[length-1-i];
    }
    tmp_tag.assign(arrayh, arrayh+ hops);
    
}


void path_request4(int policy, int dst) {
    int temp[MAX_NODE];
    int label;
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
            label=i;/*should be substituted by pathlength[src][dst]*/
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
    int hops, counts;/*the enhanced load balance algorithm*/
    if(label%HOPS) {
        hops=label/HOPS+1;
        counts=HOPS-(label%HOPS);
    }
    else {
        hops=label/HOPS;
        counts=0;
    }
    int compare_1=findpath(label,counts,0,label);
    int tempnumber1=selecthops;
    int compare_2=findpath(label,counts+HOPS,0,label);
    int tempnumber2=selecthops;
    int compare_3=findpath(label,counts+(HOPS*2),0,label);
    int tempnumber3=selecthops;
    if (compare_1<=compare_2) {
        selecthops=tempnumber1;
    }
    else if (compare_2<=compare_3) {
        selecthops=tempnumber2;
    }
    else {
        selecthops=tempnumber3;
    }
    if(selecthops<HOPS) {
        testnumber++;
    }
    tmp_tag.assign(arrayh, arrayh+ selecthops);
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
        if (refused==0) {
            recordflow=k;
        }
        refused++;
        tmp_tag.clear();
        position=temp_flow.dst;
        return;
    }
    else {
        switch_data[position].cnt++;
        switch_data[position].end_time.push_back(endtime);
    }
    
    //if ((switch_data[position].table_entry).find(dst) == (switch_data[position].table_entry).end())
    //{
    if (schematic)
    {
        path_request2(0,dst);
    }
    else {
        path_request1(0,dst);
    }
    (switch_data[position].table_entry)[dst] = tmp_tag;
    //}
    //else {
    //    path_request1(0,dst);
    //    (switch_data[position].table_entry)[dst] = tmp_tag;
    
    //}
    
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

int poisson_dis(float criteria) {
    float temp = float(uni_generation());
    if (temp/10.0 < criteria) {
        return 1;
    }
    else
        return 0;
}



void rand_generation(int flownumber,int numberofnodes,float criteria_generate) {
    
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
    
    int usage_counter[110][11];
    int counter_overflow[110];
    int  number;
    int flownumber = atoi(argv[1]);
    int numberofnodes=atoi(argv[2]);
    loop_time=atoi(argv[3]);
    string total_flow = itos(flownumber * loop_time);
    float criteria_generate = 0.1;
    float statistic[200][2];
    schematic=atoi(argv[4]);
    string string1[2];
    string1[0]="Max_Hop_";
    string1[1]="New_Function_";
    
    for (int i = 0; i < 110; i++) {
        counter_overflow[i] = -1;
        for (int j = 0; j < 11; j++) {
            usage_counter[i][j] = -1;
        }
    }
    
    vector<vector<int>> p(1000);
    for(auto &v : p) {
        v.resize(1000,0);
    }
    
    string filename = "Case1_"+string1[schematic] + total_flow + ".txt";
    //cout<<"Enter your file name here"<<endl;
    //cin>>filename;
    ofstream fout(filename.c_str(),ofstream::out | ofstream::app);
    
    
    
    fout<<endl<<endl<<"-------------------------------------------------------------------------------"<<endl<<flownumber<<"\ttotally flow number"<<endl;
    fout<<endl;
    //----------------------------build up topology-----------------------------
    number=PathCost_input2("topology.txt",0,numberofnodes);
    calculate_path(0,number);
    trafficdemand();
    //cout<<"-----------------------------------------------------------------------------"<<endl<<endl;
    
    
    //---------------------------flow handling 1013 version---------------------
    
    for (int time = 0; time < loop_time; time++) {
        rand_generation(flownumber,numberofnodes,criteria_generate);
    }
    //--------------------------processing loop per time step--------------------
    for (iter_multimap iter = flow_setup.begin(); iter != flow_setup.end();) {
        
        //iter_multimap iter_per_entry = flow_setup.find(iter->first);
        list_size entry_num = flow_setup.count(iter->first);
        
        //check switch time table
        //entry_destroy(iter->first,numberofnodes);
        flowtime = iter->first;
        
        
        for (list_size entry_cnt = 0; entry_cnt != entry_num; ++entry_cnt,++iter) {
            
            //flow parameter setting up, preparing for processing per flow
            iter_vector iter_vector_per_entry = (iter->second).begin();
            temp_flow.src = *iter_vector_per_entry;
            
            iter_vector_per_entry++;
            temp_flow.dst = *iter_vector_per_entry;
            
            iter_vector_per_entry++;
            
            temp_flow.end_time = *iter_vector_per_entry;
            k++;
            //processing per flow
            position = temp_flow.src;
            while (position != temp_flow.dst) {
                express_handle(temp_flow.dst,temp_flow.end_time);
            }
        }
        
        
        //        for (int i = 0; i<numberofnodes; i++)
        //        {
        //            fout<<switch_data[i].cnt<<'\t';
        //            //k+=switch_data[i].cnt;
        //
        //        }
        
        for (int i = 0; i<numberofnodes; i++)
        {
            distribution[flowtime][i] = switch_data[i].cnt;
            int index = switch_data[i].cnt/100;
            usage_counter[flowtime][index]++;
            if (switch_data[i].cnt == MAX_ENTRY) {
                counter_overflow[flowtime]++;
            }
            //k+=switch_data[i].cnt;
            
        }
        
        int unittime_flows=0;
        float averageInNodes=average_func(numberofnodes);
        float RMSE;
        for (int i = 0; i<numberofnodes; i++)
        {
            //fout<<switch_data[i].cnt<<endl;
            unittime_flows+=switch_data[i].cnt;
        }
        RMSE=RMSE_func(averageInNodes,numberofnodes);
        statistic[flowtime][0] = averageInNodes;
        statistic[flowtime][1] = RMSE;
        
        cout<<"number of refused flow's is "<<refused<<endl;
        //fout<<fixed<<averageInNodes<<'\t'<<RMSE<<endl;
        cout<<"arrival time is "<<flowtime<<endl;
        //fout<<endl;
        
    }
    
    for (int i = 0; i < sizeof(distribution)/sizeof(distribution[0]); i++) {
        fout<<"arrival time is "<<i<<endl;
        for (int j = 0; j < numberofnodes; j++) {
            fout<<distribution[i][j]<<'\t';
        }
        fout<<endl;
    }
    
    fout.close();
    
    filename = "Average_RMSE_"+ string1[schematic] + total_flow + ".txt";
    
    ofstream datafile(filename.c_str(),ofstream::out | ofstream::app);
    
    for (int i = 0; i < sizeof(statistic)/sizeof(statistic[0]); i++) {
        datafile<<"total flow number is "<<total_flow<<endl;
        datafile<<fixed<<statistic[i][0]<<'\t'<<statistic[i][1]<<endl;
    }
    
    
    datafile.close();
    
    filename = "usage distribution_"+ string1[schematic] + total_flow + ".txt";
    
    ofstream data_analysis(filename.c_str(),ofstream::out | ofstream::app);
    
    data_analysis<<"-------------------------Distribution of TCAM Usage------------------------"<<endl;
    
    for (int i = 0; i < sizeof(usage_counter)/sizeof(usage_counter[0]); i++) {
        for (int j = 0; j < sizeof(usage_counter)/sizeof(int); j++) {
            data_analysis<<usage_counter[i][j]<<'\t';
        }
        data_analysis<<endl;
    }
    
    data_analysis<<"-------------------------Number of TCAM Overflow occurence-----------------"<<endl;
    
    for (int i = 0; i < sizeof(counter_overflow)/sizeof(int); i++) {
        data_analysis<<counter_overflow[i]<<endl;
    }
    
    data_analysis.close();
    
    cout<<"entry destroyed "<<destroyed<<endl;
}
