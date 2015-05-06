#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;

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


int main(int argc, char **argv) {

    vector<vector<int>> x;
    if(trafficdemand(x)){
	cout<<"Error"<<endl;
	return -1;
    }
    for(vector<int> y: x){
	cout<<"Source is "<<y[0]<<", Destination is "<<y[1]<<endl;
    }
    return 0;
}
