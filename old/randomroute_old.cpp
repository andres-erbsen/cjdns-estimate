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

#define NUM_MAX INT_MAX
typedef int Num;
typedef int Ix;
typedef int Addr;

Ix N, E;
Ix nodes[MAXN];
map<Ix,Num> neighs[MAXN];
map<Ix,Num> remotes[MAXN];
Num shortestpath[MAXN][MAXN];
char sp_done[MAXN][MAXN];
Addr addr[MAXN];

struct Dijk {
	Ix node;
	Num cost;
	Dijk(Ix r, Num c) {
		node=r; cost=c;
	}
	inline bool operator< (const Dijk& other) const {
		if (cost < other.cost) return 0;
		if (cost > other.cost) return 1;
		if (node > other.node) return 0;
		return 1;
	}
};

Addr measure(Ix a, Ix b) {
	// return addr[a] > addr[b] ? addr[a] - addr[b] : addr[b] - addr[a];
	return addr[a] ^ addr[b];
	// return a^b;
}

Num query(Ix n, Ix target) {
	Num ret = 0;
	while (n != target) {
		// printf("%d->",n);
		Num bestcost = NUM_MAX;
		Ix next = 0, m = 0;
		map<Ix,Num>::iterator it;
		priority_queue<Dijk> Q; // frontier
		for ( it=remotes[n].begin(); it != remotes[n].end(); ++it ) {
			Ix v = it->first;
			Q.push(Dijk(v,shortestpath[n][v]));
			if ( measure(target,v) < measure(target,n)
					&& shortestpath[n][v] < bestcost ) { // closer in addrspace
				bestcost = shortestpath[n][v]; // route there :)
				next = v;
			}
		}
		if (bestcost != NUM_MAX) {
			ret += bestcost;
			n = next;
			continue;
		}
		unordered_set<Ix> D; // nodes already expanded
		D.insert(n);
		while (!Q.empty()) {
			Dijk so_far = Q.top(); Q.pop();
			Ix v = so_far.node;
			if (D.find(v) != D.end()) continue;
			D.insert(v);
			if ( measure(target,v) < measure(target,n) ) { // closer in addrspace
				bestcost = so_far.cost; // route there :)
				next = v;
				break;
			}
			map<Ix,Num>::iterator it; // send search query to v
			for ( it=remotes[v].begin(); it != remotes[v].end(); ++it ) {
				Ix w = it->first; Num weight = it->second;
				if ( measure(target,w) <= measure(target,v) && D.find(w) == D.end()) {
					Q.push(Dijk(w,so_far.cost + weight)); // search response
				}
			}
		}
		if (bestcost == NUM_MAX) return -1;
		ret += bestcost;
		n = next;
	}
	return ret;
}

Ix n_remotes(Ix n_neighs) {
	return min( N-(1+n_neighs), (Ix) (
			0.002*N
			));
}

void setremotes() {
	double totalremotes = 0, totalneighs = 0;
	for (Ix ni=0; ni<N; ni++) {
		Ix i = nodes[ni];
		remotes[i].clear();
		remotes[i].insert( neighs[i].begin(), neighs[i].end() );
		if ( !neighs[i].empty() ) {
			// printf("adding %d remotes with neighs=%d N=%d\n", n_remotes(neighs[i].size()), neighs[i].size(), N);
			totalneighs += neighs[i].size();
			Ix g = n_remotes(neighs[i].size());
			totalremotes += g;
			for (Ix j=0; j<g; j++) {
				Ix remote = 0;
				do {remote = nodes[rand() % N];} 
					while ( remote == i  // not self
						|| remotes[i].find(remote) != remotes[i].end() ); // no dupes
				remotes[i][remote] = shortestpath[i][remote];
			}
			assert(remotes[i].size() == neighs[i].size()+ n_remotes(neighs[i].size()));
		}
	}
	printf("%d nodes total. Average %.2f remotes + %0.2f neighs per node\n",N,totalremotes/N, totalneighs/N);
}

void printremotes() {
	printf("\nRemotes:\n");
	for (Ix vi=0; vi<N; vi++) {
		Ix v = nodes[vi];
		printf("%d: ",v);
		map<Ix,Num>::iterator it;
		for ( it=remotes[v].begin(); it != remotes[v].end(); ++it ) {
			Ix w = it->first;
			printf("%d ",w);
		} printf("\n");
	} printf("\n");
}

void printshorestpaths() {
	printf("\nShortest paths:\n");
	printf(" \t");
	for (int i=0;i<N;i++) {
		printf("%d\t",nodes[i]);
	} printf("\n");
	for (int ni=0;ni<N;ni++) {
		Ix i = nodes[ni];
		printf ("%d:\t",i);
		for(int nj=0;nj<N;nj++) {
			Ix j = nodes[nj];
			if (sp_done[i][j]) printf("%d\t", shortestpath[i][j]);
			else printf("\t");
		} printf("\n");
	}
}

