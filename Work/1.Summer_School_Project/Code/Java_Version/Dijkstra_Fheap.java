package CH_Java;
import java.util.ArrayList;

//////////////////////////////////////////////////////////////
// Begin of  class Dijkstra_Fheap.
//////////////////////////////////////////////////////////////

// Dijkstra's algo based on Fibonacci heap
class Dijkstra_Fheap {  
	private	ArrayList<Integer>visited;  // record all visited (reached and settled) nodes for reseting their state to EXIST, and delete pointers of FiboNodes, set node[].dist to INFINITE.
	private	int nVisitedNodes;  // it is set by visitedNodesReset() 
	private ArrayList<Node> node;
	public int getNVisitedNodes() { return nVisitedNodes; }  // nVisitedNodes is the size of W[]. nVisitedNodes is set in visitedNodesReset() // -- only for test

	Dijkstra_Fheap(ArrayList<Node> node0) { node = node0; visited = new ArrayList<Integer>(); } 
	//~Dijkstra_fheap() { } 

	/** 
	 * Compute single-source single destination shortest distance from s to t.
 	 * It's an implementation of the traditional single-directional DJ algo.
     * This is to verify results of CH algo or bidi DJ algo.
     * It is twice slower than bidi version of DJ but its correctness is believed guaranteed.
	 */
	public int classicalDJ(int s, int t) {//, ReadRoad *g) {			
		adList p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative; // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually

		//int maxHeapSize = 0;

		if(s == t)	return 0;
		if(node.get(s).state == Metadata.NON_EXIST || node.get(t).state == Metadata.NON_EXIST) {
			System.out.print( "source or target don't exist: "+ s + "\n"); 
			return 0;
		}
	
		////assert(node.get(s).state != NON_EXIST);
		FibonacciHeap fheap = new FibonacciHeap();  
		FibonacciHeapNode U = new FibonacciHeapNode(s, 0);
		fheap.insert(U);    // insert source (s, 0)	
		node.get(s).dist[0] = 0;
		node.get(s).fp[0] = U;
		dist = 0;  
		visited = new ArrayList<Integer>();
		visited.add(s);  // !!

		while(!fheap.isEmpty()) {
			//U = fheap.removeMin();  // O(logn) ---- the most time-consuming operation!!!
			//id = U.data;             // euqally, id = node[fp.data].ID;	
			id = fheap.removeMin().data;

			//if(maxHeapSize < fheap.size()) maxHeapSize = fheap.size();

			dist = node.get(id).dist[0]; //  dist = node[fp.data].dist
			node.get(id).state = Metadata.SETTLED;
			if(id == t) {				
				//System.out.print( "\nMaxHeapSize(single):" + maxHeapSize);
				visitedNodesReset();
				return dist; 
			}
			p = node.get(id).adlistout;
			while(p!=null) {  // for each neighbor v of u:  
				id_v = p.ID;
				v = node.get(id_v);		// v points to one neighbour of u
				if(v.state == Metadata.SETTLED) { // || v.state == CONTRACTED) {
					p=p.ptr;
					continue;
				}
				weight = p.wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v.dist[0]) {
					if(v.dist[0] == Metadata.INFINITE) {  // this implies v is UNREACHED on this direction
						v.dist[0] = tentative;      
						v.fp[0] = new FibonacciHeapNode(id_v, tentative);    
						fheap.insert(v.fp[0]); 
						visited.add(id_v);
					}
					else{
						v.dist[0] = tentative;      
						fheap.decreaseKey(v.fp[0], v.dist[0]); 
					} 
				}
				p = p.ptr; //next neighbour of u
			}
		}

		//System.out.print( "\nmaxHeapSize: " + maxHeapSize);

