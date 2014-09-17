// SAT60.h
#include <stdio.h>
#include <stdlib.h>
#include <fstream> 
#include <string.h>
#include <conio.h>
#include <time.h>

#define CONSTANT1 32767  // when return -1 if ~A+A was found.
#define MAX_WIDTH 3  // how many variables in one item (speically for new item)
#define MAX_M  100001  // for item[]
#define MAX_M_IN_SECONDBK 500001 // for SecondBK[]
#define MAX_VAR_NUM   500
#define MAX_CLZB_NUM  2000
#define MAX_CLZS_NUM  100 
#define LEVEL 8

typedef struct Subexpress   /*struct of item*/
{
	int var[MAX_WIDTH];    /* -32767~+32768. eg., -17 means ~x17, 17 means x17. Specially 0 means no variable */
	int var_num;   
}Subexpress;

typedef struct ClauseBasic    
{
	int var1;
	int var2;
	int status;
}ClauseBasic;

typedef struct ClauseSecondary    
{
	int var;
	unsigned clzb_index; //index of clause in BasicWK[]. 
}ClauseSecondary;

typedef struct BasicArray    
{
	ClauseBasic clzb[MAX_CLZB_NUM+1];
}BasicArray;

typedef struct SecondaryArray    
{
	ClauseSecondary clzs[MAX_CLZS_NUM+1];
}SecondaryArray;

typedef struct DoubleProber    
{
	int var1;
	int var2;
	unsigned long length;
}DoubleProber;

typedef struct SecondaryBackArray    
{
	int v;
	int v1;
	unsigned j; // clause index in SecondWK[][]
	int var;
	unsigned clzb_index;
}SecondaryBackArray;

Subexpress *item;

BasicArray BasicBK[LEVEL][MAX_VAR_NUM*2+1];
BasicArray BasicWK[MAX_VAR_NUM*2+1];
SecondaryArray SecondWK[MAX_VAR_NUM*2+1][MAX_VAR_NUM*2+1];  // work data of each loop
SecondaryBackArray SecondBK[MAX_M_IN_SECONDBK];

//int Depth[LEVEL]={5,5,5,6,6,6,6,6};
//int Depth[LEVEL]={6,5,5,4,4,3,3,3};
int Depth[LEVEL]={6,6,6,6,6,6,6,6};
int prober0[LEVEL][30]; 
int prober[30]; // an assignment of prober0[], for each probing loop.
#define TIMETEST 0 
#define TESTLOOP 10
int testloop = 1 << TESTLOOP; 

int Level;  // current level, 0~LEVEL-1
unsigned Max_Level_M;

int  N=0;  /* Really how many variable in the given 3SAT express.*/
unsigned long M=0;  // the total number of clauses
char const* filename;
std::ifstream pFile;
//int file_no;  // 0~9, standing for 3SAT file D:\3SAT\3SAT0, 3SAT1...

char *line;          // Read one line from 3SATx.txt 
char *str_int;       /* To store the index of variable temporarily. */
int variable[MAX_VAR_NUM+1];
unsigned I0[LEVEL];//, loop;
int Down=1; //go down to next level? or go up to precious level? defaut is going down.
unsigned long total_generated = 0;
int A;   
time_t start1,end1,start2,end2;  // running time
double t_1=0.0;
double t_2=0.0;
double t_expand=0.0;

int max_lengthB=0;  
int max_lengthS=0;  
unsigned max_loop_generated=0;
unsigned max_total_clz_num=0;

void initialize(); 
unsigned add_new_generated(int v1, int v2, int v3);
int add_to_Backup(int v, int v1, int v2);
int back_data();
unsigned long expand(); 
int handle_single(int single_var);
int level();
int load_data();
void out(int v1, int v2, int v3);
void select_prober();
void select_prober_v2();
void set_prober(unsigned I);

void read_items();
void solve();
void solution_check();
void solution_print();
void printf_info(int);
void print_WK();
void Exit(int);

unsigned add_new_generated_called=0;
unsigned add_to_Backup_called=0;
unsigned back_data_called=0;
unsigned expand_called=0;
unsigned handle_single_called=0;
unsigned level_called=0;
unsigned load_data_called=0;
unsigned out_called=0;
unsigned select_prober_called=0;
unsigned set_prober_called=0;
unsigned total_loops=0;

void clause_generator(unsigned n);
unsigned long clause_count();  
unsigned long extra_expand();
void thin();
unsigned long expand_v2();
void print_spectrum();
back_data_v2();
int load_data_v2();

