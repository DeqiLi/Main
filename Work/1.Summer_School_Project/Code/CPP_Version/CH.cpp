/*
 * Summer School Project 2014
 * Deqi Li@University of Auckland
 *
 * /

/* CH algorithm: search single-source single-target shortest path 
 
 * Contraction Hierarchies (CH) algorithm is one of currently the most e.cient algorithms
 * to solve NSSP (non-negative single-source shortest path problem). It was developed
 * by Geisberger et al. in 2008 [1, 2] to speed up point-to-point shortest path nding for
 * continental-sized road networks. It involves two phases: the preprocessing phase and
 * the query phase. The former is relatively more time-consuming than the later and the
 * e.ciency of CH algorithm is gained by amortizing the preprocessing time over many
 * NSSP queries. In the CH preprocessing phase, by the technique node contraction, nearly
 * as the same number as the original graph of shortcuts are added into the graph, and each
 * node is set to a unique order represented its priority during the query phase.
 *
 * The CH query algorithm, quite similar to the traditional bi-directional Dijkstra's algo-
 * rithm, is also bi-directional, that is, the query alternates to explore nodes in two directions
 * from the source and from the target respectively. The critical dierence is that the bi-
 * directional Dijkstra's algorithm does not prune any nodes, however CH algorithm only
 * search nodes in ascending order in both directions until it satises a stopping criterion.
 * Metaphorically, CH query algorithm is such an algorithm that jumps on shortcuts steered
 * by node orders. Thus CH algorithm dramatically reduces the number of visited nodes in
 * the query phase and saves much query time.
*/

/*
 * This code has been successfully compiled under:
 * -- Windows 7 Home Premium (Service Pack 1) plus Visual C++ 6.0
 * -- Linux Ubuntu 13.04 (compiling command: g++ -std=c++11 o ch CH.cpp) with g++ (GCC) 4.7.3;
 * -- Linux Fedora release 17 (Beety Miracle) with g++ (GCC) 4.7.2.
*/

/*
 * A typical output:

[dli267@cdmtcs CH]$ ./ch
begin read data from 'USA-road-d.NY.gr.preprocessed'...
.........
file reading finished. 
nodes=264346, edges=805538, read in 0 second

Bheap-CHDJ PERFORMANCE TEST

    #    source    target    distance1   distance2   nVisited1  nVisited2

 1000#    177445    133533     420058    521989   1753   2111
 2000#    243748    117641     840765    145638   2445    166
 3000#    143156     34977     575409    561248   1914   1610
 4000#     83361    204906     804066    578933   3121   2402
 5000#    177439    120593     767983    712654   2798   1490
 6000#    212318    233993     703267   1045761   2503   1929
 7000#    213327     21433     535173    560588   2622   2164
 8000#    256687    207933     395259    407572   1794   1659
 9000#     82025    124700     457262    329344   1392   2401
10000#    190653    262490     614173    527825   2447   2901

------------------------------------------------------------
--------------- Bheap-CHDJ PERFORMANCE TEST --------------
------------------------------------------------------------
 Query time totally used:      51s (10000 pairs)
 Average query time per path:  2.55ms
 Average visited nodes:        1806
 srand(year):                  2012
 Data file:                   'USA-road-d.NY.gr.preprocessed'
 Nodes, Edges:                 264346, 805538
------------------------------------------------------------


Fheap-CHDJ PERFORMANCE TEST

    #    source    target    distance1   distance2   nVisited1  nVisited2

 1000#    177445    133533     420058    521989   1753   2111
 2000#    243748    117641     840765    145638   2445    166
 3000#    143156     34977     575409    561248   1914   1610
 4000#     83361    204906     804066    578933   3121   2402
 5000#    177439    120593     767983    712654   2798   1490
 6000#    212318    233993     703267   1045761   2503   1929
 7000#    213327     21433     535173    560588   2622   2164
 8000#    256687    207933     395259    407572   1794   1659
 9000#     82025    124700     457262    329344   1392   2401
10000#    190653    262490     614173    527825   2447   2901

------------------------------------------------------------
--------------- Fheap-CHDJ PERFORMANCE TEST --------------
------------------------------------------------------------
 Query time totally used:      55s (10000 pairs)
 Average query time per path:  2.75ms
 Average visited nodes:        1806
 srand(year):                  2012
 Data file:                   'USA-road-d.NY.gr.preprocessed'
 Nodes, Edges:                 264346, 805538
------------------------------------------------------------


Trivial-CHDJ PERFORMANCE TEST

    #    source    target    distance1   distance2   nVisited1  nVisited2

 1000#    177445    133533     420058    521989   1753   2111
 2000#    243748    117641     840765    145638   2445    166
 3000#    143156     34977     575409    561248   1914   1610
 4000#     83361    204906     804066    578933   3121   2402
 5000#    177439    120593     767983    712654   2798   1490
 6000#    212318    233993     703267   1045761   2503   1929
 7000#    213327     21433     535173    560588   2622   2164
 8000#    256687    207933     395259    407572   1794   1659
 9000#     82025    124700     457262    329344   1392   2401
10000#    190653    262490     614173    527825   2447   2901

------------------------------------------------------------
--------------- Trivial-CHDJ PERFORMANCE TEST --------------
------------------------------------------------------------
 Query time totally used:      62s (10000 pairs)
 Average query time per path:  3.1ms
 Average visited nodes:        1806
 srand(year):                  2012
 Data file:                   'USA-road-d.NY.gr.preprocessed'
 Nodes, Edges:                 264346, 805538
------------------------------------------------------------

[dli267@cdmtcs CH]$ 
*/

#include "common.h"
#include "datafile.h"
#include <cassert>
#define FiboArraySize 20 // for array used by consolidate(): ArraySize ~ arraySize = ((unsigned) floor(log(nNodes) * oneOverLogPhi)) + 1; // = 20*2 + 1 = 41, not neccessarily big // 16 is bigger enough for 'USA-road-d.USA.gr'; 20 is quite safe. Proposed by Deqi Li, Dec 24, 2013.
#define SIZE_W 300 // W.size() for edgeDiffbyMultiDest; over this size setInitialOrder() is triggered to avoid too long time on node ordering (contration); 150 is bigger enough for 'USA-road-d.USA.gr'; 300 is quite safe.
#define PREPROCESSING 1  // 1: first proprocess, then test performance    0: only test performance

using namespace std;

//////////////////////////////////////////////////////////////
// Begin of  class FibonacciHeapNode.
//////////////////////////////////////////////////////////////

/**
 * Implements a node of the Fibonacci heap. 
 * It holds the information necessary for maintaining the structure of the heap. 
 * It also holds the reference to the key value (which is used to determine the heap structure).
 *
 * @author Nathan Fiedler ---- Modified by Deqi Li
 */
//class FibonacciHeapNode<T> {
//class FibonacciHeapNode {
//template <class T> class FibonacciHeapNode {
class FibonacciHeapNode {
public:
	int data;      // could be ID of node of graph, or order of nodes
    int key;        // the shortest distance between this node and the source
    int degree;  // rank, how many child this node has in Fibo tree
    bool mark;              // mark how many children has been removed; for later cut() and cascadingCut()
    //FibonacciHeapNode<T> parent;
    //FibonacciHeapNode<T> child;
    //FibonacciHeapNode<T> left;
    //FibonacciHeapNode<T> right;
	FibonacciHeapNode* parent;
	FibonacciHeapNode* child;
	FibonacciHeapNode* left;
	FibonacciHeapNode* right;

//public:
	//FibonacciHeapNode(T data, int key){
	FibonacciHeapNode(int Data, int Key){
        right = this;
        left = this;
		parent = NULL;
		child = NULL;
        //this.data = Data;  
        //this.key = Key;
		data = Data; 
		key = Key;
		degree = 0;
    }

    //static T getKey() {
	int getKey() {
        return key;
    }

    //static T getData() {
	//static int getData() {
	int getData() {
        return data;
    }
};
//////////////////////////////////////////////////////////////
// End of  class FibonacciHeapNode.
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// Begin of  class FibonacciHeap.
//////////////////////////////////////////////////////////////

//class FibonacciHeap<T>
//template <class T> class FibonacciHeap {
//@author Nathan Fiedler ---- Modified by Deqi Li
class FibonacciHeap {
private: //public:
	//int FiboArraySize; //10 for preprocessing or 26 or query
	//int oneOverLogPhi; //= 0; //1.0 / log((1.0 + sqrt(5.0)) / 2.0);  // ??? 

    //FibonacciHeapNode<T> minNode;
	FibonacciHeapNode *minNode;
    int nNodes;   // Used by consolidate() for computing arraySize
	//FibonacciHeapNode* array[FiboArraySize];  // using static array to speedup consolidate()
	int reached; // # nodes have been reached.  ---- only for test
	int settled;   // # nodes have been settled.    ---- only for test
	int removeMinOperations;
	int decreaseKeyOperations;

public:
	void incrementReached() { reached++; }  // ---- only for test
	void incrementSettled()    { settled++;  }   // ---- only for test
	void incrementRemoveMinOperations() { removeMinOperations++; }  // ---- only for test
	void incrementDecreaseKeyOperations()    { decreaseKeyOperations++;  }   // ---- only for test
	int getReached() { return reached; }  // ---- only for test
	int getSettled()   { return settled; }     // ---- only for test
	int getRemoveMinOperations() { return removeMinOperations; }  // ---- only for test
	int getDecreaseKeyOperations()   { return decreaseKeyOperations; }     // ---- only for test

	FibonacciHeap() { 
		reached = 0; settled = 0;  removeMinOperations = 0; 
		decreaseKeyOperations = 0; nNodes = 0; minNode = NULL; 
		//if(PREPROCESSING) FiboArraySize = 10; 
		//else FiboArraySize = 26; 
	}//  oneOverLogPhi = 1.0 / log((1.0 + sqrt(5.0)) / 2.0); } 
	//~FibonacciHeap() {}
    bool isEmpty() {   return minNode == NULL;  }
	int size() {  return nNodes; }
    //FibonacciHeapNode<T> min()
	FibonacciHeapNode *min()  {   return minNode;  }  // FibonacciHeapNode* min()?