void printedges() {
	printf("\nConnections:\n");
	printf(" \t");
	for (int i=0;i<N;i++) {
		printf("%d\t",nodes[i]);
	} printf("\n");
	for (int ni=0;ni<N;ni++) {
		Ix i = nodes[ni];
		printf ("%d:\t",i);
		for(int nj=0;nj<N;nj++) {
			Ix j = nodes[nj];
			if (neighs[i].find(j) != neighs[i].end()) printf("%d\t",neighs[i][j]);
			else printf("\t");
		} printf("\n");
	}
}

void assert_shortest_symmetric() {
	for (int i=0; i<N; i++) { Ix u = nodes[i];
		for (int j=0; j<i; j++) { Ix v = nodes[j];
			assert(shortestpath[u][v] == shortestpath[v][u]);
		}
	}
}

void printaddrdst() {
	printf("\nAddress-space distances:\n");
	printf(" \t");
	for (int i=0;i<N;i++) {
		printf("%d\t",nodes[i]);
	} printf("\n");
	for (int ni=0;ni<N;ni++) {
		Ix i = nodes[ni];
		printf ("%d:\t",i);
		for(int nj=0;nj<N;nj++) {
			Ix j = nodes[nj];
			printf("%d\t", measure(i,j));
		} printf("\n");
	}
}

int main() {
	Ix a = 0, b = 0;
	Addr adr = 0;
	Num c = 0;

	while ( scanf ("%d %i",&a,&adr) == 2 ) {
		addr[a] = adr;
		nodes[N] = a;
		N++;
	}
	scanf("#");
	while ( scanf ("%d %d %d",&a,&b,&c) == 3 ) {
		assert(a < MAXN && b < MAXN);
		neighs[a][b] = c;
		neighs[b][a] = c;
		E++;
	}

	printf("Calculating shorest paths...\n");
    // shortest paths between all nodes using Dijkstras algorithm
	for (Ix s=0; s<N; s++) { Ix start = nodes[s];
		priority_queue<Dijk> Q;
		Q.push(Dijk(start,0));
		while (!Q.empty()) {
			Dijk so_far = Q.top(); Q.pop();
			Ix v = so_far.node;
			if (sp_done[start][v]) continue;
			sp_done[start][v] = 1;
			shortestpath[start][v] = so_far.cost;
			map<Ix,Num>::iterator it;
			for ( it=neighs[v].begin(); it != neighs[v].end(); ++it ) {
				Ix w = it->first; Num weight = it->second;
				if (!sp_done[start][w]) Q.push(Dijk(w,so_far.cost + weight));
			}
		}
	}
	printf("Calculating shorest paths... done.\n");
	//printedges();
	//printshorestpaths();
	//assert_shortest_symmetric();
	//printaddrdst();
	
	setremotes();
	double total=0, failed=0, stretch=0;
	while (total += 1) {
		Ix u,v;
		u = nodes[rand()%N];
		do {v = nodes[rand()%N];} while (u==v);
		Num cjdnspath = query(u,v);
		if (cjdnspath < 0) failed++;
		else {
			assert(cjdnspath >= shortestpath[u][v]);
			assert(sp_done[u][v]);
			assert(0 != shortestpath[u][v]);
			double ratio = cjdnspath / shortestpath[u][v];
			stretch += ratio-1;
		}
		if ( (int)total % (10000000/N) == 0) {
			printf("total=%.0f; failed=%.0f; failrate=%f; stretch=%f;\n",
					total, failed, failed/total, stretch/total );
			setremotes();
		}
	}


	/*
	double tot_stretch = 0;
	Num tot_n = 0;
	while (1) {
		double sum_stretch = 0;
		Num n_stretch = 0, n_fail=0;
		printf("Setting remotes...\n");
		setremotes();
		printf("Setting remotes... done.\n");
	    for (int i=0;i<N;i++) assert(!remotes[nodes[i]].empty());
		//printremotes();
		for (Ix i=0; i<N; i++) {
			if (i%10 == 0) {
				printf("%d/%d\n",i,N);
				printf("Stretch this run: %f, overall average: %f\n)", 
												sum_stretch/n_stretch, 
												tot_stretch/tot_n);
				printf("%d calculated, %d failed, failure rate: %f\n",n_stretch,n_fail,n_fail/(double)(n_stretch+n_fail));
				for (Ix j=0; j<min(N,80); j++) printf("%d ",remotes[nodes[j]].size()); printf("\n");
			}
			for (Ix j=0; j<i; j++) {
				Ix u=nodes[i],v=nodes[j];
				Num cjdnspath = query(u,v);
				if (cjdnspath >= 0) {
					double ratio = ((double)cjdnspath) / ((double) shortestpath[u][v]);
					sum_stretch += ratio - 1;
					// printf("%d / %d -1 = %f @ %d -- %d\n",cjdnspath,shortestpath[u][v],ratio-1,u,v);
					n_stretch++;
					// printf("%d\n", query(u,v));
				} else n_fail++;
			}
		}
		tot_stretch += sum_stretch;
		tot_n += n_stretch;
		printf("Stretch this run: %f, overall average: %f\n)", 
				                        sum_stretch/n_stretch, 
										tot_stretch/tot_n);
		printf("%d calculated, %d failed, failure rate: %f\n",n_stretch,n_fail,n_fail/(double)(n_stretch+n_fail));
		// fails: assert( N*(N-1)/2 == n_stretch);
	}
	*/
	return 0;
}
