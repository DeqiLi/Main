/*
	This program is to generate a random 3SAT instance with n variables and m clauses.
	The generated 3SAT instance may have no solution.
	Coded by Deqi Li, Jan 25,2012
  */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <conio.h>
#include <math.h>
#include <time.h>

void main(int argc, char *argv[]){
	unsigned n,m;
	int var1,var2,var3;
	int min, mid, max;

	if(argc>2) { 
		n=atoi(argv[1]); 
		m=atoi(argv[2]); 
	}
	else{
		//printf("Usage example: generator 100 430 \n");
		//exit(0);
		n=100;
		m=430;
	}

	srand((unsigned int)time(0));
	
	for(int i=0; i<m; i++) {
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

		printf(" %4d %4d %4d ",min,mid,max);
		if(i<m-1) printf("\n");
	}

	return;
}