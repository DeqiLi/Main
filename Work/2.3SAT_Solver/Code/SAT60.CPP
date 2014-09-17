/*
	SAT60:

	Input:  3SAT express;
	Handle: First expand the express by generating and adding new clauses, then simplify it;
	Output: Solution(s).

	There are two documents "20120808--SAT60设计思路.docx","20120811--SAT60设计流程图.docx" for your refrence.
	
	Deqi Li,
	2012.08.12

*/	

/* 2012.9.11:
	What is the most important in SAT60 is the application of the data structures--BasicWK[].[] and
	SecondWK[][].[]--which make expand(), add_new_generated() and out() work quickly.

	Let n as the variable number, m the current clause number. Then n=N, m>M. We have
	2*N*lengthB = m;       // lengthB is the average length of variables' BasicWK[].
	2*N*lengthS = lengthB; // lengthS is the average length of variables' SecondWK[][].
	4*N*N*lengthS = m;

	So, normally and on average, lengthB << m, and lengthS << lengthB.
	Furtherly, we can even consider lengthS as a SMALL CONSTANT number that is indenpendable with N.-- I will check
	this later on.
*/
 
#include "sat60.h"

void main(int argc, char *argv[]){  
	item=new Subexpress[MAX_M+1];
	line = (char *)malloc(64);
	str_int=(char *)malloc(64);  /* To store the index of variable temporarily. */
	if(item==NULL){
		printf("\07\nItems memory allocation failed. Abort.\n");
	}

	if(argc>1) {
		N = atoi(argv[1]);
		if(N<10)  filename = argv[1];
	}
	else{
		printf("Usage: SAT60 filename || SAT60 number_of_variables\nAbort\n"); 
		exit(0);
	}

	if(argc>=LEVEL+2) for(int i=0; i<LEVEL; i++) Depth[i] = atoi(argv[i+2]);

	initialize(); // It includes read_items() in SAT60, for initializing N,M.
	start1=time(NULL);
	Level=0; 
	Down=1; // 1: go down to next level; 0: go up to precious level
	int ret=level();  //////
	if(ret==-1) { printf("\nConstant 1.\n"); Exit(1); }
}
  
int level(){
	int ret;
	unsigned I, loop; 
	unsigned Loops = 1 << Depth[Level];
	level_called++;

	printf("\n\nlevel(): Level=%d ",Level);

	if(Level<0) return 0;  
	if(Down) {  
		select_prober();
		back_data(); 
		I = 0;
		I0[Level]=0;  
	}
	else I = I0[Level] + 1;  
	printf("\nL0 loop=%d, L1 loop=%d, time used %.1fs\n",I0[0],I0[1],difftime(time(0),start1));

	for(loop=I; loop<Loops; loop++) {
		if(Level<0) return 0;
		total_loops++;
		printf("\nL:%d loop:%ld",Level, loop);
		set_prober(loop); 
		ret = load_data();
		if(ret==-1) {
			continue;
		}
		printf(" clause:%ld",clause_count());
		ret = expand();  ////
	    //ret = expand_v2();
		if(ret==-1) {
			continue;
		}
		else if(!ret) { 
			//printf(" / %ld",clause_count());
			solve(); 
			Level = -1; 
			return 0; 
		}  //  ret==0 means expand(): fresh(): --fresh() return with total_clz_sum=0
		else if(Level < (LEVEL-1)) {
			printf(" / %ld",clause_count());
			I0[Level] = loop;	
			Down = 1; 
			Level++; 
			level(); 
		}
		else { 
			solve(); 
			Level = -1; 
			return 0; 
		}	
	}

	if(Level>0) { 
		Level--; 
		Down = 0; 
	} 

	if(Level<0) return 0;

	printf("\n--level() end. Level=%d\n", Level);

	return -1;
}

void select_prober(){
	unsigned i,j;
	unsigned *var_freq;
	unsigned *max_freq;

	int prober_num = Depth[Level];

	select_prober_called++;

	var_freq = new unsigned[N+1];  
	max_freq = new unsigned[prober_num];  

	for(i=0; i<prober_num; i++) max_freq[i]=0;
	for(i=1; i<=N; i++) {
		if(!Level)var_freq[i] = BasicBK[Level][N+i].clzb[MAX_CLZB_NUM].var1 + BasicBK[Level][N-i].clzb[MAX_CLZB_NUM].var1;
		else var_freq[i] = BasicWK[N+i].clzb[MAX_CLZB_NUM].var1 + BasicWK[N-i].clzb[MAX_CLZB_NUM].var1;
	}	
	
	int prober_index = 0;
	int high_freq, high_freq_var; 
	for(i=0; i<prober_num; i++) {
		high_freq = 0;
		for(j=1; j<=N; j++) {
			if(var_freq[j] > high_freq) {
				high_freq = var_freq[j];
				high_freq_var = j;
			}
		}
		prober0[Level][i] = high_freq_var;
		var_freq[high_freq_var] = 0;
	}
}

void select_prober_v2(){
	unsigned i; 
	unsigned j=1;
	unsigned lengthS, min_length, max_length, min_index;
	int v1, v2;
	int var2;

	DoubleProber *Prober;
	Prober = new DoubleProber[Depth[Level]/2];
	for(i=0; i<Depth[Level]/2; i++) {
		Prober[i].var1 = 0;
		Prober[i].var2 = 0;
		Prober[i].length = 0;
	}  

	for(v1=1; v1<N; v1++) {  
    if(!BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1) continue;
    max_length = 0;
    for(v2=v1+1; v2<=N; v2++) { 
      if(!BasicWK[N+v2].clzb[MAX_CLZB_NUM].var1) continue; 
      
      lengthS = SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var;
      if(lengthS > max_length) {
        max_length = lengthS;
        var2 = v2;
      }

      lengthS = SecondWK[N+v1][N-v2].clzs[MAX_CLZS_NUM].var;
      if(lengthS > max_length) {
        max_length = lengthS;
        var2 = v2;  
      }

      lengthS = SecondWK[N-v1][N+v2].clzs[MAX_CLZS_NUM].var;
      if(lengthS > max_length) {
        max_length = lengthS;
        var2 = v2;
      }

      lengthS = SecondWK[N-v1][N-v2].clzs[MAX_CLZS_NUM].var;
      if(lengthS > max_length) {
        max_length = lengthS;
        var2 = v2;
      }
    }

    if(!max_length) continue;
    min_index = 0;
	min_length = Prober[0].length;
	for(i=1; i<Depth[Level]/2; i++) {  
		if(Prober[i].length < min_length) {
			min_length = Prober[i].length;
			min_index = i;
		}
	} 

    if(max_length <= min_length) continue;
    Prober[min_index].var1 = v1;     
    Prober[min_index].var2 = var2;  
    Prober[min_index].length = max_length;
	}

    printf("prober0[%d][]: ", Level);
    for(i=0; i<Depth[Level]; i++) {
      if(i%2 == 0) { prober0[Level][i] = abs(Prober[i/2].var1); printf("%d/%d ",prober0[Level][i], Prober[i/2].length); }
      else { prober0[Level][i] = abs(Prober[(i-1)/2].var2); ; printf("%d/%d ",prober0[Level][i], Prober[(i-1)/2].length); }
    }
}

void set_prober(unsigned I){
	int h,i;
	set_prober_called++;

	for(i=0; i<Depth[Level]; i++) {
		h = ((I & (1<<i)) >> i);  
		if(h) prober[i] = prober0[Level][i];  
		else  prober[i] = 0-prober0[Level][i];  
	}

	return;
}

int handle_single(int svar){
	int i,lengthB;
	int v1,v2;
	int single[MAX_VAR_NUM+1];
	int p, n;  // p: the position/index of the last single var in single[]; n: the number of single vars--it maybe varies during this procedure.
	int single_var;
	int ret=0;  

	handle_single_called++;

	single[0]=svar;
	p=0; 
	n=1; 

	while(p<n) {
		single_var = single[p];
		if(BasicWK[N-single_var].clzb[0].var1 == CONSTANT1) { 
			return -1;
		}
 
		lengthB = BasicWK[N+single_var].clzb[MAX_CLZB_NUM].var1;
		for(i=0; i<lengthB; i++) {
			//if(BasicWK[N+single_var].clzb[i].status == -1) continue;
			//BasicWK[N+single_var].clzb[i].status = -1;  
			v1 = BasicWK[N+single_var].clzb[i].var1;
			v2 = BasicWK[N+single_var].clzb[i].var2;
			out(v1, single_var, v2);  
			if(v2) out(v2, single_var, v1);	// out clause 'v1+single_var' from v2's Basic & Second 
		}
 
		BasicWK[N+single_var].clzb[MAX_CLZB_NUM].var1 = 0;
		BasicWK[N+single_var].clzb[0].var1 = CONSTANT1; 
		//BasicWK[N+single_var].clzb[0].status = 1;
		BasicWK[N+single_var].clzb[0].status = 0;  // !!!
 
		lengthB = BasicWK[N-single_var].clzb[MAX_CLZB_NUM].var1;
		for(i=0; i<lengthB; i++) {
			//if(BasicWK[N-single_var].clzb[i].status == -1) continue;
			//BasicWK[N-single_var].clzb[i].status = -1; 
			v1 = BasicWK[N-single_var].clzb[i].var1;
			v2 = BasicWK[N-single_var].clzb[i].var2;
			if(!v2) { 
				for(int j=0; j<n; j++) {
					if(v1  == single[j]) break;       
					if(-v1 == single[j]) return -1;  
				}
				if(j==n) single[n++] = v1; 
				continue;  
			}
			else {
				if(SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var < 0)
					printf("SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var=%d", SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var);
				if(SecondWK[N+v2][N+v1].clzs[MAX_CLZS_NUM].var < 0)
					printf("SecondWK[N+v2][N+v1].clzs[MAX_CLZS_NUM].var=%d", SecondWK[N+v2][N+v1].clzs[MAX_CLZS_NUM].var);
        
				add_new_generated(v1, v2, 0);  
				add_new_generated(v2, v1, 0);  
			}
		}

		BasicWK[N-single_var].clzb[MAX_CLZB_NUM].var1 = 0;
		BasicWK[N-single_var].clzb[0].var1 = 0; 

		p++;
	}

	return ret;
}