    /**
     * Removes all elements from this heap.  ----  ?? How about the memory release of nodes
     */
    void clear() {
        minNode = NULL;
        nNodes = 0;
    }
    // clear

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
    //void insert(FibonacciHeapNode<T> node, int key)
	//void insert(FibonacciHeapNode *node, int key)
	void insert(FibonacciHeapNode *node)
    {
		//node->key = key;          // how about its degree when insert a new node?

        // concatenate node into min list
        if (minNode != NULL) {
            node->left = minNode;
            node->right = minNode->right; 
            minNode->right = node;
            node->right->left = node;

            if (node->key < minNode->key) {
                //minNode = node; //?????
				FibonacciHeapNode *p = minNode;
				minNode = node;
				node = p;
				//swap(minNode, node);
            }
        } else {
            minNode = node; //?????
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
    //void decreaseKey(FibonacciHeapNode<T> x, int k)
	void decreaseKey(FibonacciHeapNode *x, int k)  
    {
        //if (k > x.key) {
		if (k > x->key) {
			return;
        }

        //x.key = k;
		x->key = k;

        //FibonacciHeapNode<T> y = x.parent;
		//FibonacciHeapNode y = x.parent;
		FibonacciHeapNode* y = x->parent;

        //if ((y != NULL) && (x.key < y.key)) {
		//if ((y != NULL) && (x->key < y->key)) { // here is a bug; because we cannot determine and use "y != NULL" in one expression
		//if( (unsigned)y > 0x0000000c ) {  // I don't know an appropriate approach to determine whether a pointer is a NULL pointer
		if (y != NULL) {
			if(x->key < y->key) {  // separate "(y!=NULL)" and "(x->key < y->key )"
				cut(x, y);
				cascadingCut(y);
			}
        }

        //if (x.key < minNode.key) {
		//if (x->key < minNode->key) { // here is a bug; because minNode could be NULL after the last node is removed
		if(minNode != NULL) {  // we must determine "if(minNode != NULL)" first
			if (x->key < minNode->key) {
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
    //FibonacciHeapNode<T> removeMin()
	FibonacciHeapNode *removeMin()  // consolidate() // instructs = 96 + 3000 + 300 =  3400
    {
        //FibonacciHeapNode<T> z = minNode;
		FibonacciHeapNode *z = minNode; // instruct 1

        if (z != NULL) { // instruct 2
            int numKids = z->degree; // instruct 3
            //FibonacciHeapNode<T> x = z->child;
            //FibonacciHeapNode<T> tempRight;
			FibonacciHeapNode* x = z->child; // instruct 4
            FibonacciHeapNode* tempRight; // instruct 5
            // for each child of z do->->->
            while (numKids > 0) { // suppose numKids = 2. instruct 6  + numKids*(10 + C )= instruct 3400
                tempRight = x->right; 

                // remove x from child list
                x->left->right = x->right;
                x->right->left = x->left;

                // add x to root list of heap
                x->left = minNode;
                x->right = minNode->right;
                minNode->right = x;
                x->right->left = x;

                // set parent[x] to NULL
                x->parent = NULL;
                x = tempRight;
                numKids--;
            }

            // remove z from root list of heap
            z->left->right = z->right; //instruct 27
            z->right->left = z->left;  //instruct 28

            if (z == z->right) {  //instruct 29
                minNode = NULL;  //instruct 30
            } else {
                minNode = z->right;
                consolidate();  // //C= 3400, instruct 30 + C =  3400
            }

            // decrement size of heap
            nNodes--;  //instruct ?
        }

        return z;  //instruct ?
    }
    // removeMin


    /**
     * Creates a String representation of this Fibonacci heap.
     *
     * @return String of this.
     */
	void toString() {
		string s;
        if (minNode == NULL ) {
            //return "FibonacciHeap=[]";
			cout <<  "\nFibonacciHeap=[]\n";
			return ;
        }
		//cout <<  "\nFibonacciHeap=[]\n";

        // create a new stack and put root on it
        //vector<FibonacciHeapNode<T>> stack;
		vector<FibonacciHeapNode*> stack;//  vector<FibonacciHeapNode *> stack;
		//stack.allocator.allocate(512*sizeof(FibonacciHeapNode*), FibonacciHeapNode *);
		//ack.reserve(512);
		
        stack.push_back(minNode);

        stringstream buf;
		buf.clear();
        buf << "FibonacciHeap=[";

        // do a simple breadth-first traversal on the tree
		FibonacciHeapNode *curr;
		FibonacciHeapNode *start;
        while (!stack.empty()) {
            //FibonacciHeapNode<T> curr = stack.pop();
			curr = stack.back();  
			stack.pop_back();
            buf << " (" << curr->getData() << "," << curr->getKey() << ")";
			buf >> s;
			cout << s;

			if(curr!=NULL) {
				if (curr->child != NULL) {
					stack.push_back(curr->child);
				}
			}
			else continue;

            //FibonacciHeapNode<T> start = curr;
			start = curr;
			curr = curr->right;

            while (curr != start) {
				buf << " (" << curr->getData() << "," << curr->getKey() << ")";
				buf >> s;
				cout << s;

				if(curr!=NULL) {
					if (curr->child != NULL) {
						stack.push_back(curr->child);
					}
				}
				else continue;
                curr = curr->right;
            }
        }

        buf << "]\n"; 
		buf >> s;
 		cout << s << endl;
        //return s;
    }

    // toString
 
	//  The following 4 functions are protected.

protected:

    /**
     * The reverse of the link operation: removes x from the child list of y.
     * This method assumes that min is non-NULL.
     *
     * <p>Running time: O(1)</p>
     *
     * @param x child of y to be removed from y's child list
     * @param y parent of x about to lose a child
     */
    //void cut(FibonacciHeapNode<T> x, FibonacciHeapNode<T> y)
	void cut(FibonacciHeapNode *x, FibonacciHeapNode *y)
    {
        // remove x from childlist of y and decrement degree[y]
        x->left->right = x->right;
        x->right->left = x->left;
        y->degree--;

        // reset y->child if necessary
        if (y->child == x) {
            y->child = x->right;
        }

        if (y->degree == 0) {
            y->child = NULL;
        }

        // add x to root list of heap
        x->left = minNode;
        x->right = minNode->right;
        minNode->right = x;
        x->right->left = x;

        // set parent[x] to nil
        x->parent = NULL;
        // set mark[x] to false
        x->mark = false;
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
	//void cascadingCut(FibonacciHeapNode<T> y)
	void cascadingCut(FibonacciHeapNode *y)
    {
        //FibonacciHeapNode<T> z = y->parent;
		FibonacciHeapNode *z = y->parent;

        // if there's a parent->->->
        if (z != NULL) {
            // if y is unmarked, set it marked
            if (!y->mark) {
                y->mark = true;
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
    //void link(FibonacciHeapNode<T> y, FibonacciHeapNode<T> x)
	void link(FibonacciHeapNode *y, FibonacciHeapNode *x)
    {
        // remove y from root list of heap
        y->left->right = y->right;
        y->right->left = y->left;

        // make y a child of x
        y->parent = x;

        if (x->child == NULL) {
            x->child = y;
            y->right = y;
            y->left = y;
        } else {
            y->left = x->child;
            y->right = x->child->right;
            x->child->right = y;
            y->right->left = y;
        }

        // increase degree[x]
        x->degree++;

        // set mark[y] false
        y->mark = false;
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
	*    which involves thousands of C++ instructs (~3000-4000 instructs).
	*/
    void consolidate() // instructs = 96 + 3000 + 300 =  3400
    {
		int i;
        //int arraySize = ((unsigned) floor(log(nNodes) * oneOverLogPhi)) + 1;   // its typical value is about 16 when n=3590K
		//cout <<"\n'0'" << int('0') << "\noneOverLogPhi: " << 1.0 / log((1.0 + sqrt(5.0)) / 2.0);  // 2.07809

/*	  
        // for test	the live nodes in Fibo heap.
		// the typical example is: (n = 3598k, graph="USA-road-d.E.gr")
		//  
        //int arraySize = ((unsigned) floor(log(nNodes) * oneOverLogPhi)) + 1;   // its typical value is about 16 when n=3590K
		//int static max_arraySize =0;    
		//if(max_arraySize < arraySize) {
			//max_arraySize = arraySize;
			//cout <<"\nmax_arraySize: " << max_arraySize <<"\n";  
		//}
		// max_numRoots: 24
		// max_nNodes: 1878
		//
		int static max_nNodes=0;    
		int static max_numRoots=0;

		if(max_nNodes < nNodes) {
			max_nNodes = nNodes;
			cout <<"max_nNodes: " << max_nNodes <<"\n";  
		}
*/
		// Create & Initialize degree array
		//vector<FibonacciHeapNode<T>> array = (NULL, arraySize);    
		//vector<FibonacciHeapNode> array = (NULL, arraySize);    
		//vector<FibonacciHeapNode*> array(arraySize);    
		// FiboarraySize: should be as large as O(log sqrt(nNodes))
		FibonacciHeapNode* array[FiboArraySize];   //instruct 1
		for (i = 0; i < FiboArraySize; i++) {     //instruct 1 + 30 = 31
			array[i] = NULL;
		}

        // Find the number of root nodes.
        int numRoots = 0;   //instruct 32
        //FibonacciHeapNode<T> x = minNode;
		FibonacciHeapNode *x = minNode; //instruct 33

        if (x != NULL) { //instruct 34
            numRoots++; //instruct 35
            x = x->right;  //instruct 36

            while (x != minNode) { //suppose numRoots = 30; instruct 36 + numRoots * 2 = 96
                numRoots++;
                x = x->right; // x = 0x138874e0, x->right = 0x1388af80, x->right->right = 0x138874e0 = x,  infinite loop
            }
        }
		//cout << numRoots <<"\n";
/*
		if(max_numRoots < numRoots) {
			max_numRoots = numRoots;
			cout <<"max_numRoots: " << max_numRoots <<"\n";  
		}
*/
        // For each node in root list do...
		//  suppose for() runs 5 times, and numRoots = 30.  instructs = numRoots * for = 30 * (5*20) = 3000.
        while (numRoots > 0) {                          // numRoots: how much is it usually? O(log sqrt (nNodes))? Yes, usually 10-15 when n=3600K
            // Access this node's degree..
            int d = x->degree;
            //FibonacciHeapNode<T> next = x.right;
			//FibonacciHeapNode next = x.right;
			FibonacciHeapNode* next = x->right;

            // ..and see if there's another of the same degree.
            for (;;) {  // the condition variable d is changing by d++;  
                //FibonacciHeapNode<T> y = array[d]; // FibonacciHeapNode<T> y = array.get(d);
				//FibonacciHeapNode y = array[d];  
				FibonacciHeapNode* y = array[d];  
                if (y == NULL) {
                    // Nope.
                    break;
                }

                // There is, make one of the nodes a child of the other.
                // Do this based on the key value.
                if (x->key > y->key) {  // swap x and y
                    //FibonacciHeapNode<T> temp = y;
					FibonacciHeapNode* temp = y;
                    y = x;
                    x = temp;
                }

                // FibonacciHeapNode<T> y disappears from root list->
                link(y, x);  // 10 instructs

                // We've handled this degree, go to next one
                array[d] = NULL; //array->set(d, NULL);
                d++;
            }  // for

            // Save this node for later when we might encounter another
            // of the same degree
            array[d] = x; //array->set(d, x);

            // Move forward through list
            x = next;
            numRoots--;
        } 

        // Set min to NULL (effectively losing the root list) and
        // reconstruct the root list from the array entries in array[]
        minNode = NULL;

        for (i = 0; i < FiboArraySize; i++) {  // instructs = 30 * 10 = 300 
            //FibonacciHeapNode<T> y = array[i] ;  // y = array->get(i);
			FibonacciHeapNode* y = array[i] ;  // y = array->get(i);
            if (y == NULL) {
                continue;
            }

            // We've got a live one, add it to root list
            if (minNode != NULL) {
                // First remove node from root list
                y->left->right = y->right;
                y->right->left = y->left;

                // Now add to root list, again
                y->left = minNode;
                y->right = minNode->right;
                minNode->right = y;
                y->right->left = y;

                // Check if this is a new min
                if (y->key < minNode->key) {
                    minNode = y;
                }
            } else {
                minNode = y;
            }
        }
    }

    // consolidate
  
};
//////////////////////////////////////////////////////////////
// End of  class FibonacciHeap.
//////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////
// Begin of class graph.
//////////////////////////////////////////////////////////////
class ODID {  // for mapping OD to ID of a node;  "OD": OrDer
public:
	int ID;
	int OD;
//	ODID(int id, int od) { ID = id; OD = od; }
};

bool odidSort(const ODID &u1, const ODID &u2) { return u1.OD < u2.OD; }  // sort odid[] in ascending order

class adList{     //for the incoming/outgoing neighbours of a node
public:
	int ID; //node ID
	int wt; //weight
	adList * ptr;   //pointer
};

class Node{  // the index is node's ID, not order
public:

// static graph:
	//-- int ID; //node ID
	int OD;          // order used in CH algo
	//adList* adlistin; //the head of adjacency list; list:for incoming edges and nodes
	//-- adList* adlistin_end; // the end of the adlist
	adList* adlistout; 
	//-- adList* adlistout_end;

// running time parameters:
	//int dist;   // distance to source in DJ algo
	int dist[2];   // distance to source in DJ algo
	int previous[2];  // ID of previous node in DJ algo
	char state; // used in CH algo and DJ algo  //0: contracted / unreached;  1: normally exist / settled;  2: reached
	//int state;
	FibonacciHeapNode *fp[2];  // a pointer pointing to the FibonacciHeapNode in Fibonacci heap corresponding to this node in graph
	int bp[2]; // binary heap node pointers / indices

	Node() { // initial the node
		//-- ID = 0;
		OD = 0;
		state = NON_EXIST;  // 0
		//adlistin = NULL;
		//-- adlistin_end = NULL;
		adlistout = NULL; 
		//-- adlistout_end = NULL;
		dist[0] = INFINITE; 
		dist[1] = INFINITE;
	    previous[0] = 0;
		fp[0] = NULL;
		fp[1] = NULL;
		bp[0] = -1;
		bp[1] = -1;
	}
};

vector<Node>node;  // nodes of the graph G

class MultiDestination {  // for 
public:
	int ID;
	int distDJ;  // dist computed by DJ from v to w
	int dist1Hop; // weight(v,w); could be INFINITE
	int length; // weight(v,u) + weight(u,w)
};

//////////////////////////////////////////////////////////////
// End of class graph.
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// Begin of class ReadRoad.
//////////////////////////////////////////////////////////////

class ReadRoad {  // read road data from file
private:
	int n, m;  // the numbers of nodes and edges, equally |V| and |E|, of a graph G = (V, E, W), where W is weights of edges
	string filename;  // write preprocessed data into this file

public:
	int getNodes() { return n; }
	int getEdges()  { return m; }
	string getFilename () { return filename; }
	// re-initialized states of each node
	void clearNodeState() {  // after DJ, the nodes' state has been changed; so before next DJ, they should be re-initialized.
		for(int i = 1;  i<=n; i++ ) {
			if(node[i].state == NON_EXIST) continue;
			node[i].state = EXIST;
			node[i].dist[0] = INFINITE; 
			node[i].dist[1] = INFINITE; 
			//node[i].fp = NULL;
			//node[i].previous = NULL;
		}
	}

	// read node information from the road data file filename1, 
	// storing them in vector<Node>node
	// in each node[i], the index i is just the ID of the node,
	// i.e.,  i == ID, ranging from 1 to n.
	ReadRoad(string filename1) {  
		string line;  // a line from data file
		int ptr; //positions of ID or weight of a node in line
		//int v, u, weight;  // node IDs and weight on an arc (v,u)
		int v[2], weight;  // node IDs and weight on an arc (format in data file:  'a'  v[0]   v[1]  weight)
		adList* p;
		adList* head;
		int flag = 1;

		filename = filename1.c_str();
		ifstream roaddata(filename1.c_str());  
		if(roaddata == NULL) {
			cout << "file '"<< filename <<"' cannot open.\n";
			exit(-1);
		}

	  n=0;
	  m=0;
	  node.resize(23950000);
	  //node.resize(100000);  
	  cout << "begin read data from '" <<filename1 << "'...\n";
	  clock_t time = clock();
	  while ( !roaddata.eof()) {
		  getline (roaddata, line);
		   if(line.length() < 5) {
			   if(line[0]=='o') break;
			   else continue;  // some kind of line like "", " ", etc., is considered as the last line of the data file. "a 1 2 3" has the shortest length of a line
		   }
		  if(line[0]!='a') continue;

		  //if(m%2) { getline (roaddata, line); m++; continue; }  // skip the symmetric arc of the last one, because they have the identical information
		  
		  ptr = 2;
		  v[0] = line[ptr] - 48;  // stringstream ss is too slow for parsing the string to digits(500 edges/s), so we have to use converting chars to digits
		  while(line[++ptr]!=' ') v[0] = v[0]* 10 +  line[ptr] - 48;  // 48 is '0'
		  v[1] = line[++ptr] - 48;
		  while(line[++ptr]!=' ') v[1] = v[1] * 10 +  line[ptr] - 48;

		  if(v[0] >= v[1]) { m++; continue; }  // skip the symmetric arc of the last one, because they have the identical information

		  weight = line[++ptr] - 48;
		  while(line[++ptr]!='\0') weight = weight * 10 +  line[ptr] - 48;
			  
		  //if(node.size() < u || node.size() < v ) node.resize(node.size()+1000);  // ???????????? why it would fail ?
		  //-- node[v].ID = v;
		  for(flag=0; flag<2; flag++) {
			  node[v[flag]].state = EXIST;
			  p = new adList;
			  p->ID = v[1-flag];	
			  p->wt = weight;
			  head = node[v[flag]].adlistout;
			  if (head == NULL) {
				  node[v[flag]].adlistout = p;		  
				  p->ptr = NULL;  // the one first inserted is always the last one
			  }
			  else {
				  p->ptr = head;
				  node[v[flag]].adlistout = p;  // the new one is always the first one
			  }
			  //node[v[flag]].adlistin = node[v[flag]].adlistout; // to save space, for symmetric directed graph, we only store one-direct of an edge for each node
		  }
		  //-- node[u].ID = u;
/*
		  node[u].state= EXIST;
		  p = new adList;
		  p->ID = v;	
		  p->wt = weight;
		  head = node[u].adlistin;
		  if (head == NULL) {
			  node[u].adlistin = p;		  
			  p->ptr = NULL;  // the one first inserted is always the last one
		  }
		  else {
			  p->ptr = head;
			  node[u].adlistin = p;  // the new one is always the first one
		  }		  
*/		  
		  if(n<v[1]) n=v[1];
		  //if(n<u) n=u;
		  m++;
		  if(m % 100000 == 1) cout <<"."; 
	   }

	   // read node orders:
	  int order;
	  while (!roaddata.eof()) {
		  getline (roaddata, line);
		  if(line.length() < 5) continue;
		  if(line[0]!='o') continue;  // some kind of line like "", " ", etc., is considered as the last line of the data file. "a 1 2 3" has the shortest length of a line
		  ptr = 2;
		  v[0] = line[ptr] - 48;  // stringstream ss is too slow for parsing the string to digits(500 edges/s), so we have to use converting chars to digits
		  while(line[++ptr]!=' ') v[0] = v[0]* 10 +  line[ptr] - 48;
		  order = line[++ptr] - 48;
		  while(line[++ptr]!='\0') order = order * 10 +  line[ptr] - 48;
		  node[v[0]].OD = order;
		  //cout <<"\n" << v << " " << node[v].OD;
	  }

	   roaddata.close(); 
	   time = clock() - time;
	   cout << "\nfile reading finished. \nnodes="<<n<<", edges=" << m;
	   if(time / CLOCKS_PER_SEC >0) { 
		   cout << ", read in "<< time / CLOCKS_PER_SEC <<" seconds\n" ;
		   //cout <<"edges read per second: "<< m / (time / CLOCKS_PER_SEC) << endl;
	   }
	   else cout<<", read in "<< time / CLOCKS_PER_SEC <<" second\n";

	   //cout << "node.size: " << node.size() <<endl;

	   //for(int i = 0; i < node.size(); i++) {
	   for(int i = 0; i <= n; i++) {
		   if(node[i].state != EXIST) {
				node[i].state = NON_EXIST;
		   }
	   }

/*
	   // read it out for validation:
	   for(i = 0; i <=n; i++) {	
		   cout <<endl << endl << i <<"#: \n";
		   cout <<"adlistout: \n";
		   tail = node[i].adlistout;
		   while(tail != NULL) {
			   cout << tail->ID << " " ; //<< tail->wt <<"; ";
			   tail = tail->ptr;
		   }
		   cout <<"\nadlistin: \n";
		   tail = node[i].adlistin;
		   while(tail != NULL) {
			   cout << tail->ID << " ";// << tail->wt <<"; ";
			   tail = tail->ptr;
		   }
	   }
*/
	}
};
//////////////////////////////////////////////////////////////
// End of  class ReadRoad.
//////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////
// Begin of  class Dijkstra_Fheap.
//////////////////////////////////////////////////////////////

// Dijkstra's algo based on Fibonacci heap
class Dijkstra_Fheap {  
private:
	vector<int>visited;  // record all visited (reached and settled) nodes for reseting their state to EXIST, and delete pointers of FiboNodes, set node[].dist to INFINITE.
	int nVisitedNodes;  // it is set by visitedNodesReset() 

	int op1, op2, op3, op4, op41, op42, op43;  // count runs of each block;  use for temporary testing why time1(bidi-DJ) / time2(CHDJ) < < nVisited1*lognVisited1 / nVisited2*lognVisited2 

public:
	//int getSettled(FibonacciHeap *fp) { return fp->getSettled(); }  // -- only for test
	//int getReached(FibonacciHeap *fp) { return fp->getReached(); } // -- only for test
	int getNVisitedNodes() { return nVisitedNodes; }  // nVisitedNodes is the size of W[]. nVisitedNodes is set in visitedNodesReset() // -- only for test

	//Dijkstra_Fheap() {  } 
	//~Dijkstra_fheap() { } 

	/** 
	 * Compute single-source single destination shortest distance from s to t.
 	 * It's an implementation of the traditional single-directional DJ algo.
     * This is to verify results of CH algo or bidi DJ algo.
     * It is twice slower than bidi version of DJ but its correctness is believed guaranteed.
	 */
	int classicalDJ(int s, int t) {//, ReadRoad *g) {			
		adList* p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node *v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative; // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually

		//int maxHeapSize = 0;

		if(s == t)	return 0;
		if(node[s].state == NON_EXIST || node[t].state == NON_EXIST) {
			cout << "source or target don't exist: "<< s << endl; 
			return 0;
		}
	
		////assert(node[s].state != NON_EXIST);
		FibonacciHeap fheap;  
		FibonacciHeapNode *U;		
		U = new FibonacciHeapNode(s, 0);
		fheap.insert(U);    // insert source (s, 0)	
		node[s].dist[0] = 0;
		node[s].fp[0] = U;
		dist = 0;  
		visited.push_back(s);  // !!

		while(!fheap.isEmpty()) {
			U = fheap.removeMin();  // O(logn) ---- the most time-consuming operation!!!
			id = U->data;             // euqally, id = node[fp->data].ID;			
			//if(maxHeapSize < fheap.size()) maxHeapSize = fheap.size();

			dist = node[id].dist[0]; //  dist = node[fp->data].dist
			node[id].state = SETTLED;
			if(id == t) {
				
				//cout << "\nMaxHeapSize(single):" << maxHeapSize;

				visitedNodesReset();
				return dist; 
			}
			p = node[id].adlistout;
			while(p!=NULL) {  // for each neighbor v of u:  
				id_v = p->ID;
				v = &(node[id_v]);		// v points to one neighbour of u
				if(v->state == SETTLED) { // || v->state == CONTRACTED) {
					p=p->ptr;
					continue;
				}
				weight = p->wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v->dist[0]) {
					if(v->dist[0] == INFINITE) {  // this implies v is UNREACHED on this direction
						v->dist[0] = tentative;      
						v->fp[0] = new FibonacciHeapNode(id_v, tentative);    
						fheap.insert(v->fp[0]); 
						visited.push_back(id_v);
					}
					else{
						v->dist[0] = tentative;      
						fheap.decreaseKey(v->fp[0], v->dist[0]); 
					} 
				}
				p = p->ptr; //next neighbour of u
			}
		}

		//cout << "\nmaxHeapSize: " << maxHeapSize;

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
	void onetoManyClassicalDJ(int s, vector<MultiDestination>& W, int maxLength) {//, ReadRoad *g) {			
		adList* p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node *v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int nSettled;
		int sizeW = W.size();

		////assert(node[s].state != NON_EXIST);
		FibonacciHeap fheap;  
		FibonacciHeapNode *U;		
		U = new FibonacciHeapNode(s, 0);
		fheap.insert(U);    // insert source (s, 0)	
		node[s].dist[0] = 0;
		node[s].fp[0] = U;
		dist = 0;  
		visited.push_back(s);  // !!

		while(!fheap.isEmpty()) {
			U = fheap.removeMin();  // O(logn) ---- the most time-consuming operation!!!
			id = U->data;             // euqally, id = node[fp->data].ID;			
			dist = node[id].dist[0]; //  dist = node[fp->data].dist
			node[id].state = SETTLED;
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

			p = node[id].adlistout;
			while(p!=NULL) {  // for each neighbor v of u:  
				id_v = p->ID;
				v = &(node[id_v]);		// v points to one neighbour of u
				if(v->state == SETTLED || v->state == CONTRACTED) {
					p=p->ptr;
					continue;
				}
				weight = p->wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v->dist[0]) {
					if(v->dist[0] == INFINITE) {  // this implies v is UNREACHED on this direction
						v->dist[0] = tentative;      
						v->fp[0] = new FibonacciHeapNode(id_v, tentative);    
						fheap.insert(v->fp[0]); 
						visited.push_back(id_v);
					}
					else{
						v->dist[0] = tentative;      
						fheap.decreaseKey(v->fp[0], v->dist[0]); 
					} 
				}
				p = p->ptr; //next neighbour of u
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
		//int static count = 0;
		adList* p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node *v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = INFINITE;    // tentative best shortest length of (s, t)
		bool flag = 1;  // turns to forward search(true) or backward search(false)
		//int lastSettledNode; // from this two nodes, to traceback the shortest path in two reverse directions.
		//int nNodesOnPath[2];
		//lastSettledNode = -1;

		//op1 = 0; op2 = 0;  op3 = 0;  op4 = 0;  op41 = 0; op42 = 0;  op43 = 0; 

		//int maxHeapSize = 0;

		if(s == t)	return 0;
		if(node[s].state == NON_EXIST || node[t].state == NON_EXIST) {
			cout << "source or target don't exist: "<< s << endl; 
			return 0;
		}

		FibonacciHeap fheap[2];   
		FibonacciHeapNode *U;		
		U = new FibonacciHeapNode(s, 0);
		fheap[flag].insert(U);    // insert source (s, 0) into Fibo heap
		node[s].dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node[s].fp[flag] = U;     //   the associated Fibo Node storing ID and dist of this node
		node[s].state = SETTLED*(flag+1);  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= INFINITE -- initially when it is unreached, dist==INFINITE)
		//node[s].previous[flag] = -1;  // the previoud node on shortest path; for s and t, this is -1 -- non existing node.

		U = new FibonacciHeapNode(t, 0);
		fheap[!flag].insert(U);   
		node[t].dist[!flag] = 0;
		node[t].fp[!flag] = U;
		node[t].state = SETTLED*(flag);
		//node[t].previous[!flag] = -1;
		visited.push_back(s);  // !!
		visited.push_back(t);  // !!
		dist = 0;  

		while(!fheap[flag].isEmpty() || !fheap[!flag].isEmpty()) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(fheap[flag].isEmpty()) { flag = !flag; }

//			if(maxHeapSize < fheap[flag].size()) maxHeapSize = fheap[flag].size();

			U = fheap[flag].removeMin();   
			id = U->data;  
			dist = node[id].dist[flag];  
			//delete U; // delete U in visitedNodesReset()
			//U = NULL; 
			//lastSettledNode = id;
			
			if(node[id].state == SETTLED*(2-flag)) {// deleted from both queues, so terminate! "bidirectional DJ_lecture17.pdf", P13
				/*if(node[id].dist[0] + node[id].dist[1] < best) {   // Is it really necessary ?????????
					best = node[id].dist[0] + node[id].dist[1];
					lastSettledNode = id;
				}*/
				//cout << "  Weak: " << best <<endl;
				//nNodesOnPath[1] = showPath(s, lastSettledNode, 1);
				//nNodesOnPath[0] = showPath(t, lastSettledNode, 0);
				//cout <<"\nnNodesOnPath: " << nNodesOnPath[1] << " " << nNodesOnPath[0];
				//cout <<endl;

				//cout <<"\nmaxHeapSize (bidi):" << maxHeapSize;

				visitedNodesReset();
				return best; 
			}
			if(!fheap[!flag].isEmpty()){  
				if(node[fheap[!flag].min()->data].dist[!flag] + dist > best)  { // Stop when topf + topr > u.//EPP shortest path algorithms Bidirectional DJ.df P3
					//cout << " Strong: " << best <<endl;
				//nNodesOnPath[1] = showPath(s, lastSettledNode, 1);
				//nNodesOnPath[0] = showPath(t, lastSettledNode, 0);
				//cout <<"\nnNodesOnPath: " << nNodesOnPath[1] << " " << nNodesOnPath[0];
				//cout <<endl;
					visitedNodesReset();
					return best; 
				}
				else {   // Is it really necessary ?????????
					;//best = node[fheap[!flag].min()->data].dist[!flag] + dist; 
					//lastSettledNode = id;
				} // do nothing????? not update best = node[fheap[!flag].min()->data].dist[!flag] + dist?
			}
			if( (id == t || id == s) && dist ) { // This rarely happens in bi-di DJ search, except very small graph; two searches in two directions tend to meet in the middle of graph
				//cout << "  d(s, t): " << dist << endl;
				visitedNodesReset();
				return dist; 
			}
			//cout << "\n\nid: "<< id;

			node[id].state = SETTLED*(flag+1);   

			//if(flag) p = node[id].adlistout;  // keep consistent with the first node put in fheap[flag]
			//else      p = node[id].adlistin; 

			//op1++;

			p = node[id].adlistout;

			while(p!=NULL) {  // for each neighbor v of u:  
				id_v = p->ID;
				
				//op2++;

				//cout <<"\n  id_v: " << id_v;
				v = &(node[id_v]);		// v points to one neighbour of u
				if(v->state == SETTLED*(flag+1)) {  // v has been settled in the flag direction
					p=p->ptr;
					continue;
				}
				weight = p->wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v

				//op3++;

				if (tentative < v->dist[flag]) {  // update v->dist[flag], and also update best if v is settled by !flag direction
					//v->previous[flag] = id;  // ???????

					//op4++;

					if(v->dist[flag] == INFINITE) {  // this implies v is UNREACHED on this direction
						v->dist[flag] = tentative;      
						v->fp[flag] = new FibonacciHeapNode(id_v, tentative);    
						fheap[flag].insert(v->fp[flag]); 
						visited.push_back(id_v);

						//op41++;

					}
					else{
						v->dist[flag] = tentative;      
						fheap[flag].decreaseKey(v->fp[flag], tentative); 

						//op42++;

					} 
					if(v->dist[!flag] < INFINITE) {  // Reach a settled node v (v has been settled in the !flag direction).
						if( best > tentative + v->dist[!flag]) { // Update the tentative shortest distance "best".
							 best = tentative + v->dist[!flag] ;  //  This is the unique statement where "best" is updated.
							 //cout <<"\n  best update at: "<< id_v << " " << best;
							 //lastSettledNode = id_v;  // for showing the path, starting the last node on which best is last updated.

							 //op43++;

						}
					}
					/* Which one is correct? "if(v->state == SETTLED*(2-flag)) " or "if(v->dist[!flag] < INFINITE)  -- REACHED"?
					if(v->state == SETTLED*(2-flag)) {  // Reach a settled node v (v has been settled in the !flag direction).
						if( best > tentative + v->dist[!flag]) { // Update the tentative shortest distance "best".
							 best = tentative + v->dist[!flag] ;  //  This is the unique statement where "best" is updated.
							 cout <<"\n  best update at: "<< id_v << " " << best;
							 lastSettledNode = id_v;  // for showing the path, starting the last node on which best is last updated.
						}
					}
					*/
				}
				//cout <<"\nv.dist: " << v->dist[flag]; 
				p = p->ptr; //next neighbour of u
			}//while(p!=NULL) 
			flag = !flag;  // turn to another searching direction
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
		//int static count = 0;
		adList* p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node *v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = INFINITE;    // tentative best shortest length of (s, t)
		bool flag = 1;  // turns to forward search(true) or backward search(false)
		int toprID=0, toprDist=0;


		//int lastSettledNode; // from this two nodes, to traceback the shortest path in two reverse directions.
		//int nNodesOnPath[2];
		//lastSettledNode = -1;

		if(s == t)	return 0;
		if(node[s].state == NON_EXIST || node[t].state == NON_EXIST) {
			cout << "source or target don't exist: "<< s << endl; 
			return 0;
		}

		FibonacciHeap fheap[2];   
		FibonacciHeapNode *U;		
		U = new FibonacciHeapNode(s, 0);
		fheap[flag].insert(U);    // insert source (s, 0) into Fibo heap
		node[s].dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node[s].fp[flag] = U;     //   the associated Fibo Node storing ID and dist of this node
		node[s].state = SETTLED*(flag+1);  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= INFINITE -- initially when it is unreached, dist==INFINITE)
		//node[s].previous[flag] = -1;  // the previoud node on shortest path; for s and t, this is -1 -- non existing node.

		U = new FibonacciHeapNode(t, 0);
		fheap[!flag].insert(U);   
		node[t].dist[!flag] = 0;
		node[t].fp[!flag] = U;
		node[t].state = SETTLED*(flag);
		//node[t].previous[!flag] = -1;
		visited.push_back(s);  // !!
		visited.push_back(t);  // !!
		dist = 0;  

		//op1 = 0; op2 = 0;  op3 = 0;  op4 = 0;  op41 = 0; op42 = 0;  op43 = 0; 

		while(!fheap[flag].isEmpty() || !fheap[!flag].isEmpty()) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(fheap[flag].isEmpty()) { flag = !flag; }

			U = fheap[flag].removeMin();   
			id = U->data;  
			dist = node[id].dist[flag];  
			node[id].state = SETTLED*(flag+1);  // ????????
			
			//op1++;

			//delete U; // delete U in visitedNodesReset()
			//U = NULL; 
			//lastSettledNode = id;

//if(flag)cout << "\n\nid: "<< id << " (" << dist << ")"; else cout << "\nid: "<< id << " (" << dist << ")";

/*
			if(!fheap[!flag].isEmpty()){   // A new stopping criterion to constitute/replace the traditional strong stopping criterion of bi-di DJ algo because the later is invalid for CH algo. // Proposed by Deqi Li, Jan 15,2014.
				int toprID = fheap[!flag].min()->data;
				int toprDist = node[toprID].dist[!flag];
				if((toprDist + dist) > best)  { // Stop when topf + topr > u.//EPP shortest path algorithms Bidirectional DJ.df P3
				//if((toprDist + dist) > best && (node[id].OD > node[toprID].OD))  { 
					//cout << " Strong: " << best <<endl;
				//nNodesOnPath[1] = showPath(s, lastSettledNode, 1);
				//nNodesOnPath[0] = showPath(t, lastSettledNode, 0);
				//cout <<"\nnNodesOnPath: " << nNodesOnPath[1] << " " << nNodesOnPath[0];
				//cout <<endl;
					//cout << "\ntempdist + dist: " << tempdist + dist;
					visitedNodesReset();
					return best; 
				}
				else {   // Is it really necessary ?????????
					;//best = node[fheap[!flag].min()->data].dist[!flag] + dist; 
					//lastSettledNode = id;
				} // do nothing????? not update best = node[fheap[!flag].min()->data].dist[!flag] + dist?
			}
			if(node[id].state == SETTLED*(2-flag)) {// deleted from both queues, so terminate! "bidirectional DJ_lecture17.pdf", P13
				//cout << "  Weak: " << best <<endl;
				//nNodesOnPath[1] = showPath(s, lastSettledNode, 1);
				//nNodesOnPath[0] = showPath(t, lastSettledNode, 0);
				//cout <<"\nnNodesOnPath: " << nNodesOnPath[1] << " " << nNodesOnPath[0];
				//cout <<endl;
				visitedNodesReset();
				return best; 
			}		
			if( (id == t || id == s) && dist ) { // This rarely happens in bi-di DJ search, except very small graph; two searches in two directions tend to meet in the middle of graph
				//cout << "  d(s, t): " << dist << endl;
				visitedNodesReset();
				return dist; 
			}
*/
/*		
			if(!fheap[!flag].isEmpty()){   // A new stopping criterion to constitute/replace the traditional strong stopping criterion of bi-di DJ algo because the later is invalid for CH algo. // Proposed by Deqi Li, Jan 15,2014.
				toprID = fheap[!flag].min()->data;
				toprDist = node[toprID].dist[!flag];
				if((toprDist + dist) >= best)  { // Stop when topf + topr >= u.//EPP shortest path algorithms Bidirectional DJ.df P3
					if(node[toprID].OD > node[id].OD) {
						fheap[!flag].clear(); // stop reverse search
						cout << " Stopping_r: " << best <<endl; 
					}
					else {
						fheap[flag].clear(); 
						flag = !flag; 
						cout << " Stopping_f1: " << best <<endl; 
						continue; 
					}	  
				}
			}
			else if((toprDist + dist) >= best && node[toprID].OD < node[id].OD) { // new f search won't meet all r search scope because r's old OD is smaller than f's new OD
					fheap[flag].clear(); 
					flag = !flag; 
					cout << " Stopping_f2: " << best <<endl; 
					continue;  
			}

*/
			if(dist >= best) {  // One-side stopping criterion of Contraction Hierarchies algo. From PHAST P3?
				fheap[flag].clear();
				flag = !flag; 
//cout << " Stopping: " << best <<endl; 
				continue; 
			}


			//if(flag) p = node[id].adlistout;  // keep consistent with the first node put in fheap[flag]
			//else      p = node[id].adlistin; 

			p = node[id].adlistout;

			while(p!=NULL) {  // for each neighbor v of u:  
				id_v = p->ID;
				//cout <<"\n  v: " << id_v;
				
				//op2++;

///////////////////////////////////////////////////////////////////////////////////
				// Contraction Hierarchies query Algo: 
				// Search is in ascending order in both bi-directions.  
				if(node[id_v].OD < node[id].OD ) { 
					p = p->ptr;
					//cout << " prun";
					continue;
				}
///////////////////////////////////////////////////////////////////////////////////
				
//cout <<"\n  v: " << id_v;

				v = &(node[id_v]);		// v points to one neighbour of u
				if(v->state == SETTLED*(flag+1)) {  // v has been settled in the flag direction
					p=p->ptr;
					continue;
				}
				weight = p->wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
           // NOTICE: If a node v was reached by r, and is also reached by f, we must update it at f; otherwise in future no any more chance to update best at v due to stopping criterion

				//op3++;

				if (tentative < v->dist[flag]) {  // update v->dist[flag], and also update best if v is settled by !flag direction
					//v->previous[flag] = id;  // ???????

//cout <<" (" << tentative << ")";
					
					//op4++;

					if(v->dist[flag] == INFINITE) {  // this implies v is UNREACHED on this direction
						v->dist[flag] = tentative;      
						v->fp[flag] = new FibonacciHeapNode(id_v, tentative);    
						fheap[flag].insert(v->fp[flag]); 
						visited.push_back(id_v);

						//op41++;

					}
					else{
						v->dist[flag] = tentative;      
						fheap[flag].decreaseKey(v->fp[flag], tentative); 

						//op42++;

					}

					if(v->dist[!flag] < INFINITE) {  // Here is CORRECT!
						if( best > tentative + v->dist[!flag]) { 
							 best = tentative + v->dist[!flag] ;  
							 
							 //op43++;
//cout <<", best update at: "<< id_v << " (" << best <<")";

							 //lastSettledNode = id_v;  // for showing the path, starting the last node on which best is last updated.
						}
					}

/*					//Which one is correct? "if(v->state == SETTLED*(2-flag)) " or "if(v->dist[!flag] < INFINITE)  -- REACHED"?
					//  ----> The follow one is INCORRECT!
					if(v->state == SETTLED*(2-flag)) {  // Reach a settled node v (v has been settled in the !flag direction).
						if( best > tentative + v->dist[!flag]) { // Update the tentative shortest distance "best".
							 best = tentative + v->dist[!flag] ;  //  This is the unique statement where "best" is updated.
							 //cout <<"\n  best update at: "<< id_v << " " << best;
							 //lastSettledNode = id_v;  // for showing the path, starting the last node on which best is last updated.
						}
					}
*/					
				}
				//cout <<"\n  v.dist: " << v->dist[flag]; 
				p = p->ptr; //next neighbour of u
			}//while(p!=NULL) 
			flag = !flag;  // turn to another searching direction
		} // while(!fheap[flag].isEmpty()...) 
		
		visitedNodesReset();	
		return best;
	}
 

	int Trivial_CHDJ(int s, int t) {
		adList* p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node *v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = INFINITE;    // tentative best shortest length of (s, t)
		bool flag = 1;  // turns to forward search(true) or backward search(false)

		const int MyHeapSize = 10000;
		vector<ODID>myheap[2];
		myheap[0].resize(MyHeapSize);
		myheap[1].resize(MyHeapSize);
		int size[2] = { 0, 0 }; //myheap size

		if(s == t)	return 0;
		if(node[s].state == NON_EXIST || node[t].state == NON_EXIST) {
			cout << "source or target don't exist: "<< s << endl; 
			return 0;
		}

		myheap[flag][size[flag]].ID = s;      
		myheap[flag][size[flag]].OD = 0;  
		size[flag]++;
		node[s].dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node[s].state = SETTLED*(flag+1);  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= INFINITE -- initially when it is unreached, dist==INFINITE)

		myheap[!flag][size[!flag]].ID = t;   
		myheap[!flag][size[!flag]].OD = 0;  
		size[!flag]++;
		node[t].dist[!flag] = 0;
		node[t].state = SETTLED*(flag);
		dist = 0;  
		visited.push_back(s);  // !!
		visited.push_back(t);  // !!

		while(size[flag] > 0 ||size[!flag] > 0) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(size[flag] == 0) { flag = !flag; }
			//U = myheap[flag].removeMin();   
			//id = U->data;  
			//dist = node[id].dist[flag];  			
			int minDist = 0x7FFFFFFF;  // removeMin(): O(2*size[flag])
			int minIdx;
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
			//assert(minDist == node[id].dist[flag]);
			//assert(i==size[flag]);
			if(i>0) {
				myheap[flag][minIdx].ID   = myheap[flag][i-1].ID;
				myheap[flag][minIdx].OD = myheap[flag][i-1].OD;
			}
			size[flag]--;


			node[id].state = SETTLED*(flag+1); 
			if(dist >= best) {  // One-side stopping criterion of Contraction Hierarchies algo. From PHAST P3?
				//myheap[flag].clear();
				size[flag]=0;
				flag = !flag; 
				continue; 
			}
			p = node[id].adlistout;

			while(p!=NULL) {  // for each neighbor v of u:  
				id_v = p->ID;
				//cout <<"\n  v: " << id_v;

///////////////////////////////////////////////////////////////////////////////////
				// Contraction Hierarchies query Algo: 
				// Search is in ascending order in both bi-directions.  
				if(node[id_v].OD < node[id].OD ) { 
					p = p->ptr;
					//cout << " prun";
					continue;
				}
///////////////////////////////////////////////////////////////////////////////////
				
				v = &(node[id_v]);		// v points to one neighbour of u
				if(v->state == SETTLED*(flag+1)) {  // v has been settled in the flag direction
					p=p->ptr;
					continue;
				}
				weight = p->wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v->dist[flag]) {  // update v->dist[flag], and also update best if v is settled by !flag direction
					//cout <<" (" << tentative << ")";
					if(v->dist[flag] == INFINITE) {  // this implies v is UNREACHED on this direction
 						 v->dist[flag] = tentative;      
 					     //v->fp[flag] = new myHeapNode(id_v, tentative);    
						//myheap[flag].insert(v->fp[flag]); 
						myheap[flag][size[flag]].ID = id_v;      // insert(): O(3)
						myheap[flag][size[flag]].OD = tentative;  
						//cout << "  insert (" << id_v << ", " << tentative << ") @ " << size[flag];
						size[flag]++;
						if(size[flag]==myheap[flag].size()) myheap[flag].resize(size[flag] + MyHeapSize);
						visited.push_back(id_v);
					}
					else{				
						v->dist[flag] = tentative;      
						//myheap[flag].decreaseKey(v->fp[flag], tentative); 
						for(i=0; i<size[flag]; i++) {  // decreaseKey(): O(size[flag]/2)
							if(myheap[flag][i].ID == id_v) {
								myheap[flag][i].OD = tentative;
								break;
							}
						}
						//cout << "  update (" << id_v << ", " << tentative << ") @ " << i;
						//assert(i<size[flag]);
					}

					if(v->dist[!flag] < INFINITE) {  // Here is CORRECT!
						if( best > tentative + v->dist[!flag]) { 
							 best = tentative + v->dist[!flag] ;  
							//cout <<", best update at: "<< id_v << " (" << best <<")";
						}
					}				
				}
				p = p->ptr; //next neighbour of u
			}//while(p!=NULL) 
			flag = !flag;  // turn to another searching direction
		} // while(!myheap[flag].isEmpty()...) 
		myheap[0].clear();
		myheap[1].clear();
		visitedNodesReset();	
		return best;
	}

		int Trivial_bidi_classicalDJ(int s, int t) { //, ReadRoad *g) {	// s and t are the ID of source node and destination node of graph		
		//int static count = 0;
		adList* p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node *v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = INFINITE;    // tentative best shortest length of (s, t)
		bool flag = 1;  // turns to forward search(true) or backward search(false)
		//int lastSettledNode; // from this two nodes, to traceback the shortest path in two reverse directions.
		//int nNodesOnPath[2];
		//lastSettledNode = -1;

		if(s == t)	return 0;
		if(node[s].state == NON_EXIST || node[t].state == NON_EXIST) {
			cout << "source or target don't exist: "<< s << endl; 
			return 0;
		}

		const int MyHeapSize = 10000;
		vector<ODID>myheap[2];
		myheap[0].resize(MyHeapSize);
		myheap[1].resize(MyHeapSize);
		int size[2] = { 0, 0 }; //myheap size

		myheap[flag][size[flag]].ID = s;      
		myheap[flag][size[flag]].OD = 0;  
		size[flag]++;
		node[s].dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node[s].state = SETTLED*(flag+1);  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= INFINITE -- initially when it is unreached, dist==INFINITE)

		myheap[!flag][size[!flag]].ID = t;   
		myheap[!flag][size[!flag]].OD = 0;  
		size[!flag]++;
		node[t].dist[!flag] = 0;
		node[t].state = SETTLED*(flag);
		visited.push_back(s);  // !!
		visited.push_back(t);  // !!
		dist = 0;  

		while(size[flag] > 0 ||size[!flag] > 0) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(size[flag] == 0) { flag = !flag; }

			int minDist = 0x7FFFFFFF;  // removeMin(): O(2*size[flag])
			int minIdx;
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
			//assert(minDist == node[id].dist[flag]);
			//assert(i==size[flag]);
			if(i>0) {
				myheap[flag][minIdx].ID   = myheap[flag][i-1].ID;
				myheap[flag][minIdx].OD = myheap[flag][i-1].OD;
			}
			size[flag]--;
				
			if(node[id].state == SETTLED*(2-flag)) {// deleted from both queues, so terminate! "bidirectional DJ_lecture17.pdf", P13
				//cout << "  Weak: " << best <<endl;
				visitedNodesReset();
				return best; 
			}
			if(size[!flag] > 0){  
				//if(node[fheap[!flag].min()->data].dist[!flag] + dist > best)  { // Stop when topf + topr > u.//EPP shortest path algorithms Bidirectional DJ.df P3
					//cout << " Strong: " << best <<endl;
					//visitedNodesReset();
					//return best; 
				//}
				minDist = 0x7FFFFFFF;  // getMin(): O(2*size[flag])
				for(i=0; i<size[!flag]; i++) {
					if(minDist > myheap[!flag][i].OD) {
						minDist = myheap[!flag][i].OD;
					}
				}
				if(minDist + dist > best)  { // Stop when topf + topr > u.//EPP shortest path algorithms Bidirectional DJ.df P3
					//cout << " Strong: " << best <<endl;
					visitedNodesReset();
					return best; 
				}


			}
			if( (id == t || id == s) && dist ) { // This rarely happens in bi-di DJ search, except very small graph; two searches in two directions tend to meet in the middle of graph
				//cout << "  d(s, t): " << dist << endl;
				visitedNodesReset();
				return dist; 
			}
			//cout << "\n\nid: "<< id;
			node[id].state = SETTLED*(flag+1);   

			//if(flag) p = node[id].adlistout;  // keep consistent with the first node put in fheap[flag]
			//else      p = node[id].adlistin; 
			p = node[id].adlistout;

			while(p!=NULL) {  // for each neighbor v of u:  
				id_v = p->ID;
				//cout <<"\n  id_v: " << id_v;
				v = &(node[id_v]);		// v points to one neighbour of u
				if(v->state == SETTLED*(flag+1)) {  // v has been settled in the flag direction
					p=p->ptr;
					continue;
				}
				weight = p->wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v->dist[flag]) {  // update v->dist[flag], and also update best if v is settled by !flag direction
					//v->previous[flag] = id;  // ???????
					if(v->dist[flag] == INFINITE) {  // this implies v is UNREACHED on this direction
						v->dist[flag] = tentative;      
						myheap[flag][size[flag]].ID = id_v;      // insert(): O(3)
						myheap[flag][size[flag]].OD = tentative;  
						//cout << "  insert (" << id_v << ", " << tentative << ") @ " << size[flag];
						size[flag]++;
						if(size[flag]==myheap[flag].size()) myheap[flag].resize(size[flag] + MyHeapSize);
						visited.push_back(id_v);
					}
					else{
						v->dist[flag] = tentative;      
						for(i=0; i<size[flag]; i++) {  // decreaseKey(): O(size[flag]/2)
							if(myheap[flag][i].ID == id_v) {
								myheap[flag][i].OD = tentative;
								break;
							}
						}
					} 
					if(v->dist[!flag] < INFINITE) {  // Reach a settled node v (v has been settled in the !flag direction).
						if( best > tentative + v->dist[!flag]) { // Update the tentative shortest distance "best".
							 best = tentative + v->dist[!flag] ;  //  This is the unique statement where "best" is updated.
							 //cout <<"\n  best update at: "<< id_v << " " << best;
							 //lastSettledNode = id_v;  // for showing the path, starting the last node on which best is last updated.
						}
					}
					/* Which one is correct? "if(v->state == SETTLED*(2-flag)) " or "if(v->dist[!flag] < INFINITE)  -- REACHED"?
					if(v->state == SETTLED*(2-flag)) {  // Reach a settled node v (v has been settled in the !flag direction).
						if( best > tentative + v->dist[!flag]) { // Update the tentative shortest distance "best".
							 best = tentative + v->dist[!flag] ;  //  This is the unique statement where "best" is updated.
							 cout <<"\n  best update at: "<< id_v << " " << best;
							 lastSettledNode = id_v;  // for showing the path, starting the last node on which best is last updated.
						}
					}
					*/
				}
				//cout <<"\nv.dist: " << v->dist[flag]; 
				p = p->ptr; //next neighbour of u
			}//while(p!=NULL) 
			flag = !flag;  // turn to another searching direction
		} // while(!fheap[flag].isEmpty()...) 
		visitedNodesReset();
		return best;
	}

		int Trivial_classicalDJ(int s, int t) {//, ReadRoad *g) {			
		adList* p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node *v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative; // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually

		if(s == t)	return 0;
		if(node[s].state == NON_EXIST || node[t].state == NON_EXIST) {
			cout << "source or target don't exist: "<< s << endl; 
			return 0;
		}
	
		const int MyHeapSize = 10000;
		vector<ODID>myheap;
		myheap.resize(MyHeapSize);
		int size = 0; //myheap size

		myheap[size].ID = s;      
		myheap[size].OD = 0;  
		size++;		
		node[s].dist[0] = 0;
		dist = 0;  
		visited.push_back(s);  // !!

		while(size > 0) {
			int minDist = 0x7FFFFFFF;  // removeMin(): O(2*size[flag])
			int minIdx;
			int i;
			for(i=0; i<size; i++) {
				if(minDist > myheap[i].OD) {
					minDist = myheap[i].OD;
					minIdx  = i;
				}
			}
			id = myheap[minIdx].ID;
			dist = minDist;
			//assert(minIdx < size);
			//assert(minDist == node[id].dist[0]);
			//assert(i==size);
			if(i>0) { // overwrite the min
				myheap[minIdx].ID   = myheap[i-1].ID;
				myheap[minIdx].OD = myheap[i-1].OD;
			}
			size--;
			
			node[id].state = SETTLED;
			if(id == t) {
				visitedNodesReset();
				return dist; 
			}
			p = node[id].adlistout;
			while(p!=NULL) {  // for each neighbor v of u:  
				id_v = p->ID;
				v = &(node[id_v]);		// v points to one neighbour of u
				if(v->state == SETTLED) { // || v->state == CONTRACTED) {
					p=p->ptr;
					continue;
				}
				weight = p->wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v->dist[0]) {
					if(v->dist[0] == INFINITE) {  // this implies v is UNREACHED on this direction
						v->dist[0] = tentative;      
						myheap[size].ID = id_v;      // insert(): O(3)
						myheap[size].OD = tentative;  
//cout << "  insert (" << id_v << ", " << tentative << ") @ " << size[flag];
						size++;
						if(size==myheap.size()) myheap.resize(size + MyHeapSize);
						visited.push_back(id_v);
					}
					else{
						v->dist[0] = tentative;      
						for(i=0; i<size; i++) {  // decreaseKey(): O(size[flag]/2)
							if(myheap[i].ID == id_v) {
								myheap[i].OD = tentative;
								break;
							}
						}
					} 
				}
				p = p->ptr; //next neighbour of u
			}
		}
		visitedNodesReset();
		return dist;
	}

	void unpackPath() {
		// Not implement it at present.
	}

protected:

	// Check if all nodes in W[] are settled for onetoManyClassicalDJ();
	// If so,  onetoManyClassicalDJ() can return.
	bool allSettled(vector<MultiDestination>& W) {
		int i;
		for(i=0; i<W.size(); i++) {
			if(node[W[i].ID].state != SETTLED) return false;
		}
		for(i=0; i<W.size(); i++) {
			W[i].distDJ = node[W[i].ID].dist[0];
		}
		return true;
	}

	void visitedNodesReset() {
		nVisitedNodes = visited.size();   // nVisitedNodes is used by getNVisitedNodes()
		for(vector<int>::iterator it=visited.begin(); it!=visited.end(); ++it) {
			node[*it].state = EXIST;
			node[*it].dist[0] = INFINITE; 
			node[*it].dist[1] = INFINITE; 
			if(node[*it].fp[0] != NULL) { delete node[*it].fp[0]; node[*it].fp[0] = NULL; }
			if(node[*it].fp[1] != NULL) { delete node[*it].fp[1]; node[*it].fp[1] = NULL; }
		}
		//visited.clear();
		visited.resize(0);
		//cout <<"\n_visitedNodesReset()" ;
	}

	int showPath(int s, int start, bool flag) {
		if(start < 0 || node[start].previous[flag] < 0) return 0;

		int temp = start;		 
		int nNodesOnPath = 1;		
		cout  << "\npath: " << start;
		do{
			cout << "<-" <<node[temp].previous[flag];
			temp = node[temp].previous[flag];
			nNodesOnPath++;
		}while(temp != s);
	
		//cout << "\nthe shortest distance between " << s <<" and " << t << " is: " << node[t].dist <<endl;
		return nNodesOnPath;
	}

};
//////////////////////////////////////////////////////////////
// End of  class Dijkstra_Fheap.
//////////////////////////////////////////////////////////////

 

void fheapTest() {
	FibonacciHeap fheap; 
	FibonacciHeapNode node1(3, 2);
	fheap.insert(&node1);
	cout << "&node1: "<< &node1 << ": " << node1.data <<", " << node1.key <<endl;

	FibonacciHeapNode *fp;
	fp = new FibonacciHeapNode(0, 1);
	fheap.insert(fp);  
	cout << "&node2: "<< fp << ": " << fp->data <<", " << fp->key <<endl;
	fp = new FibonacciHeapNode(2, 5);
	fheap.insert(fp);  
	cout << "&node3: "<< fp << ": " << fp->data <<", " << fp->key <<endl;
	fp = new FibonacciHeapNode(1, 3);
	fheap.insert(fp);  
	cout << "&node4: "<< fp << ": " << fp->data <<", " << fp->key <<endl;
	fp = new FibonacciHeapNode(4, 6);
	fheap.insert(fp);  
	cout << "&node5: "<< fp << ": " << fp->data <<", " << fp->key <<endl;

	fp = fheap.min(); 
	cout << "minNode: " << fp->getData() <<", " << fp->getKey() <<endl;
	cout << "nNodes: " << fheap.size() << endl;
	cout << "\nFibonacciHeap's representation:\n";
	fheap.toString();	

//// test1:
	cout << "\n\ntest1: insert(5,4)\n" ;
	FibonacciHeapNode *fp2;
	fp2 = new FibonacciHeapNode(5, 4);
	fheap.insert(fp2);  
	cout << "&node6: "<< fp2 << ": " << fp2->data <<", " << fp2->key <<endl;

	fp = fheap.min(); 
	cout << "minNode: " << fp->getData() <<", " << fp->getKey() <<endl;
	cout << "nNodes: " << fheap.size() << endl;
	cout << "\nFibonacciHeap's representation:\n";
	fheap.toString();	

//// test2:
	cout << "\n\ntest2: decreaseKey(fp2,44)\n" ;
	fheap.decreaseKey(fp2,44);  
	cout << "&node6: "<< fp2 << ": " << fp2->data <<", " << fp2->key <<endl;

	fp = fheap.min(); 
	cout << "minNode: " << fp->getData() <<", " << fp->getKey() <<endl;
	cout << "nNodes: " << fheap.size() << endl;
	cout << "\nFibonacciHeap's representation:\n";
	fheap.toString();	

//// test3:
	cout << "\n\ntest3: removeMin()\n" ;
	fp2 = fheap.removeMin(); 
	cout << "&node6: "<< fp2 << ": " << fp2->data <<", " << fp2->key <<endl;

	fp = fheap.min(); 
	cout << "minNode: " << fp->getData() <<", " << fp->getKey() <<endl;
	cout << "nNodes: " << fheap.size() << endl;
	cout << "\nFibonacciHeap's representation:\n";
	fheap.toString();
	
//// test4: 
	cout  << "\n\ntest4: removeMin() until heap is empty\n" ;
	while(!fheap.isEmpty()) {
		cout << "\nFibonacciHeap's representation:\n";
		fheap.toString();
		cout << "nNodes: " << fheap.size() << endl;
		fp2 = fheap.removeMin(); 
		cout << "minNode: "<< fp2 << ": " << fp2->data <<", " << fp2->key <<endl;
	}

}

// test performance of Fibo heap:
// insert 10000 nodes, 
// then decreaseKey on every of them, 
// then removeMin until heap is empty
void fheapPerformanceTest() { 
	FibonacciHeap fheap;  
	FibonacciHeapNode *fp;
	FibonacciHeapNode *fp_array[50001];
	int key, i;
	time_t start = (unsigned) time(NULL);

	cout << "\n\ntest1: insert 50000 nodes:\n" ;
	for(i=1; i <=50000; i++){
		key = unsigned(rand()) % 60001;
		fp = new FibonacciHeapNode(i, key);
		fp_array[i] = fp;
		fheap.insert(fp);  
		cout << "insert Node: #"<< i <<": "<< fp << ": " << fp->data <<", " << fp->key <<endl;
		//fp = fheap.min(); 
	}

 	cout << "\n\ntest2: update 50000 nodes:\n" ;
	for(i=1; i <=50000; i++){
		fp =  fp_array[i];
		fheap.decreaseKey(fp, rand()%30001); //update the key of node i; 
		cout << "update Node: #"<< i <<": "<< fp << ": " << fp->data <<", " << fp->key <<endl;
		//fp = fheap.min(); 
	}

 	cout  << "\n\ntest3: removeMin() until heap is empty:\n" ;
	while(!fheap.isEmpty()) {
		fp = fheap.removeMin(); 
		cout << "minNode: "<< fp << ": " << fp->data <<", " << fp->key <<endl;
	}

	cout << "\n\ntime used: " << (unsigned) time(NULL) -  start << endl;
}  


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
private:
	int nNodes;
	vector<ODID>heap;  // ODID is the node structure

public:
	BinaryHeap(int n) { nNodes = 0; heap.resize(n+1); }
	BinaryHeap() { nNodes = 0; heap.resize(20000);  }  // Usually  20000 is big enough for the large file 'USA-road-d.USA.gr' file.
	int getSize() { return nNodes; }
	bool isEmpty() { if(nNodes > 0) return false; else return true; }
	int min() { return heap[1].OD; }  // the minimum node is heap[1]
	void clear() { nNodes = 0; }  // heap.resize(0); heap.clear();
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
	int insert(int id, int key, int flag) { 
		nNodes++;
		if(nNodes >= heap.size()) heap.resize(nNodes + nNodes);  // double heap size if necessary

		// insert the new node at the end of heap
		heap[nNodes].ID = id;
		heap[nNodes].OD = key;
		int idx = nNodes; 
		int parent = idx / 2;
		node[id].bp[flag] = idx;  // set binary node pointer of graph node

		//adjust it to satisfy the heap property
		while(parent > 0 && key < heap[parent].OD ) { // Bubble up: swap the node with its parent if its key is larger than its parent's.
			swapNodes(idx, parent, flag);
			idx = parent;
			parent = idx / 2;
			//key = heap[idx].OD;
			//cout <<"\n    parent: " << parent;
		}
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
	int removeMin(int flag) {
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

	bool decreaseKey(int idx, int newKey, int flag) { //Decrease key of element x to newKey
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
			//cout <<"\n    parent: " << parent;
		}
		
		return true;
	}

	/**
	  * temporarily test its corretness
	  */
	void test(int n) { 
		int i, j, ID, key, idx;
		node.resize(n);

		cout <<"\n ---------------- Binary Heap CORRECTNESS TEST ----------------- \n";
		cout <<"\ninsert() test:";
		for(i=1; i<=15; i++) {
			ID = i + 1000;
			key = 16 - i;
			cout << "\n  insert(" << ID << ", " << key << ")";
			insert(ID, key, 0);
		}
	    for(i=1; i<=getSize(); i++) cout << "\n  " << i << "#: (" << heap[i].ID <<", " << heap[i].OD <<")";
		cout << "\nSize: " << getSize();

		cout <<"\n\ndecreaseKey() test:";
		for(j=1; j<=3; j++) {
			idx =  j*5;
			key = heap[j].OD - 99999;
			cout << "\n  decreaseKey(" << idx << ", " << key << ")";
			decreaseKey(idx, key, 0);
		    for(i=1; i<=getSize(); i++) cout << "\n  " << i << "#: (" << heap[i].ID <<", " << heap[i].OD <<")";
		}
		
		cout <<"\n\nremoveMin() test:";
		for(j=1; j<=16; j++) {
			cout<<"\n  min, size: " << removeMin(0); cout << " " << getSize();
			for(i=1; i<=getSize(); i++) cout << "\n  " << i << "#: (" << heap[i].ID <<", " << heap[i].OD <<")";
		}
		 
	}

protected:
	/**
	  * swap two nodes of binary heap; update bp of graph node
	  *
	  * <p>: O(1)
	  *
	  *@param i, j: indices of nodes of binary heap
	  */
	void swapNodes(int i, int j, int flag) {
		if(i==j) return;

		int id = heap[i].ID;
		int key = heap[i].OD;

		heap[i].OD = heap[j].OD;
		heap[i].ID  = heap[j].ID;
		node[heap[j].ID].bp[flag] = i; // ????????? // update binary node pointer of graph node; see graph node class.

		heap[j].OD = key;
		heap[j].ID = id;
		node[id].bp[flag] = j;  // ????????
	}
};

//////////////////////////////////////////////////////////////
// End of class Binary Heap.
//////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////
// Begin of class Dijkstra_Bheap.
//////////////////////////////////////////////////////////////
// The following Dijkstra's algo is based on Binary heaps rather than Fibonacci heaps.
class Dijkstra_Bheap {
private:
	vector<int>visited;  // record all visited (reached and settled) nodes for reseting their state to EXIST, and delete pointers of FiboNodes, set node[].dist to INFINITE.
	int lastMeetNode;
	int nVisitedNodes;

public:
	int getNVisitedNodes() { return nVisitedNodes; }  // nVisitedNodes is the size of W[]. nVisitedNodes is set in visitedNodesReset() // -- only for test

