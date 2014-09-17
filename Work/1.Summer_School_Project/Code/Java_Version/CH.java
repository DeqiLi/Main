package CH_Java;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Random;

//////////////////////////////////////////////////////////////
// Begin of class CH -- Contraction Hierarchies.
//////////////////////////////////////////////////////////////
// Mainly for preprocessing of data file to generate node orderings.
public class CH {
	private	long t_getWeight, t_distMDSD, t_addShortcutsOnW;
	private	long t_onetoManyDest;
	
	private	int maxShortcuts; // max # of shortcuts a node added
	private	int maxEdges;        // max # of edges a node has
	private	int nAddedShortcuts; // totally # of shortcuts were added by all nodes
	private	int nRemovedEdges;   // totally # of edges of graph were removed 
	private	int maxSizeW;  //if W.size() is over this value, setInitialOrder() is triggered to avoid too many shortcuts generated 
	private ArrayList<Node> node;
	private ArrayList<ODID> odid; // map between ID and OD; for shuffling and sorting nodes by OD.
	private int n;
	private ReadRoad g;
	
	CH(ArrayList<Node> node0, ReadRoad G) {
		t_getWeight=0;
		t_distMDSD=0;
		t_addShortcutsOnW=0; //timing for functions
		t_onetoManyDest =0;/* = new long[4];
		t_onetoManyDest[0] = 0;
		t_onetoManyDest[1] = 0;
		t_onetoManyDest[2] = 0;
		t_onetoManyDest[3] = 0;*/
		
		g = G;
		n = g.getNodes();
		
		odid = new ArrayList<ODID>();
		for(int i=0; i<=n; i++) {
			ODID od = new ODID();
			od.ID = i; //// added on Sep 3, 2014
			odid.add(od);
		}

		node = node0; maxShortcuts = 0;  maxEdges = 0; 
		nAddedShortcuts = 0; nRemovedEdges = 0;
	}
	
	/**
	  * "MDSD" means "Multi-Destination Shortest Distances".
	  * Search for shortest distance from single-source to multi-destinations.
	  * Called by edgeDiffbyMutliDest().
	  * Return with the real dist of the source to each destination.
	  * 
	  * <p> O(d^2), one d for W.size(), another d for DJ algo local search.
	  *
	  * @param W: vector of multi destination's ID and dist.
	  */
	protected void distMDSD(int source, ArrayList<MultiDestination> W, int maxLength) { 	
		Dijkstra_Fheap DJfh = new Dijkstra_Fheap(node); 
		DJfh.onetoManyClassicalDJ(source, W, maxLength);  
		return;
	}

	/**
	  * "SDSD" means "Single-Destination Shortest Distances".
	  * Search for shortest distance from single-source to multi-destinations.
	  * Called by edgeDiffbyMutliDest().
	  * Return with the real dist of the source to each destination.
	  * 
	  * <p> O(d^2), one d for W.size(), another d for DJ algo local search.
	  *
	  */
/*
		int distSDSD(int source, int target, int contractNode) { //, int maxSettled){
			Dijkstra_Fheap DJfh; 
			//g.clearNodeState();  //use resetVisitedNode() to replace it
			return DJfh.shortestDistanceLimited(source, target, contractNode);//, maxSettled);
		}
*/
	/**
	  * Add a new arc (v, w) into graph.
	  * Add it on the head (not tail) of the adList of node v and node w.
	  * 
	  *@param v: the source node
	  *@param w: the target node
	  *@param wt: the weight of the new arc
	  *
	  * return: # of shortcuts added
	  */
	  protected int addShortcuts(int v, int w, int wt) {
		//static int _addShortcuts=0;
		//System.out.print("\naddShortcuts: " + _addShortcuts++ + ": (" + v + "," + w +")=" +wt);
		//System.out.print("\nv, w, weight: (" + v + ", " + w + "), " + wt);
		//System.out.print("\na " + v + " " + w + " " + wt);
		//System.out.print("\na " + v + " " + w + " " + wt + " passby " + u);

		if( v >= w ) return 0;  // omit the edges like ( 2 1  365) -- we only store edges with two nodes from small to large like (1 2 365)
		
		nAddedShortcuts += 2;  // add two shortcuts (v, w, wt) and (w, v, wt) into graph

		adList p;
		adList head;
		int flag; 
		int x[] = { v, w };

		for(flag=0; flag<2; flag++) {
			head = node.get(x[flag]).adlistout;
			p = new adList();
			p.ID = x[1 - flag];
			p.wt = wt;
			if (head == null) {  // add the first node into node[v].adlistout
			   node.get(x[flag]).adlistout = p;		  
			   p.ptr = null;
			}
			else {  // put a new node at the head of node[v].adlistout
			   p.ptr = head;
			   node.get(x[flag]).adlistout = p;
			}
			// Note:  We only store outgoing edges; considering incoming edges are the same set as the outgoing edge set.  
			// No need: --->  node[x[flag]].adlistin = node[x[flag]].adlistout; // to save space, for symmetric directed graph, we only store one-direct of an edge for each node
		}
		return 2;
	}


