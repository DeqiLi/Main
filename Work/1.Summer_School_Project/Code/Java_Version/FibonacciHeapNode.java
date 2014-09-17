package CH_Java;

//////////////////////////////////////////////////////////////
// Begin of  class FibonacciHeapNode.
//////////////////////////////////////////////////////////////

/**
 * Implements a node of the Fibonacci heap. 
 * It holds the information necessary for maintaining the structure of the heap. 
 * It also holds the reference to the key value (which is used to determine the heap structure).
 *
 * @author Nathan Fiedler
 */
class FibonacciHeapNode {
	public int data;       // could be ID of node of graph, or order of nodes
	public int key;        // the shortest distance between this node and the source
	public int degree;     // rank, how many child this node has in Fibo tree
	public boolean mark;   // mark how many children has been removed; for later cut() and cascadingCut()
	public FibonacciHeapNode parent;
	public FibonacciHeapNode child;
	public FibonacciHeapNode left;
	public FibonacciHeapNode right;

	FibonacciHeapNode(int Data, int Key){
        right = this;
        left = this;
		parent = null;
		child = null;
		data = Data;  
		key = Key;
		degree = 0;  
    }

	int getKey() {
        return key;
    }

 	int getData() {
        return data;
    }
}
//////////////////////////////////////////////////////////////
// End of  class FibonacciHeapNode.
//////////////////////////////////////////////////////////////

