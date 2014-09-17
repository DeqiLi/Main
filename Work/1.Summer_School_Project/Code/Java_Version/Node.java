package CH_Java;

// nodes of the graph G

class Node {  // the index is node's ID, not order
/*	final int NON_EXIST = 0;
	final int CONTRACTED = 1; 
	final int SETTLED=3;
	final int EXIST=127;
	final int INFINITE = Integer.MAX_VALUE;
*/	
	public int OD;  // order used in CH algo
	//adList* adlistin; //the head of adjacency list; list:for incoming edges and nodes
	//-- adList* adlistin_end; // the end of the adlist
	public adList adlistout; 

	// running time parameters:
	public int dist[] = new int[2];   // distance to source in DJ algo
	// int previous[2];  // ID of previous node in DJ algo
	public byte state; // used in CH algo and DJ algo  //0: contracted / unreached;  1: normally exist / settled;  2: reached
	// int state;
	public FibonacciHeapNode fp[] = new FibonacciHeapNode[2];  // a pointer pointing to the FibonacciHeapNode in Fibonacci heap corresponding to this node in graph
	public int bp[] = new int[2]; // binary heap node pointers / indices

	Node() { // initial the node
		//-- ID = 0;
		OD = 0;
		state = Metadata.NON_EXIST;  // 0
		//adlistin = null;
		//-- adlistin_end = null;
		adlistout = null; 
		dist[0] = Metadata.INFINITE; 
		dist[1] = Metadata.INFINITE;
	    //previous[0] = 0;
		fp[0] = null;
		fp[1] = null;
		bp[0] = -1;
		bp[1] = -1;
	}
}

 