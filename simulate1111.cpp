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
#define HOPS     2
#define MAX_ENTRY 2000
#define MEAN     10
#define DURATIONTIME 10

int selecthops;
int arrayh[MAX_NODE];
int testnumber=0;
int loop_time;
int key=0;
string scheme_name;


struct node {
    //int ingress_count;
    //int egress_count;
    //int cost;
    map<int,vector <int>>               table_entry;            /* flow table entry in TCAM */
    //std::string                            pktbuffer[BUFNUM];     /* packets stored in each node*/
    int cnt=0;
    int degree = 0;
    vector <int> end_time;
};

struct flow
{
    int src ;
    int dst ;
    int end_time;
    int over_head;
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
int flowentry_used = 0;
////////////////////////////////////////////////

default_random_engine generator(0);

uniform_int_distribution<int> uniform_dis(0,DURATIONTIME);
uniform_int_distribution<int> generation(1,40);

auto uni_generation = bind(generation,generator);
auto uni_dis = bind(uniform_dis,generator);

int temppath[MAX_NODE];
int tempUsage[MAX_NODE];
int k= 0;
/////////////////////add 1103//////////////////////
int schematic;
int usage_counter[110][11];
int counter_overflow[110];
int usage_counter1[110][11];
int counter_overflow1[110];
int key_value;
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
    float temp_sum=0.0;
    for (int node_id = 0; node_id < numberofnodes; node_id++) {
        sum += switch_data[node_id].cnt;
        if (switch_data[node_id].cnt) {
            temp_sum+=1.0;
        }
    }
    float average = float(sum)/512;
    //cout<<temp_sum<<endl;
    return average;
}

float RMSE_func(float average, int numberofnodes) {
    float RMSE = 0.0;
    float temp_sum=0.0;
    int node_id = 0;
    while (node_id < numberofnodes) {
        RMSE += pow((float(switch_data[node_id].cnt)-average)/100000.0,2.0);
        node_id+=1;
        if (switch_data[node_id].cnt) {
            temp_sum+=1.0;
        }
    }
    //RMSE -= 155*(pow((0.0-average)/1000.0,2.0));
    RMSE = sqrt(RMSE/temp_sum);
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
    
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < h; j++) {
            if (pathcost[policy][i][j] != 99999) {
                switch_data[i].degree++;
            }
        }
    }
    
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
    int  label=0;
    if (HOPS == 1) {
        scheme_name = "Open_Flow";
    }
    else
        scheme_name = "Max_Hop";

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

    //cout<<"print the path"<<endl;
    //for(int i=0;i<label;++i){cout<<temp[i]<<"<-";}
    //cout<<endl;


    tmp_tag.clear();
    //for(int i=0;i<MAX_NODE;i++){arrayh[i]=0;}
    for(int i=0; (label-i)>0; i++) {
        arrayh[i]=temp[label-1-i];
            //cout<<"temp is "<<arrayh[i]<<endl;
    }


    if (label>HOPS) {
        label=HOPS;
    }

    /// this part is for Nov11 testing data
    if ((switch_data[arrayh[HOPS]].cnt < MAX_ENTRY-switch_data[arrayh[HOPS]].degree) 
        && switch_data[position].cnt < MAX_ENTRY - switch_data[arrayh[HOPS]].degree)
    {
        tmp_tag.assign(arrayh, arrayh+ label);
        flowentry_used++;
        vector<int>::iterator iterOverHead = find(tmp_tag.begin(),tmp_tag.end(),temp_flow.dst);
        if(iterOverHead != tmp_tag.end() && tmp_tag.size() == HOPS)
            temp_flow.over_head++;
        temp_flow.over_head++;
    }
    else
    {
        refused++;
        tmp_tag.clear();
        position=temp_flow.dst;
        temp_flow.over_head = 0;
        return;
    }
    

}