/* 
 * keep some clauses from precious Level for the next Level
 */ 
int back_data(){ 
	int i; 
	int v, v1, v2;
	int lengthBW;

	back_data_called++;

	if(Level == 0) {
		return 0;  
	}
 
	for(v=-N; v<=N; v++){  // !!!!!
		BasicBK[Level][N+v].clzb[0].var1 = 0;
		BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1 = 0;
	}

	for(v=-N; v<=N; v++) {  
		if(BasicWK[N+v].clzb[0].var1 == CONSTANT1) {  
			BasicBK[Level][N+v].clzb[0].var1 = CONSTANT1;
			//BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1 = 0;
			continue;
		}
		lengthBW = BasicWK[N+v].clzb[MAX_CLZB_NUM].var1;
		BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1 = lengthBW;
		for(i=0; i<lengthBW; i++) {
			BasicBK[Level][N+v].clzb[i].var1 = BasicWK[N+v].clzb[i].var1;
			BasicBK[Level][N+v].clzb[i].var2 = BasicWK[N+v].clzb[i].var2;
			//BasicBK[Level][N+v].clzb[i].status = 1;
		}
	}

	for(i=0; i<M; i++) {  // load all original items to BasicBK[Level][]
		v  = item[i].var[0];
		v1 = item[i].var[1];
		v2 = item[i].var[2];
		if(add_to_Backup(v, v1, v2)==-1) { printf("\n--back_data() exit.\n"); printf("\nConstant 1.\n"); Exit(1);}
	}

	return 0;
}

int back_data_v2(){  
	unsigned long i, j;
	int v, v1;
	int lengthBW;

	back_data_called++;
 
	if(Level == 0) {
		return 0; 
	}
 
	for(v=-N; v<=N; v++){  // !!!!! 
		BasicBK[Level][N+v].clzb[0].var1 = 0;
		BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1 = 0;
    }  

	for(v=-N; v<=N; v++) {  
		if(BasicWK[N+v].clzb[0].var1 == CONSTANT1) {  
			BasicBK[Level][N+v].clzb[0].var1 = CONSTANT1;
			continue;
		}
		lengthBW = BasicWK[N+v].clzb[MAX_CLZB_NUM].var1;
		BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1 = lengthBW;
		for(i=0; i<lengthBW; i++) {
			BasicBK[Level][N+v].clzb[i].var1 = BasicWK[N+v].clzb[i].var1;
			BasicBK[Level][N+v].clzb[i].var2 = BasicWK[N+v].clzb[i].var2;
		}
	} 

	i = 0;
	unsigned lengthS;
	for(v=-N; v<=N; v++) {
		if(!v) continue;
		for(v1=-N; v1<=N; v1++) {
			if(!v1 || v1==v) continue;
			lengthS = SecondWK[N+v][N+v1].clzs[MAX_CLZS_NUM].var;
			if(!lengthS) continue;
			SecondBK[i+lengthS].j = MAX_CLZS_NUM; 
			SecondBK[i+lengthS].var = lengthS;
			for(j=0; j<lengthS; j++) {
				SecondBK[i].v = v;
				SecondBK[i].v1 = v1;
				SecondBK[i].j = j;
				SecondBK[i].var = SecondWK[N+v][N+v1].clzs[j].var;
				SecondBK[i].clzb_index = SecondWK[N+v][N+v1].clzs[j].clzb_index;
				i++;
			}
			i++;  
		}
	}
	SecondBK[MAX_M_IN_SECONDBK].var = i;  
	
	return 0;
}
 
unsigned long expand(){
	int i,j;
	int v1, v2, v3, v4, lengthB, lengthB_, lengthS_, index;
	int status;
	int ret;
	unsigned long loop_generated;	

	expand_called++;

	do{
		loop_generated = 0;

		for(v1=1; v1<=N; v1++) { 
			// 1)  	 
			if(BasicWK[N+v1].clzb[0].var1 == CONSTANT1 && BasicWK[N+v1].clzb[0].status == 1) { // When back_data()--add_to_Backup(), the status of single var is 1
				ret = handle_single(v1);
				if(ret == -1){
					return -1;
				}
			}

			if(BasicWK[N-v1].clzb[0].var1 == CONSTANT1 && BasicWK[N-v1].clzb[0].status == 1) { // When back_data()--add_to_Backup(), the status of single var is 1
				ret = handle_single(-v1);
				if(ret == -1){
					return -1;
				}
			}

			A = v1; 
			lengthB  = BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1;
			lengthB_ = BasicWK[N-v1].clzb[MAX_CLZB_NUM].var1;

			// 2) 
			for(i=0; i<lengthB; i++) { 
				lengthB  = BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1;  //// ?? //
				if(!lengthB) break; //// ?? //
				if(BasicWK[N+v1].clzb[i].var2) continue;
				v2 = BasicWK[N+v1].clzb[i].var1;    // v2!=0
				status = BasicWK[N+v1].clzb[i].status;
				for(j=0; j<lengthB_; j++) { 
					lengthB_ = BasicWK[N-v1].clzb[MAX_CLZB_NUM].var1;  //// ?? //
					if(!lengthB_) break; //// ?? //
					if(!(BasicWK[N-v1].clzb[j].status==1 || status==1)) continue; 
					v3 = BasicWK[N-v1].clzb[j].var1;  // v3!=0
					v4 = BasicWK[N-v1].clzb[j].var2;  
					if(v3==-v2 || v4==-v2) continue;   
					if(v3==v2 || v4==v2) {         
						if(v4) { 
							ret = add_new_generated(v3, v4, 0);
							if(ret){
								loop_generated += ret;
								add_new_generated(v4, v3, 0);										
							}
						}
						else{ // v3!=0, v4==0
							ret = handle_single(v3);
							if(ret == -1){
								return -1;
							}
						}
					}
					else {
						ret = add_new_generated(v2, v3, v4);   
						if(ret){ 
							loop_generated += ret;
							add_new_generated(v3, v2, v4);// same return as (v2, v3, v4)
							if(v4) add_new_generated(v4, v2, v3);// same return as (v2, v3, v4), or return 0 if v4==0
						}
					}
				}
			}

			// 3-1) 
			//lengthB  = BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1; // !!! // ?? // different from 3SAT_baseline
			for(i=0; i<lengthB; i++) { 				
				v3 = BasicWK[N+v1].clzb[i].var2;
				if(!v3) continue;   
				v2 = BasicWK[N+v1].clzb[i].var1;    // v2!=0
				status = BasicWK[N+v1].clzb[i].status;

				lengthS_ = SecondWK[N-v1][N+v2].clzs[MAX_CLZS_NUM].var; // ABC+~ABX; X=newvar=0,32767, C or other
				for(j=0; j<lengthS_; j++) {
					index = SecondWK[N-v1][N+v2].clzs[j].clzb_index;
					if(!(BasicWK[N-v1].clzb[index].status==1 || status==1)) continue; 
					v4 = SecondWK[N-v1][N+v2].clzs[j].var;  // v4 = CONSTANT1, v3, ~v3, 0,or other
					if(v4 == -v3) continue;   
					if(v4 == v3 || v4 == CONSTANT1) v4 = 0;  // ABC+~ABC, or ABC+~AB; v4 is C

					ret = add_new_generated(v2, v3, v4); // ret ==0 or 1;  0: add fail, 1: add success.
					if(ret){ 
						loop_generated += ret;
						add_new_generated(v3, v2, v4);// same return as (v2, v3, v4)
						if(v4) add_new_generated(v4, v2, v3);// same return as (v2, v3, v4), or return 0 if v4==0
					}
				}

				lengthS_ = SecondWK[N-v1][N+v3].clzs[MAX_CLZS_NUM].var; // ABC+~ACX; X=newvar=0,32767, C or other
				for(j=0; j<lengthS_; j++) {
					index = SecondWK[N-v1][N+v3].clzs[j].clzb_index;
					if(!(BasicWK[N-v1].clzb[index].status==1 || status==1)) continue; 
					v4 = SecondWK[N-v1][N+v3].clzs[j].var;
					if(v4 == -v2) continue;   
					if(v4 == v2 || v4 == CONSTANT1) v4 = 0;  // ABC+~ACB, or ABC+~AC; v4 is B

					ret = add_new_generated(v2, v3, v4);
					if(ret){ 
						loop_generated += ret;
						add_new_generated(v3, v2, v4);// same return as (v2, v3, v4)
						if(v4) add_new_generated(v4, v2, v3);// same return as (v2, v3, v4), or return 0 if v4==0
					}
				}

			  // 3-2)  
			  lengthB_ = BasicWK[N-v1].clzb[MAX_CLZB_NUM].var1; // !!!
				for(j=0; j<lengthB_; j++) { 
					if(BasicWK[N-v1].clzb[j].var2) continue;  
					if(!(BasicWK[N-v1].clzb[j].status==1 || status==1)) continue; 
					v4 = BasicWK[N-v1].clzb[j].var1;   // v4!=0    
					if(v4==-v2 || v4==-v3) continue;   
					if(v4==v2  || v4==v3)  continue;   
					ret = add_new_generated(v2, v3, v4);
					if(ret){
						loop_generated += ret; // ret == 0 or 1
						add_new_generated(v3, v2, v4);
						add_new_generated(v4, v2, v3);
					}
				}
			}

			A = 0;
			i = 0;
			while(i<lengthB){ 
				if(BasicWK[N+v1].clzb[i].status != -1) { BasicWK[N+v1].clzb[i].status = 0; i++; continue; }
				v2 = BasicWK[N+v1].clzb[i].var1; 
				v3 = BasicWK[N+v1].clzb[i].var2; 
				out(v1, v2, v3); 
				lengthB--;
			} 

			i = 0;
			while(i<lengthB_){  
				if(BasicWK[N-v1].clzb[i].status != -1) { BasicWK[N-v1].clzb[i].status = 0; i++; continue; }
				v2 = BasicWK[N-v1].clzb[i].var1; 
				v3 = BasicWK[N-v1].clzb[i].var2; 
				out(-v1, v2, v3);  
				lengthB_--;
			}
		}
	}while(loop_generated);

	return clause_count();
}