	// get the weight of an edge (v,w)  
	int getWeight(int v, int w) {
		//System.out.print("\n(_v, _w): " + v +", " + w);
		adList p;
		p = node.get(v).adlistout;
		while(p != null) {
			if(p.ID == w) return p.wt;
			p = p.ptr;
		}
		return Metadata.INFINITE;  // v and w were not neighbours
	}


	/**
	  * Assign the edge (v, w) a new weight, as well as check if v,w are connected
	  * Return false if they are not neighbors.
	  * 
	  */
	void updateWeight(int v, int w, int weight) {
		//static int _updateWeight = 0;
		//System.out.print("\n_updateWeight: " + _updateWeight++);
		//System.out.print("\nnew weight: " + weight);
		
		if(v >= w) return;

		int flag; 
		int x[] = { v, w };

		for(flag=0; flag<2; flag++) {
			adList p = node.get(x[flag]).adlistout;
			while(p != null) {
				if(p.ID != x[1 - flag]) { p = p.ptr; continue; }
				p.wt = weight;
				break;
			}
		}
	}

	/**
	  * Remove an edge(v, w) from graph permanently.
	  * Is it so called "On-the-Fly Edge Reduction"?
	  * In fact, only very few edges are removed due to it is shorter than distDJ (v, w).
	  * So practically it is not useful.
	  *
	  * Remark: it operates on only one edge ( not two arcs <v, w> and <w, v>)
	  */
	void removeEdge(int v, int w) {

		if(v >= w) return;

		nRemovedEdges++;

		int flag; 
		int x[] = { v, w };

		for(flag=0; flag<2; flag++) {
			adList p1 = node.get(x[flag]).adlistout;
			adList p2 = p1.ptr;

			if(p1.ID == x[1-flag]) { node.get(x[flag]).adlistout = p2; p1 = null; return; }
			while(p2 != null) {
				if(p2.ID != x[1-flag]) { p1 = p2; p2 = p2.ptr;  continue; }
				p1.ptr = p2.ptr;
				//delete p2;
				p2 = null;
			}
		}
	}