	//Dijkstra_Bheap() {  } 
	//~Dijkstra_Bheap() { } 

	/** 
	 * Compute single-source single destination shortest distance from s to t.
 	 * It's an implementation of the traditional single-directional DJ algo.
     * This is to verify results of CH algo or bidi DJ algo.
     * It is twice slower than bidi version of DJ but its correctness is believed guaranteed.
	 */
	int classicalDJ(int s, int t) {//, ReadRoad *g) {			
		adList* p;
		int id;     // the graph node ID of  the fheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node *v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative; // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually

		//int maxHeapSize = 0;

		if(s == t)	return 0;
		if(node[s].state == NON_EXIST || node[t].state == NON_EXIST) {
			cout << "source or target don't exist: "<< s << endl; 
			return 0;
		}
	
		BinaryHeap bheap;  
		bheap.insert(s, 0, 0);
		node[s].dist[0] = 0;
		node[s].bp[0] = 1;
		dist = 0;  
		visited.push_back(s);   

		while(!bheap.isEmpty()) {
			id = bheap.removeMin(0);  // O(logn) ---- the most time-consuming operation!!!

			//if(maxHeapSize < bheap.getSize()) maxHeapSize = bheap.getSize();
			dist = node[id].dist[0];       //  dist = node[fp->data].dist
			node[id].state = SETTLED;
			if(id == t) {
				visitedNodesReset();
				return dist; 
			}
			p = node[id].adlistout;
			while(p!=NULL) {  // for each neighbor v of u:  
				id_v = p->ID;
				v = &(node[id_v]);		// v points to one neighbour of u
				if(v->state == SETTLED) { // || v->state == CONTRACTED) {
					p=p->ptr;
					continue;
				}
				weight = p->wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v->dist[0]) {
					if(v->dist[0] == INFINITE) {  // this implies v is UNREACHED on this direction
						v->dist[0] = tentative;      
						v->bp[0] = bheap.insert(id_v, tentative, 0);  						
						visited.push_back(id_v);
					}
					else{
						v->dist[0] = tentative;      
						bheap.decreaseKey(v->bp[0], tentative, 0); 
					} 
				}
				p = p->ptr; //next neighbour of u
			}
		}

