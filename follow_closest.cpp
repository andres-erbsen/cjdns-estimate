/* vim: set ts=4 sw=4 : */
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

#define MAXN 10000
typedef unsigned int uint;

uint N, E;
uint nodes[MAXN];
map<uint,uint> neighs[MAXN];
map<uint,uint> remotes[MAXN];
uint shortestpath[MAXN][MAXN];
bool sp_done[MAXN][MAXN];
uint addr[MAXN];

struct Dijk {
	uint node, cost;
	Dijk(uint r, uint c) {
		node=r; cost=c;
	}
	inline bool operator< (const Dijk& other) const {
		if (cost < other.cost) return 0;
		if (cost > other.cost) return 1;
		if (node > other.node) return 0;
		return 1;
	}
};

uint measure(uint a, uint b) {
	// return addr[a] > addr[b] ? addr[a] - addr[b] : addr[b] - addr[a];
	return addr[a] ^ addr[b];
}

uint n_remotes(uint u) {
	uint possible = N - 1 - uint(remotes[u].size());
	uint suggested = N/500 * neighs[u].size()/(E*2.0/N);
	return min(possible,suggested);
}

uint setremotes() {
	uint count = 0;
	for (uint i=0; i<N; i++) {  uint u = nodes[i];
		remotes[u].clear();
		remotes[u].insert( neighs[u].begin(), neighs[u].end() );
		uint needed = n_remotes(u);
		count += needed;
		for (uint j=0; j<needed; j++) {
			uint v = nodes[rand() % N];
			while ( v == u || remotes[u].find(v) != remotes[u].end() )
				 v = nodes[rand() % N];
			remotes[u][v] = shortestpath[u][v];
		}
	}
	return count;
}

uint query(uint u, uint target) {
	uint cost = 0;
	while (u != target) {
		uint min_weight = UINT_MAX;
		uint next;
		map<uint,uint>::iterator it;
		priority_queue<Dijk> Q; // frontier of possible searches to come
		for ( it=remotes[u].begin(); it != remotes[u].end(); ++it ) {
			uint v = it->first; uint weight = it->second;
			if (v == target) return cost + weight;
			Q.push(Dijk(v,weight));
			if ( measure(v,target) < measure(u,target) && weight < min_weight ) {
				min_weight = shortestpath[u][v];
				next = v;
			}
		}
		if (min_weight != UINT_MAX) { // no need to search
			cost += min_weight;
			u = next;
			continue;
		}
		unordered_set<uint> D; D.insert(u); // nodes already expanded
		while (!Q.empty()) {
			Dijk so_far = Q.top(); Q.pop();
			uint v = so_far.node;
			if (D.find(v) != D.end()) continue;
			D.insert(v);
			if ( measure(target,v) < measure(target,u) ) { // closer in addrspace
				u = v;
				min_weight = so_far.cost;
				cost += so_far.cost; // route there :)
				break;
			}
			map<uint,uint>::iterator it; // send search query to v
			for ( it=remotes[v].begin(); it != remotes[v].end(); ++it ) {
				uint w = it->first; uint weight = it->second;
				if ( measure(target,w) <= measure(target,v) && D.find(w) == D.end()) {
					Q.push(Dijk(w,so_far.cost + weight)); // search response
				}
			}
		}
		if (min_weight == UINT_MAX) return UINT_MAX;
	}
	return cost;
}


int main() {
	uint a = 0, b = 0, cost = 0, adr = 0;
	// input a graph in "trivial graph format". The graph MUST be connected
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

	fprintf(stderr,"Calculating shorest paths...\n");
    // shortest paths between all nodes using Dijkstras algorithm
	for (uint s=0; s<N; s++) { uint start = nodes[s];
		priority_queue<Dijk> Q;
		Q.push(Dijk(start,0));
		while (!Q.empty()) {
			Dijk so_far = Q.top(); Q.pop();
			uint v = so_far.node;
			if (sp_done[start][v]) continue;
			sp_done[start][v] = 1;
			shortestpath[start][v] = so_far.cost;
			map<uint,uint>::iterator it;
			for ( it=neighs[v].begin(); it != neighs[v].end(); ++it ) {
				uint w = it->first; uint weight = it->second;
				if (!sp_done[start][w]) Q.push(Dijk(w,so_far.cost + weight));
			}
		}
	}
	fprintf(stderr,"Calculating shorest paths... done.\n");
	
	// sample length of path between random nodes
	float total=0, failed=0, stretch=0;
	while (1) {
		float R = setremotes();
		for (uint i=0; i<1000; i++) {
			total += 1;
			uint u,v;
			u = nodes[rand()%N];
			do {v = nodes[rand()%N];} while (u==v);
			uint cjdnspath = query(u,v);
			if (cjdnspath == UINT_MAX) failed++;
			else {
				assert(cjdnspath >= shortestpath[u][v]);
				assert(sp_done[u][v]);
				assert(0 != shortestpath[u][v]);
				float ratio = cjdnspath / shortestpath[u][v];
				stretch += ratio;
			}
		}
		fprintf(stderr,"N=%d; remotes=%.4f; neighs= %0.4f;\n",
				   N,          R/N,        E*2.0/N);
		fprintf(stderr,"total=%.0fK; failed=%.0f; failrate=%f; stretch=%f;\n",
				total/1000, failed, failed/total, stretch/(total-failed) );
		setremotes();
	}
	return 0;
}
