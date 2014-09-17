package CH_Java;
import java.util.ArrayList;

//////////////////////////////////////////////////////////////
// Begin of class Dijkstra_Bheap.
//////////////////////////////////////////////////////////////
// The following Dijkstra's algo is based on Binary heaps rather than Fibonacci heaps.
class Dijkstra_Bheap {
	private	ArrayList<Integer>visited;  // record all visited (reached and settled) nodes for reseting their state to EXIST, and delete pointers of FiboNodes, set node[].dist to INFINITE.
	private int lastMeetNode;
	private int nVisitedNodes;
	private ArrayList<Node> node;
	
	Dijkstra_Bheap(ArrayList<Node> node0) { 		
		node = node0; visited = new ArrayList<Integer>(); 
	}
	
	public int getNVisitedNodes() { return nVisitedNodes; }  // nVisitedNodes is the size of W[]. nVisitedNodes is set in visitedNodesReset() // -- only for test

	void onetoManyClassicalDJ(int s, ArrayList<MultiDestination> W, int maxLength) {//, ReadRoad *g) {			
		adList p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative; // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int nSettled=0;
		int sizeW = W.size();
//System.out.print("\n\n  onetoManyClassicalDJ(" + s + ", |W|=" + sizeW + ", " + maxLength + ")");
//long start;
		
		visited = new ArrayList<Integer>();
		visited.add(s);  // !!  -- These visited nodes should be recorded for restoring their states

		BinaryHeap bheap = new BinaryHeap(node);
		bheap.insert(s, 0, 0);    // insert source (s, 0)	
		node.get(s).dist[0] = 0;
		node.get(s).bp[0] = 1;
		dist = 0;  
		
		while(!bheap.isEmpty()) {
//start = System.currentTimeMillis();						
			id = bheap.removeMin(0);  // O(logn)
//t[0] += System.currentTimeMillis() - start;						
			dist = node.get(id).dist[0]; //  dist = node[fp.data].dist
			node.get(id).state = Metadata.SETTLED;
			nSettled++;
//System.out.print("\n  id=" + id + " dist=" + dist);

			if(dist > maxLength) {  // Proposed by Deqi Li, Jan 19,2014.
//System.out.print("\n  dist=" + dist + " > maxLength, return");
//start = System.currentTimeMillis();						
				visitedNodesReset(); 
//t[3] += System.currentTimeMillis() - start;						
				return; //_removeMin+t_insert+t_decreaseKey+t_visitedReset; 
			}

			if(nSettled >= sizeW) {  // Proposed by Deqi Li, Jan 11,2014.
				if(allSettled(W)) {
//System.out.print("\n  allSettled(W)=true, return");					
//start = System.currentTimeMillis();						
					visitedNodesReset(); 
//t[3] += System.currentTimeMillis() - start;						
					return; 
				}
			}

			p = node.get(id).adlistout;
			while(p!=null) {  // for each neighbor v of u:  
				id_v = p.ID;
				v = node.get(id_v);		// v points to one neighbour of u
				if(v.state == Metadata.SETTLED || v.state == Metadata.CONTRACTED) {
					p=p.ptr;
					continue;
				}
				weight = p.wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
//System.out.print("\n    id_v=" + id_v + ", weight=" + weight + ", tentative=" + tentative);
				if (tentative < v.dist[0]) {
					if(v.dist[0] == Metadata.INFINITE) {  // this implies v is UNREACHED on this direction
						v.dist[0] = tentative;      
//start = System.currentTimeMillis();						
						v.bp[0] = bheap.insert(id_v, tentative, 0);  						
//t[1] += System.currentTimeMillis() - start;						
//System.out.print("\n     bheap.insert(" + id_v + ", " + tentative + ", 0)");						
						visited.add(id_v);
					}
					else{
						v.dist[0] = tentative;
//start = System.currentTimeMillis();						
						bheap.decreaseKey(v.bp[0], tentative, 0);
//t[2] += System.currentTimeMillis() - start;						
//System.out.print("\n     bheap.decreaseKey(" + v.bp[0] + ", " + tentative + ", 0)");
					} 
				}
				p = p.ptr; //next neighbour of u
			}
		}
		return;
	}

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
	
		BinaryHeap bheap = new BinaryHeap(node);  
		bheap.insert(s, 0, 0);
		node.get(s).dist[0] = 0;
		node.get(s).bp[0] = 1;
		dist = 0;  
		visited = new ArrayList<Integer>();
		visited.add(s);   

