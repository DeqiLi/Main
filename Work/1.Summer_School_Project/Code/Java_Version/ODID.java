package CH_Java;

//////////////////////////////////////////////////////////////
// Begin of class graph.
//////////////////////////////////////////////////////////////
class ODID implements Comparable<ODID>{  // for mapping OD to ID of a node;  "OD": OrDer
	public int ID;
	public int OD;
//	ODID(int id, int od) { ID = id; OD = od; }

	@Override
	public int compareTo(ODID other) {
	    return this.OD - other.OD;
	}
	
} // if sort on ArrayList<Node> node, then we have two problems:
  // 1. node uses indices as IDs of nodes; that is, node[100] <--> vertex=100. So after sorting we don't know node[100] <--> vertex=? 
  // 2. node object is much bigger than ODID, so the sorting is much time-consuming. 
  // So we need ODID to map OD to ID.
	