/*------------------new objective function help function, to get the first context switch's position--------------------*/
int path_firsthop(unsigned int number,unsigned int length) {
    int number1=1;
    for(int i=0; i<length; ++i)
    {   //cout<<"the number is"<<number<<endl;
        if (number/myPow(2,i))
        {
            //cout<<"the returned length"<<length-i<<endl;
            //return (length-i);
            number1=length-i;
            //cout<<"the length is=--------------------"<<length<<endl;
            //cout<<"the bit examing is "<<number/myPow(2,i)<<endl;
        }
        //number1=HOPS;
    }
    //cout<<"the key value inside is----------------------"<<number1<<endl;
    return number1;
}

//-------------------------------the new objective function-------------------------------
double path_cost2(double average, int length, int number)
{

    double costx=0.0;
    int temp=0;
    int tempnumber;
    int temp_count;
    int temp1=0;
    

    for(int i=0; i<length; ++i) {
        //tempnumber=number%(myPow(2,i+1))-number%(myPow(2,i));  //Get the bit in ith hop

        tempnumber=number/(myPow(2,i))-(number/myPow(2,i+1))*2;

        temp_count=switch_data[temppath[i]].cnt;
        
        if(!tempnumber)
        {
            temp++;
            if(temp>=HOPS) {
                //cout<<"cost 999999 returned////////////////////////"<<endl;
                return 999999.99;   //If no less than the max number of hops are 0, then it is not valid, return big value as invalid
            }
        }
        else
        {
            key_value=length-i;
            temp=0;
            if(temp_count<333*2)
            {
                costx+=0.01;
            }
            else if(temp_count<666*2)
            {
                costx+=0.03;
            }
            else if(temp_count<900*2)
            {
                costx+=0.1;
            }
            else if(temp_count<1000*2)
            {
                costx+=0.7;
            }
            else {
                return 999999.99;
            }
        }   //Count the consecutive '0's

    }
    //cout<<"cost "<<costx<<"returned////////////////////////"<<endl;
    return costx;
}

double path_cost1(double average, int length, int number)
{
    double cost=0.0;
    int temp=0;
    int tempnumber;
    for(int i=0; i<length; ++i) {

        tempnumber=number/(myPow(2,i))-(number/myPow(2,i+1))*2;  //Get the bit in ith hop


        if(tempnumber)
        {
            temp=0;
            if(temppath[i]>average)
            {
                cost+=2;
            }
        }
        else {
            temp++;
            if(temp>=HOPS) {
                return 999999.99;   //If no less than the max number of hops are 0, then it is not valid, return big value as invalid
            }
            cost-=0.2;
        }   //Count the consecutive '0's
    }
    return cost;
}