// Out clause 'v2+v3'/'v3+v2' from v1's Basic, and 
// Out clause v2 from v3's Second, and
// Out clause v3 from v2's Second

// Only 3-var clauses will be out. Nothing can out 2-var clauses.
void out(int v1, int v2, int v3) { // out clause 'v2+v3'/'v3+v2' from v1's BasicWK & SecondWK 
	int i,lengthB, lengthS,index;
	int var1, var2;
	out_called++;

	index = -1; // !
	if(!v3) {  
		index = SecondWK[N+v1][N+v2].clzs[0].clzb_index;
		if(v1 == A || v1 == -A) { 
			BasicWK[N+v1].clzb[index].status = -1;
			return; 
		} //
		SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var = 0; 
		SecondWK[N+v1][N+v2].clzs[0].var = 0;
	}
	else { 
		lengthS=SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var; // out clause 'v2+v3'
		for(i=0; i<lengthS; i++){
			if(SecondWK[N+v1][N+v2].clzs[i].var == v3) { //v3!=0, so SecondWK[N+v1][N+v2].clzs[i].var is a real clause
				index = SecondWK[N+v1][N+v2].clzs[i].clzb_index;
				if(v1 == A || v1 == -A) { 
					BasicWK[N+v1].clzb[index].status = -1;
					return; 
				} //
				// out clause v3 from v2's Second; if lengthS==1, then kill it by lengthS--.
				SecondWK[N+v1][N+v2].clzs[i].var = SecondWK[N+v1][N+v2].clzs[lengthS-1].var;
				SecondWK[N+v1][N+v2].clzs[i].clzb_index = SecondWK[N+v1][N+v2].clzs[lengthS-1].clzb_index;		
				SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var--; //lengthS--
				/*
				if(SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var < 0){
					printf("out(%d %d %d)_1: SecondWK[N+%d][N+%d].clzs[%d].var=%d", v1,v2,v3, v1, v2, MAX_CLZS_NUM, SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var);
					Exit(-9);
				}*/
				//printf("\nout(%d %d %d) from BasicWK[%d] and overwrite SecondWK[%d][%d]\n",v1,v2,v3, v1,v1,v2);
				break;
			}
		}

		lengthS=SecondWK[N+v1][N+v3].clzs[MAX_CLZS_NUM].var;
		for(i=0; i<lengthS; i++){
			if(SecondWK[N+v1][N+v3].clzs[i].var == v2) {
				// out clause v2 from v3's Second
				SecondWK[N+v1][N+v3].clzs[i].var = SecondWK[N+v1][N+v3].clzs[lengthS-1].var;
				SecondWK[N+v1][N+v3].clzs[i].clzb_index = SecondWK[N+v1][N+v3].clzs[lengthS-1].clzb_index;			
				SecondWK[N+v1][N+v3].clzs[MAX_CLZS_NUM].var--; //lengthS--
				/*
				if(SecondWK[N+v1][N+v3].clzs[MAX_CLZS_NUM].var < 0){
					printf("out(%d %d %d)_2: SecondWK[N+%d][N+%d].clzs[%d].var=%d", v1,v2,v3, v1, v3, MAX_CLZS_NUM, SecondWK[N+v1][N+v3].clzs[MAX_CLZS_NUM].var);
					Exit(-9);
				}*/
				//printf("\nout(%d %d %d)-overwrite SecondWK[%d][%d]\n",v1,v2,v3, v1,v3);
				break;
			}
		}
	}
	
	lengthB = BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1;

	if(!lengthB || index<0 || index>=MAX_CLZB_NUM) return;   
	
	var1 =  BasicWK[N+v1].clzb[lengthB-1].var1;
	var2 =  BasicWK[N+v1].clzb[lengthB-1].var2;
	BasicWK[N+v1].clzb[index].var1 = var1;
	BasicWK[N+v1].clzb[index].var2 = var2;
	BasicWK[N+v1].clzb[index].status = BasicWK[N+v1].clzb[lengthB-1].status;
	BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1--;
 
	if(!var2) SecondWK[N+v1][N+var1].clzs[0].clzb_index = index;   
	else{
		if(index+1 == lengthB) return;  
		i=0;
		while(i<MAX_CLZS_NUM){  
			if(SecondWK[N+v1][N+var1].clzs[i].var == var2) { 
				SecondWK[N+v1][N+var1].clzs[i].clzb_index = index;
				/*
				if(SecondWK[N+v1][N+var1].clzs[MAX_CLZS_NUM].var < 0){
					printf("out(%d %d %d)_3: SecondWK[N+%d][N+%d].clzs[%d].var=%d", v1,v2,v3, v1, var1, MAX_CLZS_NUM, SecondWK[N+v1][N+var1].clzs[MAX_CLZS_NUM].var);
					Exit(-9);
				}*/
				break;
			}
			i++;
		}

		i=0;
		while(i<MAX_CLZS_NUM){  // !!! ????? 
			if(SecondWK[N+v1][N+var2].clzs[i].var == var1) {
				SecondWK[N+v1][N+var2].clzs[i].clzb_index = index;
				/*
				if(SecondWK[N+v1][N+var2].clzs[MAX_CLZS_NUM].var < 0){
					printf("out(%d %d %d)_4: SecondWK[N+%d][N+%d].clzs[%d].var=%d", v1,v2,v3, v1, var2, MAX_CLZS_NUM, SecondWK[N+v1][N+var2].clzs[MAX_CLZS_NUM].var);
					Exit(-9);
				}*/
				break;
			}
			i++;
		}
	}

	return;
}

// Add a new generated clause(v1, v2, v3) when expand() or handle_single().
// add clause 'v2+v3' to v1's BasicWK;  
// add clause v2 to v3's Second, and
// add clause v3 to v2's Second