		while(!bheap.isEmpty()) {
			id = bheap.removeMin(0);  // O(logn) ---- the most time-consuming operation!!!
			dist = node.get(id).dist[0];       //  dist = node[fp.data].dist
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
						v.bp[0] = bheap.insert(id_v, tentative, 0);  						
						visited.add(id_v);
					}
					else{
						v.dist[0] = tentative;      
						bheap.decreaseKey(v.bp[0], tentative, 0); 
					} 
				}
				p = p.ptr; //next neighbour of u
			}
		}

		//System.out.print( "\nmaxHeapSize (single-Dj): " + maxHeapSize;

		visitedNodesReset();
		return dist;
	}	
 
	public int bidi_classicalDJ(int s, int t) { //, ReadRoad *g) {	// s and t are the ID of source node and destination node of graph		
		adList p;
		int id;     // the graph node ID of  the bheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = Metadata.INFINITE;    // tentative best shortest length of (s, t)
		int flag = 1;  // turns to forward search(true) or backward search(false)
		//int maxHeapSize = 0;

		if(s == t)	return 0;
		if(node.get(s).state == Metadata.NON_EXIST || node.get(t).state == Metadata.NON_EXIST) {
			System.out.print( "\nsource or target don't exist."); 
			return 0;
		}

		BinaryHeap bheap[] = new BinaryHeap[2];
		bheap[0] = new BinaryHeap(node);
		bheap[1] = new BinaryHeap(node);
		bheap[flag].insert(s, 0, flag);    // insert source (s, 0) into Fibo heap
		node.get(s).dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node.get(s).bp[flag] = 1;     //   the associated Fibo Node storing ID and dist of this node
		node.get(s).state = (byte) (Metadata.SETTLED*(flag+1));  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= Metadata.INFINITE -- initially when it is unreached, dist==INFINITE)

		bheap[1-flag].insert(t, 0, flag);   
		node.get(t).dist[1-flag] = 0;
		node.get(t).bp[1-flag] = 1;
		node.get(t).state = (byte) (Metadata.SETTLED*(flag));
		visited = new ArrayList<Integer>();
		visited.add(s);   
		visited.add(t);  
		dist = 0;  

		while(!bheap[flag].isEmpty() || !bheap[1-flag].isEmpty()) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(bheap[flag].isEmpty()) { flag = 1-flag; }

			//if(maxHeapSize < bheap[flag].getSize()) maxHeapSize = bheap[flag].getSize();	
			id =  bheap[flag].removeMin(flag);   // the flag is for trace index of heap node for the graph node[x].bp[flag] when swapNodes()
			dist = node.get(id).dist[flag];  
			
			if(node.get(id).state == Metadata.SETTLED*(2-flag)) { 
				//System.out.print( "  Weak: " + best +"\n");
				visitedNodesReset();
				return best; 
			}
			if(!bheap[1-flag].isEmpty()){  
				if(bheap[1-flag].min() + dist > best)  { // Stop when topf + topr > u.//EPP shortest path algorithms Bidirectional DJ.df P3
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

			node.get(id).state = (byte) (Metadata.SETTLED*(flag+1));   

			//System.out.print( "\n\nid: "+ id;

			//if(flag) p = node.get(id).adlistout;  // keep consistent with the first node put in bheap[flag]
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
					if(v.dist[flag] == Metadata.INFINITE) {  // this implies v is UNREACHED on this direction
						v.dist[flag] = tentative;      
						v.bp[flag] = bheap[flag].insert(id_v, tentative, flag);    
						visited.add(id_v);
					}
					else{
						v.dist[flag] = tentative;      
						bheap[flag].decreaseKey(v.bp[flag], tentative, flag); 
					} 
					if(v.dist[1-flag] < Metadata.INFINITE) {  // Reach a settled node v (v has been settled in the 1-flag direction).
						if( best > tentative + v.dist[1-flag]) { // Update the tentative shortest distance "best".
							 best = tentative + v.dist[1-flag];  //  This is the unique statement where "best" is updated.
							 //System.out.print("\n  best update at: "+ id_v + " " + best;
						}
					}
				}
				//System.out.print("\nv.dist: " + v.dist[flag]; 
				p = p.ptr; //next neighbour of u
			}//while(p!=null) 
			flag = 1-flag;  // turn to another searching direction
		} // while(!bheap[flag].isEmpty()...) 
		
		//System.out.print( "\nmaxHeapSize(bi-di Classical-DJ): " + maxHeapSize;

		visitedNodesReset();
		return best;
	}


	/*
	/////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                                                                                          /////
	//   The following is the main body of bidirectional Dijkstra's algorithm                             /////
	//   based on Binary heap and Contraction Hierarchies.                                                   /////
	//                                                                                                                                                          /////
	////////////////////////////////////////////////////////////////////////////////////////////
	*
	*  Coded by Deqi Li, 2014.1.22
	*/

	public int ContractionHierarchiesDJ(int s, int t) { //, ReadRoad *g) {	// s and t are the ID of source node and destination node of graph		
		//int  count = 0;
		adList p;
		int id;     // the graph node ID of  the bheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = Metadata.INFINITE;    // tentative best shortest length of (s, t)
		int flag = 1;  // turns to forward search(true) or backward search(false)

		if(s == t)	return 0;
		if(node.get(s).state == Metadata.NON_EXIST || node.get(t).state == Metadata.NON_EXIST) {
			System.out.print( "\nsource or target don't exist."); 
			return 0;
		}
		
		BinaryHeap[] bheap = new BinaryHeap[2]; 
		bheap[0] = new BinaryHeap(node);
		bheap[1] = new BinaryHeap(node);
		
		bheap[flag].insert(s, 0, flag);    // insert source (s, 0) into Fibo heap
		node.get(s).dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node.get(s).bp[flag] = 1;     //   the associated Fibo Node storing ID and dist of this node
		node.get(s).state = (byte) (Metadata.SETTLED*(flag+1));  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= Metadata.INFINITE -- initially when it is unreached, dist==INFINITE)

		bheap[1-flag].insert(t, 0, 1-flag);   
		node.get(t).dist[1-flag] = 0;
		node.get(t).bp[1-flag] = 1;
		node.get(t).state = (byte) (Metadata.SETTLED*(flag));
		visited = new ArrayList<Integer>();
		visited.add(s);
		visited.add(t);
		dist = 0;  

		while(!bheap[flag].isEmpty() || !bheap[1-flag].isEmpty()) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(bheap[flag].isEmpty()) { 
				//System.out.print("\n\n bheap["+flag+"].isEmpty");
				flag = 1-flag; 
			}

			id =  bheap[flag].removeMin(flag);   
			dist = node.get(id).dist[flag];  
			node.get(id).state = (byte) (Metadata.SETTLED*(flag+1));   
			
			if(dist >= best) {  // One-side stopping criterion of Contraction Hierarchies algo. From PHAST P3?
				bheap[flag].clear();
				flag = 1-flag; 
				continue; 
			}

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
				v = node.get(id_v);		// v points to one neighbour of u				
				if(v.state == Metadata.SETTLED*(flag+1)) {  // v has been settled in the flag direction
					p=p.ptr;
					continue;
				}
				weight = p.wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v.dist[flag]) {  // update v.dist[flag], and also update best if v is settled by 1-flag direction
					if(v.dist[flag] == Metadata.INFINITE) {  // this implies v is UNREACHED on this direction
						v.dist[flag] = tentative;      
						v.bp[flag] = bheap[flag].insert(id_v, tentative, flag);
						visited.add(id_v);
					}
					else{
						v.dist[flag] = tentative;      
						bheap[flag].decreaseKey(v.bp[flag], tentative, flag); 
					} 

					//v.previous[flag] = id;

					if(v.dist[1-flag] < Metadata.INFINITE) {  // Reach a settled node v (v has been settled in the 1-flag direction).
						if( best > tentative + v.dist[1-flag]) { // Update the tentative shortest distance "best".
							best = tentative + v.dist[1-flag];  //  This is the unique statement where "best" is updated.
							 //lastMeetNode = id_v;		
						}
					}
				}
				//System.out.print("\n   v.dist: " + v.dist[flag] + ", v.dist[!flag]: "+ v.dist[1-flag]); 
				p = p.ptr; //next neighbour of u
			}//while(p!=null) 
			flag = 1-flag;  // turn to another searching direction
			//System.out.print("\n flag: " + flag);
		} // while(!bheap[flag].isEmpty()...) 
		//showPath(s, t);
		visitedNodesReset();	
		return best;
	}

