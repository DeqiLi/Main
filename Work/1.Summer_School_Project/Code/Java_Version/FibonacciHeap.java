package CH_Java;


//////////////////////////////////////////////////////////////
// Begin of  class FibonacciHeap.
//////////////////////////////////////////////////////////////

//class FibonacciHeap<T>
//template <class T> class FibonacciHeap {
class FibonacciHeap {
	private static final int FiboArraySize = 20;
	private	FibonacciHeapNode minNode;
	private int nNodes;   // Used by consolidate() for computing arraySize
	private int reached; // # nodes have been reached.  ---- only for test
	private int settled;   // # nodes have been settled.    ---- only for test
	private int removeMinOperations;
	private int decreaseKeyOperations;

	public void incrementReached() { reached++; }  // ---- only for test
	public void incrementSettled()    { settled++;  }   // ---- only for test
	public void incrementRemoveMinOperations() { removeMinOperations++; }  // ---- only for test
	public void incrementDecreaseKeyOperations()    { decreaseKeyOperations++;  }   // ---- only for test
	public int getReached() { return reached; }  // ---- only for test
	public int getSettled()   { return settled; }     // ---- only for test
	public int getRemoveMinOperations() { return removeMinOperations; }  // ---- only for test
	public int getDecreaseKeyOperations()   { return decreaseKeyOperations; }     // ---- only for test

	FibonacciHeap() { 
		reached = 0; settled = 0;  removeMinOperations = 0; 
		decreaseKeyOperations = 0; nNodes = 0; minNode = null; 
	}
	
	boolean isEmpty() {   return minNode == null;  }
	int size() {  return nNodes; }
	FibonacciHeapNode min()  {   return minNode;  } 

    /**
     * Removes all elements from this heap. 
     */
    void clear() {
        minNode = null;
        nNodes = 0;
    }


	 /**
     * Inserts a new data element into the heap. No heap consolidation is
     * performed at this time, the new node is simply inserted into the root
     * list of this heap.
     *
     * <p>Running time: O(1) actual</p>
     *
     * @param node new node to insert into heap
     * @param key key value associated with data object
     */
	void insert(FibonacciHeapNode node)
    {
        // concatenate node into min list
        if (minNode != null) {
            node.left = minNode;
            node.right = minNode.right; 
            minNode.right = node;
            node.right.left = node;

            if (node.key < minNode.key) {
  				FibonacciHeapNode p = minNode;
				minNode = node;
				node = p;
				//swap(minNode, node);
            }
        } else {
            minNode = node; 
        }

        nNodes++;
    }

	// insert

	
    /**
     * Decreases the key value for a heap node, given the new value to take on.
     * The structure of the heap may be changed and will not be consolidated.
     *
     * <p>Running time: O(1) amortized</p>
     *
     * @param x node to decrease the key of
     * @param k new key value for node x
     *
     * @exception IllegalArgumentException Thrown if k is larger than x.key
     * value.
     */
	void decreaseKey(FibonacciHeapNode x, int k)  
    {
  		if (k > x.key) {
			return;
        }

 		x.key = k;

 		FibonacciHeapNode y = x.parent;

        //if ((y != null) && (x.key < y.key)) {
		//if ((y != null) && (x.key < y.key)) { // here is a bug; because we cannot determine and use "y != null" in one expression
		//if( (unsigned)y > 0x0000000c ) {  // I don't know an appropriate approach to determine whether a pointer is a null pointer
		if (y != null) {
			if(x.key < y.key) {  // separate "(y!=null)" and "(x.key < y.key )"
				cut(x, y);
				cascadingCut(y);
			}
        }

        //if (x.key < minNode.key) {
		//if (x.key < minNode.key) { // here is a bug; because minNode could be null after the last node is removed
		if(minNode != null) {  // we must determine "if(minNode != null)" first
			if (x.key < minNode.key) {
				minNode = x; //????? No need swap(minNode, x)??  ---- No need. Only set x as minNode.
			// Fredman, Tarjan, P6: "If the new key of i is smaller than the key of the minimum
			// node, we redefine the minimum node to be x."
			}
        }
    }

    // decreaseKey