	/**
	  * Compute u's edge difference (the priority term of node order) by the formula: 
	  *			order = 190 * (nShortcuts - nEdges) when contract node u.
	  * Called by nodeOrdering() and setInitialOrder().
	  * Return with nShortcuts - nEdges due to contraction of u.
	  * We wish this value to be as small as possible.
	  * 
	  * <p> O(d^3), d^2 for pairs (v, w), another d for DJ algo local search.
	  *
	  * @param u: node to be contracted.
	  * @realContract: 
	  *            false: if it is called by setInitialOrder(), it doesn't really contract the node and not add shortcuts -- just simulates.
	  *            true:  Does contract the node and add shortcuts.
	  */
	int edgeDiffbyMultiDest(int u, boolean realContract) {
		int nShortcuts = 0;
		int nEdges = 0;
		int v, w; // v.u.w; u is the node to be contracted (if realContract is true, it do be contracted).
		int maxLength;  // max length of v to each node in W[]; if dist in onetoManyDJ over this value, then onetoManyDJ can terminate and return
		adList p1;   // p1.ID is v
		adList p2;   // p2.ID is w
		MultiDestination md;
		
		ArrayList<MultiDestination> W;
		//long start;
		
		// count # of edges:
		if(!realContract) {  // only do for setInitialOrder(); we don't need it when contract a node.
			p2 = node.get(u).adlistout;
			while(p2 != null) {
				if(node.get(p2.ID).state != Metadata.CONTRACTED)  nEdges++;			
				p2 = p2.ptr;
			}
		}

		// count # of shortcuts:
		//System.out.print("  u: " + u); 
		//p1 = node[u].adlistin; 
		p1 = node.get(u).adlistout;
		while(p1 != null) {
			v = p1.ID;
			
			if(node.get(v).state == Metadata.CONTRACTED) { p1 = p1.ptr; continue; }
			
			//fill all outgoing nodes w to vector W[] and compute their distDJ
			W = null;
			W = new ArrayList<MultiDestination>();
			p2 = node.get(u).adlistout;
			maxLength = 0;
			while(p2 != null) {
				w = p2.ID;
				
				if(node.get(w).state == Metadata.CONTRACTED || v >= w) { p2 = p2.ptr; continue; }   
				md = new MultiDestination();
				md.ID = w;				
				md.dist1Hop = getWeight(v, w);					
				md.distDJ = Metadata.INFINITE;
				md.length = p1.wt + p2.wt;
				if(maxLength < md.length) maxLength = md.length;
				
				W.add(md);
				
				p2 = p2.ptr;
			}	
			
			if(W.size() > 0) {
				//start = System.currentTimeMillis();
				distMDSD(v, W, maxLength);  // TIME CONSUMING! -- Computing multiple shortest dist from v to each w in W[]
				//t_distMDSD += System.currentTimeMillis() - start;
			}
			
			nShortcuts += addShortcutsOnW(v, u, W, realContract);
			//System.out.print("\n nShortcuts:" + nShortcuts);
			p1 = p1.ptr;
		}
		
		if(maxShortcuts < nShortcuts) maxShortcuts = nShortcuts;
		if(maxEdges < nEdges) maxEdges = nEdges;
		//System.out.print("\n\n nShortcuts, nEdges: " + nShortcuts +" " + nEdges);
				
		return nShortcuts -  nEdges;
	}

	/**
	  * After distMDSD computed multi-destination shortest paths(W[i].distDJ) , 
	  * we compare them with dist1Hop and 
	  * length (dist2Hop); if shorter, than add shortcuts (v, W[i].ID, W[i].length).
	  * 
	  * @param v: single source
	  * @param u: node to be contracted
	  * @param W: multi destinations
	  * @param readContract: if true, we really add shortcuts; otherwise, it means we just initially 
	  *          estimate the initial order of node u                                       
	  */
	int addShortcutsOnW(int v, int u, ArrayList<MultiDestination> W, boolean realContract){
		int w, distDJ, dist1Hop, length;   
		int nShortcuts = 0;

		for(int i=0; i<W.size(); i++) {
			w = W.get(i).ID;
			distDJ = W.get(i).distDJ;
			dist1Hop = W.get(i).dist1Hop;
			length = W.get(i).length;

			if(distDJ == length) { // assume v-u-w is shortest path
				if(realContract) {
					if(dist1Hop == Metadata.INFINITE) { 
						addShortcuts(v, w, length); // when setInitialOrder(), we don't really add shortcuts into graph
						nShortcuts++;
					}
					else updateWeight(v, w, length);
				}
				else if(dist1Hop == Metadata.INFINITE) {
					nShortcuts++;
				}
				//else {;} // do nothing; not updateWeight() for setinitialorder()
			}
			//else { //if(distDJ < dist)
				//;  // do nothing, because we have distDJ as "witness path"
			//}			
		}

		return nShortcuts;
	}
	// addShortcutsOnW()


