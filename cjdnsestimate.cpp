/* vim: set ts=4 sw=4 expandtab : */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <queue>
#include <algorithm>
#include <map>
#include <tr1/unordered_set>

using namespace std;
using namespace tr1;

typedef unsigned int uint;

#define MAXN 10000

uint N, E;
uint nodes[MAXN];
uint addr[MAXN];

map<uint,uint> neighs[MAXN];
map<uint,uint> remotes[MAXN];

uint shortestpath[MAXN][MAXN];
uint hopsbetween[MAXN][MAXN];
bool sp_done[MAXN][MAXN];


struct Dijk {
    uint node, cost, aux;
    Dijk(uint r, uint c) {
        node=r; cost=c;
    }
    Dijk(uint r, uint c, uint a) {
        node=r; cost=c; aux=a;
    }
    inline bool operator< (const Dijk& other) const { // max-priority queue: bigger cost is "less"
        if (cost > other.cost) return 1;
        if (cost < other.cost) return 0;
        if (aux > other.aux) return 1;
        if (aux < other.aux) return 0;
        return (node > other.node);
    }
};

uint measure(uint a, uint b) {
    return addr[a] ^ addr[b];
}

uint n_remotes(uint u) {
    uint possible = N - 1 - uint(remotes[u].size());
    uint calculated = N/500 * neighs[u].size()/(E*2.0/N);
    return min(possible, calculated);
}

uint setremotes() {
    uint count = 0, u = 0, v = 0, needed = 0;
    for (uint i=0; i<N; i++) { u = nodes[i];
        remotes[u].clear();
        remotes[u].insert( neighs[u].begin(), neighs[u].end() ); // neighs act as remotes
        needed = n_remotes(u);
        for (uint j=0; j<needed; j++) { // add a new, unique remote
            do { v = nodes[rand() % N]; } while ( v==u || remotes[u].find(v) != remotes[u].end() );
            remotes[u][v] = shortestpath[u][v];
        }
        count += needed;
    }
    return count;
}

uint search(uint u, uint target, uint* next) {
    unordered_set<uint> Done; Done.insert(u); // nodes already searched.
    priority_queue<Dijk> Q; // Queue of nodes for sending search queries
    for (map<uint,uint>::iterator it=remotes[u].begin(); it != remotes[u].end(); ++it ) {
        uint v = it->first, weight = it->second;
        // we can send search queries to all reamotes
        #ifdef BY_HOPS
        Q.push(Dijk(v, hopsbetween[u][v], weight));
        #elif defined BY_ADDR
        Q.push(Dijk(v, measure(u,v), weight ));
        #else
        Q.push(Dijk(v, weight)); // by distance
        #endif
    }
    while (!Q.empty()) {
        Dijk so_far = Q.top(); Q.pop();
        uint v = so_far.node;
        if (Done.find(v) != Done.end()) continue;
        Done.insert(v);
        // We sent a search query to v. Iterate over all nodes that he can respond with
        for (map<uint,uint>::iterator it=remotes[v].begin(); it != remotes[v].end(); ++it) {
            uint w = it->first; uint weight = it->second;
            // valid search responses are closer in address space than the responding node
            if ( w != u && measure(target,w) < measure(target,v)) { 
                if ( measure(target,w) < measure(target,u) ) { // closer in addres space than us
                    *next = w; // route there :)
                    #ifdef BY_HOPS
                    return so_far.aux;
                    #elif defined BY_ADDR
                    return so_far.aux;
                    #else
                    return so_far.cost;
                    #endif
                }
                // we can also search through the nodes in the response
                if (Done.find(w) == Done.end()) {
                    #ifdef BY_HOPS
                    Q.push(Dijk(w, so_far.cost+1, so_far.aux+weight));
                    #elif defined BY_ADDR
                    Q.push(Dijk(w, measure(u,w), so_far.aux+weight ));
                    #else
                    Q.push(Dijk(w, so_far.cost+weight)); // by distance
                    #endif
                }
            }
        }
    }
    // found nothing
    return UINT_MAX;
}