		visitedNodesReset();
		return dist;
	}

	/** 
	 * Compute all shortest distance from one source s to multiple destinations in W[].
 	 * For finding witness path when contracting a node u.
	 * Use single-directional DJ algo.
     * This is O(d) times faster than one-to-one DJ local search for many destinations separately,
	 * where d is the average degree of nodes in the graph.
	 * 
	 * param s: the single source node
	 * param vector<MultiDestination>& W: it contains the multiple destinations
	 *
	 * The idea above is proposed by Deqi Li, 2014.1.12
	 */
	void onetoManyClassicalDJ(int s, ArrayList<MultiDestination> W, int maxLength) {//, ReadRoad *g) {			
		adList p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int nSettled=0;
		int sizeW = W.size();
		
		visited = new ArrayList<Integer>();
		visited.add(s);  // !!  -- These visited nodes should be recorded for restoring their states

		////assert(node.get(s).state != NON_EXIST);
		FibonacciHeap fheap = new FibonacciHeap();  
		FibonacciHeapNode U = new FibonacciHeapNode(s, 0);
		fheap.insert(U);    // insert source (s, 0)	
		node.get(s).dist[0] = 0;
		node.get(s).fp[0] = U;
		dist = 0;  

		
		//System.out.println(" s: "+s);
		
		
		while(!fheap.isEmpty()) {
			//U = fheap.removeMin();  // O(logn) ---- the most time-consuming operation!!!
			//id = U.data;             // euqally, id = node[fp.data].ID;
			id = fheap.removeMin().data;
			
			
			//System.out.println("  Min: "+id);
			
			
			dist = node.get(id).dist[0]; //  dist = node[fp.data].dist
			node.get(id).state = Metadata.SETTLED;
			nSettled++;

			if(dist > maxLength) {  // Proposed by Deqi Li, Jan 19,2014.
				visitedNodesReset(); 
				return; 
			}

			if(nSettled >= sizeW) {  // Proposed by Deqi Li, Jan 11,2014.
				if(allSettled(W)) {
					visitedNodesReset(); 
					return; 
				}
			}

			p = node.get(id).adlistout;
			while(p!=null) {  // for each neighbor v of u:  
				id_v = p.ID;
				
				
				//System.out.println("  id_v: " + id_v);
				
				
				v = node.get(id_v);		// v points to one neighbour of u
				if(v.state == Metadata.SETTLED || v.state == Metadata.CONTRACTED) {
					p=p.ptr;
					continue;
				}
				weight = p.wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v.dist[0]) {
					if(v.dist[0] == Metadata.INFINITE) {  // this implies v is UNREACHED on this direction
						v.dist[0] = tentative;      
						v.fp[0] = new FibonacciHeapNode(id_v, tentative);    
						fheap.insert(v.fp[0]); 
						visited.add(id_v);
					}
					else{
						v.dist[0] = tentative;      
						fheap.decreaseKey(v.fp[0], v.dist[0]); 
					} 
				}
				p = p.ptr; //next neighbour of u
			}
		}
		return;
	}


	/*
	/////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                      /////
	//   The following is the main body of bidirectional Dijkstra's algorithm               /////
	//   based on Fibonacci heap.                                                           /////
	//                                                                                      /////
	/////////////////////////////////////////////////////////////////////////////////////////////
	*
	*  Coded by Deqi Li, 2013.12.31
	*/

	int bidi_classicalDJ(int s, int t) { //, ReadRoad *g) {	// s and t are the ID of source node and destination node of graph		
		adList p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = Metadata.INFINITE;    // tentative best shortest length of (s, t)
		int flag = 1;  // turns to forward search(true) or backward search(false)
 
		if(s == t)	return 0;
		if(node.get(s).state == Metadata.NON_EXIST || node.get(t).state == Metadata.NON_EXIST) {
			System.out.print( "source or target don't exist: "+ s + "\n"); 
			return 0;
		}

		FibonacciHeap[] fheap = new FibonacciHeap[2]; 
		fheap[0] = new FibonacciHeap();
		fheap[1] = new FibonacciHeap();
		FibonacciHeapNode U = new FibonacciHeapNode(s, 0);
		fheap[flag].insert(U);    // insert source (s, 0) into Fibo heap
		node.get(s).dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node.get(s).fp[flag] = U;     //   the associated Fibo Node storing ID and dist of this node
		node.get(s).state = (byte) (Metadata.SETTLED*(flag+1));  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= Metadata.INFINITE -- initially when it is unreached, dist==INFINITE)
		//node.get(s).previous[flag] = -1;  // the previoud node on shortest path; for s and t, this is -1 -- non existing node.

		U = new FibonacciHeapNode(t, 0);
		fheap[1-flag].insert(U);   
		node.get(t).dist[1-flag] = 0;
		node.get(t).fp[1-flag] = U;
		node.get(t).state = (byte) (Metadata.SETTLED*flag);
		//node.get(t).previous[1-flag] = -1;
		visited = new ArrayList<Integer>();
		visited.add(s);  // !!
		visited.add(t);  // !!
		dist = 0;  

		while(!fheap[flag].isEmpty() || !fheap[1-flag].isEmpty()) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(fheap[flag].isEmpty()) { flag = 1-flag; }

			//U = fheap[flag].removeMin();   
			//id = U.data;  
			id = fheap[flag].removeMin().data;

			dist = node.get(id).dist[flag];  
			
			if(node.get(id).state == Metadata.SETTLED*(2-flag)) {// deleted from both queues, so terminate! "bidirectional DJ_lecture17.pdf", P13
				visitedNodesReset();
				return best; 
			}
			if(!fheap[1-flag].isEmpty()){  
				if(node.get(fheap[1-flag].min().data).dist[1-flag] + dist > best)  { // Stop when topf + topr > u.//EPP shortest path algorithms Bidirectional DJ.df P3
					visitedNodesReset();
					return best; 
				}
				else {   // Is it really necessary ?????????
					;//best = node[fheap[1-flag].min().data].dist[1-flag] + dist; 
					//lastSettledNode = id;
				} // do nothing????? not update best = node[fheap[1-flag].min().data].dist[1-flag] + dist?
			}
			if( (id == t || id == s) && (dist!=0) ) { // This rarely happens in bi-di DJ search, except very small graph; two searches in two directions tend to meet in the middle of graph
				//System.out.print( "  d(s, t): " + dist + "\n");
				visitedNodesReset();
				return dist; 
			}
			//System.out.print( "\n\nid: "+ id;

			node.get(id).state = (byte) (Metadata.SETTLED*(flag+1));   
			p = node.get(id).adlistout;

			while(p!=null) {  // for each neighbor v of u:  
				id_v = p.ID;
				//System.out.print("\n  id_v: " + id_v;
				v = node.get(id_v);		// v points to one neighbour of u
				if(v.state == Metadata.SETTLED*(flag+1)) {  // v has been settled in the flag direction
					p=p.ptr;
					continue;
				}
				weight = p.wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v.dist[flag]) {  // update v.dist[flag], and also update best if v is settled by 1-flag direction
					//v.previous[flag] = id;  // ???????

					if(v.dist[flag] == Metadata.INFINITE) {  // this implies v is UNREACHED on this direction
						v.dist[flag] = tentative;      
						v.fp[flag] = new FibonacciHeapNode(id_v, tentative);    
						fheap[flag].insert(v.fp[flag]); 
						visited.add(id_v);
					}
					else{
						v.dist[flag] = tentative;      
						fheap[flag].decreaseKey(v.fp[flag], tentative); 
					} 
					if(v.dist[1-flag] < Metadata.INFINITE) {  // Reach a settled node v (v has been settled in the 1-flag direction).
						if( best > tentative + v.dist[1-flag]) { // Update the tentative shortest distance "best".
							 best = tentative + v.dist[1-flag] ;  //  This is the unique statement where "best" is updated.
							 //System.out.print("\n  best update at: "+ id_v + " " + best;
							 //lastSettledNode = id_v;  // for showing the path, starting the last node on which best is last updated.
						}
					}
				}
				//System.out.print("\nv.dist: " + v.dist[flag]; 
				p = p.ptr; //next neighbour of u
			}//while(p!=null) 
			flag = 1-flag;  // turn to another searching direction
		} // while(!fheap[flag].isEmpty()...) 
		visitedNodesReset();
		return best;
	}


	/*
	/////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                                                                                          /////
	//   The following is the main body of bidirectional Dijkstra's algorithm                             /////
	//   based on Fibonacci heap and Contraction Hierarchies.                                                   /////
	//                                                                                                                                                          /////
	////////////////////////////////////////////////////////////////////////////////////////////
	*
	*  Coded by Deqi Li, 2014.1.16
	*/

	int ContractionHierarchiesDJ(int s, int t) { //, ReadRoad *g) {	// s and t are the ID of source node and destination node of graph		
		adList p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = Metadata.INFINITE;    // tentative best shortest length of (s, t)
		int flag = 1;  // turns to forward search(true) or backward search(false)
		//int toprID=0, toprDist=0;

		if(s == t)	return 0;
		if(node.get(s).state == Metadata.NON_EXIST || node.get(t).state == Metadata.NON_EXIST) {
			System.out.print( "source or target don't exist: "+ s + "\n"); 
			return 0;
		}
		FibonacciHeap[] fheap = new FibonacciHeap[2]; 
		fheap[0] = new FibonacciHeap();
		fheap[1] = new FibonacciHeap();
		
		FibonacciHeapNode U = new FibonacciHeapNode(s, 0);
		fheap[flag].insert(U);    // insert source (s, 0) into Fibo heap
		node.get(s).dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node.get(s).fp[flag] = U;     //   the associated Fibo Node storing ID and dist of this node
		node.get(s).state = (byte) (Metadata.SETTLED*(flag+1));  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= Metadata.INFINITE -- initially when it is unreached, dist==INFINITE)
		//node.get(s).previous[flag] = -1;  // the previoud node on shortest path; for s and t, this is -1 -- non existing node.

		U = new FibonacciHeapNode(t, 0);
		fheap[1-flag].insert(U);   
		node.get(t).dist[1-flag] = 0;
		node.get(t).fp[1-flag] = U;
		node.get(t).state = (byte) (Metadata.SETTLED*(flag));
		//node.get(t).previous[1-flag] = -1;
		visited = new ArrayList<Integer>();
		visited.add(s);  // !!
		visited.add(t);  // !!
		dist = 0;  

		while(!fheap[flag].isEmpty() || !fheap[1-flag].isEmpty()) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(fheap[flag].isEmpty()) { flag = 1-flag; }

			//U = fheap[flag].removeMin();   
			//id = U.data;
			id = fheap[flag].removeMin().data;

			dist = node.get(id).dist[flag];  
			node.get(id).state = (byte) (Metadata.SETTLED*(flag+1));  // ????????
 
			if(dist >= best) {  // One-side stopping criterion of Contraction Hierarchies algo. From PHAST P3?
				fheap[flag].clear();
				flag = 1-flag; 
//System.out.print( " Stopping: " + best +"\n"); 
				continue; 
			}


			//if(flag) p = node.get(id).adlistout;  // keep consistent with the first node put in fheap[flag]
			//else      p = node.get(id).adlistin; 

			p = node.get(id).adlistout;

			while(p!=null) {  // for each neighbor v of u:  
				id_v = p.ID;
				//System.out.print("\n  v: " + id_v);
				
///////////////////////////////////////////////////////////////////////////////////
				// Contraction Hierarchies query Algo: 
				// Search is in ascending order in both bi-directions.  
				if(node.get(id_v).OD < node.get(id).OD ) { 
					p = p.ptr;
					//System.out.print( " prun");
					continue;
				}
///////////////////////////////////////////////////////////////////////////////////
				
//System.out.print("\n  v: " + id_v);

				v = node.get(id_v);		// v points to one neighbour of u
				if(v.state == Metadata.SETTLED*(flag+1)) {  // v has been settled in the flag direction
					p=p.ptr;
					continue;
				}
				weight = p.wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
           // NOTICE: If a node v was reached by r, and is also reached by f, we must update it at f; otherwise in future no any more chance to update best at v due to stopping criterion

				if (tentative < v.dist[flag]) {  // update v.dist[flag], and also update best if v is settled by 1-flag direction
					//v.previous[flag] = id;  // ???????

//System.out.print(" (" + tentative + ")");

					if(v.dist[flag] == Metadata.INFINITE) {  // this implies v is UNREACHED on this direction
						v.dist[flag] = tentative;      
						v.fp[flag] = new FibonacciHeapNode(id_v, tentative);    
						fheap[flag].insert(v.fp[flag]); 
						visited.add(id_v);
					}
					else{
						v.dist[flag] = tentative;      
						fheap[flag].decreaseKey(v.fp[flag], tentative); 
					}

					if(v.dist[1-flag] < Metadata.INFINITE) {  // Here is CORRECT!
						if( best > tentative + v.dist[1-flag]) { 
							 best = tentative + v.dist[1-flag] ;  
							 //lastSettledNode = id_v;  // for showing the path, starting the last node on which best is last updated.
						}
					}				
				}
				//System.out.print("\n  v.dist: " + v.dist[flag]; 
				p = p.ptr; //next neighbour of u
			}//while(p!=null) 
			flag = 1-flag;  // turn to another searching direction
		} // while(!fheap[flag].isEmpty()...) 
		
		visitedNodesReset();	
		return best;
	}
 

	int Trivial_CHDJ(int s, int t) {
		adList p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = Metadata.INFINITE;    // tentative best shortest length of (s, t)
		int flag = 1;  // turns to forward search(true) or backward search(false)
		
		//ArrayList<ODID> myheap[] = null;//   new ArrayList<ODID>();
		//myheap[0] = new ArrayList<ODID>();
		//myheap[1] = new ArrayList<ODID>();
		
		//ArrayList<ArrayList<ODID>> myheap;
		//myheap.add(new ArrayList<ODID>());
		//myheap.add(new ArrayList<ODID>());
		
		//myheap[0].resize(MyHeapSize);
		//myheap[1].resize(MyHeapSize);
		int MyHeapSize = 10000;
		ODID myheap[][] = new ODID[2][MyHeapSize];		
		int size[] = { 0, 0 }; //myheap size

		if(s == t)	return 0;
		if(node.get(s).state == Metadata.NON_EXIST || node.get(t).state == Metadata.NON_EXIST) {
			System.out.print( "source or target don't exist: "+ s + "\n"); 
			return 0;
		}

		ODID odidNode = new ODID();
		odidNode.ID = s;
		odidNode.OD = 0;
		myheap[flag][size[flag]] = odidNode;
		size[flag]++;
		
		node.get(s).dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node.get(s).state = (byte) (Metadata.SETTLED*(flag+1));  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= Metadata.INFINITE -- initially when it is unreached, dist==INFINITE)
		
		odidNode = new ODID();
		odidNode.ID = t;
		odidNode.OD = 0;		
		myheap[1-flag][size[1-flag]]= odidNode;     
		size[1-flag]++;
		
		node.get(t).dist[1-flag] = 0;
		node.get(t).state = (byte) (Metadata.SETTLED*(flag));
		dist = 0;  
		visited = new ArrayList<Integer>();
		visited.add(s);  // !!
		visited.add(t);  // !!

		while(size[flag] > 0 ||size[1-flag] > 0) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(size[flag] == 0) { flag = 1-flag; }
			//U = myheap[flag].removeMin();   
			//id = U.data;  
			//dist = node.get(id).dist[flag];  			
			int minDist = 0x7FFFFFFF;  // removeMin(): O(2*size[flag])
			int minIdx=0;
			int i;
			for(i=0; i<size[flag]; i++) {
				if(minDist > myheap[flag][i].OD) {
					minDist = myheap[flag][i].OD;
					minIdx  = i;
				}
			}
			id = myheap[flag][minIdx].ID;
			dist = minDist;
			//assert(minIdx < size[flag]);
			//assert(minDist == node.get(id).dist[flag]);
			//assert(i==size[flag]);
			if(i>0) {
				myheap[flag][minIdx].ID = myheap[flag][i-1].ID;
				myheap[flag][minIdx].OD = myheap[flag][i-1].OD;
			}
			size[flag]--;

