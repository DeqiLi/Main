package CH_Java;
import java.util.ArrayList;

//////////////////////////////////////////////////////////////
// Begin of class Binary Heap.
//////////////////////////////////////////////////////////////
  
/**
  * A basic implementation of binary heap to support bidi-Dijkstra's algo and bidi-CH-DJ algo.
  * It includes the operations insert(), removeMin(), decreaseKey() and getSize() only.
  * It initializes with an initial heap size.
  * 
  * Use an array:  no need for explicit parent or child pointers.
  * Parent(i) = |_ i/2 _|
  * Left(i)   = 2i 
  * Right(i)  = 2i + 1 
  * The first one heap[1] is the root as well as the minimum node.
  * Reference to Princeton slides "Binary and Binomial Heaps".
  * 
  * ---- Coded by Deqi Li, 2014.1.22
  */
class BinaryHeap { 
	private int nNodes;
	private int maxHeapSize;
	private ODID heap[];  // ODID is the node structure
	private ArrayList<Node>node;
	//public BinaryHeap(int n) { nNodes = 0; heap.resize(n+1); }
	public BinaryHeap(ArrayList<Node> node0) {
		maxHeapSize = 120001;
		node = node0; nNodes = 0; 
		heap = new ODID[maxHeapSize]; // Usually  20000 is big enough for the large file 'USA-road-d.USA.gr' file.
		for(int i=0; i<maxHeapSize; i++) {
			heap[i]=new ODID();
		}
		heap[0].OD = 0; heap[0].ID = 0;// the first node in the heap is not be used; the first graph node is inserted into heap[1] rather than heap[0]
	}
	/*
	public void init(ArrayList<Node> node0) {
		maxHeapSize = 120001;
		node = node0; nNodes = 0; 
		heap = new ODID[maxHeapSize]; // Usually  20000 is big enough for the large file 'USA-road-d.USA.gr' file.
		for(int i=0; i<maxHeapSize; i++) {
			heap[i]=new ODID();
		}
		heap[0].OD = 0; heap[0].ID = 0;// the first node in the heap is not be used; the first graph node is inserted into heap[1] rather than heap[0]
	}*/
	
	public int getSize() { return nNodes; }
	public int getmaxHeapSize() { return maxHeapSize; }
	public boolean isEmpty() { if(nNodes > 0) return false; else return true; }
	public int min() { return heap[1].OD; }  // the minimum node is heap[1]
	public void clear() { nNodes = 0; }  // heap.resize(0); heap.clear();
	
	/**
	  *  Insert element x, x.id = id, x.key = key. 
	  *  The first one is inserted into heap[1] rather than heap[0]
	  *  Return with the index of x in heap.
	  *
	  *  <p>: O(log(nNodes))
	  *
	  * @param id:     the ID of graph node in DJ algo
	  * @param key:  the tentative dist of this node
	  * @param flag:  the flag of the bi-direction of DJ algo; flag: 0 or 1.
	  */
	public int insert(int id, int key, int flag) { 
/*		for(int i=0; i<=nNodes; i++) {
			System.out.print("\n    heap ID/OD: " +  heap[i].ID + " "+ heap[i].OD);
		}
		System.out.print("\n    nNodes="+nNodes+"\n");
		
		if(nNodes>4)System.exit(0); */

		nNodes++;
		//if(nNodes >= heap.size()) heap.resize(nNodes + nNodes);  // double heap size if necessary

		// insert the new node at the end of heap
		heap[nNodes].ID = id;
		heap[nNodes].OD = key;
	
		int idx = nNodes; 
		int parent = idx / 2;
		node.get(id).bp[flag] = idx;
		
		//System.out.print("\n    key ID/ds:" + id + "/" + key + "; parent ID/ds:" + heap[parent].ID + "/" + heap[parent].OD);
		//adjust it to satisfy the heap property
		while((parent > 0) && (key < heap[parent].OD)) { // Bubble up: swap the node with its parent if its key is larger than its parent's.
			//System.out.print("\n    swapNodes(" + idx+", "+parent+", "+flag+")");
			swapNodes(idx, parent, flag);
			
			idx = parent;
			parent = idx / 2;
			//key = heap[idx].OD;
			//System.out.print("\n    parent: " + parent);
		}
		/*
		for(int i=0; i<=nNodes; i++) {
			System.out.print("\n    heap ID/OD: " +  heap[i].ID + " "+ heap[i].OD);
		}
		System.out.print("\n    nNodes="+nNodes+"\n");
		
		if(nNodes>4)System.exit(0); */
		
		return idx;
	}

	/**
	  *  Delete minimum element and 
	  *  return the ID of graph node of DJ algo
	  *
	  *  <p>: O(log(nNodes))
	  *
	  * @param flag:  the flag of the bi-direction of DJ algo; flag: 0 or 1.
	  */
	public int removeMin(int flag) {
//System.out.print("\nremoveMin("+flag+")");

		if(nNodes == 0) return -1;
		int ID = heap[1].ID;  // ID: graph node ID;  heap[1]: root

		// Exchange root with rightmost leaf: 
		swapNodes(1, nNodes, flag);
		int key = heap[1].OD;
		// Remove the rightmost leaf:
		//node[heap[nNodes].ID].bp[flag] = -1;
		nNodes--;
		// Bubble root down until it's heap ordered; power struggle principle:  better subordinate is promoted
		int lchild, rchild; 
		int idx = 1; 
		while(true) {
			lchild = 2 * idx;
			rchild = lchild + 1; 
			if(lchild > nNodes) break; // stop bubble root down
			if(rchild > nNodes) {  // no rchild; lchild is the last leaf of the heap
				if(heap[lchild].OD < key)  swapNodes(idx, lchild, flag);
				break;  // stop bubble root down, bcz lchild is the last leaf of the heap
			}
			// rchild <= nNodes 
			if(heap[lchild].OD >= key && heap[rchild].OD >= key) break; // stop bubble root down
			if(heap[lchild].OD <= heap[rchild].OD) { swapNodes(idx, lchild, flag); idx = lchild; }
			else { swapNodes(idx, rchild, flag); idx = rchild; }
		}

		return ID;
	}

