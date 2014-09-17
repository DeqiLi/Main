package CH_Java;

public class MySwap {
	private  int tmp1;
	private  int tmp2;
	
	public  void swap(int x1, int x2, int y1, int y2) {
		tmp1 = x1;
		x1 = x2;
		x2 = tmp1;
		
		tmp2 = y1;
		y1 = y2;
		y2 = tmp2;
	}
}