	/**
	  * Update 'D' for u's neighbours. Its coefficient is 120.
	  *  u is the node which is being contracted. 
	  *  This guarantees that the nodes contracted later have higher orders than the nodes contracted earlier.
	  *  The purpose, I guess, is also to delay contraction of the nodes whose neighbor has been contracted 
	  *  (so possibly these nodes has more edges?)
	  * 
	  */
	void updateNeighbours(int u) { // update 'D' for u's neighbours; u is the node which is being contracted. This guarantees that the nodes later contracted have higher orders
		int v;
		adList p;

		//p = node[u].adlistin;
		
		p = node.get(u).adlistout;

		while(p != null) { 
			v = p.ID;
			if(node.get(v).state != Metadata.CONTRACTED) {
				node.get(v).OD += 120;
				if(node.get(v).OD <= node.get(u).OD) {
					node.get(v).OD = node.get(u).OD + 1;  // node[v].OD = min { node[v].OD, node[u].OD-1 }
				}
			}
			p = p.ptr;
		}
	}

	/**
	  * Contract node u from graph -- only label it "CONTRACTED".
	  * But in nodeOrdering(), this will affect the order of other node -- D = deletedNeighbours.
	  */
	void contractNode(int u){ 
		//System.out.print("\ncontract node: " + u);
		edgeDiffbyMultiDest(u, true); // v1.5 multi-desti for finding witness path  
		node.get(u).state = Metadata.CONTRACTED;
		updateNeighbours(u);  // This leads to re-shuffle and re-order odid[]
	} 
	
	/* Not used.
	 
	// Implementing Fisher¨CYates shuffle on odid    		   
	void shuffleArray(int start, int end) {	
		Random rnd = new Random();
	    ODID a;
	    for (int i = end - 1; i > start; i--){
	    	int index = rnd.nextInt(i+1);//+start;
	    	// Simple swap
	    	a = odid.get(index);
	    	odid.set(index, odid.get(i));
	    	odid.set(i, a);
	    }
	}
	
	//sort odid in a range by OD:
	void sortOdid(int start, int end) {	
		merge_sort(start, end);
	}	

	void merge(int p, int r) {
		int mid = (p + r) / 2; //floor((p + r) / 2);
		int i1 = 0;
		int i2 = p;
		int i3 = mid + 1;

		// Temp array
		int[] tempOD = new int[r-p+1];
		int[] tempID = new int[r-p+1];
		
		// Merge in sorted form the 2 arrays
		while ( i2 <= mid && i3 <= r ) {
			if ( odid.get(i2).OD < odid.get(i3).OD ) {
				tempOD[i1] = odid.get(i2).OD; //data[i2++];
				tempID[i1] = odid.get(i2).ID;
				i1++; i2++;
			}
			else {
				tempOD[i1] = odid.get(i3).OD; //data[i3++];
				tempID[i1] = odid.get(i3).ID;
				i1++; i3++;
			}
		}
		// Merge the remaining elements in left array
		while ( i2 <= mid ) {
			tempOD[i1] = odid.get(i2).OD; //data[i2++];
			tempID[i1] = odid.get(i2).ID;
			i1++; i2++;
		}

		// Merge the remaining elements in right array
		while ( i3 <= r ) {
			tempOD[i1] = odid.get(i3).OD; //data[i3++];
			tempID[i1] = odid.get(i3).ID;
			i1++; i3++;
		}
		// Move from temp array to master array
		for ( int i = p; i <= r; i++ ) {
			odid.get(i).OD = tempOD[i-p]; //data[i] = temp[i-p];
			odid.get(i).ID = tempID[i-p];
		}
	}

	void merge_sort(int p, int r) {
		if ( p < r ) {
			int mid = (p + r) / 2;//(int) Math.floor((p + r) / 2);
			merge_sort(p, mid);
			merge_sort(mid + 1, r);
			merge(p, r);
		}
	}
	*/
	
	/**
	  * Set initial order to be "-190 *  # of edgeDiff" for each node.
	  * edgeDiff = nShortcuts - nEdges. 
	  *
	  * <p> O(?)
	  *
	  * @param n1: set initial order starting from node n1
	  * @param odid: map of ID to OD
	  */