// The format of input clause is only like 'AB0', or 'ABC'. 
// Not allowed '0A','0AB','A0B','00A', and ignore 'A00'
unsigned add_new_generated(int v1, int v2, int v3) {  
	int i; 
	int v;
	int lengthB, lengthS;
	
	/*
	if(SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var < 0){
		printf("add_new_generated(%d, %d, %d)_0: SecondWK[N+%d][N+%d].clzs[%d].var=%d",v1,v2,v3, v1,v2, MAX_CLZS_NUM, SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var);
		Exit(-9);
	}*/

	add_new_generated_called++;

	if(!v1 || (!v2 && v3) ) {
		//printf("\n--add_new_generated(%d, %d, %d) return 0 for the invalid format.\n",v1,v2,v3);
		return 0;
	}

	if(BasicWK[N+v1].clzb[0].var1 == CONSTANT1 || BasicWK[N+v2].clzb[0].var1 == CONSTANT1 || BasicWK[N+v3].clzb[0].var1 == CONSTANT1){
		return 0; 
	}

	if(!v2 && !v3) { 
		return 0;
	}

	if(v1 && v2 && v3) {  
		/*if(BasicWK[N+v1].clzb[0].var == CONSTANT1 ||BasicWK[N+v2].clzb[0].var == CONSTANT1 || BasicWK[N+v3].clzb[0].var == CONSTANT1) {
			//printf("\n--add_new_generated(%d, %d, %d) return 0.\n",v1,v2,v3);			
			return 0;
		}*/
		if(SecondWK[N+v1][N+v2].clzs[0].var == CONSTANT1 || SecondWK[N+v1][N+v3].clzs[0].var == CONSTANT1 || SecondWK[N+v2][N+v3].clzs[0].var == CONSTANT1) {
			return 0;
		}

		lengthS = SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var;
		for(i=0; i < lengthS; i++) {
			if(v3 == SecondWK[N+v1][N+v2].clzs[i].var){
				return 0;  
			}
		}

		lengthB = BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1;  // add 'v2+v3' to basic
		if(lengthB >= MAX_CLZB_NUM) {  return 0;}  
		lengthS = SecondWK[N+v1][N+v3].clzs[MAX_CLZS_NUM].var;
		if(lengthB >= 8*MAX_CLZS_NUM || lengthS >= MAX_CLZS_NUM) {  return 0;}   
		lengthS = SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var;
		if(lengthS >= MAX_CLZS_NUM) {  return 0;}   
		BasicWK[N+v1].clzb[lengthB].var1 = v2;
		BasicWK[N+v1].clzb[lengthB].var2 = v3;
		BasicWK[N+v1].clzb[lengthB].status = 1;
		BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1++;

		SecondWK[N+v1][N+v2].clzs[lengthS].var = v3; 
		SecondWK[N+v1][N+v2].clzs[lengthS].clzb_index = lengthB;
		SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var++;  

		lengthS = SecondWK[N+v1][N+v3].clzs[MAX_CLZS_NUM].var;
		SecondWK[N+v1][N+v3].clzs[lengthS].var = v2;
		SecondWK[N+v1][N+v3].clzs[lengthS].clzb_index = lengthB;
		SecondWK[N+v1][N+v3].clzs[MAX_CLZS_NUM].var++;   

		return 1;
	}

	if(v2 && !v3) {  //if(v1 && v2 && !v3) {   // ?? if(v2 && !v3) { 
		if(SecondWK[N+v1][N+v2].clzs[0].var == CONSTANT1) return 0; 
		if(SecondWK[N+v2][N+v1].clzs[0].var != CONSTANT1) {  
			lengthS = SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var;
			i = 0;
			while(lengthS){ 
				if(v1==A || v1==-A){
					v = SecondWK[N+v1][N+v2].clzs[i++].var;  
					out(v, v2, v1); // out clause 'v2+v1'/'v1+v2' from v's Basic & Second  // out xBC/xCB
					out(v2, v, v1); // out clause 'v+v1'/'v1+v' from v2's Basic & Second   // out CBx/CxB
					lengthS--;
				}
				else{ 
					v = SecondWK[N+v1][N+v2].clzs[0].var;  
					out(v, v2, v1); // out clause 'v2+v1'/'v1+v2' from v's Basic & Second  // out xBC/xCB
					out(v2, v, v1); // out clause 'v+v1'/'v1+v' from v2's Basic & Second   // out CBx/CxB
					out(v1, v2, v);  
					lengthS = SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var;
				}
			}
		}

		lengthB = BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1;
		if(lengthB >= MAX_CLZB_NUM) { return 0;} 
		BasicWK[N+v1].clzb[lengthB].var1 = v2;
		BasicWK[N+v1].clzb[lengthB].var2 = 0;
		BasicWK[N+v1].clzb[lengthB].status = 1;
		BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1++;

		SecondWK[N+v1][N+v2].clzs[0].var = CONSTANT1;
		SecondWK[N+v1][N+v2].clzs[0].clzb_index = lengthB;
		SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var = 1; 

		return 1;
	}

	return 0;
}

// add clause 'v2+v3' to v's BasicBK[Level];  and
// add clause 'v+v3' to v2's BasicBK[Level];  and
// add clause 'v+v2' to v3's BasicBK[Level];  
 
// v, v1, v2 come from item[], so the format 
// is only like 'A00', 'AB0', or 'ABC'
// No format like '0A','0AB','A0B','00A'
// Now SecondBK[Level] is not available. 
int add_to_Backup(int v, int v1, int v2){  // For all Levels including Level==0
	int j, lengthBK;
	int repeated_or_smaller = 0;
	add_to_Backup_called++;

	if(BasicBK[Level][N+v].clzb[0].var1 == CONSTANT1 || BasicBK[Level][N+v1].clzb[0].var1 == CONSTANT1 || BasicBK[Level][N+v2].clzb[0].var1 == CONSTANT1) {
		return 0; 
	}

	if(!v || (!v1 && v2)) {
		return 0; 
	}

	if(!v1 && !v2) { 
		BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1 = 0;  
		BasicBK[Level][N+v].clzb[0].var1 = CONSTANT1;      
		//BasicBK[Level][N+v].clzb[0].status = 1;  // !!!
		if(BasicBK[Level][N-v].clzb[0].var1 == CONSTANT1){
			//printf("--add_to_Backup() return -1.\n");		
			return -1;  
		}
		//printf("--add_to_Backup() return 1.\n");		
		return 1;
	}

//////////////////////////////////////////////////////////////////////////////////
	if(v1 && v2) {  
		lengthBK = BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1; //add 'v+v1+v2' to BasicBK, not BasicWK;
		for(j=0; j<lengthBK; j++){
			if(BasicBK[Level][N+v].clzb[j].var2==0) { repeated_or_smaller = 1; break; }
			else if(BasicBK[Level][N+v].clzb[j].var1==v1 && BasicBK[Level][N+v].clzb[j].var2==v2) { repeated_or_smaller = 1; break; }
			else if(BasicBK[Level][N+v].clzb[j].var1==v2 && BasicBK[Level][N+v].clzb[j].var2==v1) { repeated_or_smaller = 1; break; }
		}
		if(repeated_or_smaller) {
			return 0; 
		}
	}
//////////////////////////////////////////////////////////////////////////////////
  
	if(v1 && !v2) { 
		lengthBK = BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1; //add 'v+v1+v2' to BasicBK, not BasicWK;
		for(j=0; j<lengthBK; j++){
			if(BasicBK[Level][N+v].clzb[j].var1==v1 && BasicBK[Level][N+v].clzb[j].var2==v2) { repeated_or_smaller = 1; break; }
		}
		if(repeated_or_smaller) {
			return 0; 
		}
	}

	if(!v1 && v2) { 
		return 0; 
	}  

	if(BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1 == MAX_CLZB_NUM || BasicBK[Level][N+v1].clzb[MAX_CLZB_NUM].var1 == MAX_CLZB_NUM || BasicBK[Level][N+v2].clzb[MAX_CLZB_NUM].var1 == MAX_CLZB_NUM) 	 
		return 0;

	lengthBK = BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1;  
	if(lengthBK >= MAX_CLZB_NUM) { printf("\nLoad all original items to BasicBK[] error: over MAX_CLZB_NUM.\n"); Exit(-2);} //continue;
	BasicBK[Level][N+v].clzb[lengthBK].var1 = v1;
	BasicBK[Level][N+v].clzb[lengthBK].var2 = v2;
	BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1++;
	
	lengthBK = BasicBK[Level][N+v1].clzb[MAX_CLZB_NUM].var1; //add 'v1+v+v2' to BasicBK; v1!=0
 	BasicBK[Level][N+v1].clzb[lengthBK].var1 = v;
	BasicBK[Level][N+v1].clzb[lengthBK].var2 = v2;
	BasicBK[Level][N+v1].clzb[MAX_CLZB_NUM].var1++;

	if(!v2) {
 		return 1;
	}
	lengthBK = BasicBK[Level][N+v2].clzb[MAX_CLZB_NUM].var1; //add 'v2+v+v1' to BasicBK
 	BasicBK[Level][N+v2].clzb[lengthBK].var1 = v;
	BasicBK[Level][N+v2].clzb[lengthBK].var2 = v1;
 	BasicBK[Level][N+v2].clzb[MAX_CLZB_NUM].var1++;
 
	return 1;
} 
 
int load_data(){
	int i, lengthB;
	int v, v1, v2;
	int ret;

	load_data_called++;

	for(v=-N; v<=N; v++) {
		BasicWK[N+v].clzb[0].var1 = 0;
		BasicWK[N+v].clzb[MAX_CLZB_NUM].var1 = 0;   
		for(v1=-N; v1<=N; v1++){   
			SecondWK[N+v][N+v1].clzs[0].var = 0;  
			SecondWK[N+v][N+v1].clzs[MAX_CLZS_NUM].var = 0;
		}
	}

	for(v=-N; v<=N; v++) { // Copy BasicBK[Level] to BasicWK[], and generate SecondWK
		if(!v) continue;
		if(BasicBK[Level][N+v].clzb[0].var1 == CONSTANT1) { // When back_data()--add_to_Backup(), the status of single var is 1
			ret = handle_single(v);
			if(ret == -1){
				return -1;
			}
			BasicWK[N+v].clzb[0].status = 1; // !!!
			continue;  // ?? //
		}

		lengthB = BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1;
		for(i=0; i<lengthB; i++) {  
			v1 = BasicBK[Level][N+v].clzb[i].var1;  // v1!=0  
			v2 = BasicBK[Level][N+v].clzb[i].var2;   
			add_new_generated(v, v1, v2); 
			//ret = add_new_generated(v, v1, v2); 
			/*if(ret){ 
				add_new_generated(v1, v, v2); 
				if(v2)add_new_generated(v2, v, v1);
			}*/
		}
	}

	A = 0;   
	for(i=0; i<Depth[Level]; i++) { 	
		if(handle_single(prober[i]) == -1){ 
			return -1;
		}
		BasicWK[N+prober[i]].clzb[0].status = 1;
	}
	return 0;
}

