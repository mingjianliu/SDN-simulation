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

using namespace std;
#define MAX_NODE 1000
#define MAX_NUM  1000
#define MULTIPLE 10
#define HOPS     3
#define MAX_ENTRY 10000
#define MEAN     10

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
    //vector <int> end_time;
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
//int cnt = 0;
int destory_number=0;
//////////////10.20 added parameter/////////////
std::map<int,int> link_of_nodes;
std::map<int,int> nodes_usage;
////////////////////////////////////////////////
std::map<int,int> nodes_counter;


void calculate_links(int numberofnodes) {
    int count=0;
    for(int i=0; i<numberofnodes; i++)
    {   link_of_nodes[i]=0;
        for(int j=0; j<numberofnodes; j++)
        {
            if(i!=j){
            if(i==shortestpath[0][i][j]) {
                link_of_nodes[i]++;
                link_of_nodes[j]++;
            }}
        }
    }
    
    string filename="link_of_nodes";
    ofstream fout(filename.c_str(),ofstream::out | ofstream::app);
    //fout<<"totally "<<numberofnodes<<" of nodes"<<endl;
    for( std::map<int,int>::iterator cit=link_of_nodes.begin() ; cit != link_of_nodes.end() ; ++cit){
        //printf("Port #%-5d: %-10d packets\n", cit->first, cit->second);
        fout<<(cit->first)<<"\t"<<(cit->second)<<endl;
        count+=(cit->first);
    }
    fout<<"the totally number of links are "<<count<<endl;
    fout.close();
    return;
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

int PathCost_input2(const char* filename,const int policy, const int number
                    /*here the number is used for hard code, should be deleted afterward*/)
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

};



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


/*-------------------load balance---------------------*/
void path_request2(int policy, int dst) {
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

    int hops, counts;
    if(label%HOPS) {
        hops=label/HOPS+1;
        counts=HOPS-(label%HOPS);
    }
    else {
        hops=label/HOPS;
        counts=0;
    }
    findpath(label,counts,0,label);
    if(selecthops<HOPS) {
        testnumber++;
    }
    tmp_tag.assign(arrayh, arrayh+ selecthops);

}

/*-------------------enhanced 1 more context---------------------*/
void path_request3(int policy, int dst) {
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
    //int compare_3=findpath(label,counts+(HOPS*2),0,label);
    //int tempnumber3=selecthops;

    if (compare_1<=compare_2) {
        selecthops=tempnumber1;
    }
    //else if (compare_2<=compare_3) {
    //    selecthops=tempnumber2;
    //}
    //else {
    //    selecthops=tempnumber3;
    //}

    if(selecthops<HOPS) {
        testnumber++;
    }
    tmp_tag.assign(arrayh, arrayh+ selecthops);

}

/*-------------------enhanced 2 more context---------------------*/
void path_request4(int policy, int dst) {
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


int poisson_dis(float criteria) {
    srand(clock());
    float temp = float(rand() % 10 + 1);
    if (temp/10.0 < criteria) {
        return 1;
    }
    else
        return 0;
}

void entry_destroy(int numberofnodes,float criteria_terminate)
{
    for (int i = 0; i < numberofnodes; i++)
    {
        //cout<<"run the loop "<<endl;
        int temp_counter = switch_data[i].cnt;
        for (int j = 0; j < temp_counter; j++) {
            if(poisson_dis(criteria_terminate)) {
                temp_counter--;
                destory_number++;
                ///cout<<"really destroy "<<endl;
            }
            //cnt++;
            //cout<<"destroy run "<<endl;
        }
        switch_data[i].cnt = temp_counter;
        //cout<<"node"<<i<<"counter"<<switch_data[i].cnt<<endl;

    }

    return;
}


void tcam_lookup(int dst)
{
    if(switch_data[position].cnt>=MAX_ENTRY) {
        if (refused==0) {
            recordflow=flowtime;
        }
        refused++;
        tmp_tag.clear();
        position=dst;
        return;
    }
    else {
        switch_data[position].cnt++;
    }

    if ((switch_data[position].table_entry).find(dst) == (switch_data[position].table_entry).end())
    {
        path_request1(0,dst);
        (switch_data[position].table_entry)[dst] = tmp_tag;
        //cout<<"calling path request......"<<endl<<endl;
        //test_cnt++;
    }
    else {
        path_request1(0,dst);
        (switch_data[position].table_entry)[dst] = tmp_tag;

    }

    return;
}


void express_handle(int dst)
{
    vector <int> pass_tag;
    vector <int>::iterator tag_iter = tmp_tag.begin();
    if(tmp_tag.empty() == true)
    {
        tcam_lookup(dst);
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




int main(int argc, char **argv
        ) {
    int number;
    //int flownumber =atoi(argv[1]);
    int flownumber=9609;
    int numberofnodes=atoi(argv[2]);
    vector<vector<int>> p(1000);

    float criteria_terminate = 0;
    float criteria_generate = 1;
    for(auto &v : p) {
        v.resize(1000,0);
    }

//----------------------------build up topology-----------------------------
    number=PathCost_input2("topology_44.txt",0,numberofnodes);
    calculate_path(0,number);
    calculate_links(numberofnodes);
    
    trafficdemand();
    int k= 0;

        string filename="node_usage";
        ofstream fout(filename.c_str(),ofstream::out | ofstream::app);
        fout<<endl<<endl<<"-------------------------------------------------------------------------------"<<endl<<flownumber<<"\ttotally flow number"<<endl<<endl;
        for(int all_nodes=0;all_nodes<numberofnodes;all_nodes++){
		nodes_usage[all_nodes]=0;
	}

 
//--------------------------flow_handling 1019 version-------------------------//
    for (int time_stamp = 0, flow_num = 0; flow_num < flownumber; time_stamp++) {

        //int src = 0;
        //int dst = 0;

        for (int i = 0; i < 9609; i++) {
            int src = demand[i][0];
            int dst = demand[i][1];
            position = src;
            if (poisson_dis(criteria_generate) && (flow_num < flownumber)) {
                flow_num++;
                k++;
                while (position != dst) {
                    nodes_usage[position]++;
                    express_handle(dst);
                }nodes_usage[dst]++;
            }
        }

        entry_destroy(numberofnodes,criteria_terminate);
        
        //fout<<"-------------------------------------------------------------------------------"<<endl<<k<<"\ttotally flow entry"<<endl;
        //cout<<test_cnt<<" testing counter"<<endl;

        //fout<<"testnumber is "<<testnumber<<endl;
        //fout<<"arrival time is "<<time_stamp<<endl;
        //fout<<"number of refused flow's is "<<refused<<endl;
        //fout<<"refused times is "<<recordflow<<endl;
        //fout<<"deleted entries are "<<destory_number<<endl;
        //fout<<"-------------------------------------------------------------------------------"<<endl;
        
    }
    for( std::map<int,int>::iterator cit=nodes_usage.begin() ; cit != nodes_usage.end() ; ++cit){
        //printf("Port #%-5d: %-10d packets\n", cit->first, cit->second);
        fout<<(cit->first)<<"\t"<<(cit->second)<<endl;
    }
    fout.close();
}
