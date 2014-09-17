package CH_Java;

public class Metadata {
	
	// node states:
	public static final int NON_EXIST = 0;
	public static final int CONTRACTED = 1; 
	public static final int SETTLED=3;
	public static final int EXIST=127;
	public static final int INFINITE = Integer.MAX_VALUE;
	
	// heap size:
	public static final int FiboArraySize = 20; // for array used by consolidate(): ArraySize ~ arraySize = ((unsigned) floor(log(nNodes) * oneOverLogPhi)) + 1; // = 20*2 + 1 = 41, not neccessarily big // 16 is bigger enough for 'USA-road-d.USA.gr'; 20 is quite safe. Proposed by Deqi Li, Dec 24, 2013.
	public static final int SIZE_W = 300;       // W.size() for edgeDiffbyMultiDest; over this size setInitialOrder() is triggered to avoid too long time on node ordering (contration); 150 is bigger enough for 'USA-road-d.USA.gr'; 300 is quite safe.
	
	// # of nodes in data file: 
	public static final int NODE_NUM=23950000; 

	// data file preprocessing or path finding:
	public static final int PREPROCESSING=1;  // do we need preprocess datafile at first?
}