int load_data_v2(){
	unsigned long i, length;
	int lengthB;
	int v, v1;
	load_data_called++; 
	for(v=-N; v<=N; v++) {
		BasicWK[N+v].clzb[0].var1 = 0;
		BasicWK[N+v].clzb[MAX_CLZB_NUM].var1 = 0;   
		for(v1=-N; v1<=N; v1++){   
			SecondWK[N+v][N+v1].clzs[0].var = 0;  
			SecondWK[N+v][N+v1].clzs[MAX_CLZS_NUM].var = 0;
		}
	}

	start2=time(0); 
	for(v=-N; v<=N; v++) {  
		if(BasicBK[Level][N+v].clzb[0].var1 == CONSTANT1) {  
			BasicWK[N+v].clzb[0].var1 = CONSTANT1;
			BasicWK[N+v].clzb[MAX_CLZB_NUM].var1 = 0; 
			BasicWK[N+v].clzb[0].status = 1;
			continue;
		}
		lengthB = BasicBK[Level][N+v].clzb[MAX_CLZB_NUM].var1;
		BasicWK[N+v].clzb[MAX_CLZB_NUM].var1 = lengthB;
		for(i=0; i<lengthB; i++) {
			BasicWK[N+v].clzb[i].var1 = BasicBK[Level][N+v].clzb[i].var1;
			BasicWK[N+v].clzb[i].var2 = BasicBK[Level][N+v].clzb[i].var2;
			BasicWK[N+v].clzb[i].status = 1;
		}
	}

	//Load SecondWK[][] from SecondBK[]
	unsigned j;
	length = SecondBK[MAX_M_IN_SECONDBK].var;
	for(i=0; i<length; i++){
		v   = SecondBK[i].v;
		v1  = SecondBK[i].v1;
		j   = SecondBK[i].j;
		SecondWK[N+v][N+v1].clzs[j].var = SecondBK[i].var;
		SecondWK[N+v][N+v1].clzs[j].clzb_index = SecondBK[i].clzb_index;
	}
	t_1 += difftime(time(0),start2);     

	A = 0;  
 	for(i=0; i<Depth[Level]; i++) { 	
		if(handle_single(prober[i]) == -1){
 			return -1;
		}
		BasicWK[N+prober[i]].clzb[0].status = 1;
	}
 
	return 0;
}
 
void solve(){
	int v, v1, V1;
	printf("\nsolve():\n");
 
	for(v=1; v<=N; v++) {
		v1 = 0;
		if(BasicWK[N+v].clzb[0].var1 == CONSTANT1 || BasicWK[N-v].clzb[0].var1 == CONSTANT1) continue;
		if(BasicWK[N+v].clzb[MAX_CLZB_NUM].var1) v1 = v; // && BasicWK[N-v].clzb[0].var1 != CONSTANT1){
		else if(BasicWK[N-v].clzb[MAX_CLZB_NUM].var1) v1 = -v;
		else continue;  
		handle_single(v1);	 
	}
	for(v=1; v<=N; v++) { 
		v1 = BasicWK[N+v].clzb[0].var1;
		V1 = BasicWK[N-v].clzb[0].var1;
		if(v1 == CONSTANT1 && V1 == CONSTANT1) { 
			printf("\n--solve() exit.\n"); 
			Exit(1); 
		}
		else if(v1 == CONSTANT1) variable[v] = 0;
		else if(V1 == CONSTANT1) variable[v] = 1;
		else variable[v] = 9; // either 0 or 1 is OK.
	}

	solution_print();
	solution_check();
	Exit(0);
}

void solution_check(){
	unsigned i;
	int j;
	int item_value=0; 

	for(i=0;i<M;i++){
		item_value=1;
		for(j=0;j<item[i].var_num;j++){
			if( (variable[abs(item[i].var[j])]==0) && (item[i].var[j]>0) ||
			  (variable[abs(item[i].var[j])]==1) && (item[i].var[j]<0) ){
				item_value=0; 
				break;   
			}
		}
		if(item_value==1) break;  
	}

	if((i<M) ||(item_value==1)){
		printf("\nGot wrong solution for the original T(solution_check()):");
		printf("\nitem[%d]=(%d %d %d) = 1", i+1, item[i].var[0],item[i].var[1],item[i].var[2]);
		Exit(-1);
	} 

	return;
}

void solution_print(){
	int var_assigned=0; 
	for(int i=1; i<=N; i++) if(variable[i] !=9) var_assigned++;
	printf("\n%d variables assigned.'*' means '0/1'.\n",var_assigned);
	printf("Solution[]=\n");
	for(i=1;i<=N;i++){
		if(variable[i]!=9) printf("%d",variable[i]);
		else printf("*");
		if((i%10) ==0) printf("\n");
	}
	return;
}

void initialize()
{
	printf("\ninitialize():\n");

	unsigned i;
	int l,j;

	if(N<10) read_items();  
	else clause_generator(N);  

	// The Depth is estimated; it need to be refined.
	if(N<200) for(i=0; i<LEVEL; i++) Depth[i] = Depth[i] - 1;
	if(N>350) for(i=0; i<LEVEL; i++) Depth[i] = Depth[i] + (N-300)/100;

	Max_Level_M =  M * 64;	//Max_Level_M =  M * 8;
	Level=0;
	total_loops = 0;
 
	// fill 0 to each array:
	for(l=0; l<LEVEL; l++) {
		for(i=0; i<=N+N; i++) {
			for(j=0; j<=MAX_CLZB_NUM; j++) {
				BasicBK[l][i].clzb[j].status = 0;
				BasicBK[l][i].clzb[j].var1 = 0;
				BasicBK[l][i].clzb[j].var2 = 0;
			}
		}
	}

	for(i=0; i<=N+N; i++) {
		for(j=0; j<=MAX_CLZB_NUM; j++) {
			BasicWK[i].clzb[j].status = 0;
			BasicWK[i].clzb[j].var1 = 0;
			BasicWK[i].clzb[j].var2 = 0;
		}
	}

	for(l=0; l<=N+N; l++) {
		for(i=0; i<=N+N; i++) {
			for(j=0; j<=MAX_CLZS_NUM; j++) {
				SecondWK[l][i].clzs[j].clzb_index = 0;
				SecondWK[l][i].clzs[j].var = 0;
			}
		}
	}

	for(l=0; l<LEVEL; l++) {
		for(i=0; i<N; i++) {
			prober0[l][i] = 0;
		}
	}

	for(i=0; i<Depth[Level]; i++) {
		prober[i] = 0;
	}

	for(l=0; l<LEVEL; l++) {
		I0[l] = 0;
	}

	int v, v1, v2;
 
	for(i=0; i<M; i++) {  // load all original items to BasicBK[0][];
		v  = item[i].var[0];
		v1 = item[i].var[1];
		v2 = item[i].var[2];
		if(add_to_Backup(v, v1, v2)==-1) { printf("\n--initialize() exit for add_to_Backup(%d %d %d).\n", v, v1, v2);printf("\nConstant 1.\n"); start1=time(0); Exit(1);}
	}

	printf("\n--initialize() end.\n");
	return;
}

