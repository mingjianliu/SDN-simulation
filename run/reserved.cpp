int selecthops;
int arrayh[MAX_NODE];

int pathcost[MULTIPLE][MAX_NODE][MAX_NODE];
int shortestpath[MULTIPLE][MAX_NODE][MAX_NODE];
int pathlength[MULTIPLE][MAX_NODE][MAX_NODE];


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
};

multimap <int,vector<int>>      flow_setup;

struct node switch_data[MAX_NODE];
struct flow temp_flow;

typedef vector<int>::iterator iter_vector;
typedef multimap <int,vector<int>>::iterator iter_multimap;
typedef multimap <int,vector<int>>::size_type list_size;


//////////////added parameter///////////////////
int refused=0;
int recordflow=99999;
int flowtime=0;

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

int k= 0;
int key_value;
///////////////////////////////////////////////////

/*------------------------------------------------------------------------
	This part convert the number to a string in the purpose of I/O
	Input
------------------------------------------------------------------------*/
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
	This part take the input trafficdemand1.txt as input and get the demand[] as E for all edges
	Input	:	trafficdemand1.txt
	Output	:	demand[9609][0:src,1:dst]
------------------------------------------------------------------------*/
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
        int x,y,n,i=0;
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

/*------------------------------------------------------------------------
	This part record the node to node cost, save it in pathcost
	Input	:	topology.txt, numberofnodes, (global)pathcost[policy][][]
	Output	:	0 or -1
------------------------------------------------------------------------*/
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
        cout<<"File doesn'td exist."<<endl;
        fclose(file);
        return -1;
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

    return 0;
}


/*------------------------------------------------------------------------
	This part calculate x to the power of p
	Input	:	policy, numberofnodes, (global) shortestpath, pathcost, pathlength
	Output	:
------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------
	This part is now used by Alpha_beta objective function
	Input	:	x, y, z, w
	Output	:
------------------------------------------------------------------------*/
int findpath(int x, int y, int z,int w) {
    if(x==0) {
        //cout<<(pathlength-x)<<" the end"<<endl;
        return 0;
    }
    /*when in the middle, we try to find out the minimum of maximum path recursively*/
    else {
        int min_max=MAX_ENTRY;
        int number=0;
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

/*------------------------------------------------------------------------
			Max Hop - Openflow method
	This function calculates next HOPS' node
	Input	:	policy, dst, (global) HOPS
	Output	:
------------------------------------------------------------------------*/
void path_request1(int policy, int dst) {
    int  temp[MAX_NODE];
    int  label=0;

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
    for(int i=0; i<label; ++i) {
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
    {
        if (number/myPow(2,i))
        {
            number1=length-i;
        }
    }
    return number1;
}

//-------------------------------the new objective function-------------------------------
double path_cost2(double average, int length, int number)
{

    double costx=0.0;
    int temp=0;

    for(int i=0; i<length; ++i) {
        //tempnumber=number%(myPow(2,i+1))-number%(myPow(2,i));  //Get the bit in ith hop

        int tempnumber=number/(myPow(2,i))-(number/myPow(2,i+1))*2;

        int temp_count=switch_data[temppath[i]].cnt;

        if(!tempnumber)
        {
            temp++;
            if(temp>=HOPS) {
                return 999999.99;   //If no less than the max number of hops are 0, then it is not valid, return big value as invalid
            }
        }
        else
        {
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
        }

    }
    return costx;
}

/*------------------------------------------------------------------------
	This function randomly generate some flow, flownumber represents how many rules to generate, with different probability, the density changes
	Input	:	flownumber, numberofnodes, criteria_generate
	Output	:
------------------------------------------------------------------------*/
void rand_generation_unicast(int flownumber,int numberofnodes,float criteria_generate) {

    int flow_num = 0;
    for (int time_stamp = 0; flow_num < flownumber; time_stamp++)
    {
        for (int tmp_rule = 0; tmp_rule < 9609; tmp_rule++)
        {

            if(poisson_dis(criteria_generate) && (flow_num < flownumber))
            {
                //from 0 to 9609, with probability, randomly generate some flows
                vector <int> flow_in;
                flow_in.push_back(demand[tmp_rule][0]);
                flow_in.push_back(demand[tmp_rule][1]);
                flow_in.push_back(time_stamp + uni_dis());


                flow_setup.insert(make_pair(time_stamp,flow_in));
                flow_in.clear();
                flow_num++;
            }
        }
    }
}

/*------------------------------------------------------------------------
	This function randomly generate some flow, flownumber represents how many rules to generate, with different probability, the density changes
	Input	:	flownumber, numberofnodes, criteria_generate
	Output	:
------------------------------------------------------------------------*/
void rand_generation_multicast(int flownumber,int numberofnodes,float criteria_generate) {


    return;
}

