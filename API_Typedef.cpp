#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>
#include <random>

#define DURATION 100

using namespace std;


struct flows{
        int src;
        vector<int> dst;
        std::map<int, int> endtime;
};


typedef vector <int> destination;
typedef map<int, destination> traffic;
typedef map<int, destination>::iterator traffic_iter;

default_random_engine generator(time(NULL));
uniform_int_distribution<int> dstGeneration(1,50);
uniform_int_distribution<int> durationGeneration(1,DURATION);
auto uniRandn = bind(dstGeneration, generator);
auto durationRandn = bind(durationGeneration, generator);


flows* Generate_Flows(traffic &MultiTraffic, float criteria){
	flows *currentFlow = new flows;
	//int src = currentFlow->src;
	int src = 1;
	destination tempDst;
	std::map<int,int> tempEndTime;
	destination dstPattern = MultiTraffic[src];
	for(vector<int>::iterator iter = dstPattern.begin(); iter != dstPattern.end();++iter){
		if(float(uniRandn()/50.0) < criteria)
			tempDst.push_back(*iter);
			tempEndTime.insert(make_pair(*iter, durationRandn()));
	}
	currentFlow->dst = tempDst;
	currentFlow->endtime = tempEndTime;
	return currentFlow;
}
 int main(void){
 	destination testDes;
 	for(int i = 0; i < 10;i++){
 		testDes.push_back(i);
 	}
 	traffic testTraffic;
 	testTraffic.insert(make_pair(1, testDes));
 	testTraffic.insert(make_pair(2, testDes));
 	flows *test = Generate_Flows(testTraffic, 0.4);
 	for(vector<int>::iterator iter = test->dst.begin(); iter != test->dst.end();++iter){
 		cout<<*iter<<" "<<test->endtime[*iter]<<endl;
 	}
 	return 0;
 }
