package CH_Java;

import java.io.FileNotFoundException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Random;
import java.util.Vector;

public class CH_Java {
	private static ArrayList<Node> node;	
	CH_Java() {  } //CH_Java(ArrayList<Node> node0) { node = node0; } 
	
	private static void init(ArrayList<Node> node0){
		node = new ArrayList<Node>();
		node = node0;
	}
	
	/**
	 * Verify the correctness of classical DJ, bi-directional classicalDJ, and CH DJ. 
	 * All the 3 versions are based on Fibonacci heap.
	 */ 
	static int testCorrectnessDJ(int source, int target,  int no) { 
		Dijkstra_Fheap DJfh = new Dijkstra_Fheap(node);
		int dist1, dist2, dist3, dist4;  
	/*
	dist4 = DJfh.TrivialDJ(1, 5); 
	System.out.print( "\n---------------------------------------------------------------\n");
	System.out.print( "No. " + no +"  dist(" + source + ", " + target +"): " + dist4 + "\n");  
	System.out.print( "---------------------------------------------------------------\n");
	return 0;
	*/
	
		System.out.print("\n" + no + "#  ");
		System.out.print( source + "  " + target + "   ");
	
		dist1 = DJfh.ContractionHierarchiesDJ(source, target);  
		System.out.print(  "  " + dist1);
	
		dist2 = DJfh.ContractionHierarchiesDJ(target, source);  
		System.out.print( "  " + dist2);
	/*
		dist3 = DJfh.bidi_classicalDJ(source, target);  
		System.out.print( "  " +  dist3;
	
		dist4 = DJfh.bidi_classicalDJ(target, source);  
		System.out.print( "  " +  dist4;
	
	*/	
	/*	
	   dist3 = DJfh.Trivial_CHDJ(source, target);  
		System.out.print( "  " +  dist3;
	
		dist4 = DJfh.Trivial_CHDJ(target, source);  
		System.out.print( "  "  +  dist4;
	
	
		dist3 = DJfh.classicalDJ(source, target);  
		System.out.print( "  " +  dist3;
	
		dist4 = DJfh.classicalDJ(target, source);  
		System.out.print( "  " +  dist4;
	
		dist3 = DJfh.Trivial_bidi_classicalDJ(source, target);  
		System.out.print( "  " +  dist3;
	
		dist4 = DJfh.Trivial_bidi_classicalDJ(target, source);  
		System.out.print( "  "  +  dist4;
	*/
		Dijkstra_Bheap DJbh = new Dijkstra_Bheap(node); 
	/*
		dist3 = DJbh.classicalDJ(source, target);  
		System.out.print( "  " +  dist3;
	
		dist4 = DJbh.classicalDJ(target, source);  
		System.out.print( "  " +  dist4;
	
		dist3 = DJbh.bidi_classicalDJ(source, target);  
		System.out.print( "  " +  dist3;
	
		dist4 = DJbh.bidi_classicalDJ(target, source);  
		System.out.print( "  " +  dist4;
	*/
		dist3 = DJbh.ContractionHierarchiesDJ(source, target);  
		System.out.print( "  " + dist3);
	
		dist4 = DJbh.ContractionHierarchiesDJ(target, source);  
		System.out.print( "  " + dist4);
	
		if(!(dist1==dist2 && dist2 == dist3 && dist3 == dist4)) {
			System.out.print( "\nCAUTION: d(" + source + " " + target +"): " + dist1 + " " + dist2 + " " + dist3 + " " + dist4 + "\n");
			//exit(0);
		}
		return dist1;
	}	
	 