    /**
     * Removes the smallest element from the heap. This will cause the trees in
     * the heap to be consolidated, if necessary.
     *
     * <p>Running time: O(log n) amortized</p>
     *
     * @return node with the smallest key
     */

	FibonacciHeapNode removeMin()
    {
		FibonacciHeapNode z = minNode; 

        if (z != null) { 
            int numKids = z.degree; 
 			FibonacciHeapNode x = z.child; 
            FibonacciHeapNode tempRight; 
            // for each child of z do...
            while (numKids > 0) { // suppose numKids = 2. instruct 6  + numKids*(10 + C )= instruct 3400
                tempRight = x.right; 

                // remove x from child list
                x.left.right = x.right;
                x.right.left = x.left;

                // add x to root list of heap
                x.left = minNode;
                x.right = minNode.right;
                minNode.right = x;
                x.right.left = x;

                // set parent[x] to null
                x.parent = null;
                x = tempRight;
                numKids--;
            }

            // remove z from root list of heap
            z.left.right = z.right;
            z.right.left = z.left;

            if (z == z.right) { 
                minNode = null;
            } else {
                minNode = z.right;
                consolidate(); 
            }

            // decrement size of heap
            nNodes--;  
        }

        return z;  
    }

    // removeMin


    /**
     * Creates a String representation of this Fibonacci heap.
     *
     * @return String of this.
     */
/*	
	void toString_() {
		String s;
        if (minNode == null ) {
 			System.out.print("\nFibonacciHeap=[]\n");
			return;
        }

        // create a new stack and put root on it
 		Vector stack = new Vector();
        stack.add(minNode);

        s = "FibonacciHeap=[";

        // do a simple breadth-first traversal on the tree
		FibonacciHeapNode curr;
		FibonacciHeapNode start;
        while (!stack.isEmpty()) {
            //FibonacciHeapNode<T> curr = stack.pop();
			curr = stack.lastElement();  
			stack.pop_back();
            buf << " (" << curr.getData() << "," << curr.getKey() << ")";
			buf >> s;
			cout << s;

			if(curr!=null) {
				if (curr.child != null) {
					stack.push_back(curr.child);
				}
			}
			else continue;

            //FibonacciHeapNode<T> start = curr;
			start = curr;
			curr = curr.right;

            while (curr != start) {
				buf << " (" << curr.getData() << "," << curr.getKey() << ")";
				buf >> s;
				cout << s;

				if(curr!=null) {
					if (curr.child != null) {
						stack.push_back(curr.child);
					}
				}
				else continue;
                curr = curr.right;
            }
        }

        buf << "]\n"; 
		buf >> s;
 		cout << s << endl;
        //return s;
    }
    // toString
 
	//  The following 4 functions are protected.
*/

    /**
     * The reverse of the link operation: removes x from the child list of y.
     * This method assumes that min is non-null.
     *
     * <p>Running time: O(1)</p>
     *
     * @param x child of y to be removed from y's child list
     * @param y parent of x about to lose a child
     */
 	protected void cut(FibonacciHeapNode x, FibonacciHeapNode y)
    {
        // remove x from childlist of y and decrement degree[y]
        x.left.right = x.right;
        x.right.left = x.left;
        y.degree--;

        // reset y.child if necessary
        if (y.child == x) {
            y.child = x.right;
        }

        if (y.degree == 0) {
            y.child = null;
        }

        // add x to root list of heap
        x.left = minNode;
        x.right = minNode.right;
        minNode.right = x;
        x.right.left = x;

        // set parent[x] to nil
        x.parent = null;
        // set mark[x] to false
        x.mark = false;
    }

    // cut

	    /**
     * Performs a cascading cut operation. This cuts y from its parent and then
     * does the same for its parent, and so on up the tree.
     *
     * <p>Running time: O(log n); O(1) excluding the recursion</p>
     *
     * @param y node to perform cascading cut on
     */
	protected void cascadingCut(FibonacciHeapNode y)
    {
 		FibonacciHeapNode z = y.parent;

        // if there's a parent...
        if (z != null) {
            // if y is unmarked, set it marked
            if (!y.mark) {
                y.mark = true;
            } else {
                // it's marked, cut it from parent
                cut(y, z);

                // cut its parent as well
                cascadingCut(z);
            }
        }
    }

    // cascadingCut
 