void path_request2(int policy, int dst)
{
//the objective function 1

    scheme_name = "New_Func";
    int length=1;
    //int temp_position=dst;
    int src=position;
    int hops=1;
    double average,cost=999999.99,temp_cost=999999.99;

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

    //cout<<"print the path"<<endl;
    //for(int i=0;i<length;++i){cout<<temppath[i]<<"<-";}
    //cout<<endl;

    //for(int i=0; i<=length; ++i) //substitute all nodes with their TCAM entry number
    //{
    //    average+=switch_data[temppath[i]].cnt;
    //}

    //average/=(length+1);

    for(int i=0; i<myPow(2,length); ++i)
    {
        //cout<<"now trying number "<<i<<endl;
        //if (i<(myPow(2,length)/myPow(2,hops)))
        //    continue;

        if (schematic==1)
        {
            temp_cost=path_cost2(average,length,i);
            //cout<<"///////////cost is "<<temp_cost<<endl;
        }
        else {
            temp_cost=path_cost1(average,length,i);
        }
        if(cost>temp_cost)   //here we start the array from destination to the next switch to source switch
        {
            cost=temp_cost;
            hops=path_firsthop(i,length);
        }
    }

    //cout<<"----------the cost is "<<cost<<endl;
    if (cost==999999.99) {
        refused++;
        tmp_tag.clear();
        position=temp_flow.dst;
        temp_flow.over_head = 0;
        return;
    }


    //this  is for Nov11 testing data
    flowentry_used++;


    tmp_tag.clear();
    //for(int i=0; i<MAX_NODE; i++) {
    //    arrayh[i]=0;
    //}
    for(int i=0; (length-i)>0; i++) {
        arrayh[i]=temppath[length-1-i];
    //    cout<<"the hops is "<<arrayh[i]<<endl;
    }

    //cout<<"----------the key value is "<<hops<<endl;
    temp_flow.over_head++;
    tmp_tag.assign(arrayh, arrayh+ hops);
    vector<int>::iterator iterOverHead = find(tmp_tag.begin(),tmp_tag.end(),temp_flow.dst);
    if(iterOverHead != tmp_tag.end() && tmp_tag.size() == HOPS)
        temp_flow.over_head++;


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
    int flowtable = MAX_ENTRY - switch_data[position].degree;

    if(switch_data[position].cnt>=flowtable) {
        if (refused==0) {
            recordflow=key;
        }
        refused++;
        tmp_tag.clear();
        position=temp_flow.dst;
        temp_flow.over_head = 0;
        return;
    }
    else {
        switch_data[position].cnt++;
        switch_data[position].end_time.push_back(endtime);
    }

    //if ((switch_data[position].table_entry).find(dst) == (switch_data[position].table_entry).end())
    //{
    if (schematic==1||schematic==3)
    {
        path_request2(0,dst);
    }
    else if(schematic==0) {
        path_request1(0,dst);
    }
    else if(schematic==2) {
        path_request4(0,dst);
    }

    //(switch_data[position].table_entry)[dst] = tmp_tag;
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
        // if(position == temp_flow.dst && tmp_tag.size() == 1)
        //     temp_flow.over_head++;
        tag_iter++;
        if(tag_iter != tmp_tag.end())
        {
            for(; tag_iter != tmp_tag.end(); ++tag_iter)
            {
                pass_tag.push_back(*tag_iter);
            }
        }
        tmp_tag.clear();
        tmp_tag = pass_tag;
    }
}

