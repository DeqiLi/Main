package CH_Java;

class MultiDestination {  // for CH.edgeDiffbyMultiDest()
	public int ID;
	public int distDJ;   // dist computed by DJ from v to w
	public int dist1Hop; // weight(v,w); could be INFINITE
	public int length;   // weight(v,u) + weight(u,w)
}