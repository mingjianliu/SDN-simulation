#include <map>
#include <vector>

using namespace std;

typedef vector <int> destination;
typedef map<int, destination> traffic;
typedef map<int, destination>::iterator traffic_iter;

struct traffic_node {
    int label;
    map<int, traffic_node*> traffic_output;
};

traffic_node* init_treenode(int number) {
    traffic_node *ret = (traffic_node *)malloc(sizeof(traffic_node));
    ret->label = number;
    return ret;
}

void GenerateMulti_Tree(traffic &MultiTraffic, map<int, traffic_node*> &traffic_tree, int[][][] &ShortestPath, int policy) {
    for (traffic_iter iter = MultiTraffic.begin(); iter != MultiTraffic.end(); iter++) {

        traffic_node* root = init_treenode(iter->first);
        traffic_tree.insert({iter->first, root});

        for(int destination: iter->second) {
            //Use DFS to insert every nodes from root
            traffic_node* node = root;
            //1. Get the shortest path
            vector<int> path;
            int position = iter->first;
            while(position!=iter->second) {
                position = ShortestPath[policy][position][iter->second];
                path.push_back(position);
            }

            //2. Using vector path, traverse the tree
            for(int cur : path) {
                auto temp = node->traffic_output.find(cur);
                if( temp == node->traffic_output.end() ) {
                    node->traffic_output.insert({cur,init_treenode(cur)});
                    temp = node->traffic_output.find(cur);
                }
                node = temp->second;
            }
        }
    }
    return;
}