	// -------------------------------------PERFORMANCE TEST-------------------------------------
	// ------------------------------------------------------------------------------------------------------
	static void testFheap_classicalDJ(int n, int m, String filename1) {
		int tries, source, target, dist1, dist2, nVisited1, nVisited2;
	///////////////////////////////////////////////////////////////
		String caption = "Fheap-classicalDJ PERFORMANCE TEST";      // CORRECTNESS
	///////////////////////////////////////////////////////////////
		System.out.print( "\n" + caption + "\n");
		System.out.print( "\n"  + "    #  " +"  source  "  +"  target  " + "  distance1 " +  "  distance2 " + "  nVisited1 " + " nVisited2\n");
	///////////////////////////////////////////////////////////////
	
	    long start = System.currentTimeMillis() ;  
	    int year=2012;
		Random rnd = new Random(year); 
		int nVisited = 0;
		Dijkstra_Fheap DJfh = new Dijkstra_Fheap(node);;
		for(tries = 1; tries <= 500; tries++) {
			source = rnd.nextInt(n)+1;//tries;      //rnd.nextInt(n)+1;
			target = rnd.nextInt(n)+1;//tries*1000 % n + 1;//rnd.nextInt(n)+1;
			if(0==(tries%10)) {
				System.out.print("\n" +  tries + "#  ");
				System.out.print(  source + "  " +  target + "   ");
			} 
			dist1 = DJfh.classicalDJ(source, target);  
			nVisited1  = DJfh.getNVisitedNodes();
			dist2 = DJfh.classicalDJ(target, source);  
			nVisited2  = DJfh.getNVisitedNodes();
			nVisited += nVisited1 + nVisited2;
			if(0==(tries%10)) System.out.print(  dist1 + "  " +  dist2 + "  " +  nVisited1 + "  " +  nVisited2);
		}
		long end = System.currentTimeMillis() ; 
	
		System.out.print( "\n");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n--------------- " + caption + " --------------");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n Query time totally used:      " + (end - start) / 1000.0 + "s (" + (tries-1) + " pairs)");
		System.out.print( "\n Average query time per path:  " + (end - start) / (tries-1) / 2 + "ms");
		System.out.print( "\n Average visited nodes:        " + nVisited / (tries-1) / 2);
		System.out.print( "\n srand(year):                  " + year);
		System.out.print( "\n Data file:                   '" + filename1 + "'");
		System.out.print( "\n Nodes, Edges:                 " + n + ", " + m);
		System.out.print( "\n------------------------------------------------------------\n\n");
	///////////////////////////////////////////////////////////////
		return;
	///////////////////////////////////////////////////////////////
	}
	
	static void testFheap_bidi_classicalDJ(int n, int m, String filename1) {
		int tries, source, target, dist1, dist2, nVisited1, nVisited2;
	///////////////////////////////////////////////////////////////
		String caption = "Fheap-bidi-classicalDJ PERFORMANCE TEST"; // PERFORMANCE
		//String caption = "BinaryHeap-CHDJ PERFORMANCE TEST");
	///////////////////////////////////////////////////////////////
		System.out.print( "\n" + caption + "\n");
		System.out.print( "\n"  + "    #  " +"  source  "  +"  target  " + "  distance1 " +  "  distance2 " + "  nVisited1 " + " nVisited2\n");
	///////////////////////////////////////////////////////////////
	
	    long start = System.currentTimeMillis() ;  
	    int year=2012;
		Random rnd = new Random(year); 
		Dijkstra_Fheap DJfh = new Dijkstra_Fheap(node);
		int nVisited = 0;
		for(tries = 1; tries <= 500; tries++) {
			source = rnd.nextInt(n)+1;//tries;      //rnd.nextInt(n)+1;
			target = rnd.nextInt(n)+1;//tries*1000 % n + 1;//rnd.nextInt(n)+1;
			if(0==(tries%10)) {
				System.out.print("\n" +  tries + "#  ");
				System.out.print(  source + "  " +  target + "   ");
			}
			dist1 = DJfh.bidi_classicalDJ(source, target);  
			nVisited1  = DJfh.getNVisitedNodes();
			dist2 = DJfh.bidi_classicalDJ(target, source);  
			nVisited2  = DJfh.getNVisitedNodes();
			nVisited += nVisited1 + nVisited2;
			if(0==(tries%10)) System.out.print(  dist1 + "  " +  dist2 + "  " +  nVisited1 + "  " +  nVisited2);
		}
		long end = System.currentTimeMillis(); 
	
		System.out.print( "\n");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n--------------- " + caption + " --------------");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n Query time totally used:      " + (end - start) /1000.0 + "s (" + (tries-1) + " pairs)");
		System.out.print( "\n Average query time per path:  " + (end - start) / (tries-1) / 2 + "ms");
		System.out.print( "\n Average visited nodes:        " + nVisited / (tries-1) / 2);
		System.out.print( "\n srand(year):                  " + year);
		System.out.print( "\n Data file:                   '" + filename1 + "'");
		System.out.print( "\n Nodes, Edges:                 " + n + ", " + m);
		System.out.print( "\n------------------------------------------------------------\n\n");
	///////////////////////////////////////////////////////////////
		return;
	///////////////////////////////////////////////////////////////
	}
	
	
	static void testFheap_CHDJ(int n, int m, String filename1) {
		int tries, source, target, dist1, dist2, nVisited1, nVisited2;
	///////////////////////////////////////////////////////////////
		String caption = "Fheap-CHDJ PERFORMANCE TEST"; // PERFORMANCE
		//String caption = "BinaryHeap-CHDJ PERFORMANCE TEST");
	///////////////////////////////////////////////////////////////
		System.out.print( "\n" + caption + "\n");
		System.out.print( "\n"  + "    #  " +"  source  "  +"  target  " + "  distance1 " +  "  distance2 " + "  nVisited1 " + " nVisited2\n");
	///////////////////////////////////////////////////////////////
	
	    long start = System.currentTimeMillis() ;  
		int year=2012;
		Random rnd = new Random(year); 
		Dijkstra_Fheap DJfh = new Dijkstra_Fheap(node); ;
		int nVisited = 0;
		for(tries = 1; tries <= 10000; tries++) {
			source = rnd.nextInt(n)+1;//tries;      //rnd.nextInt(n)+1;
			target = rnd.nextInt(n)+1;//tries*1000 % n + 1;//rnd.nextInt(n)+1;
			if(0==(tries%1000)) {
				System.out.print("\n" +  tries + "#  ");
				System.out.print(  source + "  " +  target + "   ");
			}
			dist1 = DJfh.ContractionHierarchiesDJ(source, target);  
			nVisited1  = DJfh.getNVisitedNodes();
			dist2 = DJfh.ContractionHierarchiesDJ(target, source);  
			nVisited2  = DJfh.getNVisitedNodes();
			nVisited += nVisited1 + nVisited2;
			if(0==(tries%1000)) 
				System.out.print(dist1 + "  " +  dist2 + "  " +  nVisited1 + "  " +  nVisited2);
		}
		long end = System.currentTimeMillis() ; 
	
		System.out.print( "\n");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n--------------- " + caption + " --------------");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n Query time totally used:      " + (end - start) / 1000.0 + "s (" + (tries-1) + " pairs)");
		System.out.print( "\n Average query time per path:  " + (end - start) / (tries-1) / 2 + "ms");
		System.out.print( "\n Average visited nodes:        " + nVisited / (tries-1) / 2);
		System.out.print( "\n srand(year):                  " + year);
		System.out.print( "\n Data file:                   '" + filename1 + "'");
		System.out.print( "\n Nodes, Edges:                 " + n + ", " + m);
		System.out.print( "\n------------------------------------------------------------\n\n");
	///////////////////////////////////////////////////////////////
		return;
	///////////////////////////////////////////////////////////////
	}
	
	///////////////////////////////////////////////////////////////
	// 	Trivial:
	///////////////////////////////////////////////////////////////
	static void testTrivial_classicalDJ(int n, int m, String filename1) {
		int tries, source, target, dist1, dist2, nVisited1, nVisited2;
	///////////////////////////////////////////////////////////////
		String caption = "Trivial-classicalDJ PERFORMANCE TEST"; // PERFORMANCE
	///////////////////////////////////////////////////////////////
		System.out.print( "\n" + caption + "\n");
		System.out.print( "\n"  + "    #  " +"  source  "  +"  target  " + "  distance1 " +  "  distance2 " + "  nVisited1 " + " nVisited2\n");
	///////////////////////////////////////////////////////////////
	
	    long start = System.currentTimeMillis() ;  
	    int year=2012;
		Random rnd = new Random(year); 
		Dijkstra_Fheap DJfh = new Dijkstra_Fheap(node);  
		int nVisited = 0;
		for(tries = 1; tries <= 500; tries++) {
			source = rnd.nextInt(n)+1;//tries;      //rnd.nextInt(n)+1;
			target = rnd.nextInt(n)+1;//tries*1000 % n + 1;//rnd.nextInt(n)+1;
			if(0==(tries%10)) {
				System.out.print("\n" +  tries + "#  ");
				System.out.print(  source + "  " +  target + "   ");
			}
			dist1 = DJfh.Trivial_classicalDJ(source, target);  
			nVisited1  = DJfh.getNVisitedNodes();
			dist2 = DJfh.Trivial_classicalDJ(target, source);  
			nVisited2  = DJfh.getNVisitedNodes();
			nVisited += nVisited1 + nVisited2;
			if(0==(tries%10))
			 System.out.print(  dist1 + "  " +  dist2 + "  " +  nVisited1 + "  " +  nVisited2);
		}
		long end = System.currentTimeMillis() ; 
	
		System.out.print( "\n");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n--------------- " + caption + " --------------");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n Query time totally used:      " + (end - start) / 1000.0 + "s (" + (tries-1) + " pairs)");
		System.out.print( "\n Average query time per path:  " + (end - start) / (tries-1) / 2 + "ms");
		System.out.print( "\n Average visited nodes:        " + nVisited / (tries-1) / 2);
		System.out.print( "\n srand(year):                  " + year);
		System.out.print( "\n Data file:                   '" + filename1 + "'");
		System.out.print( "\n Nodes, Edges:                 " + n + ", " + m);
		System.out.print( "\n------------------------------------------------------------\n\n");
	///////////////////////////////////////////////////////////////
		return;
	///////////////////////////////////////////////////////////////
	}
	
	static void testTrivial_bidi_classicalDJ(int n, int m, String filename1) {
		int tries, source, target, dist1, dist2, nVisited1, nVisited2;
	///////////////////////////////////////////////////////////////
		String caption = "Trivial-bidi-classicalDJ PERFORMANCE TEST"; // PERFORMANCE
	///////////////////////////////////////////////////////////////
		System.out.print( "\n" + caption + "\n");
		System.out.print( "\n"  + "    #  " +"  source  "  +"  target  " + "  distance1 " +  "  distance2 " + "  nVisited1 " + " nVisited2\n");
	///////////////////////////////////////////////////////////////
	
	    long start = System.currentTimeMillis() ;  
	    int year=2012;
		Random rnd = new Random(year); 
		Dijkstra_Fheap DJfh = new Dijkstra_Fheap(node);  
		int nVisited = 0;
		for(tries = 1; tries <= 500; tries++) {
			source = rnd.nextInt(n)+1;//tries;      //rnd.nextInt(n)+1;
			target = rnd.nextInt(n)+1;//tries*1000 % n + 1;//rnd.nextInt(n)+1;
			if(0==(tries%10)) {
				System.out.print("\n" +  tries + "#  ");
				System.out.print(  source + "  " +  target + "   ");
			}
			dist1 = DJfh.Trivial_bidi_classicalDJ(source, target);  
			nVisited1  = DJfh.getNVisitedNodes();
			dist2 = DJfh.Trivial_bidi_classicalDJ(target, source);  
			nVisited2  = DJfh.getNVisitedNodes();
			nVisited += nVisited1 + nVisited2;
			if(0==(tries%10)) 
			System.out.print(  dist1 + "  " +  dist2 + "  " +  nVisited1 + "  " +  nVisited2);
		}
		long end = System.currentTimeMillis() ; 
	
		System.out.print( "\n");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n--------------- " + caption + " --------------");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n Query time totally used:      " + (end - start) / 1000.0 + "s (" + (tries-1) + " pairs)");
		System.out.print( "\n Average query time per path:  " + (end - start) / (tries-1) / 2 + "ms");
		System.out.print( "\n Average visited nodes:        " + nVisited / (tries-1) / 2);
		System.out.print( "\n srand(year):                  " + year);
		System.out.print( "\n Data file:                   '" + filename1 + "'");
		System.out.print( "\n Nodes, Edges:                 " + n + ", " + m);
		System.out.print( "\n------------------------------------------------------------\n\n");
	///////////////////////////////////////////////////////////////
		return;
	///////////////////////////////////////////////////////////////
	}
	
	
	static void testTrivial_CHDJ(int n, int m, String filename1) {
		int tries, source, target, dist1, dist2, nVisited1, nVisited2;
	///////////////////////////////////////////////////////////////
		String caption = "Trivial-CHDJ PERFORMANCE TEST"; // PERFORMANCE
	///////////////////////////////////////////////////////////////
		System.out.print( "\n" + caption + "\n");
		System.out.print( "\n"  + "    #  " +"  source  "  +"  target  " + "  distance1 " +  "  distance2 " + "  nVisited1 " + " nVisited2\n");
	///////////////////////////////////////////////////////////////
	
	    long start = System.currentTimeMillis() ;  
		int year=2012;
		Random rnd = new Random(year); 
		Dijkstra_Fheap DJfh = new Dijkstra_Fheap(node);  
		int nVisited = 0;
		for(tries = 1; tries <= 10000; tries++) {
			source = rnd.nextInt(n)+1;//tries;      //rnd.nextInt(n)+1;
			target = rnd.nextInt(n)+1;//tries*1000 % n + 1;//rnd.nextInt(n)+1;
			if(0==(tries%1000)) {
				System.out.print("\n" +  tries + "#  ");
				System.out.print(  source + "  " +  target + "   ");
			}
			dist1 = DJfh.Trivial_CHDJ(source, target);  
			nVisited1  = DJfh.getNVisitedNodes();
			dist2 = DJfh.Trivial_CHDJ(target, source);  
			nVisited2  = DJfh.getNVisitedNodes();
			nVisited += nVisited1 + nVisited2;
			if(0==(tries%1000)) System.out.print(dist1 + "  " +  dist2 + "  " +  nVisited1 + "  " +  nVisited2);
		}
		long end = System.currentTimeMillis() ; 
	
		System.out.print( "\n");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n--------------- " + caption + " --------------");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n Query time totally used:      " + (end - start) / 1000.0 + "s (" + (tries-1) + " pairs)");
		System.out.print( "\n Average query time per path:  " + (end - start) / (tries-1) / 2 + "ms");
		System.out.print( "\n Average visited nodes:        " + nVisited / (tries-1) / 2);
		System.out.print( "\n srand(year):                  " + year);
		System.out.print( "\n Data file:                   '" + filename1 + "'");
		System.out.print( "\n Nodes, Edges:                 " + n + ", " + m);
		System.out.print( "\n------------------------------------------------------------\n\n");
	///////////////////////////////////////////////////////////////
		return;
	///////////////////////////////////////////////////////////////
	}
	
	
	///////////////////////////////////////////////////////////////
	// Bheap:
	///////////////////////////////////////////////////////////////
	
	
	static void testBheap_classicalDJ(int n, int m, String filename1) {
		int tries, source, target, dist1, dist2, nVisited1, nVisited2;
	///////////////////////////////////////////////////////////////
		String caption = "Bheap-classicalDJ PERFORMANCE TEST";      // CORRECTNESS
	///////////////////////////////////////////////////////////////
		System.out.print( "\n" + caption + "\n");
		System.out.print( "\n"  + "    #  " +"  source  "  +"  target  " + "  distance1 " +  "  distance2 " + "  nVisited1 " + " nVisited2\n");
	///////////////////////////////////////////////////////////////
	
	    long start = System.currentTimeMillis() ;  
	    int year=2012;
		Random rnd = new Random(year); 
		int nVisited = 0;
		Dijkstra_Bheap DJbh = new Dijkstra_Bheap(node); 
		for(tries = 1; tries <= 500; tries++) {
			source = rnd.nextInt(n)+1;//tries;      //rnd.nextInt(n)+1;
			target = rnd.nextInt(n)+1;//tries*1000 % n + 1;//rnd.nextInt(n)+1;
			if(0==(tries%10)) {
				System.out.print("\n" +  tries + "#  ");
				System.out.print(  source + "  " +  target + "   ");
			}
			dist1 = DJbh.classicalDJ(source, target);  
			nVisited1  = DJbh.getNVisitedNodes();
			dist2 = DJbh.classicalDJ(target, source);  
			nVisited2  = DJbh.getNVisitedNodes();
			nVisited += nVisited1 + nVisited2;
			if(0==(tries%10)) System.out.print(  dist1 + "  " +  dist2 + "  " +  nVisited1 + "  " +  nVisited2);
		}
		long end = System.currentTimeMillis() ; 
	
		System.out.print( "\n");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n--------------- " + caption + " --------------");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n Query time totally used:      " + (end - start) / 1000.0 + "s (" + (tries-1) + " pairs)");
		System.out.print( "\n Average query time per path:  " + (end - start) / (tries-1) / 2 + "ms");
		System.out.print( "\n Average visited nodes:        " + nVisited / (tries-1) / 2);
		System.out.print( "\n srand(year):                  " + year);
		System.out.print( "\n Data file:                   '" + filename1 + "'");
		System.out.print( "\n Nodes, Edges:                 " + n + ", " + m);
		System.out.print( "\n------------------------------------------------------------\n\n");
	///////////////////////////////////////////////////////////////
		return;
	///////////////////////////////////////////////////////////////
	}
	
	static void testBheap_bidi_classicalDJ(int n, int m, String filename1) {
		int tries, source, target, dist1, dist2, nVisited1, nVisited2;
	///////////////////////////////////////////////////////////////
		String caption = "Bheap-bidi-classicalDJ PERFORMANCE TEST"; // PERFORMANCE
	///////////////////////////////////////////////////////////////
		System.out.print( "\n" + caption + "\n");
		System.out.print( "\n"  + "    #  " +"  source  "  +"  target  " + "  distance1 " +  "  distance2 " + "  nVisited1 " + " nVisited2\n");
	///////////////////////////////////////////////////////////////
	
	    long start = System.currentTimeMillis() ;  
	    int year=2012;
		Random rnd = new Random(year); 
		Dijkstra_Bheap DJbh = new Dijkstra_Bheap(node); 
		int nVisited = 0;
		for(tries = 1; tries <= 500; tries++) {
			source = rnd.nextInt(n)+1;//tries;      //rnd.nextInt(n)+1;
			target = rnd.nextInt(n)+1;//tries*1000 % n + 1;//rnd.nextInt(n)+1;
			if(0==(tries%10)) {
				System.out.print("\n" +  tries + "#  ");
				System.out.print(  source + "  " +  target + "   ");
			}
			dist1 = DJbh.bidi_classicalDJ(source, target);  
			nVisited1  = DJbh.getNVisitedNodes();
			dist2 = DJbh.bidi_classicalDJ(target, source);  
			nVisited2  = DJbh.getNVisitedNodes();
			nVisited += nVisited1 + nVisited2;
			if(0==(tries%10)) System.out.print(  dist1 + "  " +  dist2 + "  " +  nVisited1 + "  " +  nVisited2);
		}
		long end = System.currentTimeMillis() ; 

		System.out.print( "\n");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n--------------- " + caption + " --------------");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n Query time totally used:      " + (end - start) / 1000.0 +"s (" + (tries-1) + " pairs)");
		System.out.print( "\n Average query time per path:  " + (end - start) / (tries-1) / 2 + "ms");
		System.out.print( "\n Average visited nodes:        " + nVisited / (tries-1) / 2);
		System.out.print( "\n srand(year):                  " + year);
		System.out.print( "\n Data file:                   '" + filename1 + "'");
		System.out.print( "\n Nodes, Edges:                 " + n + ", " + m);
		System.out.print( "\n------------------------------------------------------------\n\n");
	///////////////////////////////////////////////////////////////
		return;
	///////////////////////////////////////////////////////////////
	}
	
	
	static void testBheap_CHDJ(int n, int m, String filename1) {
		int tries, source, target, dist1, dist2, nVisited1, nVisited2;
	///////////////////////////////////////////////////////////////
		String caption = "Bheap-CHDJ PERFORMANCE TEST"; // PERFORMANCE
	///////////////////////////////////////////////////////////////
		System.out.print( "\n" + caption + "\n");
		System.out.print( "\n"  + "    #  " +"  source  "  +"  target  " + "  distance1 " +  "  distance2 " + "  nVisited1 " + " nVisited2\n");
	///////////////////////////////////////////////////////////////
	
	    long start = System.currentTimeMillis() ;  
		int year=2012;
		Random rnd = new Random(year); 
		Dijkstra_Bheap DJbh = new Dijkstra_Bheap(node); 
		int nVisited = 0;
		for(tries = 1; tries <= 10000; tries++) {
			source = rnd.nextInt(n)+1;//tries;      //rnd.nextInt(n)+1;
			target = rnd.nextInt(n)+1;//tries*1000 % n + 1;//rnd.nextInt(n)+1;
			if(0==(tries%1000)) {
				System.out.print("\n" +  tries + "#  ");
				System.out.print(  source + "  " +  target + "   ");
			}
			dist1 = DJbh.ContractionHierarchiesDJ(source, target);  
			nVisited1  = DJbh.getNVisitedNodes();	
			dist2 = DJbh.ContractionHierarchiesDJ(target, source);  
			nVisited2  = DJbh.getNVisitedNodes();
			nVisited += nVisited1 + nVisited2;
			if(0==(tries%1000)) System.out.print(dist1 + "  " +  dist2 + "  " +  nVisited1 + "  " +  nVisited2);
		}
		long end = System.currentTimeMillis(); 
	
		System.out.print( "\n");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n--------------- " + caption + " --------------");
		System.out.print( "\n------------------------------------------------------------");
		System.out.print( "\n Query time totally used:      " + (end - start) / 1000.0 +"s (" + (tries-1) + " pairs)");
		System.out.print( "\n Average query time per path:  " + (end - start) / (tries-1) / 2 + "ms");
		System.out.print( "\n Average visited nodes:        " + nVisited / (tries-1) / 2);
		System.out.print( "\n srand(year):                  " + year);
		System.out.print( "\n Data file:                   '" + filename1 + "'");
		System.out.print( "\n Nodes, Edges:                 " + n + ", " + m);
		System.out.print( "\n------------------------------------------------------------\n\n");
	///////////////////////////////////////////////////////////////
		//System.exit(0);
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
	
	static void testPerformanceDJ(int n, int m, String filename1){	
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

	public static void main(String[] args){ 		
		ArrayList<Node> node0 = new ArrayList<Node>(); // the nodes of graph G (road map)
		//ArrayList<ODID> odid = new ArrayList<ODID>(); // the orderings of nodes after preprocessing
		
		String filename1;
		if(Metadata.PREPROCESSING == 1) filename1 = "USA-road-d.NY.gr";
		else filename1 = "USA-road-d.NY.gr.preprocJava";

		ReadRoad graph=null;
		try {
			graph = new ReadRoad(filename1, node0); // read data file into node
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} // read road data file, with the result stored in the vector node
		System.out.print("Totally there are "+ graph.getNodes()+ " nodes.\n");
		
/*	    
		System.out.print("\n ---------------- Binary Heap Performance TEST ----------------- \n");
		BinaryHeap bhp = new BinaryHeap(node0);
		long start = System.currentTimeMillis(); 
		for(int i=110001; i<=120000; i++) {
			bhp.testPerformance(i);
			if(i % 10000 == 0) { 
				System.out.print("\n"+i+"#");
				long end = System.currentTimeMillis();
				System.out.print("\ntime used: " + (end - start)/1000.0 +"s");
			}
		}
		//long end = System.currentTimeMillis(); 
		//System.out.print("\ntime used: " + (end - start)/1000.0 +"s");
		System.out.print("\n ----------- Binary Heap Performance TEST Finished -------------- \n");
*/		
		
	    
		if(Metadata.PREPROCESSING == 1) {
			CH ch = new CH(node0, graph);
			ch.preprocessing();
			try {
				ch.writeRoad();
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (UnsupportedEncodingException e) {
				e.printStackTrace();
			}
		}
		
		init(node0);
		testPerformanceDJ(graph.getNodes(), graph.getEdges(), filename1);
	}
}