int poisson_dis(float criteria) {
    float temp = float(uni_generation());
    if (temp/40.0 < criteria) {
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

void collection_usage_stage1(int number1_) {

    if(number1_<333)
    {
        usage_counter[flowtime][0]++;
    }
    else if(number1_<666)
    {
        usage_counter[flowtime][1]++;
    }
    else if(number1_<900)
    {
        usage_counter[flowtime][2]++;
    }
    else if(number1_<=1000)
    {
        usage_counter[flowtime][3]++;
    }

    if (number1_ == MAX_ENTRY) {
        counter_overflow[flowtime]++;
        usage_counter[flowtime][4]++;
    }
    return;
}

void collection_usage_stage2(int number1_) {
    int index = number1_/100;
    if (number1_ < MAX_ENTRY) {
        usage_counter1[flowtime][index]++;
    }
    else {
        counter_overflow1[flowtime]++;
        usage_counter1[flowtime][11]++;
    }
    return;
}

int main(int argc, char **argv
        ) {

    int  number;
    int overhead_counter = 0;
    int route_request = 0;
    int flownumber = atoi(argv[1]);
    int numberofnodes=atoi(argv[2]);
    loop_time=atoi(argv[3]);
    string total_flow = itos(flownumber * loop_time);
    float criteria_generate = 0.1;
    float statistic[200][2];
    schematic=atoi(argv[4]);
    string string1[4];
    string1[0]="Max_Hop_";
    string1[1]="New_Function_";
    string1[2]="Load_Balance_";
    string1[3]="Alpha_beta_";


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



    fout<<endl<<endl<<"-------------------------------------------------------------------------------"<<endl<<flownumber*loop_time<<"\ttotally flow number"<<endl;
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
        if (!atoi(argv[5]))
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
            k++;
            temp_flow.over_head = 0;
            //processing per flow
            position = temp_flow.src;
            route_request++;
            while (position != temp_flow.dst) {
                express_handle(temp_flow.dst,temp_flow.end_time);

            }
            overhead_counter += temp_flow.over_head;    
            tmp_tag.clear();
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

            if (switch_data[i].cnt == 0) {
                continue;
            }

            collection_usage_stage1(switch_data[i].cnt);
            collection_usage_stage2(switch_data[i].cnt);

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

        //cout<<"number of refused flow's is "<<refused<<endl;
        //fout<<fixed<<averageInNodes<<'\t'<<RMSE<<endl;
        //cout<<"arrival time is "<<flowtime<<endl;
        //fout<<endl;
        key++;

    }

    for (int i = 0; i < 105; i++) {
        fout<<"arrival time is "<<i<<endl;
        for (int j = 0; j < numberofnodes; j++) {
            fout<<distribution[i][j]<<'\t';
        }
        fout<<endl;
    }

    fout.close();

    filename = "Average_RMSE_"+ string1[schematic] + total_flow + ".txt";

    ofstream datafile(filename.c_str(),ofstream::out | ofstream::app);

    for (int i = 0; i < 110; i++) {
        //datafile<<"total flow number is "<<total_flow<<endl;
        datafile<<fixed<<statistic[i][0]<<'\t'<<statistic[i][1]<<endl;
    }


    datafile.close();

    filename = "usage distribution_"+ string1[schematic] + total_flow + ".txt";

    ofstream data_analysis(filename.c_str(),ofstream::out | ofstream::app);

    data_analysis<<"-------------------------Distribution of TCAM Usage------------------------"<<endl;

    for (int i = 0; i < 110; i++) {
        for (int j = 0; j < 11; j++) {
            data_analysis<<usage_counter[i][j]<<'\t';
        }
        data_analysis<<endl;
    }

    data_analysis<<"-------------------------Number of TCAM Overflow occurence-----------------"<<endl;

    for (int i = 0; i < 110; i++) {
        data_analysis<<counter_overflow[i]<<endl;
    }

    data_analysis.close();


    filename = "usage distribution_"+ string1[schematic] + total_flow + "1.txt";

    ofstream data_analysis1(filename.c_str(),ofstream::out | ofstream::app);

    data_analysis1<<"-------------------------Distribution of TCAM Usage------------------------"<<endl;

    for (int i = 0; i < 110; i++) {
        for (int j = 0; j < 11; j++) {
            data_analysis1<<usage_counter1[i][j]<<'\t';
        }
        data_analysis1<<endl;
    }

    data_analysis1<<"-------------------------Number of TCAM Overflow occurence-----------------"<<endl;

    for (int i = 0; i < 110; i++) {
        data_analysis1<<counter_overflow1[i]<<endl;
    }

    data_analysis1.close();
    
    filename = "node degree" + total_flow + ".txt";
    ofstream node_degree(filename.c_str(),ofstream::out | ofstream::app);
    
    for (int i = 0; i < numberofnodes; i++) {
        node_degree<<switch_data[i].degree<<endl;
    }
    node_degree.close();

    //for(int i=0;i<512;i++){
    //cout<<shortestpath[0][25][481]<<" ";
    //cout<<shortestpath[0][25][476]<<" ";
    //cout<<shortestpath[0][25][204]<<" ";
    //cout<<shortestpath[0][25][41]<<" ";
    //cout<<shortestpath[0][25][68]<<"/t";
    //}

    int count = 0;

    for (int i = 0; i < numberofnodes; ++i)
    {
        count += switch_data[i].cnt;
    }

    cout<<"totally "<<flowentry_used<<" entries used"<<endl;

    cout<<"entry destroyed "<<destroyed<<endl;
    cout<<"refuse time"<<recordflow<<endl;
    cout<<"control message overhead is "<<overhead_counter<<endl;
    cout<<"Accepted flow num is "<<flownumber*loop_time - refused<<endl;
    cout<<"Total route request is "<<route_request<<endl;
}

