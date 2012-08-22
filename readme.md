Simulation of a simplified version of the cjdns routing algorithm to estimate the stretch of
the routes found by cjdns. Stretch is defined as length of route found divided
by the length of the best route, where (abstract) length represents the actual combination
of qualities of a route we care about.

The graph of the network is read from stdin in trivial graph format.
See `ex2.tgf` an for example, the file format is the following:

	node_id address
	...
	#
	node_id node_id length
	...


In the code, the nodes that a single node is directly connected with are
called that nodes neighbors and the nodes it knows that are in arbitrary
locations are called remotes. All neighbors are also remotes. Other remotes
are picked randomly. Routes to them are assumed to be optimal. These lists
are static. The number of remotes as a function of the number of neighbors
is a parameter of the model. Number of remotes per node is given as command
line argument to the program.

- if the node the packet starts at knows nodes that are closer in address
space to the target than the starting node, it sends the packet to the physically closest
(minimal length away) of them, to be routed the same way as a new packet.
- if it doesn't, it searches for new nodes using a remote node that is
closest by route length to the node itself. Alternative selection can be
achieved by defining one of the following macros:
 - `BY_ADDR`: Pick closest in address space to the target
 - `BY_HOPS`: Do not consider actual path length, only number of hops in it.
Also picks an arbitrary node instead of the closest if possible next hop is known.

The remote node used for search returns all remotes it knows that are
closer to the target in address space than itself. They are added to
the querying nodes routing table for the duration of routing this packet.
When the querying finds out about a node that is closer to the target in
address space than it is itself, it sends the packet to that node through
the node using which it was found.
If a such node has not been found after searching using all known remotes,
this route is considered to be failed. Keeping track of closest nodes in
the routing table is implemented using a priority queue a la Dijkstras
algorithm.

Average route stretch and failure rate estimation are calculated by trying
to route packets from a randomly picked node to another randomly picked node.

`asgr.tgf` is the graph of autonomous systems in the Internet.

The current results (if not noted otherwise) are for the variant WITHOUT `BY_HOPS` and `BY_ADDR`.
