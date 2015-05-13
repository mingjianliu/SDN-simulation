#include <stdio.h>
#include <map>
#include <iostream>
#include <vector>

using namespace std;

class Switches{
    unsigned int total;
    vector <int> count;
public:
    Switches(unsigned int size) : total(0), count( size, 0 ), temp2( new map < int, int>[size] ), switch_size(size) {} ;
    unsigned int Total_Usage() { return total; }
    vector <int> Usage() { return count; }
    void Destroy( int time ){
        unsigned int number = 0;
        for(int i = 0; i < switch_size; ++i){
            for(auto iter : temp2[i]){
                if (iter.first > time){ break;}
                number += iter.second;
                count[i] -= iter.second;
                temp2[i].erase (iter.first);
            }
        }
        total -= number;
    }
    void Insert( int time, vector <int> nodes ){
        total += nodes.size();
        for(int node : nodes){
            count[node]++;
            auto iter = temp2[node].find(time);
            if( iter == temp2[node].end() ){
                temp2[node].insert({time, 1});
            }else{
                iter->second++;
            }            
        }
        return;
    }
          
private:
    unsigned int switch_size;
    map < int, int >* temp2; //key = time, value = entry number
};

int main(int argc, char **argv) {
    Switches temp ( 10 );
    vector <int> usage;
    vector <int> temp1{1,2,3,4,5};
    temp.Insert( 0, temp1 );
    usage = temp.Usage();
    for(int x : usage){
        cout << x << "\t";
    }
    cout << endl;
    temp.Destroy (0);

    vector <int> temp2{1,2,3,4,5};
    //temp.Insert( 1, temp2 );
    usage = temp.Usage();
    for(int x : usage){
        cout << x << "\t";
    }
    cout << endl;
    


    return 0;
}


/*


    vector <int> temp2{1,2,3,4,5};
    temp.Insert( 1, temp2 );
    usage = temp.Usage();
    for(int x : usage){
        cout << x << "\t";
    }
    cout << endl;


    vector <int> temp1 = temp.Usage();
    //for( int number : temp1 )    cout << "\t" << number << endl;
    map < int, int>* temp2 = new map < int, int>[10] ;
    temp2[0].insert({1,1});
    temp2[1].insert({1,2});
    temp2[2].insert({1,3});
    temp2[3].insert({1,4});
    //cout << temp2[2].find(1)->second << endl;

    for(int i = 0; i < temp1.size(); ++i){
        cout << temp2[i].find(1)->second << endl;} 

*/