//if(flag)System.out.print( "\n\nid: "+ id + " (" + dist + ")"); else System.out.print( "\nid: "+ id + " (" + dist + ")");

			node.get(id).state = (byte) (Metadata.SETTLED*(flag+1)); 
			if(dist >= best) {  // One-side stopping criterion of Contraction Hierarchies algo. From PHAST P3?
				//myheap[flag].clear();
				size[flag]=0;
				flag = 1-flag; 
				continue; 
			}
			p = node.get(id).adlistout;

			while(p!=null) {  // for each neighbor v of u:  
				id_v = p.ID;
//				System.out.print("\n  v: " + id_v;

///////////////////////////////////////////////////////////////////////////////////
				// Contraction Hierarchies query Algo: 
				// Search is in ascending order in both bi-directions.  
				if(node.get(id_v).OD < node.get(id).OD ) { 
					p = p.ptr;
//System.out.print( " prun");
					continue;
				}
///////////////////////////////////////////////////////////////////////////////////
				
				v = node.get(id_v);		// v points to one neighbour of u
				if(v.state == Metadata.SETTLED*(flag+1)) {  // v has been settled in the flag direction
					p=p.ptr;
					continue;
				}
				weight = p.wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v.dist[flag]) {  // update v.dist[flag], and also update best if v is settled by 1-flag direction
//System.out.print(" (" + tentative + ")");
					if(v.dist[flag] == Metadata.INFINITE) {  // this implies v is UNREACHED on this direction
 						 v.dist[flag] = tentative;      
 					     //v.fp[flag] = new myHeapNode(id_v, tentative);    
						//myheap[flag].insert(v.fp[flag]); 
						//myheap[flag][size[flag]].ID = id_v;      // insert(): O(3)
						//myheap[flag][size[flag]].OD = tentative; 
						odidNode = new ODID();
						odidNode.ID = id_v;
						odidNode.OD = tentative;		
						myheap[flag][size[flag]]= odidNode;; 
						
//System.out.print( "  insert (" + id_v + ", " + tentative + ") @ " + size[flag];
						size[flag]++;
						//if(size[flag]==myheap[flag].size()) 
							//myheap[flag].resize(size[flag] + MyHeapSize);
						visited.add(id_v);
					}
					else{				
						v.dist[flag] = tentative;      
						//myheap[flag].decreaseKey(v.fp[flag], tentative); 
						for(i=0; i<size[flag]; i++) {  // decreaseKey(): O(size[flag]/2)
							if(myheap[flag][i].ID == id_v) {
								myheap[flag][i].OD = tentative;
								break;
							}
						}
//System.out.print( "  update (" + id_v + ", " + tentative + ") @ " + i;
						//assert(i<size[flag]);
					}

					if(v.dist[1-flag] < Metadata.INFINITE) {  // Here is CORRECT!
						if( best > tentative + v.dist[1-flag]) { 
							 best = tentative + v.dist[1-flag] ;  
//System.out.print(", best update at: "+ id_v + " (" + best +")");
						}
					}				
				}
				p = p.ptr; //next neighbour of u
			}//while(p!=null) 
			flag = 1-flag;  // turn to another searching direction
		} // while(!myheap[flag].isEmpty()...) 
		//myheap[0].clear();
		//myheap[1].clear();
		visitedNodesReset();	
		return best;
	}

	int Trivial_bidi_classicalDJ(int s, int t) { //, ReadRoad *g) {	// s and t are the ID of source node and destination node of graph		
		//int static count = 0;
		adList p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = Metadata.INFINITE;    // tentative best shortest length of (s, t)
		int flag = 1;  // turns to forward search(true) or backward search(false)
		//int lastSettledNode; // from this two nodes, to traceback the shortest path in two reverse directions.
		//int nNodesOnPath[2];
		//lastSettledNode = -1;

		if(s == t)	return 0;
		if(node.get(s).state == Metadata.NON_EXIST || node.get(t).state == Metadata.NON_EXIST) {
			System.out.print( "source or target don't exist: "+ s + "\n"); 
			return 0;
		}
		
		/*
		//const int MyHeapSize = 10000;
		//ArrayList<ODID>[] myheap = null;
		//myheap[0] = new ArrayList<ODID>();
		//myheap[1] = new ArrayList<ODID>();
		
		ArrayList<ArrayList<ODID>> myheap = null;
		myheap.add(new ArrayList<ODID>());
		myheap.add(new ArrayList<ODID>());*/

		int MyHeapSize = 10000;
		ODID myheap[][] = new ODID[2][MyHeapSize];		
		int size[] = { 0, 0 }; //myheap size		
		
		ODID odidNode = new ODID();
		odidNode.ID = s;
		odidNode.OD = 0; 
		myheap[flag][size[flag]] = odidNode;  
		size[flag]++;
		node.get(s).dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node.get(s).state = (byte) (Metadata.SETTLED*(flag+1));  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= Metadata.INFINITE -- initially when it is unreached, dist==INFINITE)
		
		odidNode = new ODID();
		odidNode.ID = t;
		odidNode.OD = 0;
		myheap[1-flag][size[1-flag]] = odidNode;  
		size[1-flag]++;
		node.get(t).dist[1-flag] = 0;
		node.get(t).state = (byte) (Metadata.SETTLED*(flag));
		
		visited = new ArrayList<Integer>();
		visited.add(s);  // !!
		visited.add(t);  // !!
		dist = 0;  

		while(size[flag] > 0 ||size[1-flag] > 0) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(size[flag] == 0) { flag = 1-flag; }

			int minDist = 0x7FFFFFFF;  // removeMin(): O(2*size[flag])
			int minIdx=0;
			int i;
			for(i=0; i<size[flag]; i++) {
				if(minDist > myheap[flag][i].OD) {
					minDist = myheap[flag][i].OD;
					minIdx  = i;
				}
			}
			id = myheap[flag][minIdx].ID;
			dist = minDist;
			if(i>0) {
				myheap[flag][minIdx].ID = myheap[flag][i-1].ID;
				myheap[flag][minIdx].OD = myheap[flag][i-1].OD;
			}
			size[flag]--;
				
			if(node.get(id).state == Metadata.SETTLED*(2-flag)) {// deleted from both queues, so terminate! "bidirectional DJ_lecture17.pdf", P13
				//System.out.print( "  Weak: " + best +"\n");
				visitedNodesReset();
				return best; 
			}
			if(size[1-flag] > 0){  
				//if(node[fheap[1-flag].min().data].dist[1-flag] + dist > best)  { // Stop when topf + topr > u.//EPP shortest path algorithms Bidirectional DJ.df P3
					//System.out.print( " Strong: " + best +"\n");
					//visitedNodesReset();
					//return best; 
				//}
				minDist = 0x7FFFFFFF;  // getMin(): O(2*size[flag])
				for(i=0; i<size[1-flag]; i++) {
					if(minDist > myheap[1-flag][i].OD) {
						minDist = myheap[1-flag][i].OD;
					}
				}
				if(minDist + dist > best)  { // Stop when topf + topr > u.//EPP shortest path algorithms Bidirectional DJ.df P3
					//System.out.print( " Strong: " + best +"\n");
					visitedNodesReset();
					return best; 
				}
			}
			
			if( (id == t || id == s) && dist!=0 ) { // This rarely happens in bi-di DJ search, except very small graph; two searches in two directions tend to meet in the middle of graph
				//System.out.print( "  d(s, t): " + dist + "\n");
				visitedNodesReset();
				return dist; 
			}
			//System.out.print( "\n\nid: "+ id;
			node.get(id).state = (byte) (Metadata.SETTLED*(flag+1));   

			//if(flag) p = node.get(id).adlistout;  // keep consistent with the first node put in fheap[flag]
			//else      p = node.get(id).adlistin; 
			p = node.get(id).adlistout;

			while(p!=null) {  // for each neighbor v of u:  
				id_v = p.ID;
				//System.out.print("\n  id_v: " + id_v;
				v = node.get(id_v);		// v points to one neighbour of u
				if(v.state == Metadata.SETTLED*(flag+1)) {  // v has been settled in the flag direction
					p=p.ptr;
					continue;
				}
				weight = p.wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v.dist[flag]) {  // update v.dist[flag], and also update best if v is settled by 1-flag direction
					//v.previous[flag] = id;  // ???????
					if(v.dist[flag] == Metadata.INFINITE) {  // this implies v is UNREACHED on this direction
						v.dist[flag] = tentative;  
						odidNode = new ODID();
						odidNode.ID = id_v;
						odidNode.OD = tentative;
						myheap[flag][size[flag]] = odidNode; // insert(): O(3)
//System.out.print( "  insert (" + id_v + ", " + tentative + ") @ " + size[flag];
						size[flag]++;
						visited.add(id_v);
					}
					else{
						v.dist[flag] = tentative;      
						for(i=0; i<size[flag]; i++) {  // decreaseKey(): O(size[flag]/2)
							if(myheap[flag][i].ID == id_v) {
								myheap[flag][i].OD = tentative;
								break;
							}
						}
					} 
					if(v.dist[1-flag] < Metadata.INFINITE) {  // Reach a settled node v (v has been settled in the 1-flag direction).
						if( best > tentative + v.dist[1-flag]) { // Update the tentative shortest distance "best".
							 best = tentative + v.dist[1-flag] ;  //  This is the unique statement where "best" is updated.
							 //System.out.print("\n  best update at: "+ id_v + " " + best;
							 //lastSettledNode = id_v;  // for showing the path, starting the last node on which best is last updated.
						}
					}
				}
				//System.out.print("\nv.dist: " + v.dist[flag]; 
				p = p.ptr; //next neighbour of u
			}//while(p!=null) 
			flag = 1-flag;  // turn to another searching direction
		} // while(!fheap[flag].isEmpty()...) 
		visitedNodesReset();
		return best;
	}

	int Trivial_classicalDJ(int s, int t) {//, ReadRoad *g) {			
		adList p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative; // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually

		if(s == t)	return 0;
		if(node.get(s).state == Metadata.NON_EXIST || node.get(t).state == Metadata.NON_EXIST) {
			System.out.print( "source or target don't exist: "+ s + "\n"); 
			return 0;
		}

		int MyHeapSize = 10000;
		//ArrayList<ODID>myheap = new ArrayList<ODID>();
		//myheap.resize(MyHeapSize);
		int size = 0; //myheap size

		//myheap.get(size).ID = s;      
		//myheap.get(size).OD = 0;  
			
		ODID[] myheap = new ODID[MyHeapSize];
		ODID odidNode = new ODID();
		odidNode.ID = s;
		odidNode.OD = 0;
		myheap[size] = odidNode;
		size++;	
		node.get(s).dist[0] = 0;
		dist = 0;
		
		visited = new ArrayList<Integer>();
		visited.add(s);  // !!

		while(size > 0) { //myheap.size() > 0) {
			int minDist = Metadata.INFINITE; //0x7FFFFFFF;  // removeMin(): O(2*size[flag])
			int minIdx=0;
			int i;
			//for(i=0; i<myheap.size(); i++) {
			for(i=0; i<size; i++) {
				if(minDist > myheap[i].OD) {
					minDist = myheap[i].OD;
					minIdx  = i;
				}
			}
			id = myheap[minIdx].ID;
			dist = minDist;
			//assert(minIdx < size);
			//assert(minDist == node.get(id).dist[0]);
			//assert(i==size);
			if(i>0) { // overwrite the min
				myheap[minIdx].ID = myheap[i-1].ID;
				myheap[minIdx].OD = myheap[i-1].OD;
			}
			size--;
			
			node.get(id).state = Metadata.SETTLED;
			if(id == t) {
				visitedNodesReset();
				return dist; 
			}
			
			p = node.get(id).adlistout;
			while(p!=null) {  // for each neighbor v of u:  
				id_v = p.ID;
				v = node.get(id_v);		// v points to one neighbour of u
				if(v.state == Metadata.SETTLED) { // || v.state == CONTRACTED) {
					p=p.ptr;
					continue;
				}
				weight = p.wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v.dist[0]) {
					if(v.dist[0] == Metadata.INFINITE) {  // this implies v is UNREACHED on this direction
						v.dist[0] = tentative;
						odidNode = new ODID();
						odidNode.ID = id_v;
						odidNode.OD = tentative;
						myheap[size] = odidNode;
						//myheap.get(size).ID = id_v;      // insert(): O(3)
						//myheap.get(size).OD = tentative;  
//System.out.print( "  insert (" + id_v + ", " + tentative + ") @ " + size[flag];
						size++;
						//if(size==myheap.size()) myheap.resize(size + MyHeapSize);
						visited.add(id_v);
					}
					else{
						v.dist[0] = tentative;      
						//for(i=0; i<myheap.size(); i++) {  // decreaseKey(): O(size[flag]/2)
						for(i=0; i<size; i++) { // decreaseKey(): O(size[flag]/2)
							if(myheap[i].ID == id_v) {
								myheap[i].OD = tentative;
								break;
							}
						}
					} 
				}
				p = p.ptr; //next neighbour of u
			}
		}
		visitedNodesReset();
		return dist;
	}

	