	public boolean decreaseKey(int idx, int newKey, int flag) { //Decrease key of element x to newKey
//System.out.print("\ndecreaseKey("+idx+", "+newKey+", "+flag+")");

		if(idx  < 0 || idx > nNodes)   return false;
		if(heap[idx].OD <= newKey) return false;

		heap[idx].OD = newKey;	

		//adjust it to satisfy the heap property
		// Bubble the node up until it's heap ordered; power struggle principle:  better subordinate is promoted
		int parent = idx / 2;
		while(parent > 0 && newKey < heap[parent].OD ) { // Bubble up: swap the node with its parent if its key is larger than its parent's.
			swapNodes(idx, parent, flag);
			idx = parent;
			parent = idx / 2;
			//newKey = heap[idx].OD;
			//System.out.print("\n    parent: " + parent);
		}
		
		return true;
	}

	/**
	  * temporarily test its corretness
	  */
	void test(int n) { 
		int i, j, ID, key, idx;
		//node.resize(n);
		if(n>=maxHeapSize) { System.out.println("\nthe number of nodes should be less than 20000.\n"); return; } 

		System.out.print("\n ---------------- Binary Heap CORRECTNESS TEST ----------------- \n");
		System.out.print("\ninsert() test:");
		for(i=1; i<=n; i++) {
			ID = i + 1000;
			key = n - i;
			System.out.print( "\n  insert(" + ID + ", " + key + ")");
			insert(ID, key, 0);
		}
	    for(i=1; i<=getSize(); i++) System.out.print( "\n  " + i + "#: (" +  heap[i].ID +", " +  heap[i].OD +")");
		System.out.print( "\nSize: " + getSize());

		System.out.print("\n\ndecreaseKey() test:");
		for(j=1; j<=n; j++) {
			idx =  j*5;
			key = heap[j].OD - 99999;
			System.out.print( "\n  decreaseKey(" + idx + ", " + key + ")");
			decreaseKey(idx, key, 0);
		    for(i=1; i<=getSize(); i++) System.out.print( "\n  " + i + "#: (" +  heap[i].ID +", " +  heap[i].OD +")");
		}
		
		System.out.print("\n\nremoveMin() test:");
		for(j=1; j<=n; j++) {
			System.out.print("\n  min, size: " + removeMin(0)); 
			System.out.print( " " + getSize());
			for(i=1; i<=getSize(); i++) System.out.print( "\n  " + i + "#: (" +  heap[i].ID +", " +  heap[i].OD +")");
		}
		 
	}


	/**
	  * temporarily test its performance
	  */
	void testPerformance(int n) { 
		int i, j, ID, key, idx;
		if(n>=maxHeapSize) { System.out.println("\nthe number of nodes should be less than maxHeapSize \n"+maxHeapSize); return; } 
		//node.resize(n);

		//System.out.print("\n ---------------- Binary Heap CORRECTNESS TEST ----------------- \n");
		//System.out.print("\ninsert() test:");
		for(i=1; i<=n; i++) {
			ID = i + 1000;
			key = n - i;
			//System.out.print( "\n  insert(" + ID + ", " + key + ")");
			insert(ID, key, 0);
		}
	    //for(i=1; i<=getSize(); i++) System.out.print( "\n  " + i + "#: (" +  heap[i].ID +", " +  heap[i].OD +")");
		//System.out.print( "\nSize: " + getSize());

		//System.out.print("\n\ndecreaseKey() test:");
		for(j=1; j<=n; j++) {
			idx = j;// j*5;
			key = heap[j].OD - 99999;
			//System.out.print( "\n  decreaseKey(" + idx + ", " + key + ")");
			decreaseKey(idx, key, 0);
		    //for(i=1; i<=getSize(); i++) System.out.print( "\n  " + i + "#: (" +  heap[i].ID +", " +  heap[i].OD +")");
		}
		
		//System.out.print("\n\nremoveMin() test:");
		for(j=1; j<=n; j++) {
			removeMin(0);
			//System.out.print("\n  min, size: " + removeMin(0)); 
			//System.out.print( " " + getSize());
			//for(i=1; i<=getSize(); i++) System.out.print( "\n  " + i + "#: (" +  heap[i].ID +", " +  heap[i].OD +")");
		}
		 
	}


	/**
	  * swap two nodes of binary heap; update bp of graph node
	  *
	  * <p>: O(1)
	  *
	  *@param i, j: indices of nodes of binary heap
	  */
	protected void swapNodes(int i, int j, int flag) {
		if(i==j) return;

		int id  =  heap[i].ID;
		int key =  heap[i].OD;

		heap[i].OD = heap[j].OD;
		heap[i].ID = heap[j].ID;
		node.get(heap[j].ID).bp[flag] = i; // update binary node pointer of graph node; see graph node class.

		heap[j].OD = key;
		heap[j].ID = id;
		node.get(id).bp[flag] = j;  
	}
}

//////////////////////////////////////////////////////////////
// End of class Binary Heap.
//////////////////////////////////////////////////////////////

