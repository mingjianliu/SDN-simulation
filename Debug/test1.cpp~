#include <map>
#include <stdio.h>
#include <iostream>

using namespace std;

typedef map<int, vector<int>> traffic;
typedef map<int, vector<int>>::iterator traffic_iter;

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


int main(int argc, char **argv) {

    vector<vector<int>> demand;
    traffic MultiTraffic;
    if(trafficdemand(demand)){
        cout<<"Error reading trafficdemand"<<endl;
        return -1;
    }
    GenerateMulti(MultiTraffic, demand);
    for (traffic_iter iter = MultiTraffic.begin(); iter != MultiTraffic.end(); iter++)
	for(int number: iter->second)	cout<<number<<"\t";        
	cout<<endl;

    return 0;
}