/*
	protected void showPath(int s, int t) {
		vector<int>onPath;
		//cout  + "\n\nthe shortest path to source direction: " + lastMeetNode;

		int temp = lastMeetNode;
		while(temp != s){
			onPath.add(temp);
			temp = node[temp].previous[0];
		} 
		onPath.add(s);
		
		std::reverse(onPath.begin(),onPath.end());    

		//cout  + "\nthe shortest path to target direction: " + lastMeetNode;
		temp = lastMeetNode;
		while(temp != t) {
			temp = node[temp].previous[1];
			onPath.add(temp);
		}

		//System.out.print( "\n CH found path: " + onPath.size() + "\n");
		//for (std::vector<int>::iterator it=onPath.begin(); it!=onPath.end(); ++it) System.out.print( ' ' + *it; 
		//System.out.print( "\n\n the path on original graph:\n");
		unPackPath(onPath);
		//System.out.print( "\n");
		//System.out.print( "\nshowPath finished\n");

		//System.out.print( "\nthe shortest distance between " + s +" and " + t + " is: " + node.get(t).dist +"\n");
	}

	
	protected void unPackPath(vector<int> &onPath) {
		vector<int> originalPath;
		vector<int> contractedNodes;  // these nodes will be found and inserted into the path
		vector<int>::iterator it;
		int v, u, w;
		int dist;
		adList *p1, *p2;

		for (it=onPath.begin(); it!=onPath.end()-1; ++it) {
			v =  *it;
			w = *(it+1);
			//System.out.print( "\ninsert: " + v;
			originalPath.add(v);
			// find contracted nodes and insert them into the path
			contractedNodes.clear();
			do {
				p1 = node[v].adlistout;
				dist = 0;  // dist = weight(v, w);
				while(p1 != null) {
					if(p1.ID == w) { dist = p1.wt; break;	}
					p1 = p1.ptr;
				}
				assert(dist);
				// try to find a node u which is between v and w and satisfies that weight(v,u) + weight(u,w) == dist
				// we also assume that such u is unique (even if it is not unique, it doens't matter; we still can find the original path).
				p1 = node[v].adlistout;
				while(p1 != null) {
					u = p1.ID;
					p2 = node[w].adlistout;
					while(p2 != null) { 
						if(p2.ID == u) break;
						p2 = p2.ptr; 
					}
					if(p2 != null) assert(p1.wt + p2.wt >= dist);
					if(p2 != null && (p1.wt + p2.wt) == dist) break;  				
					p1 = p1.ptr;
				}
				if(p1==null) break;
				contractedNodes.add(u);
				w = u;
			}while(true);

			// insert the contracted nodes into original path
			while(!contractedNodes.empty()) {
				//System.out.print( "\ninsert: " + contractedNodes.back();
				originalPath.add(contractedNodes.back());
				contractedNodes.pop_back();
			}
		}
		//System.out.print( "\ninsert: " + onPath.back();
		originalPath.add(onPath.back());  // insert the last node on path
		//System.out.print( "\n\n the path on original graph: " + originalPath.size() + "\n");
		//for (it=originalPath.begin(); it!=originalPath.end(); ++it) System.out.print( ' ' + *it; 
		//System.out.print( "\n\n\n");
	}
*/
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
		/*for(Node nd: node) {   ------------ too slow!!! 28940ms/2000nodes
			nd.state = Metadata.EXIST;
			nd.dist[0] = Metadata.INFINITE; 
			nd.dist[1] = Metadata.INFINITE; 
			if(nd.fp[0] != null) { nd.fp[0] = null; }
			if(nd.fp[1] != null) { nd.fp[1] = null; }
		}*/ 
		
		/*for(Integer id: visited) {
			node.get(id).state = Metadata.EXIST;
			node.get(id).dist[0] = Metadata.INFINITE; 
			node.get(id).dist[1] = Metadata.INFINITE; 
			if(node.get(id).fp[0] != null) { node.get(id).fp[0] = null; }
			if(node.get(id).fp[1] != null) { node.get(id).fp[1] = null; }
		}*/
		for(int i=0; i<visited.size(); i++) {
			node.get(visited.get(i)).state = Metadata.EXIST;
			node.get(visited.get(i)).dist[0] = Metadata.INFINITE; 
			node.get(visited.get(i)).dist[1] = Metadata.INFINITE; 
			if(node.get(visited.get(i)).fp[0] != null) { node.get(visited.get(i)).fp[0] = null; }
			if(node.get(visited.get(i)).fp[1] != null) { node.get(visited.get(i)).fp[1] = null; }
		}
		
		nVisitedNodes = visited.size();   // nVisitedNodes is used by getNVisitedNodes()
		//System.out.print("\n  nVisitedNodes="+nVisitedNodes);
		//visited.clear();
		//visited.resize(0);
		visited = null;
		//System.out.print("\n_visitedNodesReset()");
	}
}

//////////////////////////////////////////////////////////////
// End of class Dijkstra_Bheap.
//////////////////////////////////////////////////////////////

