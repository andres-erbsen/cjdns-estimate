A simplified version of the cjdns routing algorithm to estimate the stretch of
the routes found by cjdns. Stretch is defined as length of route found divided
by the length of the best route, where (abstract) length represents the actual combination
of qualities of a route we care about.

The graph of the network is read from stdin in trivial graph format.

In the code, the nodes that a single node is directly connected with are
called that nodes neighbors and the nodes it knows that are in arbitrary
locations are called remotes. All neighbors are also remotes. Other remotes
are picked randomly. Routes to them are assumed to be optimal. These lists
are static. The number of remotes as a function of the number of neighbors
is a parameter of the model. At the moment, the number of remotes is linearly
scaled with `N_nodes/500` for nodes with average number of neighbors. In reality
this is a lot smaller, but with the graphs used it would have left some nodes
without any remotes at all.

There are two similar versions, which simulate routing packets as follows:

- if the node the packet starts at knows nodes that are closer in address
space to the target than the starting node, it sends the packet to the physically closest
(minimal length away) of them, to be routed the same way as a new packet.
- if it doesn't, it searches for new nodes using a remote node that
 - `follow_addr`: is closest in address space to the target
 - `follow_closest`: is closest by route length to the node itself
the remote node used for search returns all remotes it knows that are
closer to the target in address space than itself. They are added to
the querying nodes routing table for the duration of routing this packet.
When the querying finds out about a node that is closer to the target in
address space than it is itself, it sends the packet to that node.
If a such node has not been found adter searching using all known remotes,
this route is considered to be failed. Keeping track of closest nodes in
the routing table is implemented using a priority queue a la Dijkstras
algorithm.

Average route stretch and failure rate estimation are calculated by trying
to route packets from a randomly picked node to another randomly picked node.