	void setInitialOrder(int n1) {
		int i, u;
		
		System.out.print("\n Set initial orders from node " + n1 + " to " + n +":");

		long start = System.currentTimeMillis();

		//System.out.print("\nshuffleArray");
		//shuffleArray(n1, n);
		System.out.print("\n  Shuffle...");		
		Collections.shuffle(odid.subList(n1, n));
	    
		System.out.print("\n  Set orders...");
		for(i= n1; i<= n; i++) {
			u = odid.get(i).ID;	
			odid.get(i).OD = 190 * edgeDiffbyMultiDest(u, false); // v1.5 multi-destination for finding witness path
			//if((i % 10000) == 0) {
				//System.out.print("\n  "+ i+"#: ID:" + u +"  OD:" + odid.get(i).OD);		
			//}
		}
		
		//System.out.print( "\n\nsortOdid(" + n1 + "," + n + ")");			
		//sortOdid(n1, n);
		System.out.print( "\n  Sort...");
		Collections.sort(odid.subList(n1, n)); //Collections.sort(odid.subList(n1, n1));
		System.out.print( "\n Set initial orders in time: "  + ((System.currentTimeMillis() - start)/1000.0) + "s");
		//System.out.print( "\ngetWeight(), distMDSD(), addShortcutsOnW() used time: " + (t_getWeight/1000.0) + "s, " + (t_distMDSD /1000.0) + "s, " + (t_addShortcutsOnW/1000.0) + "s");
		//System.out.print( "\nt_onetoManyDest[]: " + (t_onetoManyDest/1000.0) + "s"); //",removeMin(): " + (t_onetoManyDest[0]/1000.0) + ", insert(): " + (t_onetoManyDest[1]/1000.0) + ", decreaseKey(): " + (t_onetoManyDest[2]/1000.0) + ", visitedReset(): " + (t_onetoManyDest[3]/1000.0));
	}
    // setInitialOrder()

	/**
	  * It is also named "orderSeletion()".
	  * Compute node order (OD) for each node u in the graph.
	  * It is initial values of OD, computed by the formula
	  *			node[u].OD = 190 * E + 120 * D,
	  *			where E =  edgeDifference(u),  D = deletedNeighbours(u).
	  * It will be updated furtherly by hierarchyConstruction(). ---- ???? shortcuts become more and more, 
	  * node degrees become very large (edges and shortcuts upto 562 and 38902), and computing becomes slower and slower.
	  *
	  * Alternatively select node i or n-i. Select node ID uniformly rather than consecutively, 
	  * because usually the consecutive ID are neighbors, adding shortcuts consecutively are
	  * likely lead to complete graph
	  *
	  * <p>: O(n*delta^3), because the complexity of edgeDifference is O(delta^2 * delta), delta^2 for delta^2 pairs, and delta for local DJ search.
	  *
	  * @param g: graph G' = (V, E', W') that has been changed during computing edge differences.
	  */
 
	void nodeOrdering() {
		int i, u; 
		long start = 0;
		//int n = node.size()-1;
		int stage = 0;  // divide node[] into SEG segments
		int STAGE = 28; // n1[STAGE+1]
		
		int n1[] = {1, 
					  n/8, 2*n/8, 3*n/8, 4*n/8, 
					  n/2+n/16, n/2+2*n/16, n/2+3*n/16, n/2+4*n/16, 
					  3*n/4+n/32, 3*n/4+2*n/32, 3*n/4+3*n/32, 3*n/4+4*n/32,
					  7*n/8+n/64, 7*n/8+2*n/64, 7*n/8+3*n/64, 7*n/8+4*n/64,
                      //7*n/8+5*n/64, 7*n/8+6*n/64, 7*n/8+7*n/64, 7*n/8+8*n/64
					  15*n/16+1*n/128, 15*n/16+2*n/128, 15*n/16+3*n/128, 15*n/16+4*n/128,
					  31*n/32+1*n/256, 31*n/32+2*n/256, 31*n/32+3*n/256, 31*n/32+4*n/256,
					  31*n/32+5*n/256, 31*n/32+6*n/256, 31*n/32+7*n/256, 31*n/32+8*n/256	
					  }; //1..~n (~n nearly equal to n)  // Proposed by Deqi Li, Jan 14,2014.
				
		System.out.print( "\nNode ordering with 1/4 exponentially decreasing size...");
		
		long start0 = System.currentTimeMillis(); 
		
		i = 1;
		while(i<=n) {
			if(i == n1[stage]) { // restart initial order for un-contracted nodes at each stage
				System.out.print("\n\nStarting stage " + (stage+1) + " (out of " + (STAGE+1)+")");
				setInitialOrder(i);  // set initial order starting from node i
				start = System.currentTimeMillis(); 
				System.out.print("\n Node ordering (node contracting) starting with intial order: " + odid.get(i).OD);
			}

			u = odid.get(i).ID;			
			contractNode(u);  // contract nodes in order of OD
			node.get(u).OD = i;      // so order increases by one at each step

			if(stage < STAGE){
				if(i == n1[stage+1]-1) {
					System.out.print("\n Node ordering (node contracting) used time: " + ((System.currentTimeMillis() - start)/1000.0) + "s");//\n"); //\n";			
					System.out.print("\n Max #Shortcuts: " + maxShortcuts + ", Max #Edges: " + maxEdges);
					System.out.print("\nEnding stage " + (stage+1) + "\n");
					System.out.print("\nTime passed: " + ((System.currentTimeMillis() - start0)/1000.0) + "s"); //\n";
					stage++;
				}
			}

			i++;
		}
		System.out.print("\nnodeOrdering used time: " + (System.currentTimeMillis() - start)/1000.0 + "s"); //\n";
		System.out.print("\n\nTotally added shortcuts: " + nAddedShortcuts);// +" " + nRemovedEdges;
	}
	// nodeOrdering()

