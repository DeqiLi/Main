package CH_Java;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

class ReadRoad {  // read road data from file
	private	int n, m;  // the numbers of nodes and edges, equally |V| and |E|, of a graph G = (V, E, W), where W is weights of edges
	private	String filename;  // write preprocessed data into this file

	public int getNodes() { return n; }
	public int getEdges()  { return m; }
	public String getFilename () { return filename; }
	
	// re-initialized states of each node
	public void clearNodeState(ArrayList<Node>node) {  // after DJ, the nodes' state has been changed; so before next DJ, they should be re-initialized.
		for(int i = 1;  i<=n; i++ ) {
			if(node.get(i).state == Metadata.NON_EXIST) continue;
			node.get(i).state = Metadata.EXIST;
			node.get(i).dist[0] = Metadata.INFINITE; 
			node.get(i).dist[1] = Metadata.INFINITE;
		}
	}

	// read node information from the road data file filename1, 
	// storing them in vector<Node>node
	// in each node[i], the index i is just the ID of the node,
	// i.e.,  i == ID, ranging from 1 to n.
	ReadRoad(String filename1, ArrayList<Node> node) throws FileNotFoundException {  
		String line = null;  // a line from data file
		int ptr; //positions of ID or weight of a node in line
		//int v, u, weight;  // node IDs and weight on an arc (v,u)
		int v[] = new int[2];
		int weight;  // node IDs and weight on an arc (format in data file:  'a'  v[0]   v[1]  weight)
		//adList p;
		//adList head;
		int flag = 1;

		filename = filename1;
		
		n=0;
		m=0;

		//node.size(Metadata.NODE_NUM);
	  	System.out.print( "Begin read data from '" + filename1 + "'...\n");
	  	
	  	long startTime = System.currentTimeMillis(); //long startTime = System.nanoTime();

	  	int node_num = 0;
		BufferedReader roaddata = new BufferedReader(new FileReader(filename1));
		try {
			while ((line = roaddata.readLine()) != null) {
			   //System.out.println(line);
			   if(line.length() < 5) {
				   if(line.charAt(0)=='o') break;
				   else continue;  // some kind of line like "", " ", etc., is considered as the last line of the data file. "a 1 2 3" has the shortest length of a line
			   }
			   if(line.charAt(0)!='a') {
				   if(line.charAt(0)=='p') {
					   node_num = extractInt(line); //Integer.parseInt(line);
					   if(node_num == 0) {
						   System.out.print("There is 0 node in data file. Check line 5.\n");
						   break;
					   }
					   
					   // form node arraylist:
					   System.out.print("There is " + node_num + " node in data file. \n");
					   for(int i=0; i<= node_num; i++){
							Node nd = new Node();
							//System.out.print("\ni="+i+" "+ node.toString()+"\n");
							node.add(nd);
					   }
				   }
				   continue;
			   }
			   		   
			   ptr = 2;
			   v[0] = line.charAt(ptr) - 48;  // stringstream ss is too slow for parsing the string to digits(500 edges/s), so we have to use converting chars to digits
			   while(line.charAt(++ptr)!=' ') v[0] = v[0] * 10 + line.charAt(ptr) - 48;  // 48 is '0'
			   v[1] = line.charAt(++ptr) - 48;
			   while(line.charAt(++ptr)!=' ') v[1] = v[1] * 10 + line.charAt(ptr) - 48;
			   
			   if(v[0] >= v[1]) { m++; continue; }  // skip the symmetric arc of the last one, because they have the identical information

			   weight = line.charAt(++ptr) - 48;
			   while((++ptr) < line.length()) weight = weight * 10 +  line.charAt(ptr) - 48;
			   //System.out.println(v[0] + " " + v[1] + " " + weight);  
			   
			   for(flag=0; flag<2; flag++) {
				   node.get(v[flag]).state = Metadata.EXIST;
				   adList p = new adList();
				   p.ID = v[1-flag];
				   p.wt = weight;
				   adList head = node.get(v[flag]).adlistout;
				   if(head == null) {
					   node.get(v[flag]).adlistout = p;		  
					   p.ptr = null;  // the one first inserted is always the last one
				   }
				   else {
					   p.ptr = head;
					   node.get(v[flag]).adlistout = p;  // the new one is always the first one
				   }
				  //node[v[flag]].adlistin = node[v[flag]].adlistout; // to save space, for symmetric directed graph, we only store one-direct of an edge for each node
			   }
			   //-- node[u].ID = u;
			  
			   if(n<v[1]) n=v[1];
			   //if(n<u) n=u;
			   m++;
			   if(m % 100000 == 1) System.out.print("."); 
			   
			}	
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    
		// read node orders:
		int order;
		try {
			while ((line = roaddata.readLine()) != null) {			 
				if(line.length() < 5) continue;
				if(line.charAt(0)!='o') continue;  // some kind of line like "", " ", etc., is considered as the last line of the data file. "a 1 2 3" has the shortest length of a line
				ptr = 2;
				v[0] = line.charAt(ptr) - 48;  // stringstream ss is too slow for parsing the string to digits(500 edges/s), so we have to use converting chars to digits
				while(line.charAt(++ptr) !=' ') v[0] = v[0] * 10 +  line.charAt(ptr) - 48;
				order = line.charAt(++ptr) - 48;
				while((++ptr) < line.length()) order = order * 10 +  line.charAt(ptr) - 48;
				node.get(v[0]).OD = order;
				//System.out.print("\n" << v << " " << node[v].OD;
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		try {
			roaddata.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		
		System.out.print( "\nfile reading finished. \nnodes=" + n + ", edges=" + m);
		System.out.print( ", read in " + (System.currentTimeMillis() - startTime)/1000.0 + " seconds\n");
		//System.out.print( "node.size: " + node.size() + "\n");

		for(int i = 0; i <= n; i++) {
			if(node.get(i).state != Metadata.EXIST) {
				node.get(i).state = Metadata.NON_EXIST;
			}
		}
		
	    /*//adList p = new adList();
	    for(int id=1; id<10; id++) {
			p = node.get(id).adlistout;
			while(p!=null) {  // for each neighbor v of u:  
				int id_v = p.ID;		
				System.out.print("\n  id_v: " + id_v);
				p = p.ptr;
			}
			System.out.println();
	    }
	    //System.exit(0);*/
	    
	}
	

    //
    // Parses first group of consecutive digits found into an int.
    //
    public static int extractInt(String str) {
        Matcher matcher = Pattern.compile("\\d+").matcher(str);

        if (!matcher.find())
            throw new NumberFormatException("For input string [" + str + "]");

        return Integer.parseInt(matcher.group());
    }
}
//}
//////////////////////////////////////////////////////////////
// End of  class ReadRoad.
//////////////////////////////////////////////////////////////