		//cout << "\nmaxHeapSize (single-Dj): " << maxHeapSize;

		visitedNodesReset();
		return dist;
	}	
 
	int bidi_classicalDJ(int s, int t) { //, ReadRoad *g) {	// s and t are the ID of source node and destination node of graph		
		adList* p;
		int id;     // the graph node ID of  the bheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node *v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = INFINITE;    // tentative best shortest length of (s, t)
		bool flag = 1;  // turns to forward search(true) or backward search(false)
		//int maxHeapSize = 0;

		if(s == t)	return 0;
		if(node[s].state == NON_EXIST || node[t].state == NON_EXIST) {
			cout << "\nsource or target don't exist."; 
			return 0;
		}

		BinaryHeap bheap[2];   
		bheap[flag].insert(s, 0, flag);    // insert source (s, 0) into Fibo heap
		node[s].dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node[s].bp[flag] = 1;     //   the associated Fibo Node storing ID and dist of this node
		node[s].state = SETTLED*(flag+1);  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= INFINITE -- initially when it is unreached, dist==INFINITE)

		bheap[!flag].insert(t, 0, flag);   
		node[t].dist[!flag] = 0;
		node[t].bp[!flag] = 1;
		node[t].state = SETTLED*(flag);
		visited.push_back(s);   
		visited.push_back(t);  
		dist = 0;  