// Read each item (v1, v2, v3) from 3SAT file.
// After adjustment, the format is only like 'A00', 'AB0', or 'ABC'
// No format is like '0A','0AB','A0B','00A'.
void read_items()
{
	char *pLine=NULL;
	char ch='0';
	int i;
	int index =0;   //item index 
	int v1=0;   //variable1 index 
	int v2=0; 
	int v3=0; 
	//printf("\nread_items(fileno=%d):\n",fileno);
	N=0; 
	M=0; 
    pLine=line;  // backup its head! 

	pFile.open(filename);
	/*
	if(fileno==0)       pFile.open("3SAT0.txt");
	else if(fileno==1)  pFile.open("3SAT1.txt");
	else if(fileno==2) 	pFile.open("3SAT2.txt");
	else if(fileno==3) 	pFile.open("3SAT3.txt");
	else if(fileno==4) 	pFile.open("3SAT4.txt");
	else if(fileno==5) 	pFile.open("3SAT5.txt");
	else if(fileno==6) 	pFile.open("3SAT6.txt");
	else if(fileno==7) 	pFile.open("3SAT7.txt");
	else if(fileno==8) 	pFile.open("3SAT8.txt");
	else if(fileno==9) 	pFile.open("3SAT9.txt");
	else {
		printf("File select error. Pleaase select file number from 0-9. Abort.\n");
		Exit(-5);
	}*/

	if(pFile==NULL) {
		pFile.clear();
		pFile.close();
		printf("\07\nCannot open file 3SAT express file on the path ! Abort.\n");
		exit(0);
	}

	for(i=0;i<MAX_M;i++){
		item[i].var[0] = 0;
		item[i].var[1] = 0;
		item[i].var[2] = 0;
		item[i].var_num = 0;
	}

	while(1) {
		if(pFile.eof()) break;

		line=pLine;  // Move line pointer to its head! 
		pFile.getline(line,63,'\n');

		if((strlen(line)<1) || (strlen(line)>63)) {
			pFile.clear();
			pFile.close();
			printf("\nPlease check Line%ld.",index+1);
			printf("\07\n3SAT file format is invalid. You can copy it to WORD to check it. Abort.");
			Exit(-4);
		}

		ch=*line;
		while((ch==' ')||(ch==',')||(ch==';')||(ch=='\n')||(ch=='.')||(ch=='|')) {// Read a char until it is '-' or '0'-'9'.
			line++;
			ch=*line;
		}
		str_int=line;
		v1=atoi(str_int);

		while(((ch>='0')&&(ch<='9'))||(ch=='-')){
			line++;
			ch=*line;
		}
		while((ch==' ')||(ch==',')||(ch==';')||(ch=='\n')) {// Read a char until it is '-' or '0'-'9'.
			line++;
			ch=*line;
		}
		str_int=line;
		v2=atoi(str_int);

		while(((ch>='0')&&(ch<='9'))||(ch=='-')){
			line++;
			ch=*line;
		}
		while((ch==' ')||(ch==',')||(ch==';')||(ch=='\n')) {// Read a char until it is '-' or '0'-'9'.
			line++;
			ch=*line;
		}
		str_int=line;
		v3=atoi(str_int);

		//if(!v1 && !v2 && !v3) continue;if(v1 && (!v2 && v3) )
		if(!v1 && !v2) {v1=v3; v3=0;}
		if(!v1 && v2) {v1=v2; v2=v3; v3=0;}
		if(!v2) {v2=v3; v3=0;}   
		if(!v1 || v1==v2 || (v2==v3) && v3 || v1==v3) { printf("Clause format invalid, exit. See clause %d#",index+1); Exit(-4); }
		if((v1+v2)==0 || ((v2+v3)==0) && v3 || (v1+v3)==0) { printf("Clause format invalid, exit.See clause %d#",index+1); Exit(-4); }
  
		item[index].var[item[index].var_num]=v1;
		item[index].var_num++;

		if(v2!=0){
			item[index].var[item[index].var_num]=v2;
			item[index].var_num++;
		}
		if(v3!=0){
			item[index].var[item[index].var_num]=v3;
			item[index].var_num++;
		}

		if(item[index].var_num>0) index++;

		if(index>=MAX_M){
			pFile.clear();
			pFile.close();
			printf("\07\nToo many items in the 3SAT express! Abort.");
			printf("\nThe item number limit is %ld.",MAX_M);
			Exit(-5);
		}
	}

	M=index;
	for(i=0;i<M;i++){
		if(N<abs(item[i].var[0])) N=abs(item[i].var[0]); // To get variable number
		if(N<abs(item[i].var[1])) N=abs(item[i].var[1]); // To get variable number
		if(N<abs(item[i].var[2])) N=abs(item[i].var[2]); // To get variable number
		if(N>MAX_VAR_NUM){
			printf("\07\nToo many variables in the 3SAT express! Abort.\nThe variable number limit is %ld.",MAX_VAR_NUM);
			pFile.clear();
			pFile.close();
			Exit(-5);
		}
	}

	printf("\n3SAT has %d variables, %ld items.",N,M);
	pFile.clear();  
	pFile.close();  
	printf("\nread_items() end.\n");
	return;
}

 void print_WK(){
	int v, v1, v2, i;
	int lengthW;

 	printf("\nBasicWK[]:\n");
	
 	for(v=1; v<=N; v++) { 
		if(BasicWK[N+v].clzb[0].var1 == CONSTANT1) {
 			printf("%3d %3c %3c\n",v, ' ',' ');
		} 
		lengthW = BasicWK[N+v].clzb[MAX_CLZB_NUM].var1;
		for(i=0; i<lengthW; i++) {
			v1 = BasicWK[N+v].clzb[i].var1; 
			v2 = BasicWK[N+v].clzb[i].var2; 
 			printf("%3d",v);
			if(v1)printf(" %3d",v1);
			else printf(" %3c",' ');
			if(v2)printf(" %3d",v2);
			else printf(" %3c",' ');
 			printf("\n");
		}

		if(BasicWK[N-v].clzb[0].var1 == CONSTANT1) {
 			printf("%3d %3c %3c\n",-v, ' ',' ');
			continue;
		}
		lengthW = BasicWK[N-v].clzb[MAX_CLZB_NUM].var1;
		for(i=0; i<lengthW; i++) {
			v1 = BasicWK[N-v].clzb[i].var1; 
			v2 = BasicWK[N-v].clzb[i].var2; 
 			printf("%3d",-v);
			if(v1)printf(" %3d",v1);
			else printf(" %3c",' ');
			if(v2)printf(" %3d",v2);
			else printf(" %3c",' ');
 			printf("\n");
		}	
	}
}

void printf_info(int x){
	printf("\n\nGlobal Running Parameters:");
	printf("\n-----------------------------------------------------");
	if(!x)
	printf("\nSolution                :   T = 0. OK.\n"); 
	else if(x==1)
	printf("\nSolution                :   T = Constant 1.\n"); 
	else if(x==-1)
	printf("\nSolution                :   Exit. Need More Deep Search!\n"); 
	else if(x==-2)
	printf("\nError                   :   Over MAX_CLZB_NUM=%d.\n",MAX_CLZB_NUM); 
	else if(x==-3)
	printf("\nError                   :   Over MAX_CLZS_NUM=%d.\n",MAX_CLZS_NUM);  
	else if(x==-4)
	printf("\nError                   :   Invalid clause format.\n"); 
	else if(x==-5)
	printf("\nTime Test               :   \n"); 
	else
	printf("\nError                   :   Unexpected result.\n"); 

	printf("\nN-M-3SAT file first line:   -%d-%d-(%d %d %d)-",N,M,item[0].var[0],item[0].var[1],item[0].var[2]);
	//printf("\nN-M                     :   %d-%d",N,M);
	if(LEVEL==8)
	printf("\nLEVEL/Level_Depth       :   %d / %d %d %d %d %d %d %d %d",LEVEL, Depth[0],Depth[1],Depth[2],Depth[3],Depth[4],Depth[5],Depth[6],Depth[7]);
	else if(LEVEL==7)
	printf("\nLEVEL/Level_Depth       :   %d / %d %d %d %d %d %d %d",LEVEL, Depth[0],Depth[1],Depth[2],Depth[3],Depth[4],Depth[5],Depth[6]);
	else if(LEVEL==6)
	printf("\nLEVEL/Level_Depth       :   %d / %d %d %d %d %d %d",LEVEL, Depth[0],Depth[1],Depth[2],Depth[3],Depth[4],Depth[5]);
	else if(LEVEL==5)
	printf("\nLEVEL/Level_Depth       :   %d / %d %d %d %d %d",LEVEL, Depth[0],Depth[1],Depth[2],Depth[3],Depth[4]);
	else if(LEVEL==4)
	printf("\nLEVEL/Level_Depth       :   %d / %d %d %d %d",LEVEL, Depth[0],Depth[1],Depth[2],Depth[3]);
	else if(LEVEL==3)
	printf("\nLEVEL/Level_Depth       :   %d / %d %d %d",LEVEL, Depth[0],Depth[1],Depth[2]);
	else if(LEVEL==2)
	printf("\nLEVEL/Level_Depth       :   %d / %d %d",LEVEL, Depth[0],Depth[1]);
	printf("\nMAX_VAR_NUM             :   %d",MAX_VAR_NUM);
	printf("\nMAX_CLZB/max_lengthB    :   %d/%d",MAX_CLZB_NUM,max_lengthB);
	printf("\nMAX_CLZS/max_lengthS    :   %d/%d\n",MAX_CLZS_NUM,max_lengthS);
	printf("\nadd_new_generated()     :   %ld",add_new_generated_called);
	printf("\nout()                   :   %ld",out_called);
	printf("\nhandle_single()         :   %ld",handle_single_called);
	printf("\nload_data()/total_loops :   %ld\n",load_data_called);
	printf("\ntime used               :   %.1fs", difftime(end1,start1));
	printf("\n-----------------------------------------------------");
	printf("\nSAT60 End.\n");
}

void Exit(int x){
	end1=time(0);
	printf_info(x);
	if(x<0) exit(x);
}

void clause_generator(unsigned n){
	int var1,var2,var3;
	int min, mid, max;
	printf("\nclause_generator():\n");

	N = n;
	M = N * 4.3;

	srand((unsigned int)(time(0)+add_new_generated_called));
	
	for(unsigned long i=0; i<M; i++) {
		var1=0; var2=0; var3=0;
		min=32767;
		max=0;
		mid=0;
		
		bool NotUnique = true;
		while(NotUnique){
			do{
				var1=rand()% (n + 1);
			}while(var1==0);

			do{
				var2=rand()% (n + 1);
			}while(var2==0 || var2 == var1);

			do{
				var3=rand()% (n + 1);
			}while(var3==0 || var3 == var1  || var3 == var2);

			if(min>var1) min=var1; if(min>var2) min=var2; if(min>var3) min=var3;
			if(max<var1) max=var1; if(max<var2) max=var2; if(max<var3) max=var3;
			if(var1!=min && var1!=max) mid=var1;
			else if(var2!=min && var2!=max) mid=var2;
			else if(var3!=min && var3!=max) mid=var3;

			if(rand()%2) min = 0-min;
			if(rand()%2) mid = 0-mid;
			if(rand()%2) max = 0-max;

			for(unsigned long j=0; j<i; j++) 
				if(min==item[j].var[0] && mid==item[j].var[1] && max==item[j].var[2]) continue;

			if(j==i) NotUnique = false;
		}

		item[i].var[0] = min;
		item[i].var[1] = mid;
		item[i].var[2] = max;
		item[i].var_num = 3;
		printf("\n%4d %4d %4d", min, mid, max);
	}
	printf("\n--clause_generator() end.  N=%d, M=%d\n", N,M);
	
	return;
}

unsigned long clause_count(){
	unsigned long ret = 0;
	for(int v=-N; v<=N; v++) {   
		if(!v) continue;
		ret += BasicWK[N+v].clzb[MAX_CLZB_NUM].var1;  
	}

	return ret;
}