   /**
     * Make node y a child of node x.
     *
     * <p>Running time: O(1) actual</p>
     *
     * @param y node to become child
     * @param x node to become parent
     */
 	protected void link(FibonacciHeapNode y, FibonacciHeapNode x)
    {
        // remove y from root list of heap
        y.left.right = y.right;
        y.right.left = y.left;

        // make y a child of x
        y.parent = x;

        if (x.child == null) {
            x.child = y;
            y.right = y;
            y.left = y;
        } else {
            y.left = x.child;
            y.right = x.child.right;
            x.child.right = y;
            y.right.left = y;
        }

        // increase degree[x]
        x.degree++;

        // set mark[y] false
        y.mark = false;
    }

    // link

	/* Fredman, Tarjan:
	* "
	*	The delete min operation requires finding pairs of tree roots of the same rank to
	*	link. To do this we use an array indexed by rank, from zero up to the maximum
	*	possible rank. Each array position holds a pointer to a tree root. When performing
	*	a delete min operation, after deleting the minimum node and forming a list of the
	*	new tree roots, we insert the roots one by one into the appropriate array positions.
	*	Whenever we attempt to insert a root into an already occupied position, we perform
	*	a linking step and attempt to insert the root of the new tree into the next higher
	*	position. After successfully inserting all the roots, we scan the array, emptying it.
	*	The total time for the delete min operation is proportional to the maximum rank
	*	of any of the nodes manipulated plus the number of linking steps.
	*  "
	*   This is the most time consuming operation,
	*    which involves thousands lines of instructs (~3000-4000 instructs).
	*/
	protected void consolidate()  
    {
		int i;

		// Create & Initialize degree array   
		// FiboarraySize: should be as large as O(log sqrt(nNodes))
		FibonacciHeapNode array[] = new FibonacciHeapNode[FiboArraySize]; 
		for (i = 0; i < FiboArraySize; i++) { 
			array[i] = null;
		}

        // Find the number of root nodes.
        int numRoots = 0; 
		FibonacciHeapNode x = minNode; 

        if (x != null) { 
            numRoots++; 
            x = x.right; 

            while (x != minNode) { 
                numRoots++;
                x = x.right; 
            }
        }

        // For each node in root list do...
		//  suppose for() runs 5 times, and numRoots = 30.  instructs = numRoots * for = 30 * (5*20) = 3000.
        while (numRoots > 0) {                          // numRoots: how much is it usually? O(log sqrt (nNodes))? Yes, usually 10-15 when n=3600K
            // Access this node's degree..
            int d = x.degree;
 			FibonacciHeapNode next = x.right;

            // ..and see if there's another of the same degree.
            for (;;) {  // the condition variable d is changing by d++;  
 				FibonacciHeapNode y = array[d];  
                if (y == null) {
                    // Nope.
                    break;
                }

                // There is, make one of the nodes a child of the other.
                // Do this based on the key value.
                if (x.key > y.key) {  // swap x and y
 					FibonacciHeapNode temp = y;
                    y = x;
                    x = temp;
                }

                link(y, x); 

                // We've handled this degree, go to next one
                array[d] = null; //array.set(d, null);
                d++;
            }  // for

            // Save this node for later when we might encounter another
            // of the same degree
            array[d] = x; //array.set(d, x);

            // Move forward through list
            x = next;
            numRoots--;
        } 

        // Set min to null (effectively losing the root list) and
        // reconstruct the root list from the array entries in array[]
        minNode = null;

        for (i = 0; i < FiboArraySize; i++) {  
			FibonacciHeapNode y = array[i] ;  
            if (y == null) {
                continue;
            }

            // We've got a live one, add it to root list
            if (minNode != null) {
                // First remove node from root list
                y.left.right = y.right;
                y.right.left = y.left;

                // Now add to root list, again
                y.left = minNode;
                y.right = minNode.right;
                minNode.right = y;
                y.right.left = y;

                // Check if this is a new min
                if (y.key < minNode.key) {
                    minNode = y;
                }
            } else {
                minNode = y;
            }
        }
    }

    // consolidate
  
}
//////////////////////////////////////////////////////////////
// End of  class FibonacciHeap.
//////////////////////////////////////////////////////////////