		while(!bheap[flag].isEmpty() || !bheap[!flag].isEmpty()) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(bheap[flag].isEmpty()) { flag = !flag; }

			//if(maxHeapSize < bheap[flag].getSize()) maxHeapSize = bheap[flag].getSize();	
			id =  bheap[flag].removeMin(flag);   // the flag is for trace index of heap node for the graph node[x].bp[flag] when swapNodes()
			dist = node[id].dist[flag];  
			
			if(node[id].state == SETTLED*(2-flag)) { 
				//cout << "  Weak: " << best <<endl;
				visitedNodesReset();
				return best; 
			}
			if(!bheap[!flag].isEmpty()){  
				if(bheap[!flag].min() + dist > best)  { // Stop when topf + topr > u.//EPP shortest path algorithms Bidirectional DJ.df P3
  				    //cout << " Strong: " << best <<endl;
					visitedNodesReset();
					return best; 
				}
			}
			if( (id == t || id == s) && dist ) { // This rarely happens in bi-di DJ search, except very small graph; two searches in two directions tend to meet in the middle of graph
				//cout << "  d(s, t): " << dist << endl;
				visitedNodesReset();
				return dist; 
			}

			node[id].state = SETTLED*(flag+1);   

			//cout << "\n\nid: "<< id;

			//if(flag) p = node[id].adlistout;  // keep consistent with the first node put in bheap[flag]
			//else      p = node[id].adlistin; 
			p = node[id].adlistout;
			while(p!=NULL) {  // for each neighbor v of u:  
				id_v = p->ID;
				//cout <<"\n  id_v: " << id_v;
				v = &(node[id_v]);		// v points to one neighbour of u
				if(v->state == SETTLED*(flag+1)) {  // v has been settled in the flag direction
					p=p->ptr;
					continue;
				}
				weight = p->wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v->dist[flag]) {  // update v->dist[flag], and also update best if v is settled by !flag direction
					if(v->dist[flag] == INFINITE) {  // this implies v is UNREACHED on this direction
						v->dist[flag] = tentative;      
						v->bp[flag] = bheap[flag].insert(id_v, tentative, flag);    
						visited.push_back(id_v);
					}
					else{
						v->dist[flag] = tentative;      
						bheap[flag].decreaseKey(v->bp[flag], tentative, flag); 
					} 
					if(v->dist[!flag] < INFINITE) {  // Reach a settled node v (v has been settled in the !flag direction).
						if( best > tentative + v->dist[!flag]) { // Update the tentative shortest distance "best".
							 best = tentative + v->dist[!flag];  //  This is the unique statement where "best" is updated.
							 //cout <<"\n  best update at: "<< id_v << " " << best;
						}
					}
				}
				//cout <<"\nv.dist: " << v->dist[flag]; 
				p = p->ptr; //next neighbour of u
			}//while(p!=NULL) 
			flag = !flag;  // turn to another searching direction
		} // while(!bheap[flag].isEmpty()...) 
		
		//cout << "\nmaxHeapSize(bi-di Classical-DJ): " << maxHeapSize;

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

	int ContractionHierarchiesDJ(int s, int t) { //, ReadRoad *g) {	// s and t are the ID of source node and destination node of graph		
		//int static count = 0;
		adList* p;
		int id;     // the graph node ID of  the bheap node U -- removeMin()
		int id_v;  // the graph node ID of  the node v -- the neighbour of the node id
		Node *v;  // temporary Node pointer, pointing to a neighbour node of u
		int weight; // two nodes and the weight of edge(u,v)
		int tentative;      // tentative distance of (s, v)
		int dist;  // the dist between the node and the repective source acctually
		int best = INFINITE;    // tentative best shortest length of (s, t)
		bool flag = 1;  // turns to forward search(true) or backward search(false)
		int toprID=0, toprDist=0;

		if(s == t)	return 0;
		if(node[s].state == NON_EXIST || node[t].state == NON_EXIST) {
			cout << "\nsource or target don't exist."; 
			return 0;
		}

		BinaryHeap bheap[2];   
		bheap[flag].insert(s, 0, flag);    // insert source (s, 0) into Fibo heap
		node[s].dist[flag] = 0;   //  tentative dist to s, or real dist to s after it is settled
		node[s].bp[flag] = 1;     //   the associated Fibo Node storing ID and dist of this node
		node[s].state = SETTLED*(flag+1);  // state: SETTLED or not.  (for state "REACHED" we can know from its dist ?= INFINITE -- initially when it is unreached, dist==INFINITE)

		//bheap[!flag].insert(t, 0, flag);   
		bheap[!flag].insert(t, 0, 1-flag);    //Li, Jul 10, 2014
		node[t].dist[!flag] = 0;
		node[t].bp[!flag] = 1;
		node[t].state = SETTLED*(flag);
		visited.push_back(s);   
		visited.push_back(t);  
		dist = 0;  

		while(!bheap[flag].isEmpty() || !bheap[!flag].isEmpty()) { // alternate forward search and reverse search of bidirectional Dijkstra's aglo
			if(bheap[flag].isEmpty()) { flag = !flag; }

			id =  bheap[flag].removeMin(flag);   
			dist = node[id].dist[flag];  
			node[id].state = SETTLED*(flag+1);   
 
			if(dist >= best) {  // One-side stopping criterion of Contraction Hierarchies algo. From PHAST P3?
				bheap[flag].clear();
				flag = !flag; 
				//cout << " Stopping: " << best <<endl; 
				continue; 
			}

 			p = node[id].adlistout;
			while(p!=NULL) {  // for each neighbor v of u:  
				id_v = p->ID;
				//cout <<"\n  v: " << id_v;

///////////////////////////////////////////////////////////////////////////////////
				// Contraction Hierarchies query Algo: 
				// Search is in ascending order in both bi-directions.  
				if(node[id_v].OD < node[id].OD ) { 
					p = p->ptr;
					//cout << " prun";
					continue;
				}
///////////////////////////////////////////////////////////////////////////////////
				
				//cout <<"\n  v: " << id_v;

				v = &(node[id_v]);		// v points to one neighbour of u
				if(v->state == SETTLED*(flag+1)) {  // v has been settled in the flag direction
					p=p->ptr;
					continue;
				}
				weight = p->wt;  //weight of (u,v)
				tentative = dist + weight;  // give tentative distance for v
				if (tentative < v->dist[flag]) {  // update v->dist[flag], and also update best if v is settled by !flag direction
					if(v->dist[flag] == INFINITE) {  // this implies v is UNREACHED on this direction
						v->dist[flag] = tentative;      
						v->bp[flag] = bheap[flag].insert(id_v, tentative, flag);    
						visited.push_back(id_v);
					}
					else{
						v->dist[flag] = tentative;      
						bheap[flag].decreaseKey(v->bp[flag], tentative, flag); 
					} 

					//v->previous[flag] = id;

					if(v->dist[!flag] < INFINITE) {  // Reach a settled node v (v has been settled in the !flag direction).
						if( best > tentative + v->dist[!flag]) { // Update the tentative shortest distance "best".
							 best = tentative + v->dist[!flag];  //  This is the unique statement where "best" is updated.
							 //lastMeetNode = id_v;		
							//cout <<"\n  best update at: "<< id_v << " " << best;
						}
					}
				}
				//cout <<"\n  v.dist: " << v->dist[flag]; 
				p = p->ptr; //next neighbour of u
			}//while(p!=NULL) 
			flag = !flag;  // turn to another searching direction
		} // while(!bheap[flag].isEmpty()...) 
		//showPath(s, t);
		visitedNodesReset();	
		return best;
	}

protected:

	void showPath(int s, int t) {
		vector<int>onPath;
		//cout  << "\n\nthe shortest path to source direction: " << lastMeetNode;

		int temp = lastMeetNode;
		while(temp != s){
			onPath.push_back(temp);
			temp = node[temp].previous[0];
		} 
		onPath.push_back(s);
		
		std::reverse(onPath.begin(),onPath.end());    

		//cout  << "\nthe shortest path to target direction: " << lastMeetNode;
		temp = lastMeetNode;
		while(temp != t) {
			temp = node[temp].previous[1];
			onPath.push_back(temp);
		}

		//cout << "\n CH found path: " << onPath.size() << endl;
		//for (std::vector<int>::iterator it=onPath.begin(); it!=onPath.end(); ++it) cout << ' ' << *it; 
		//cout << "\n\n the path on original graph:\n";
		unPackPath(onPath);
		//cout << endl;
		//cout << "\nshowPath finished\n";
		//cout << "\nthe shortest distance between " << s <<" and " << t << " is: " << node[t].dist <<endl;
	}

	
	void unPackPath(vector<int> &onPath) {
		vector<int> originalPath;
		vector<int> contractedNodes;  // these nodes will be found and inserted into the path
		vector<int>::iterator it;
		int v, u, w;
		int dist;
		adList *p1, *p2;

		for (it=onPath.begin(); it!=onPath.end()-1; ++it) {
			v =  *it;
			w = *(it+1);
			//cout << "\ninsert: " << v;
			originalPath.push_back(v);
			// find contracted nodes and insert them into the path
			contractedNodes.clear();
			do {
				p1 = node[v].adlistout;
				dist = 0;  // dist = weight(v, w);
				while(p1 != NULL) {
					if(p1->ID == w) { dist = p1->wt; break;	}
					p1 = p1->ptr;
				}
				assert(dist);
				// try to find a node u which is between v and w and satisfies that weight(v,u) + weight(u,w) == dist
				// we also assume that such u is unique (even if it is not unique, it doens't matter; we still can find the original path).
				p1 = node[v].adlistout;
				while(p1 != NULL) {
					u = p1->ID;
					p2 = node[w].adlistout;
					while(p2 != NULL) { 
						if(p2->ID == u) break;
						p2 = p2->ptr; 
					}
					if(p2 != NULL) assert(p1->wt + p2->wt >= dist);
					if(p2 != NULL && (p1->wt + p2->wt) == dist) break;  				
					p1 = p1->ptr;
				}
				if(p1==NULL) break;
				contractedNodes.push_back(u);
				w = u;
			}while(true);

			// insert the contracted nodes into original path
			while(!contractedNodes.empty()) {
				//cout << "\ninsert: " << contractedNodes.back();
				originalPath.push_back(contractedNodes.back());
				contractedNodes.pop_back();
			}
		}
		//cout << "\ninsert: " << onPath.back();
		originalPath.push_back(onPath.back());  // insert the last node on path
		//cout << "\n\n the path on original graph: " << originalPath.size() << endl;
		//for (it=originalPath.begin(); it!=originalPath.end(); ++it) cout << ' ' << *it; 
		//cout << "\n\n\n";
	}

	// Check if all nodes in W[] are settled for onetoManyClassicalDJ();
	// If so,  onetoManyClassicalDJ() can return.
	bool allSettled(vector<MultiDestination>& W) {
		int i;
		for(i=0; i<W.size(); i++) {
			if(node[W[i].ID].state != SETTLED) return false;
		}
		for(i=0; i<W.size(); i++) {
			W[i].distDJ = node[W[i].ID].dist[0];
		}
		return true;
	}

	void visitedNodesReset() {
		nVisitedNodes = visited.size();   // nVisitedNodes is used by getNVisitedNodes()
		for(vector<int>::iterator it=visited.begin(); it!=visited.end(); ++it) {
			node[*it].state = EXIST;
			node[*it].dist[0] = INFINITE; 
			node[*it].dist[1] = INFINITE; 
			node[*it].bp[0] = -1;
			node[*it].bp[1] = -1;
		}
		//visited.clear();
		visited.resize(0);
		//cout <<"\n_visitedNodesReset()" ;
	}

};