// T = ABC+~ADE+XBC+~XD~E ==> BCD
unsigned long extra_expand(){
	int v1, v2;
	int var[4];
	int i,j,k;
	int lengthB, lengthB_,lengthS, lengthS_;
	int ret;
	unsigned long generated = 0;
	printf("\nextra_expand()...");

	for(v1=1; v1<N; v1++){ // A, -A
		lengthB  = BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1;
		lengthB_ = BasicWK[N-v1].clzb[MAX_CLZB_NUM].var1;
		for(i=0; i<lengthB; i++) {  // B=var[0], C=var[1]
		  var[1] = BasicWK[N+v1].clzb[i].var2;
		  if(!var[1]) continue;
		  var[0] = BasicWK[N+v1].clzb[i].var1;
		  for(j=0; j<lengthB_; j++) { // D=var[2], E=var[3]; D!=B,C,-B,-C; E!=B,C,-B,-C
			var[3] = BasicWK[N-v1].clzb[j].var2;
			if(!var[3] || var[3]==var[0] || var[3]==var[1] || var[3]==-var[0] || var[3]==-var[1]) continue;    
			var[2] = BasicWK[N-v1].clzb[j].var1;
			if(var[2]==var[0] || var[2]==var[1] || var[2]==-var[0] || var[2]==-var[1]) continue;    
        
			for(v2=v1+1; v2<=N; v2++){ // X:B::C; -X:D::-E;  X=v2 > 0
			  if(!BasicWK[N+v2].clzb[MAX_CLZB_NUM].var1 || !BasicWK[N-v2].clzb[MAX_CLZB_NUM].var1) continue;
			  lengthS = SecondWK[N+v2][N+var[0]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS; k++){  
				if(SecondWK[N+v2][N+var[0]].clzs[k].var == var[1]) break;
			  } 
			  if(k==lengthS) continue;

			  lengthS_ = SecondWK[N-v2][N+var[2]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS_; k++){
				if(SecondWK[N-v2][N+var[2]].clzs[k].var == -var[3]) break;
			  }
			  if(k==lengthS_) continue;
			  ret = add_new_generated(var[0], var[1], var[2]);
			  if(ret) {
				generated++;
				//printf("\n(%d %d %d)+(%d %d %d)+(%d %d %d)+(%d %d %d)==>", v1,var[0],var[1],  -v1,var[2],var[3],  v2,var[0],var[1],  -v2,var[2],-var[3]);
				//printf("\n%ld#: %d %d %d", generated, var[0], var[1], var[2]);
				add_new_generated(var[1], var[0], var[2]);
				add_new_generated(var[2], var[0], var[1]);
			  }
			}

			for(v2=v1+1; v2<=N; v2++){ // X:B::D; -X:C::-E;  X=v2 > 0
			  if(!BasicWK[N+v2].clzb[MAX_CLZB_NUM].var1 || !BasicWK[N-v2].clzb[MAX_CLZB_NUM].var1) continue;
			  lengthS = SecondWK[N+v2][N+var[0]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS; k++){
				if(SecondWK[N+v2][N+var[0]].clzs[k].var == var[2]) break;
			  }
			  if(k==lengthS) continue;

			  lengthS_ = SecondWK[N-v2][N+var[1]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS_; k++){
				if(SecondWK[N-v2][N+var[1]].clzs[k].var == -var[3]) break;
			  }
			  if(k==lengthS_) continue;
			  ret = add_new_generated(var[0], var[1], var[2]);
			  if(ret) {
				generated++;
				//printf("\n(%d %d %d)+(%d %d %d)+(%d %d %d)+(%d %d %d)==>", v1,var[0],var[1],  -v1,var[2],var[3],  v2,var[0],var[2],  -v2,var[1],-var[3]);
				//printf("\n%ld#: %d %d %d", generated, var[0], var[1], var[2]);
				add_new_generated(var[1], var[0], var[2]);
				add_new_generated(var[2], var[0], var[1]);
			  }
			}

			for(v2=v1+1; v2<=N; v2++){ // X:B::-E; -X:C::D;  X=v2 > 0
			  if(!BasicWK[N+v2].clzb[MAX_CLZB_NUM].var1 || !BasicWK[N-v2].clzb[MAX_CLZB_NUM].var1) continue;
			  lengthS = SecondWK[N+v2][N+var[0]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS; k++){
				if(SecondWK[N+v2][N+var[0]].clzs[k].var == -var[3]) break;
			  }
			  if(k==lengthS) continue;

			  lengthS_ = SecondWK[N-v2][N+var[1]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS_; k++){
				if(SecondWK[N-v2][N+var[1]].clzs[k].var == var[2]) break;
			  }
			  if(k==lengthS_) continue;
			  ret = add_new_generated(var[0], var[1], var[2]);
			  if(ret) {
				generated++;
				//printf("\n(%d %d %d)+(%d %d %d)+(%d %d %d)+(%d %d %d)==>", v1,var[0],var[1],  -v1,var[2],var[3],  v2,var[0],-var[3],  -v2,var[1],var[2]);
				//printf("\n%ld#: %d %d %d", generated, var[0], var[1], var[2]);
				add_new_generated(var[1], var[0], var[2]);
				add_new_generated(var[2], var[0], var[1]);
			  }
			}

			// -X
			for(v2=v1+1; v2<=N; v2++){ // -X:B::C; X:D::-E;  X=v2 > 0
			  if(!BasicWK[N+v2].clzb[MAX_CLZB_NUM].var1 || !BasicWK[N-v2].clzb[MAX_CLZB_NUM].var1) continue;
			  lengthS_ = SecondWK[N-v2][N+var[0]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS_; k++){
				if(SecondWK[N-v2][N+var[0]].clzs[k].var == var[1]) break;
			  }
			  if(k==lengthS_) continue;

			  lengthS = SecondWK[N+v2][N+var[2]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS; k++){
				if(SecondWK[N+v2][N+var[2]].clzs[k].var == -var[3]) break;
			  }
			  if(k==lengthS) continue;
			  ret = add_new_generated(var[0], var[1], var[2]);
			  if(ret) {
				generated++;
				//printf("\n(%d %d %d)+(%d %d %d)+(%d %d %d)+(%d %d %d)==>", v1,var[0],var[1],  -v1,var[2],var[3],  -v2,var[0],var[1],  v2,var[2],-var[3]);
				//printf("\n%ld#: %d %d %d", generated, var[0], var[1], var[2]);
				add_new_generated(var[1], var[0], var[2]);
				add_new_generated(var[2], var[0], var[1]);
			  }
			}

			for(v2=v1+1; v2<=N; v2++){ // -X:B::D; X:C::-E;  X=v2 > 0
			  if(!BasicWK[N+v2].clzb[MAX_CLZB_NUM].var1 || !BasicWK[N-v2].clzb[MAX_CLZB_NUM].var1) continue;
			  lengthS_ = SecondWK[N-v2][N+var[0]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS_; k++){
				if(SecondWK[N-v2][N+var[0]].clzs[k].var == var[2]) break;
			  }
			  if(k==lengthS_) continue;

			  lengthS = SecondWK[N+v2][N+var[1]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS; k++){
				if(SecondWK[N+v2][N+var[1]].clzs[k].var == -var[3]) break;
			  }
			  if(k==lengthS) continue;
			  ret = add_new_generated(var[0], var[1], var[2]);
			  if(ret) {
				generated++;
				//printf("\n(%d %d %d)+(%d %d %d)+(%d %d %d)+(%d %d %d)==>", v1,var[0],var[1],  -v1,var[2],var[3],  -v2,var[0],var[2],  v2,var[1],-var[3]);
				//printf("\n%ld#: %d %d %d", generated, var[0], var[1], var[2]);
				add_new_generated(var[1], var[0], var[2]);
				add_new_generated(var[2], var[0], var[1]);
			  }
			}

			for(v2=v1+1; v2<=N; v2++){ // -X:B::-E; X:C::D;  X=v2 > 0
			  if(!BasicWK[N+v2].clzb[MAX_CLZB_NUM].var1 || !BasicWK[N-v2].clzb[MAX_CLZB_NUM].var1) continue;
			  lengthS_ = SecondWK[N-v2][N+var[0]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS_; k++){
				if(SecondWK[N-v2][N+var[0]].clzs[k].var == -var[3]) break;
			  }
			  if(k==lengthS_) continue;

			  lengthS = SecondWK[N+v2][N+var[1]].clzs[MAX_CLZS_NUM].var;
			  for(k=0; k<lengthS; k++){
				if(SecondWK[N+v2][N+var[1]].clzs[k].var == var[2]) break;
			  }
			  if(k==lengthS) continue;
			  ret = add_new_generated(var[0], var[1], var[2]);
			  if(ret) {
				generated++;
				//printf("\n(%d %d %d)+(%d %d %d)+(%d %d %d)+(%d %d %d)==>", v1,var[0],var[1],  -v1,var[2],var[3],  -v2,var[0],-var[3],  v2,var[1],var[2]);
				//printf("\n%ld#: %d %d %d", generated, var[0], var[1], var[2]);
				add_new_generated(var[1], var[0], var[2]);
				add_new_generated(var[2], var[0], var[1]);
			  }
			}
		  }
		}
	  }

	printf(", generated: %ld\n", generated);
	return generated;
}

void thin(){
	int v1, v2, v3, i;
	int lengthS, lengthB, index;
	for(v1=-N; v1<=N; v1++){
		for(v2=-N; v2<=N; v2++){
			if(v1==v2) continue;
			lengthS = SecondWK[N+v1][N+v2].clzs[MAX_CLZS_NUM].var;
			if(lengthS > 5){
				for(i=0; i<lengthS; i++) {
				index = SecondWK[N+v1][N+v2].clzs[i].clzb_index;
				BasicWK[N+v1].clzb[index].status = -1;
			}
		}
	}

	i = 0;
	lengthB = BasicWK[N+v1].clzb[MAX_CLZB_NUM].var1;
	while(i<lengthB){ 
		if(BasicWK[N+v1].clzb[i].status != -1) { i++; continue; }
			v2 = BasicWK[N+v1].clzb[i].var1; 
			v3 = BasicWK[N+v1].clzb[i].var2; 
			out(v1, v2, v3); 
			lengthB--;
		}
	}
}