	/**
	  *  Preprocessing generates a node-ordered / hierarchical graph.
	  *  It includes node order selection, adding shortcuts and node contraction.
	  *  This is the main and most time-consuming body of CH algo.
	  *  Input: node[] of graph G = (V, E, W), a weighted directed road network;
	  *  Output: node[]' of overlay graph G' = (V, E', W'), where E' = E + shortcuts, W' = W + weights of shortcuts 
	  *  Actually the purpose is output order of each node plus shortcuts for query.
	  *
	  * <p>: O(n*delta^5), because the complexity of nodeOrdering and hierarchyConstruction are O(delta^5).
	  *
	  * @param g: graph G = (V, E, W) which is stored in node[]
	  */
	public void preprocessing() {
		long start = System.currentTimeMillis();  
		System.out.print("\npreprocessing...");// odid.size: " + odid.size());
		nodeOrdering();
		g.clearNodeState(node); 
		System.out.print("\npreprocessing finished. time used: " + ((System.currentTimeMillis() - start)/1000.0) + "s\n\n"); 

		/*hierarchyConstruction(g);*/ // It is implemented in nodeOrdering(), so it is not used.

	}
	// preprocessing()

	public void writeRoad() throws FileNotFoundException, UnsupportedEncodingException{
		String line;  // a line to data file
		int v;     // node IDs and weight on an arc (v,u)
		adList p;
		
		String filename = g.getFilename() + ".preprocJava";
		
		long start = System.currentTimeMillis();
		PrintWriter writer = new PrintWriter(filename, "UTF-8");
	    System.out.print( "begin write preprocessed data into file '" + filename + "'...\n");
	    
	    // write map data file head:
	    for(int i=1; i<=4; i++) {
	    	line = "c";
	    	writer.println(line);
	    }	    
	    line = "p sp "+ n + " " + g.getEdges();  // line 5
	    writer.println(line);
	    line = "c";
    	writer.println(line);
    	
	    for(v=1; v<=n; v++) {
		    p = node.get(v).adlistout;
		    line = "";
		    while(p!=null){
				if(v >= p.ID) { p=p.ptr; continue; }  // omit the edge like(2 1 365); only store edges like (1 2 365)
				line = line + "a " + v + " " + p.ID + " " + p.wt + "\n";
			    p = p.ptr;
			}
		    writer.print(line);
		}

	    writer.println("o");
	    for(v=1; v<=n; v++) {
	    	line = "";
	    	line = line + "o " + v + " " + node.get(v).OD;
	    	writer.println(line);
		}
	    writer.close(); 
		System.out.print("\nwriting preprocessed data finished. time used: " + ((System.currentTimeMillis() - start)/1000.0) +"s\n\n"); 
	}
	//writeRoad()
}
//////////////////////////////////////////////////////////////
// End of class CH -- Contraction Hierarchies.
//////////////////////////////////////////////////////////////