//////////////////////////////////////////////////////////////
// End of class Dijkstra_Bheap.
//////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////
// Begin of class CH -- Contraction Hierarchies.
//////////////////////////////////////////////////////////////

class CH {  
private:
		int maxShortcuts; // max # of shortcuts a node added
		int maxEdges;        // max # of edges a node has
		int nAddedShortcuts; // totally # of shortcuts were added by all nodes
		int nRemovedEdges;   // totally # of edges of graph were removed 
		int maxSizeW;  //if W.size() is over this value, setInitialOrder() is triggered to avoid too many shortcuts generated 

public:
	CH() { maxShortcuts = 0;  maxEdges = 0; nAddedShortcuts = 0; nRemovedEdges = 0;}

protected:	

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
		void distMDSD(int source, vector<MultiDestination>&W, int maxLength) { 
			Dijkstra_Fheap DJfh; 
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
			//g->clearNodeState();  //use resetVisitedNode() to replace it
			return DJfh.shortestDistanceLimited(source, target, contractNode);//, maxSettled);
		}*/

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
	int addShortcuts(int v, int w, int wt) {
		//static int _addShortcuts=0;
		//cout <<"\naddShortcuts: " << _addShortcuts++ << ": (" << v << "," << w <<")=" <<wt;
		//cout <<"\nv, w, weight: (" << v << ", " << w << "), " << wt;
		//cout <<"\na " << v << " " << w << " " << wt;
		//cout <<"\na " << v << " " << w << " " << wt << " passby " << u;
		
		//assert ( v < w);

		if( v >= w ) return 0;  // omit the edges like ( 2 1  365) -- we only store edges with two nodes from small to large like (1 2 365)
		
		nAddedShortcuts += 2;  // add two shortcuts (v, w, wt) and (w, v, wt)

		adList* p;
		adList* head;
		int flag; 
		int x[2] = { v, w };

		for(flag=0; flag<2; flag++) {
			head = node[x[flag]].adlistout;
			p = new adList;
			p->ID = x[1 - flag];
			p->wt = wt;
			if (head == NULL) {  // add the first node into node[v].adlistout
			   node[x[flag]].adlistout = p;		  
			   p->ptr = NULL;
			}
			else {  // put a new node at the head of node[v].adlistout
			   p->ptr = head;
			   node[x[flag]].adlistout = p;
			}
			// Note:  We only store outgoing edges; considering incoming edges are the same set as the outgoing edge set.  
			// No need: ---->  node[x[flag]].adlistin = node[x[flag]].adlistout; // to save space, for symmetric directed graph, we only store one-direct of an edge for each node
		}
		return 2;
/*
		head = node[w].adlistin;
		p = new adList;
		p->ID = v;
		p->wt = wt;
		if (head == NULL) {
		   node[w].adlistin = p;		  
		   p->ptr = NULL;
		}
		else {
		   p->ptr = head;
		   node[w].adlistin = p;
		}
*/
	}


	// get the weight of an edge (v,w)  
	int getWeight(int v, int w) {
		static int _getWeight=0;
		//cout <<"\n_getWeight: " << _getWeight++;
		//cout <<"\n(_v, _w): " << v <<", " << w;
		adList* p = node[v].adlistout;
		while(p != NULL) {
			if(p->ID == w) return p->wt;
			p = p->ptr;
		}
		return INFINITE;  // v and w were not neighbours
	}