uint query(uint u, uint target) {
    uint cost = 0, next = 0;
    while (u != target) {
        // Route through a known node which is the shortest physical distance while still being closer in address space
        uint min_weight = UINT_MAX;
        for (map<uint,uint>::iterator it=remotes[u].begin(); it != remotes[u].end(); ++it ) {
            uint v = it->first, weight = it->second;
            if (v == target) return cost + weight;
            if ( measure(v,target) < measure(u,target) && weight < min_weight ) {
                min_weight = weight;
                next = v;
                #ifdef BY_HOPS
                break; // all are 1 hop away so cmp by hops is arbitrary choice. So be it.
                #endif
            }
        }
        if (min_weight == UINT_MAX) { // no known nodes closer in address space than us
            min_weight = search(u,target,&next); // let's see if somebody else knows one
            if (min_weight == UINT_MAX) { // didn't work either. Giving up.
                return UINT_MAX;
            }
        }
        cost += min_weight;
        u = next;
    }
    return cost;
}


int main() {
    uint a = 0, b = 0, cost = 0, adr = 0;
    // input a graph in trivial graph format. The graph MUST be connected
    while ( scanf ("%d %i",&a,&adr) == 2 ) {
        addr[a] = adr;
        nodes[N] = a;
        N++;
    }
    scanf("#");
    while ( scanf ("%d %d %d",&a,&b,&cost) == 3 ) {
        assert(a < MAXN && b < MAXN);
        neighs[a][b] = cost;
        neighs[b][a] = cost;
        E++;
    }
    srand(N*E-1);

    // shortest paths between all nodes using repeated Dijkstra's algorithm
    // fprintf(stderr,"Calculating shorest paths...\n");
    for (uint s=0; s<N; s++) { uint start = nodes[s];
        priority_queue<Dijk> Q;
        Q.push(Dijk(start,0,0)); // we can reach node `start` by covering 0 distance in 0 hops
        while (!Q.empty()) {
            Dijk so_far = Q.top(); Q.pop();
            uint v = so_far.node;
            if (sp_done[start][v]) continue;
            sp_done[start][v] = 1;
            shortestpath[start][v] = so_far.cost;
            hopsbetween[start][v] = so_far.aux;
            for (map<uint,uint>::iterator it=neighs[v].begin(); it != neighs[v].end(); ++it ) {
                uint w = it->first, weight = it->second;
                if (!sp_done[start][w]) Q.push(Dijk(w, so_far.cost+weight, so_far.aux+1));
            }
        }
    }
    // fprintf(stderr,"Calculating shorest paths... done.\n");
    
    uint total=100000, R=setremotes(), failed=0, u=0, v=0;
    float total_stretch=0;
    for (uint i=0; i<total; i++) {
        u = nodes[rand()%N]; // pick a node at random
        do {v = nodes[rand()%N];} while (u==v); // pick another, different node
        uint cjdnspath = query(u,v);
        if (cjdnspath == UINT_MAX) failed++;
        else total_stretch += cjdnspath * 1.0/shortestpath[u][v];
    }

    printf("Nodes:\t%d\n",N);
    printf("Edges:\t%d\n",E);
    printf("Extra remotes:\t%d\n",R);
    printf("Samples:\t%d\n",total);
    printf("== Average values ==\n");
    printf("Neighbour degree:\t%f\n", E*2.0/N);
    printf("Remote degree:\t%f\n", R*1.0/N);
    printf("Total degree:\t%f\n", E*2.0/N + R*1.0/N);
    printf("== Results ==\n");
    printf("Failure rate:\t%f\n",failed*1.0/total);
    printf("Route Stretch\t%f\n",total_stretch*1.0/(total-failed));
    return 0;
}