unsigned long expand_v2(){
	int i,j;
	int v, v1, v2, v3, v4;
	unsigned lengthB, lengthB_, lengthA_, length, length_, lengthS, index;
	int status;
	int ret;
	unsigned lengthD;
	unsigned long loop_generated;	

	typedef struct double_var_clause    
	{
		int var;
		int status;
	}double_var_clause;

	double_var_clause *Double = new double_var_clause[N]; 

	do{
		loop_generated = 0;

		for(v=1; v<=N; v++) {  
			if(BasicWK[N+v].clzb[0].var1 == CONSTANT1 && BasicWK[N+v].clzb[0].status == 1) { 
				ret = handle_single(v);
				loop_generated += ret;
				if(ret == -1) return -1;
			}
			if(BasicWK[N-v].clzb[0].var1 == CONSTANT1 && BasicWK[N-v].clzb[0].status == 1) { 
			ret = handle_single(-v);
			loop_generated += ret;
			if(ret == -1) return -1;
		}
		lengthB  = BasicWK[N+v].clzb[MAX_CLZB_NUM].var1;
		lengthB_ = BasicWK[N-v].clzb[MAX_CLZB_NUM].var1;
		A = v;  
		length  = lengthB;
		length_ = lengthB_;
		if(lengthB > lengthB_) { 
			A = -v;
			length = lengthB_;
			length_ = lengthB;
		} 
		lengthD = 0;  
		if(length_ < N+N) {
			for(i=0; i<length_; i++){
				if(BasicWK[N-A].clzb[i].var2) continue;
				Double[lengthD].var = BasicWK[N-A].clzb[i].var1;
				Double[lengthD].status = BasicWK[N-A].clzb[i].status;
				lengthD++;
			}
		}
		else {
			for(v3=-N; v3<=N; v3++) {
				if(SecondWK[N-A][N+v3].clzs[0].var != CONSTANT1) continue;
           
			  if(v3==A || v3==-A || !v3) continue;       
			  index = SecondWK[N-A][N+v3].clzs[0].clzb_index;
				  Double[lengthD].var = v3;
				  Double[lengthD].status = BasicWK[N-A].clzb[index].status;
				  lengthD++;
			}
      }
 
      for(i=0; i<length; i++) {
        status = BasicWK[N+A].clzb[i].status;
        v1 = BasicWK[N+A].clzb[i].var1;
        v2 = BasicWK[N+A].clzb[i].var2;

        // 1) ABx + ~ABy/~AyB
		if(v2) {  
			lengthS = SecondWK[N-A][N+v1].clzs[MAX_CLZS_NUM].var;
			if(SecondWK[N-A][N+v1].clzs[0].var == CONSTANT1) {  
			ret = add_new_generated(v1, v2, 0);
			if(ret) {
				loop_generated++;
				add_new_generated(v2, v1, 0);  
			}
			lengthS = 0;  
         }
          for(j=0; j<lengthS; j++){  
            v3 = SecondWK[N-A][N+v1].clzs[j].var; // v3!=0
            index = SecondWK[N-A][N+v1].clzs[j].clzb_index;
            if(!status && !BasicWK[N-A].clzb[index].status) continue;  

            if(v3 == -v2) continue;  
            if(v3 == v2){ 
              ret = add_new_generated(v1, v2, 0);  
              if(ret) {
					loop_generated++;
					add_new_generated(v2, v1, 0);  
              }
            }
            else {    
              ret = add_new_generated(v1, v2, v3);  
              if(ret) {
					loop_generated++;
					add_new_generated(v2, v1, v3); 
					add_new_generated(v3, v1, v2);  
              }
            }
          } 
        }// v2!=0
        else { 
          if(SecondWK[N-A][N+v1].clzs[0].var == CONSTANT1) { 
  				  ret = handle_single(v1);
				    if(ret == -1) return -1;
            continue;
          }
          lengthA_ = BasicWK[N-A].clzb[MAX_CLZB_NUM].var1; 
          for(j=0; j<lengthA_; j++){  
            if(!status && !BasicWK[N-A].clzb[j].status) continue;  
            v3 = BasicWK[N-A].clzb[j].var1;  // v3!=0
            v4 = BasicWK[N-A].clzb[j].var2;
            if(v3 == -v1 || v4 == -v1) continue;
            if(v3 != v1 && v4 != v1) { // 形如 AB + ~AXY ==> BXY; v3=X, v4=Y
              ret = add_new_generated(v1, v3, v4); 
              if(ret) {
                loop_generated++;
                add_new_generated(v3, v1, v4); 
                if(v4)add_new_generated(v4, v1, v3); 
			  }
            }
            else if(v3 == v1) {  
              ret = add_new_generated(v1, v4, 0); 
              if(ret) {
                loop_generated++;
                add_new_generated(v4, v1, 0); 
			  }
            }
            else if(v4 == v1 || !v4) {   
              ret = add_new_generated(v1, v3, 0); 
              if(ret) {
                loop_generated++;
                add_new_generated(v3, v1, 0); 
			  }
            }
          } 
          continue;   
        } // v2==0
        
       // 2) ABC + ~ACx/~AxC:  // v2!=0
       if(SecondWK[N-A][N+v2].clzs[0].var == CONSTANT1) { // ABC + ~AC ==> BC
          ret = add_new_generated(v1, v2, 0);
          if(ret) {
            loop_generated++;
            add_new_generated(v2, v1, 0);  
          }
          continue;
        }
        lengthS = SecondWK[N-A][N+v2].clzs[MAX_CLZS_NUM].var;
        for(j=0; j<lengthS; j++){ //  ABC + ~ACX ==> BCX; v3=X
          v3 = SecondWK[N-A][N+v2].clzs[j].var; // v3!=0
          index = SecondWK[N-A][N+v2].clzs[j].clzb_index;
          if(!status && !BasicWK[N-A].clzb[index].status) continue;  

          if(v3 == -v1) continue; // ABC + ~AC~B ==>  empty
          if(v3 == v1){           //   ABC + ~ACB ==> BC
				ret = add_new_generated(v1, v2, 0);  
				if(ret) {
				  loop_generated++;
				  add_new_generated(v2, v1, 0);  
				}
          }
          else {    //  ABC + ~ACD ==> BCD
				ret = add_new_generated(v1, v2, v3);  
				if(ret) {
				  loop_generated++;
				  add_new_generated(v2, v1, v3); 
				  add_new_generated(v3, v1, v2);  
				}
          }
        }// v2!=0

        // Case3): ABC + ~AD ==> BCD
		for(j=0; j<lengthD; j++){
			v3 = Double[j].var; 
			if(v3==-v1 || v3==-v2) continue; 
			if(!status && !Double[j].status) continue;  
			if(v3==v1 || v3==v2){
				ret = add_new_generated(v1, v2, 0);  
				if(ret) {
				  loop_generated++;
				  add_new_generated(v2, v1, 0);  
				}
			}
			else {
				ret = add_new_generated(v1, v2, v3);  
				if(ret) {
				  loop_generated++;
				  add_new_generated(v2, v1, v3); 
				  add_new_generated(v3, v1, v2);  
				}
			}
		}
      }// for(v=1;...)
 		A = 0;
		i = 0;
		while(i<lengthB){ 
			if(BasicWK[N+v].clzb[i].status != -1) { BasicWK[N+v].clzb[i].status = 0; i++; continue; }
			v1 = BasicWK[N+v].clzb[i].var1; 
			v2 = BasicWK[N+v].clzb[i].var2; 
			out(v, v1, v2);  
			lengthB--;
		}

		i = 0;
		while(i<lengthB_){  
			if(BasicWK[N-v].clzb[i].status != -1) { BasicWK[N-v].clzb[i].status = 0; i++; continue; }
			v1 = BasicWK[N-v].clzb[i].var1; 
			v2 = BasicWK[N-v].clzb[i].var2; 
 			out(-v, v1, v2);  
			lengthB_--;
		}
		}
	}while(loop_generated); 
	return clause_count();
}

void print_spectrum(){
	if(clause_count()<10000) return;
	else printf("\nclause=%ld\n",clause_count());
	for(int v=1; v<=N; v++){
		unsigned lengthB  = BasicWK[N+v].clzb[MAX_CLZB_NUM].var1; 
		unsigned lengthB_ = BasicWK[N-v].clzb[MAX_CLZB_NUM].var1; 
		printf("\n\n(%d:) (lengthB,lengthB_)=(%d, %d)", v, lengthB, lengthB_);
		if(!lengthB || !lengthB_) continue;

		for(int v1=1; v1<=N; v1++){
			if(v1==v) continue;
			unsigned lengthS1 = SecondWK[N+v][N+v1].clzs[MAX_CLZS_NUM].var; 
			unsigned lengthS2 = SecondWK[N+v][N-v1].clzs[MAX_CLZS_NUM].var;
			unsigned lengthS3 = SecondWK[N-v][N+v1].clzs[MAX_CLZS_NUM].var; 
			unsigned lengthS4 = SecondWK[N-v][N-v1].clzs[MAX_CLZS_NUM].var;
			printf("\n(lengthB,lengthB_)=(%d, =%d). (%d:%d::) (lengthS1, lengthS2, lengthS3, lengthS4)=(%d, %d; %d, %d)", lengthB, lengthB_, v, v1, lengthS1, lengthS2, lengthS3, lengthS4);
		}
	}
	Exit(-9);
}