	/**
	  * Assign the edge (v, w) a new weight, as well as check if v,w are connected
	  * Return false if they are not neighbors.
	  * 
	  */
	void updateWeight(int v, int w, int weight) {
		//static int _updateWeight = 0;
		//cout <<"\n_updateWeight: " << _updateWeight++;
		//cout <<"\nnew weight: " << weight;
		
		if(v >= w) return;

		int flag; 
		int x[2] = { v, w };

		for(flag=0; flag<2; flag++) {
			adList* p = node[x[flag]].adlistout;
			while(p != NULL) {
				if(p->ID != x[1 - flag]) { p = p->ptr; continue; }
				p->wt = weight;
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
		int x[2] = { v, w };

		for(flag=0; flag<2; flag++) {
			adList* p1 = node[x[flag]].adlistout;
			adList* p2 = p1->ptr;

			if(p1->ID == x[1 - flag]) { node[x[flag]].adlistout = p2; delete p1; p1 = NULL; return; }
			while(p2 != NULL) {
				if(p2->ID != x[1 - flag]) { p1 = p2; p2 = p2->ptr;  continue; }
				p1->ptr = p2->ptr;
				delete p2;
				p2 = NULL;
			}
		}
	}


	 /**
	   * Calculate E as one of priority terms. Another one is D - # of deleted neighbours.
	   * This is preparation for order selection.
	   * The idea is: 
	   * length of <v, u, w>  is less than length(v,w) not via u? Yes, add a shortcuts (v,w) and nShortcuts++.
	   * The formula of edgeDifference E(u):
	   *		E(u) = (# of shortcuts added for contration of u) - (# of edges of u) 
	   *				= nShortcuts - noEedges.
	   *
	   * <p>: O(delta^3) per node; delta is the average degree of nodes, which is increasing during node contraction.
	   *
	   * @param u: the node ID for which we compute edge difference.
	   */
	/*
	int edgeDifference(int u, bool realContract) { 
		int nShortcuts = 0;
		int nEdges = 0;
		int dist;  // length of <v, u, w>
		int dist1Hop, distDJ; // distDJ: length of <v, ..., w> not via u.
		adList* p1;   // p1->ID is v
		adList* p2;   // p2->ID is w
		int v, w; // v->u->w; u is the node to be contracted (if realContract is true, it do be contracted).		
		//time_t start;

		if(!realContract) {  // only do for setInitialOrder(); we don't need it when contract a node.
			// count # of edges:
			p1 = node[u].adlistin;
			while(p1 != NULL) {
				if(node[p1->ID].state != CONTRACTED)  nEdges++;
				p1 = p1->ptr;
			}
			p2 = node[u].adlistout;
			while(p2 != NULL) {
				if(node[p2->ID].state != CONTRACTED)  nEdges++;			
				p2 = p2->ptr;
			}
		}

		// count # of shortcuts:
		p1 = node[u].adlistin;
		while(p1 != NULL) {
			v = p1->ID;
			////assert(v != u);
			if(node[v].state == CONTRACTED) { p1 = p1->ptr; continue; }
			p2 = node[u].adlistout;
			while(p2 != NULL) {
				w = p2->ID;
				////assert(w != u);
				if(node[w].state == CONTRACTED || v==w) { p2 = p2->ptr; continue; }  // ?????
				dist = p1->wt + p2->wt;
		
				dist1Hop = getWeight(v, w); // 1-hop from v to w
				//min = dist1Hop;
				//distDJ = DJfh.shortestDistanceLimited(v, w, u, 3); 
				distDJ = distSDSD(v, w, u); // local search for shortest path withness  // ?????? 
				//distDJ = testDJ(v, w);
				//assert(distDJ <= dist1Hop && distDJ <= dist);
				if(distDJ == dist1Hop) { ; } // assume v-w is shortest path
				else { //if(distDJ < dist1Hop) {
					if(dist1Hop < INFINITE) { // v-w is one edge, but superfulous
						removeEdge(v,w);
					}
				}

				if(distDJ == dist) { // assume v-u-w is shortest path
					if(realContract) {
						if(dist1Hop == INFINITE) { 
							addShortcuts(v, w, dist); // when setInitialOrder(), we don't really add shortcuts into graph
							nShortcuts++;
						}
						else updateWeight(v,w,dist);
					}
					else if(dist1Hop == INFINITE) {
						nShortcuts++;
					}
					else {;} // do nothing; not updateWeight() for setinitialorder()
				}
				else { //if(distDJ < dist)
					;  // do nothing, because we have distDJ as "witness path"
				}			
				p2 = p2->ptr;
			}
			p1 = p1->ptr;
		}
		//cout <<"\n" <<  setw(7) << u <<"#: "<< setw(4) << nEdges <<" "<<  setw(4) << nShortcuts << " " <<  setw(4) << nEdges - nShortcuts ;
		//if( (unsigned)time(NULL) - start){
			//cout << setw(4) << (unsigned)time(NULL) - start <<"s "; //\n";
		//}
		//cout << "\n";

		if(maxShortcuts < nShortcuts) maxShortcuts = nShortcuts;
		if(maxEdges < nEdges) maxEdges = nEdges;

		return nShortcuts -  nEdges;
	}
	// edgeDifference()
	*/

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
	int edgeDiffbyMultiDest(int u, bool realContract) { //, ReadRoad *g) { 
		int nShortcuts = 0;
		int nEdges = 0;
		int v, w; // v->u->w; u is the node to be contracted (if realContract is true, it do be contracted).
		int maxLength;  // max length of v to each node in W[]; if dist in onetoManyDJ over this value, then onetoManyDJ can terminate and return
		adList* p1;   // p1->ID is v
		adList* p2;   // p2->ID is w			
		vector<MultiDestination>W;
		
		// count # of edges:
		if(!realContract) {  // only do for setInitialOrder(); we don't need it when contract a node.
/*
			p1 = node[u].adlistin;
			while(p1 != NULL) {
				if(node[p1->ID].state != CONTRACTED)  { nEdges++; nEdges1++; }
				p1 = p1->ptr;
			}
*/
			p2 = node[u].adlistout;
			while(p2 != NULL) {
				if(node[p2->ID].state != CONTRACTED)  nEdges++;			
				p2 = p2->ptr;
			}
		}

		//count # of shortcuts:
		//cout <<"\nu: " << u; 
		//p1 = node[u].adlistin; 
		p1 = node[u].adlistout;
		while(p1 != NULL) {
			v = p1->ID;
			if(node[v].state == CONTRACTED) { p1 = p1->ptr; continue; }
			
			//fill all outgoing nodes w to vector W[] and compute their distDJ
			int i=0;
			W.resize(SIZE_W); // SIZE_W = 300 should be enough as the upper bound of # of edges of node u
			adList* p2 = node[u].adlistout;
			maxLength = 0;
			while(p2 != NULL) {
				w = p2->ID;
				//if(node[w].state == CONTRACTED || v==w) { p2 = p2->ptr; continue; }  // ?????

				if(node[w].state == CONTRACTED || v >= w) { p2 = p2->ptr; continue; }   

				W[i].ID = w;
				W[i].dist1Hop = getWeight(v, w); 

				W[i].distDJ = INFINITE;
				W[i].length = p1->wt + p2->wt;
				if(maxLength < W[i].length) maxLength = W[i].length;

				i++;
				if(i >= W.size()) W.resize(W.size() + SIZE_W); //??????
				p2 = p2->ptr;
			}
			W.resize(i);
			distMDSD(v, W, maxLength);  // TIME CONSUMING! -- Computing multiple shortest dist from v to each w in W[]
			//cout <<"\n  v: " << v; 
			//for(i=0; i < W.size(); i++) {
				//int distDJ = testDJ(v, W[i].ID, g);
				////assert(distDJ == W[i].distDJ); // && W[i].distDJ <=  W[i].dist1Hop);
			//}
			nShortcuts += addShortcutsOnW(v, u, W, realContract);
			p1 = p1->ptr;
		}

		//if(maxShortcuts < nShortcuts) maxShortcuts = nShortcuts;
		//if(maxEdges < nEdges) maxEdges = nEdges;
		//cout<<"\nnShortcuts, nEdges: " << nShortcuts <<" " << nEdges;
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
	int addShortcutsOnW(int v, int u, vector<MultiDestination>& W, bool realContract){
		int w, distDJ, dist1Hop, length;   
		int nShortcuts = 0;

		for(int i=0; i<W.size(); i++) {
			w = W[i].ID;
			distDJ = W[i].distDJ;
			dist1Hop = W[i].dist1Hop;
			length = W[i].length;
			/*if(!(distDJ <= dist1Hop && distDJ <= length)){
				cout<<"\nmy //assertion on (distDJ <= dist1Hop && distDJ <= length) failed in addShortcutsOnW: distDJ " << distDJ << ", dist1Hop " << dist1Hop << ", length " << length << "\nv, w, u, i, W.size: v " << v << ", w " << w << ", u " << u << ", " << i << ", W.size() " << W.size() <<endl;
				exit(-1);
			}				
			////assert(distDJ <= dist1Hop && distDJ <= length);
			if(distDJ == dist1Hop) { ; } // assume v-w is shortest path
			else { //if(distDJ < dist1Hop) {
				if(dist1Hop < INFINITE) { // v-w is one edge, but superfulous
					removeEdge(v,w);
				}
			}*/

			if(distDJ == length) { // assume v-u-w is shortest path
				if(realContract) {
					if(dist1Hop == INFINITE) { 
						addShortcuts(v, w, length); // when setInitialOrder(), we don't really add shortcuts into graph
						nShortcuts++;
					}
					else updateWeight(v, w, length);
				}
				else if(dist1Hop == INFINITE) {
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
	  *  D = deletedNeighbours is a priority term of node order. Initially it is generated when
	  *  we compute edgeDifference.
	  *  It simply counts all deleted (i.e. labeled "CONTRACTED") neighbour nodes.
	  *  
	  * <p>: O(delta) per node
	  *
	  * @param u: the node which counts its deleted neighbours
	  */
	/*
	int deletedNeighbours(int u) {
		static int _deletedNeighbours = 0;
		//cout <<"\n_deletedNeighbours: " << _deletedNeighbours++;

		int D = 0;
		adList* p;

		p = node[u].adlistin;  
		while(p != NULL) { 
			if(node[p->ID].state == CONTRACTED) D++;
			p = p->ptr;
		}

		p = node[u].adlistout;  
		while(p != NULL) { 
			if(node[p->ID].state == CONTRACTED) D++;
			p = p->ptr;
		}

		return D;
	}
	// deletedNeighbours()
	*/

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
		adList* p;

		//p = node[u].adlistin;
		
		p = node[u].adlistout;

		while(p != NULL) { 
			v = p->ID;
			if(node[v].state != CONTRACTED) {
				node[v].OD += 120;
				if(node[v].OD <= node[u].OD) {
					node[v].OD = node[u].OD + 1;  // node[v].OD = min { node[v].OD, node[u].OD-1 }
				}
			}
			p = p->ptr;
		}

		// if the graph is symmetric, we forget the following block:
/*
		p = node[u].adlistout;  
		while(p != NULL) { 
			v = p->ID;
			if(node[v].state != CONTRACTED) {
				//node[v].OD++;  // ??
				node[v].OD += 120;
				if(node[v].OD <= node[u].OD) {
					node[v].OD = node[u].OD + 1;  // node[v].OD = min { node[v].OD, node[u].OD-1 }
				}
			}
			p = p->ptr;
		}
*/
	}

	/**
	  * Contract node u from graph -- only label it "CONTRACTED".
	  * But in nodeOrdering(), this will affect the order of other node -- D = deletedNeighbours.
	  */
	void contractNode(int u, ReadRoad *g) { 
		//cout <<"\ncontract node: " << u;
		//edgeDifference(u, true); // v1.2 single-desti for finding witness path // "true" means we really add shortcuts and contract node u
		edgeDiffbyMultiDest(u, true); // v1.5 multi-desti for finding witness path  
		node[u].state = CONTRACTED;
		updateNeighbours(u);  // This leads to re-shuffle and re-order odid[]
	}
	// contract()

	/**
	  * Set initial order to be "-190 *  # of edgeDiff" for each node.
	  * edgeDiff = nShortcuts - nEdges. 
	  *
	  * <p> O(?)
	  *
	  * @param n1: set initial order starting from node n1
	  * @param odid: map of ID to OD
	  */	
	void setInitialOrder(int n1, vector<ODID>& odid) { //, ReadRoad *g) { // set initial order to be nEdges for each node.
		int i, u;//, OD;
		int n = odid.size() - 1;

		time_t start = (unsigned)time(NULL) ; 

		//cout << "\nsetinitialorder: ";
		if(n1==1) {
			for(i=1; i<=n; i++) {
				odid[i].ID = i;
			}
			//random_shuffle(odid.begin() + n1, odid.end());  //only shuffle once
		}

		random_shuffle(odid.begin() + n1, odid.end());  

		for(i=n1; i<=n; i++) {
			u = odid[i].ID;
			//odid[u].OD = 190 * edgeDifference(u, false); // 'false' means not really contract u.  -- Wrong! It should be "odid[i].OD" rather than "odid[u].OD"!
			//odid[i].OD = 190 * edgeDifference(u, false); // v1.2 single-destination  for finding witness path // 'false' means not really contract u.  
			odid[i].OD = 190 * edgeDiffbyMultiDest(u, false); // v1.5 multi-destination for finding witness path 
		}
		sort(odid.begin() + n1, odid.end(), odidSort);  
		cout << "\nsetinitialorder used time: " << (unsigned)time(NULL) - start <<"s"; //\n";
	}
    // setInitialOrder()


	void shuffleSelection(int n1, vector<int>& RandomSelection) { // select min 10% node and shuffle them
		int u, rd, temp;
		srand((unsigned)time(NULL));
		for(u = 1; u <= n1; u++) RandomSelection[u] = u;  // foolish but it seems that vector<int>RandomSelection; RandomSelection.resize(n+1); takes very very long time... maybe a C++ bug
		for(u = 1; u <= n1; u++) { // shuffle u so that no consecutive nodes are selected consecutively
			if(WINDOWS) rd = rand() % n1 + 1;    // rd: 1 ~ n
			else rd = rand() *  rand() % n1 + 1;
			temp = RandomSelection[rd];
			RandomSelection[rd] = RandomSelection[u];
			RandomSelection[u] = temp;
		} 
	}
	// shuffleSelection()

 
	void freshODID(vector<ODID> &odid, int noContracted, int noOfNodes) {  // remove contracted nodes from odid[]; re-read node order into odid 
		static int _freshODID = 0;
		cout <<"\n_freshODID: " << _freshODID++;
		int u = 1;

		// filter off contracted nodes from odid[]:
		for(int i = 1; i <= noOfNodes; i++)  {
			if(node[i].state == CONTRACTED) continue;  
			odid[u].ID = i;
			odid[u].OD = node[i].OD;
			u++;
		} 
		random_shuffle(odid.begin()+1, odid.begin()+u+1);  // maybe need only once
		sort(odid.begin()+1, odid.begin()+u+1, odidSort); 
	}
 


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
	
	void nodeOrdering(ReadRoad* g) {  //(int start, int end) {
		int i, u; 
		time_t start;
		int n = g->getNodes();
		int stage = 0; //= n / stageFactor; // divide node[] into SEG segments
		int STAGE = 28; // n1[STAGE+1]
		vector<ODID>odid(n+1);  // map between ID and OD; for shuffling and sorting nodes by OD.
		int n1[29] = {1, 
					  n/8, 2*n/8, 3*n/8, 4*n/8, 
					  n/2+n/16, n/2+2*n/16, n/2+3*n/16, n/2+4*n/16, 
					  3*n/4+n/32, 3*n/4+2*n/32, 3*n/4+3*n/32, 3*n/4+4*n/32,
					  7*n/8+n/64, 7*n/8+2*n/64, 7*n/8+3*n/64, 7*n/8+4*n/64,
                      //7*n/8+5*n/64, 7*n/8+6*n/64, 7*n/8+7*n/64, 7*n/8+8*n/64
					  15*n/16+1*n/128, 15*n/16+2*n/128, 15*n/16+3*n/128, 15*n/16+4*n/128,
					  31*n/32+1*n/256, 31*n/32+2*n/256, 31*n/32+3*n/256, 31*n/32+4*n/256,
					  31*n/32+5*n/256, 31*n/32+6*n/256, 31*n/32+7*n/256, 31*n/32+8*n/256	
					  }; //1..~n (~n nearly equal to n)  // Proposed by Deqi Li, Jan 14,2014.
		
		
		cout << "\nnodeOrdering with 1/4 exponentially decreasing size...";
		
		i = 1;
		while(i<=n) {
			if(i == n1[stage]) { // restart initial order for un-contracted nodes at each stage
			//if(maxSizeW > TRIGGERING_SIZE_W) {
				cout<<"\n\nstarting from stage, i: "<< stage << ", " << i <<"#";
				//cout<<"\n\nstarting from i: "<< i <<"#";
				setInitialOrder(i, odid);  // set initial order starting from node i
				//cout <<"\nmaxShortcuts, maxEdges: " << maxShortcuts << " " << maxEdges;
				start = (unsigned)time(NULL); 
				cout << "\n\nnodeOrdering (contracting) starting with intial order: " << odid[i].OD;
			}
			u = odid[i].ID;  // take nodes in ascending order; because odid[] stores nodes in ascending order
			contractNode(u, g);  // contract nodes in order of OD
			node[u].OD = i;      // so order increases by one at each step

			if(stage < STAGE){
				if(i == n1[stage+1]-1) {
					cout << "\nnodeOrdering (contracting) used time: " << (unsigned)time(NULL) - start <<"s\n"; //\n";			
					//cout <<"maxShortcuts, maxEdges: " << maxShortcuts << " " << maxEdges;
					cout <<"\nending at stage, i: " << stage << ", " << i <<"#\n";
					stage++;
				}
			}

			i++;
		}
		cout << "\nnodeOrdering used time: " << (unsigned)time(NULL) - start <<"s"; //\n";
		cout << "\n\nTotally added shortcuts: " << nAddedShortcuts;// <<" " << nRemovedEdges;
	}
	// nodeOrdering()

public:
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
	void preprocessing(ReadRoad* g) {
		time_t start = (unsigned)time(NULL) ;  
		cout <<"\npreprocessing... ";
		nodeOrdering(g);
		g->clearNodeState(); 
		cout <<"\npreprocessing finished. time used: " << (unsigned)time(NULL) - start <<"s\n\n"; 

		/*hierarchyConstruction(g);*/ // It is implemented in nodeOrdering(), so it is not used.

	}
	// preprocessing()

	void writeRoad(ReadRoad *g){
		stringstream ss;   //---- I believe stringstream is quite slow
		int v, n;  // node IDs and weight on an arc (v,u)
		adList* p;

		n = g->getNodes();
		string filename = g->getFilename() + ".preprocessed";
		ofstream roaddata(filename.c_str());  //"USA-road-d.NY.gr.mini"
		if(roaddata == NULL) {
			cout << "file '"<< filename <<"' cannot create.\n";
			exit(-1);
		}
		time_t start = (unsigned)time(NULL);
	    cout << "begin write preprocessed data into file '" << filename << "'...\n";
	    for(v=1; v<=n; v++) {
			ss.str("");  // really clear ss
		    p = node[v].adlistout;
		    while(p!=NULL){
				if(v >= p->ID) { p=p->ptr; continue; }  // omit the edge like(2 1 365); only store edges like (1 2 365)
				ss << "a " << v << " " << p->ID << " " << p->wt << "\n";
			    p=p->ptr;
			}
			roaddata << ss.str();
		}

		roaddata << "o\n";
	    for(v=1; v<=n; v++) {
			ss.str("");  // really clear ss
			ss << "o " << v << " " << node[v].OD << "\n";
			roaddata << ss.str();
		}
	    roaddata.close(); 
		cout <<"\nwriting preprocessed data finished. time used: " << (unsigned)time(NULL) - start <<"s\n\n"; 
	}
	//writeRoad()

};
//////////////////////////////////////////////////////////////
// End of class CH -- Contraction Hierarchies.
//////////////////////////////////////////////////////////////


/**
 * Verify the correctness of classical DJ, bi-directional classicalDJ, and CH DJ. 
 * All the 3 versions are based on Fibonacci heap.
 */ 
int testCorrectnessDJ(int source, int target,  int no) { 
	Dijkstra_Fheap DJfh; 
	int dist1, dist2, dist3, dist4;  
/*
dist4 = DJfh.TrivialDJ(1, 5); 
cout << "\n---------------------------------------------------------------\n";
cout << "No. " << no <<"  dist(" << source << ", " << target <<"): " << dist4 << endl;  
cout << "---------------------------------------------------------------\n";
return 0;
*/

	cout <<"\n" << setw(4) << no << "#  ";
	cout << setw(7) << source << "  " << setw(7) << target << "   ";

	dist1 = DJfh.ContractionHierarchiesDJ(source, target);  
	cout <<  "  " << setw(8) << dist1;

	dist2 = DJfh.ContractionHierarchiesDJ(target, source);  
	cout << "  " << setw(8) << dist2;
/*
	dist3 = DJfh.bidi_classicalDJ(source, target);  
	cout << "  " << setw(8) << dist3;

	dist4 = DJfh.bidi_classicalDJ(target, source);  
	cout << "  " << setw(8) << dist4;

*/	
/*	
   dist3 = DJfh.Trivial_CHDJ(source, target);  
	cout << "  " << setw(8) << dist3;

	dist4 = DJfh.Trivial_CHDJ(target, source);  
	cout << "  "  << setw(8) << dist4;


	dist3 = DJfh.classicalDJ(source, target);  
	cout << "  " << setw(8) << dist3;

	dist4 = DJfh.classicalDJ(target, source);  
	cout << "  " << setw(8) << dist4;

	dist3 = DJfh.Trivial_bidi_classicalDJ(source, target);  
	cout << "  " << setw(8) << dist3;

	dist4 = DJfh.Trivial_bidi_classicalDJ(target, source);  
	cout << "  "  << setw(8) << dist4;
*/
	Dijkstra_Bheap DJbh; 
/*
	dist3 = DJbh.classicalDJ(source, target);  
	cout << "  " << setw(8) << dist3;

	dist4 = DJbh.classicalDJ(target, source);  
	cout << "  " << setw(8) << dist4;

	dist3 = DJbh.bidi_classicalDJ(source, target);  
	cout << "  " << setw(8) << dist3;

	dist4 = DJbh.bidi_classicalDJ(target, source);  
	cout << "  " << setw(8) << dist4;
*/
	dist3 = DJbh.ContractionHierarchiesDJ(source, target);  
	cout << "  " << setw(8) << dist3;

	dist4 = DJbh.ContractionHierarchiesDJ(target, source);  
	cout << "  " << setw(8) << dist4;

	if(!(dist1==dist2 && dist2 == dist3 && dist3 == dist4)) {
		cout << "\nCAUTION: d(" << source << " " << target <<"): " << dist1 << " " << dist2 << " " << dist3 << " " << dist4 << endl;
		exit(0);
	}
	return dist1;
}	
 
// -------------------------------------PERFORMANCE TEST-------------------------------------
// ------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////
// 	Fheap test:
///////////////////////////////////////////////////////////////

void testFheap_classicalDJ(int n, int m, string filename1) {
	int tries, source, target, dist1, dist2, nVisited1, nVisited2;
///////////////////////////////////////////////////////////////
	string caption = "Fheap-classicalDJ PERFORMANCE TEST";      // CORRECTNESS
///////////////////////////////////////////////////////////////
	cout << endl << caption << endl;
	cout << "\n" << setw(7) << "    #  "<< setw(7) <<"  source  " << setw(7) <<"  target  " << setw(8) <<"  distance1 " << setw(8) << "  distance2 " << setw(5) <<"  nVisited1 " << setw(5) <<" nVisited2\n" ;
///////////////////////////////////////////////////////////////

    time_t start = (unsigned)time(NULL) ;  
	int year=2012;
	srand(year);
	int nVisited = 0;
	Dijkstra_Fheap DJfh; 
	for(tries = 1; tries <= 50; tries++) {
		source = unsigned((rand() * rand())) % n + 1;
		target =  unsigned((source * rand())) % n + 1;
		if(!(tries%10)) {
			cout <<"\n" << setw(5) << tries << "#  ";
			cout << setw(8) << source << "  " << setw(8) << target << "   ";
		} 
		dist1 = DJfh.classicalDJ(source, target);  
		nVisited1  = DJfh.getNVisitedNodes();

		//source = unsigned((rand() * rand())) % n + 1;
		//target =  unsigned((source * rand())) % n + 1;
		dist2 = DJfh.classicalDJ(target, source);  
		nVisited2  = DJfh.getNVisitedNodes();
		nVisited += nVisited1 + nVisited2;
		if(!(tries%10)) cout << setw(8) << dist1 << "  " << setw(8) << dist2 << "  " << setw(5) << nVisited1 << "  " << setw(5) << nVisited2;
	}
	time_t end = (unsigned)time(NULL) ; 

	cout << "\n";
	cout << "\n------------------------------------------------------------";
	cout << "\n--------------- " << caption << " --------------";
	cout << "\n------------------------------------------------------------";
	cout << "\n Query time totally used:      " << end - start <<"s (" << tries-1 << " pairs)";
	cout << "\n Average query time per path:  " << (end - start)*1000.0 / (tries-1) / 2 << "ms";
	cout << "\n Average visited nodes:        " << nVisited / (tries-1) / 2;
	cout << "\n srand(year):                  " << year;
	cout << "\n Data file:                   '" << filename1 << "'";
	cout << "\n Nodes, Edges:                 " << n << ", " << m;
	cout << "\n------------------------------------------------------------\n\n";
///////////////////////////////////////////////////////////////
	return;
///////////////////////////////////////////////////////////////
}

void testFheap_bidi_classicalDJ(int n, int m, string filename1) {
	int tries, source, target, dist1, dist2, nVisited1, nVisited2;
///////////////////////////////////////////////////////////////
	string caption = "Fheap-bidi-classicalDJ PERFORMANCE TEST"; // PERFORMANCE
	//string caption = "BinaryHeap-CHDJ PERFORMANCE TEST";
///////////////////////////////////////////////////////////////
	cout << endl << caption << endl;
	cout << "\n" << setw(7) << "    #  "<< setw(7) <<"  source  " << setw(7) <<"  target  " << setw(8) <<"  distance1 " << setw(8) << "  distance2 " << setw(5) <<"  nVisited1 " << setw(5) <<" nVisited2\n" ;
///////////////////////////////////////////////////////////////

    time_t start = (unsigned)time(NULL) ;  
	int year=2012;
	srand(year);
	Dijkstra_Fheap DJfh; 
	int nVisited = 0;
	for(tries = 1; tries <= 50; tries++) {
		source = unsigned((rand() * rand())) % n + 1;
		target =  unsigned((source * rand())) % n + 1;
		if(!(tries%10)) {
			cout <<"\n" << setw(5) << tries << "#  ";
			cout << setw(8) << source << "  " << setw(8) << target << "   ";
		}
		dist1 = DJfh.bidi_classicalDJ(source, target);  
		nVisited1  = DJfh.getNVisitedNodes();

		//source = unsigned((rand() * rand())) % n + 1;
		//target =  unsigned((source * rand())) % n + 1;
		dist2 = DJfh.bidi_classicalDJ(target, source);  
		nVisited2  = DJfh.getNVisitedNodes();
		nVisited += nVisited1 + nVisited2;
		if(!(tries%10)) cout << setw(8) << dist1 << "  " << setw(8) << dist2 << "  " << setw(5) << nVisited1 << "  " << setw(5) << nVisited2;
	}
	time_t end = (unsigned)time(NULL) ; 

	cout << "\n";
	cout << "\n------------------------------------------------------------";
	cout << "\n--------------- " << caption << " --------------";
	cout << "\n------------------------------------------------------------";
	cout << "\n Query time totally used:      " << end - start <<"s (" << tries-1 << " pairs)";
	cout << "\n Average query time per path:  " << (end - start)*1000.0 / (tries-1) / 2 << "ms";
	cout << "\n Average visited nodes:        " << nVisited / (tries-1) / 2;
	cout << "\n srand(year):                  " << year;
	cout << "\n Data file:                   '" << filename1 << "'";
	cout << "\n Nodes, Edges:                 " << n << ", " << m;
	cout << "\n------------------------------------------------------------\n\n";
///////////////////////////////////////////////////////////////
	return;
///////////////////////////////////////////////////////////////
}


void testFheap_CHDJ(int n, int m, string filename1) {
	int tries, source, target, dist1, dist2, nVisited1, nVisited2;
///////////////////////////////////////////////////////////////
	string caption = "Fheap-CHDJ PERFORMANCE TEST"; // PERFORMANCE
	//string caption = "BinaryHeap-CHDJ PERFORMANCE TEST";
///////////////////////////////////////////////////////////////
	cout << endl << caption << endl;
	cout << "\n" << setw(7) << "    #  "<< setw(7) <<"  source  " << setw(7) <<"  target  " << setw(8) <<"  distance1 " << setw(8) << "  distance2 " << setw(5) <<"  nVisited1 " << setw(5) <<" nVisited2\n" ;
///////////////////////////////////////////////////////////////

    time_t start = (unsigned)time(NULL) ;  
	int year=2012;
	srand(year);
	Dijkstra_Fheap DJfh; 
	int nVisited = 0;
	for(tries = 1; tries <= 10000; tries++) {
		source = unsigned((rand() * rand())) % n + 1;
		target =  unsigned((source * rand())) % n + 1;
		if(!(tries%1000)) {
			cout <<"\n" << setw(5) << tries << "#  ";
			cout << setw(8) << source << "  " << setw(8) << target << "   ";
		}
		dist1 = DJfh.ContractionHierarchiesDJ(source, target);  
		nVisited1  = DJfh.getNVisitedNodes();

		//source = unsigned((rand() * rand())) % n + 1;
		//target =  unsigned((source * rand())) % n + 1;
		dist2 = DJfh.ContractionHierarchiesDJ(target, source);  
		nVisited2  = DJfh.getNVisitedNodes();
		nVisited += nVisited1 + nVisited2;
		if(!(tries%1000)) cout << setw(8) << dist1 << "  " << setw(8) << dist2 << "  " << setw(5) << nVisited1 << "  " << setw(5) << nVisited2;
	}
	time_t end = (unsigned)time(NULL) ; 

	cout << "\n";
	cout << "\n------------------------------------------------------------";
	cout << "\n--------------- " << caption << " --------------";
	cout << "\n------------------------------------------------------------";
	cout << "\n Query time totally used:      " << end - start <<"s (" << tries-1 << " pairs)";
	cout << "\n Average query time per path:  " << (end - start)*1000.0 / (tries-1) / 2 << "ms";
	cout << "\n Average visited nodes:        " << nVisited / (tries-1) / 2;
	cout << "\n srand(year):                  " << year;
	cout << "\n Data file:                   '" << filename1 << "'";
	cout << "\n Nodes, Edges:                 " << n << ", " << m;
	cout << "\n------------------------------------------------------------\n\n";
///////////////////////////////////////////////////////////////
	return;
///////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////
// 	Trivial test:
///////////////////////////////////////////////////////////////

void testTrivial_classicalDJ(int n, int m, string filename1) {
	int tries, source, target, dist1, dist2, nVisited1, nVisited2;
///////////////////////////////////////////////////////////////
	string caption = "Trivial-classicalDJ PERFORMANCE TEST"; // PERFORMANCE
///////////////////////////////////////////////////////////////
	cout << endl << caption << endl;
	cout << "\n" << setw(7) << "    #  "<< setw(7) <<"  source  " << setw(7) <<"  target  " << setw(8) <<"  distance1 " << setw(8) << "  distance2 " << setw(5) <<"  nVisited1 " << setw(5) <<" nVisited2\n" ;
///////////////////////////////////////////////////////////////

    time_t start = (unsigned)time(NULL) ;  
	int year=2012;
	srand(year);
	Dijkstra_Fheap DJfh; 
	int nVisited = 0;
	for(tries = 1; tries <= 50; tries++) {
		source = unsigned((rand() * rand())) % n + 1;
		target =  unsigned((source * rand())) % n + 1;
		if(!(tries%10)) {
			cout <<"\n" << setw(5) << tries << "#  ";
			cout << setw(8) << source << "  " << setw(8) << target << "   ";
		}
		dist1 = DJfh.Trivial_classicalDJ(source, target);  
		nVisited1  = DJfh.getNVisitedNodes();

		//source = unsigned((rand() * rand())) % n + 1;
		//target =  unsigned((source * rand())) % n + 1;
		dist2 = DJfh.Trivial_classicalDJ(target, source);  
		nVisited2  = DJfh.getNVisitedNodes();
		nVisited += nVisited1 + nVisited2;
		if(!(tries%10))
		 cout << setw(8) << dist1 << "  " << setw(8) << dist2 << "  " << setw(5) << nVisited1 << "  " << setw(5) << nVisited2;
	}
	time_t end = (unsigned)time(NULL) ; 

	cout << "\n";
	cout << "\n------------------------------------------------------------";
	cout << "\n--------------- " << caption << " --------------";
	cout << "\n------------------------------------------------------------";
	cout << "\n Query time totally used:      " << end - start <<"s (" << tries-1 << " pairs)";
	cout << "\n Average query time per path:  " << (end - start)*1000.0 / (tries-1) / 2 << "ms";
	cout << "\n Average visited nodes:        " << nVisited / (tries-1) / 2;
	cout << "\n srand(year):                  " << year;
	cout << "\n Data file:                   '" << filename1 << "'";
	cout << "\n Nodes, Edges:                 " << n << ", " << m;
	cout << "\n------------------------------------------------------------\n\n";
///////////////////////////////////////////////////////////////
	return;
///////////////////////////////////////////////////////////////
}

void testTrivial_bidi_classicalDJ(int n, int m, string filename1) {
	int tries, source, target, dist1, dist2, nVisited1, nVisited2;
///////////////////////////////////////////////////////////////
	string caption = "Trivial-bidi-classicalDJ PERFORMANCE TEST"; // PERFORMANCE
///////////////////////////////////////////////////////////////
	cout << endl << caption << endl;
	cout << "\n" << setw(7) << "    #  "<< setw(7) <<"  source  " << setw(7) <<"  target  " << setw(8) <<"  distance1 " << setw(8) << "  distance2 " << setw(5) <<"  nVisited1 " << setw(5) <<" nVisited2\n" ;
///////////////////////////////////////////////////////////////

    time_t start = (unsigned)time(NULL) ;  
	int year=2012;
	srand(year);
	Dijkstra_Fheap DJfh; 
	int nVisited = 0;
	for(tries = 1; tries <= 50; tries++) {
		source = unsigned((rand() * rand())) % n + 1;
		target =  unsigned((source * rand())) % n + 1;
		if(!(tries%10)) {
			cout <<"\n" << setw(5) << tries << "#  ";
			cout << setw(8) << source << "  " << setw(8) << target << "   ";
		}
		dist1 = DJfh.Trivial_bidi_classicalDJ(source, target);  
		nVisited1  = DJfh.getNVisitedNodes();

		//source = unsigned((rand() * rand())) % n + 1;
		//target =  unsigned((source * rand())) % n + 1;
		dist2 = DJfh.Trivial_bidi_classicalDJ(target, source);  
		nVisited2  = DJfh.getNVisitedNodes();
		nVisited += nVisited1 + nVisited2;
		if(!(tries%10)) 
		cout << setw(8) << dist1 << "  " << setw(8) << dist2 << "  " << setw(5) << nVisited1 << "  " << setw(5) << nVisited2;
	}
	time_t end = (unsigned)time(NULL) ; 

	cout << "\n";
	cout << "\n------------------------------------------------------------";
	cout << "\n--------------- " << caption << " --------------";
	cout << "\n------------------------------------------------------------";
	cout << "\n Query time totally used:      " << end - start <<"s (" << tries-1 << " pairs)";
	cout << "\n Average query time per path:  " << (end - start)*1000.0 / (tries-1) / 2 << "ms";
	cout << "\n Average visited nodes:        " << nVisited / (tries-1) / 2;
	cout << "\n srand(year):                  " << year;
	cout << "\n Data file:                   '" << filename1 << "'";
	cout << "\n Nodes, Edges:                 " << n << ", " << m;
	cout << "\n------------------------------------------------------------\n\n";
///////////////////////////////////////////////////////////////
	return;
///////////////////////////////////////////////////////////////
}


void testTrivial_CHDJ(int n, int m, string filename1) {
	int tries, source, target, dist1, dist2, nVisited1, nVisited2;
///////////////////////////////////////////////////////////////
	string caption = "Trivial-CHDJ PERFORMANCE TEST"; // PERFORMANCE
///////////////////////////////////////////////////////////////
	cout << endl << caption << endl;
	cout << "\n" << setw(7) << "    #  "<< setw(7) <<"  source  " << setw(7) <<"  target  " << setw(8) <<"  distance1 " << setw(8) << "  distance2 " << setw(5) <<"  nVisited1 " << setw(5) <<" nVisited2\n" ;
///////////////////////////////////////////////////////////////

    time_t start = (unsigned)time(NULL) ;  
	int year=2012;
	srand(year);
	Dijkstra_Fheap DJfh; 
	int nVisited = 0;
	for(tries = 1; tries <= 10000; tries++) {
		source = unsigned((rand() * rand())) % n + 1;
		target =  unsigned((source * rand())) % n + 1;
		if(!(tries%1000)) {
			cout <<"\n" << setw(5) << tries << "#  ";
			cout << setw(8) << source << "  " << setw(8) << target << "   ";
		}
		dist1 = DJfh.Trivial_CHDJ(source, target);  
		nVisited1  = DJfh.getNVisitedNodes();

		//source = unsigned((rand() * rand())) % n + 1;
		//target =  unsigned((source * rand())) % n + 1;
		dist2 = DJfh.Trivial_CHDJ(target, source);  
		nVisited2  = DJfh.getNVisitedNodes();
		nVisited += nVisited1 + nVisited2;
		if(!(tries%1000)) cout << setw(8) << dist1 << "  " << setw(8) << dist2 << "  " << setw(5) << nVisited1 << "  " << setw(5) << nVisited2;
	}
	time_t end = (unsigned)time(NULL) ; 

	cout << "\n";
	cout << "\n------------------------------------------------------------";
	cout << "\n--------------- " << caption << " --------------";
	cout << "\n------------------------------------------------------------";
	cout << "\n Query time totally used:      " << end - start <<"s (" << tries-1 << " pairs)";
	cout << "\n Average query time per path:  " << (end - start)*1000.0 / (tries-1) / 2 << "ms";
	cout << "\n Average visited nodes:        " << nVisited / (tries-1) / 2;
	cout << "\n srand(year):                  " << year;
	cout << "\n Data file:                   '" << filename1 << "'";
	cout << "\n Nodes, Edges:                 " << n << ", " << m;
	cout << "\n------------------------------------------------------------\n\n";
///////////////////////////////////////////////////////////////
	return;
///////////////////////////////////////////////////////////////
}


///////////////////////////////////////////////////////////////
// Bheap test:
///////////////////////////////////////////////////////////////

void testBheap_classicalDJ(int n, int m, string filename1) {
	int tries, source, target, dist1, dist2, nVisited1, nVisited2;
///////////////////////////////////////////////////////////////
	string caption = "Bheap-classicalDJ PERFORMANCE TEST";      // CORRECTNESS
///////////////////////////////////////////////////////////////
	cout << endl << caption << endl;
	cout << "\n" << setw(7) << "    #  "<< setw(7) <<"  source  " << setw(7) <<"  target  " << setw(8) <<"  distance1 " << setw(8) << "  distance2 " << setw(5) <<"  nVisited1 " << setw(5) <<" nVisited2\n" ;
///////////////////////////////////////////////////////////////

    time_t start = (unsigned)time(NULL) ;  
	int year=2012;
	srand(year);
	int nVisited = 0;
	Dijkstra_Bheap DJbh; 
	for(tries = 1; tries <= 50; tries++) {
		source = unsigned((rand() * rand())) % n + 1;
		target =  unsigned((source * rand())) % n + 1;
		if(!(tries%10)) {
			cout <<"\n" << setw(5) << tries << "#  ";
			cout << setw(8) << source << "  " << setw(8) << target << "   ";
		}
		dist1 = DJbh.classicalDJ(source, target);  
		nVisited1  = DJbh.getNVisitedNodes();

		//source = unsigned((rand() * rand())) % n + 1;
		//target =  unsigned((source * rand())) % n + 1;
		dist2 = DJbh.classicalDJ(target, source);  
		nVisited2  = DJbh.getNVisitedNodes();
		nVisited += nVisited1 + nVisited2;
		if(!(tries%10)) cout << setw(8) << dist1 << "  " << setw(8) << dist2 << "  " << setw(5) << nVisited1 << "  " << setw(5) << nVisited2;
	}
	time_t end = (unsigned)time(NULL) ; 

	cout << "\n";
	cout << "\n------------------------------------------------------------";
	cout << "\n--------------- " << caption << " --------------";
	cout << "\n------------------------------------------------------------";
	cout << "\n Query time totally used:      " << end - start <<"s (" << tries-1 << " pairs)";
	cout << "\n Average query time per path:  " << (end - start)*1000.0 / (tries-1) / 2 << "ms";
	cout << "\n Average visited nodes:        " << nVisited / (tries-1) / 2;
	cout << "\n srand(year):                  " << year;
	cout << "\n Data file:                   '" << filename1 << "'";
	cout << "\n Nodes, Edges:                 " << n << ", " << m;
	cout << "\n------------------------------------------------------------\n\n";
///////////////////////////////////////////////////////////////
	return;
///////////////////////////////////////////////////////////////
}

void testBheap_bidi_classicalDJ(int n, int m, string filename1) {
	int tries, source, target, dist1, dist2, nVisited1, nVisited2;
///////////////////////////////////////////////////////////////
	string caption = "Bheap-bidi-classicalDJ PERFORMANCE TEST"; // PERFORMANCE
///////////////////////////////////////////////////////////////
	cout << endl << caption << endl;
	cout << "\n" << setw(7) << "    #  "<< setw(7) <<"  source  " << setw(7) <<"  target  " << setw(8) <<"  distance1 " << setw(8) << "  distance2 " << setw(5) <<"  nVisited1 " << setw(5) <<" nVisited2\n" ;
///////////////////////////////////////////////////////////////

    time_t start = (unsigned)time(NULL) ;  
	int year=2012;
	srand(year);
	Dijkstra_Bheap DJbh; 
	int nVisited = 0;
	for(tries = 1; tries <= 50; tries++) {
		source = unsigned((rand() * rand())) % n + 1;
		target =  unsigned((source * rand())) % n + 1;
		if(!(tries%10)) {
			cout <<"\n" << setw(5) << tries << "#  ";
			cout << setw(8) << source << "  " << setw(8) << target << "   ";
		}
		dist1 = DJbh.bidi_classicalDJ(source, target);  
		nVisited1  = DJbh.getNVisitedNodes();

		//source = unsigned((rand() * rand())) % n + 1;
		//target =  unsigned((source * rand())) % n + 1;
		dist2 = DJbh.bidi_classicalDJ(target, source);  
		nVisited2  = DJbh.getNVisitedNodes();
		nVisited += nVisited1 + nVisited2;
		if(!(tries%10)) cout << setw(8) << dist1 << "  " << setw(8) << dist2 << "  " << setw(5) << nVisited1 << "  " << setw(5) << nVisited2;
	}
	time_t end = (unsigned)time(NULL) ; 

	cout << "\n";
	cout << "\n------------------------------------------------------------";
	cout << "\n--------------- " << caption << " --------------";
	cout << "\n------------------------------------------------------------";
	cout << "\n Query time totally used:      " << end - start <<"s (" << tries-1 << " pairs)";
	cout << "\n Average query time per path:  " << (end - start)*1000.0 / (tries-1) / 2 << "ms";
	cout << "\n Average visited nodes:        " << nVisited / (tries-1) / 2;
	cout << "\n srand(year):                  " << year;
	cout << "\n Data file:                   '" << filename1 << "'";
	cout << "\n Nodes, Edges:                 " << n << ", " << m;
	cout << "\n------------------------------------------------------------\n\n";
///////////////////////////////////////////////////////////////
	return;
///////////////////////////////////////////////////////////////
}


void testBheap_CHDJ(int n, int m, string filename1) {
	int tries, source, target, dist1, dist2, nVisited1, nVisited2;
///////////////////////////////////////////////////////////////
	string caption = "Bheap-CHDJ PERFORMANCE TEST"; // PERFORMANCE
///////////////////////////////////////////////////////////////
	cout << endl << caption << endl;
	cout << "\n" << setw(7) << "    #  "<< setw(7) <<"  source  " << setw(7) <<"  target  " << setw(8) <<"  distance1 " << setw(8) << "  distance2 " << setw(5) <<"  nVisited1 " << setw(5) <<" nVisited2\n" ;
///////////////////////////////////////////////////////////////

    time_t start = (unsigned)time(NULL) ;  
	int year=2012;
	srand(year);
	Dijkstra_Bheap DJbh; 
	int nVisited = 0;
	for(tries = 1; tries <= 10000; tries++) {
		source = unsigned((rand() * rand())) % n + 1;
		target =  unsigned((source * rand())) % n + 1;
		if(!(tries%1000)) {
			cout <<"\n" << setw(5) << tries << "#  ";
			cout << setw(8) << source << "  " << setw(8) << target << "   ";
		}
		dist1 = DJbh.ContractionHierarchiesDJ(source, target);  
		nVisited1  = DJbh.getNVisitedNodes();

		//source = unsigned((rand() * rand())) % n + 1;
		//target =  unsigned((source * rand())) % n + 1;
		dist2 = DJbh.ContractionHierarchiesDJ(target, source);  
		nVisited2  = DJbh.getNVisitedNodes();
		nVisited += nVisited1 + nVisited2;
		if(!(tries%1000)) cout << setw(8) << dist1 << "  " << setw(8) << dist2 << "  " << setw(5) << nVisited1 << "  " << setw(5) << nVisited2;
	}
	time_t end = (unsigned)time(NULL) ; 

	cout << "\n";
	cout << "\n------------------------------------------------------------";
	cout << "\n--------------- " << caption << " --------------";
	cout << "\n------------------------------------------------------------";
	cout << "\n Query time totally used:      " << end - start <<"s (" << tries-1 << " pairs)";
	cout << "\n Average query time per path:  " << (end - start)*1000.0 / (tries-1) / 2 << "ms";
	cout << "\n Average visited nodes:        " << nVisited / (tries-1) / 2;
	cout << "\n srand(year):                  " << year;
	cout << "\n Data file:                   '" << filename1 << "'";
	cout << "\n Nodes, Edges:                 " << n << ", " << m;
	cout << "\n------------------------------------------------------------\n\n";
///////////////////////////////////////////////////////////////
	return;
///////////////////////////////////////////////////////////////
}

/**
 * Test the performance of classical DJ, bi-directional DJ, and CH DJ. 
 * All the 3 versions are based on Fibonacci heap.
 * The output includes distance, # of visited nodes, and total time used (in main()).
 * The distances and # of visited nodes for searching two paths s-->t and t-->s should be identical.  ---- No, not always correct.
 * eg. this is a snapshot: (distances are only shown one of them) ---- data: 'USA-road-d.E.gr.preprocessed', srand(2014) on Ubuntu 13.04
	1872#  2577625  3539991   3902541   3510     3510
	1873#  1557398   309602   4025410   2680     2680
	1874#  2408949  2246421   4798164   2930     2930
	1875#  2182815  1131948   3739083   3829     3829
	1876#  1094964  1500533   9467128   4737     4737
	1877#  1447192  2309554   10884679   4833     4833
	1878#  2282444   373925   14231036   3793     3793
	1879#  3404038  2535176   2749862   3612     3612
	1880#  3595851  1862259   4797972   3580     3580
	1881#  2368395   249125   3163014   4014     4014
 */
void testPerformanceDJ(int n, int m, string filename1){
	testBheap_CHDJ(n, m, filename1);
	testFheap_CHDJ(n, m, filename1);
	testTrivial_CHDJ(n, m, filename1);	
	
	testBheap_classicalDJ(n, m, filename1);
	testBheap_bidi_classicalDJ(n, m, filename1);
	testFheap_classicalDJ(n, m, filename1);
	testFheap_bidi_classicalDJ(n, m, filename1);
	testTrivial_classicalDJ(n, m, filename1);
	testTrivial_bidi_classicalDJ(n, m, filename1);
}

int main(int args, char *argv[]){   
	
	string filename1;
	if(args>1) filename1 = argv[1];  
	else {
		if(PREPROCESSING) filename1 = "USA-road-d.NY.gr";
		else filename1 = "USA-road-d.NY.gr.preprocessed";
	}
	ReadRoad graph(filename1);  // read road data file, with the result stored in the vector node
	unsigned n = graph.getNodes();
	if(PREPROCESSING) {
		CH ch;
		ch.preprocessing(&graph);
		ch.writeRoad(&graph);
	}

	testPerformanceDJ(n, graph.getEdges(), filename1);

	return 0;
}
 