/*
	int Trivial_classicalDJ_BACKUP(int s, int t) {//, ReadRoad *g) {			
		adList p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative; // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually

		if(s == t)	return 0;
		if(node.get(s).state == Metadata.NON_EXIST || node.get(t).state == Metadata.NON_EXIST) {
			System.out.print( "source or target don't exist: "+ s + "\n"); 
			return 0;
		}

		int MyHeapSize = 10000;
		//ArrayList<ODID>myheap = new ArrayList<ODID>();
		//myheap.resize(MyHeapSize);
		int size = 0; //myheap size

		//myheap.get(size).ID = s;      
		//myheap.get(size).OD = 0;  
			
		ODID[] myheap = new ODID[MyHeapSize];
		ODID odidNode = new ODID();
		odidNode.ID = s;
		odidNode.OD = 0;
		myheap[0] = odidNode;
		size++;	
		node.get(s).dist[0] = 0;
		dist = 0;
		
		visited = new ArrayList<Integer>();
		visited.add(s);  // !!

		while(size > 0) { //myheap.size() > 0) {
			int minDist = Metadata.INFINITE; //0x7FFFFFFF;  // removeMin(): O(2*size[flag])
			int minIdx=0;
			int i;
			//for(i=0; i<myheap.size(); i++) {
			for(i=0; i<size; i++) {
				if(minDist > myheap[i].OD) {
					minDist = myheap[i].OD;
					minIdx  = i;
				}
			}
			id = myheap[minIdx].ID;
			dist = minDist;
			//assert(minIdx < size);
			//assert(minDist == node.get(id).dist[0]);
			//assert(i==size);
			if(i>0) { // overwrite the min
				myheap.get(minIdx).ID = myheap.get(i-1).ID;
				myheap.get(minIdx).OD = myheap.get(i-1).OD;
			}
			size--;
			
			node.get(id).state = Metadata.SETTLED;
			if(id == t) {
				visitedNodesReset();
				return dist; 
			}
			
			p = node.get(id).adlistout;
			while(p!=null) {  // for each neighbor v of u:  
				id_v = p.ID;
				v = node.get(id_v);		// v points to one neighbour of u
				if(v.state == Metadata.SETTLED) { // || v.state == CONTRACTED) {
					p=p.ptr;
					continue;
				}
				weight = p.wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v.dist[0]) {
					if(v.dist[0] == Metadata.INFINITE) {  // this implies v is UNREACHED on this direction
						v.dist[0] = tentative;
						odidNode = new ODID();
						odidNode.ID = id_v;
						odidNode.OD = tentative;
						myheap.add(odidNode);
						//myheap.get(size).ID = id_v;      // insert(): O(3)
						//myheap.get(size).OD = tentative;  
//System.out.print( "  insert (" + id_v + ", " + tentative + ") @ " + size[flag];
						size++;
						//if(size==myheap.size()) myheap.resize(size + MyHeapSize);
						visited.add(id_v);
					}
					else{
						v.dist[0] = tentative;      
						//for(i=0; i<myheap.size(); i++) {  // decreaseKey(): O(size[flag]/2)
						for(i=0; i<size; i++) { // decreaseKey(): O(size[flag]/2)
							if(myheap.get(i).ID == id_v) {
								myheap.get(i).OD = tentative;
								break;
							}
						}
					} 
				}
				p = p.ptr; //next neighbour of u
			}
		}
		visitedNodesReset();
		return dist;
	}*/
	
	void unpackPath() {
		// Not implement it at present.
	}

	// Check if all nodes in W[] are settled for onetoManyClassicalDJ();
	// If so,  onetoManyClassicalDJ() can return.
	protected boolean allSettled(ArrayList<MultiDestination> W) {
		int i;
		for(i=0; i<W.size(); i++) {
			if(node.get(W.get(i).ID).state != Metadata.SETTLED) return false;
		}
		for(i=0; i<W.size(); i++) {
			W.get(i).distDJ = node.get(W.get(i).ID).dist[0];
		}
		return true;
	}

	protected void visitedNodesReset() {
		/*for(Node nd: node) {
			nd.state = Metadata.EXIST;
			nd.dist[0] = Metadata.INFINITE; 
			nd.dist[1] = Metadata.INFINITE; 
			if(nd.fp[0] != null) { nd.fp[0] = null; }
			if(nd.fp[1] != null) { nd.fp[1] = null; }
		}*/

		for(Integer id: visited) {
			node.get(id).state = Metadata.EXIST;
			node.get(id).dist[0] = Metadata.INFINITE; 
			node.get(id).dist[1] = Metadata.INFINITE; 
			if(node.get(id).fp[0] != null) { node.get(id).fp[0] = null; }
			if(node.get(id).fp[1] != null) { node.get(id).fp[1] = null; }
		}
		
		nVisitedNodes = visited.size();   // nVisitedNodes is used by getNVisitedNodes()  
		//visited.clear();
		//visited.resize(0);
		visited = null;
		//System.out.println("  VisitedNodes: " + nVisitedNodes);
	}
/*
	int showPath(int s, int start, int flag) {
		if(start < 0 || node[start].previous[flag] < 0) return 0;

		int temp = start;		 
		int nNodesOnPath = 1;
			System.out.print( "<-" +node[temp].previous[flag];
			temp = node[temp].previous[flag];
			nNodesOnPath++;
		}while(temp != s);
	
		//System.out.print( "\nthe shortest distance between " + s +" and " + t + " is: " + node.get(t).dist +"\n");
		return nNodesOnPath;
	}
*/
}
//////////////////////////////////////////////////////////////
// End of  class Dijkstra_Fheap.
//////////////////////////////////////////////////////////////
		 