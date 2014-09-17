/*
 * SortCompare.cpp
 *
 *  Created on: Jun 29, 2014
 *      Author: adamant (Deqi Li)
 */

/*
 * Compare the differrence of optimization of gcc -O2 between
 * --integer sort,
 * --quick sort,
 * --merging sort and
 * --trivial sort.
 *
 * Verify my guess that the gcc -O2 optimization is on algorithm level.
 * Deqi Li, Jun 26, 2014 @UoA
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <sstream>
#include <iomanip>      // std::setw
#include <algorithm>
#include <cassert>

using namespace std;

/*
class miniRand {
private:
	int seed;

public:
	miniRand(int initSeed) {
		seed = initSeed;
	}

	inline int mRand() {
		int x = seed;
		int size = sizeof(x)*8;

		//for(int i=1; i<size; i++) {
			//x = x | seed & (1<<(i-1));
		//}

		seed = (x << 1 ) | (int) ( (x & (1 << (size-1))) != (x & (1 << (size-2))) );  // seed_i = x_(i-1) for i >=1; seed0 = x_(size-1) XOR x_(size-2).
		return x;
	}
};
*/

class Sort {
//private:
protected:
	int N;
	vector<int>data;
	//void sortImplementation() {

	//}

public:
	Sort() {
		N=100000000;

		DataGenerate();
		cout << "data generated\n";
	}

	//~Sort();

	void DataGenerate() {
		srand(2014);
		data.resize(N);
		for (vector<int>::iterator it = data.begin(); it != data.end(); it++) {
			*it = rand();
		}
	}

	void verifyResult() {
		std::vector<int>::const_iterator it;
		for( it=data.begin(); it != data.begin() + 5; it++) {
			cout << setw(6) << *it << " ";
		}
		cout << "...\n";
		for( it =  data.end()-5; it != data.end(); it++) {
			cout << setw(6) << *it << " ";
		}
		cout << endl;
	}
};

class IntegerSort: public Sort {
public:
	void doSort() {
		cout << "data sorting starts, timing...\n";
		time_t start=time(NULL);

		if (data.size()==0){
			return;
		}

		sortImplementation();

		cout << "\nN=" << N << endl;
		cout << "------------------------------\n";
		cout << "IntegerSort time used: " << time(NULL) - start << endl;
		cout << "------------------------------\n";
		cout << "data sorted.\n";
	}

private:
	void sortImplementation() {
		integer_sort(0, data.size()-1);
	}

	void integer_sort(int p, int r) {
		int max=0;
		vector<int>::iterator it;
		for (it = data.begin(); it != data.end(); it++) {
			if(max<*it) max = *it;
		}
		vector<int>radix(max+1);

		for (it = radix.begin(); it != radix.end(); it++) {
			*it = 0;
		}

		for (it = data.begin(); it != data.end(); it++) {
			radix[*it]++;
		}

		int i, j, count;
		count = 0;
		for (i = 0; i < 5; i++) {//radix.begin(); it != radix.begin()+5; it++) {
			for(j = 0; j < radix[i]; j++) { cout << i << " "; count++; if(count==5) break; }
			if(count==5) break;
		}
		count = 0;
		for (i = radix.size()-5; i < radix.size(); i++) {//radix.begin(); it != radix.begin()+5; it++) {
			for(j = 0; j < radix[i]; j++) { cout << i << " "; count++; if(count==5) break; }
			if(count==5) break;
		}
/*
		for (it = radix.size -5; it != radix.end(); it++) {
			cout << *it << " ";
		}*/
		//return radix;
	}
};

class QuickSort: public Sort {
public:

	void doSort() {
		cout << "data sorting starts, timing...\n";
		time_t start=time(NULL);

		if (data.size()==0){
			return;
		}

		sortImplementation();
		cout << "N=" << N << endl;
		cout << "------------------------------\n";
		cout << "QuickSort time used: " << time(NULL) - start << endl;
		cout << "------------------------------\n";
		cout << "data sorted.\n";
	}

private:
	int M;
	void sortImplementation() {
		M = 8;
		Quicksort(0, data.size()-1);
	}

	void Quicksort(int low, int high) {
		int i = low, j = high;
		// Get the pivot element from the middle of the list
		if((high - low + 1) <= M) {
			insertion_sort(low, high);
			return;
		}

		int pivot = data[low + (high-low)/2];

		//System.out.println("qSort from " + low + " to "+ high);
		// Divide into two lists
		while (i <= j) {
			// If the current value from the left list is smaller than the pivot,
			// then get the next element from the left list
			while (data[i] < pivot) {
				i++;
				//qOps++;
			}
			// If the current value from the right list is greater than the pivot,
			// then get the next element from the right list
			while (data[j] > pivot) {
				j--;
				//qOps++;
			}

			// If we have found a values in the left list which is larger than
			// the pivot and if we have found a value in the right list
			// which is smaller than the pivot, then we exchange the
			// values.
			// As we are done we can increase i and decrease j
			if (i <= j) {
				exchange(i, j);
				i++;
				j--;
				//qOps++;
			}
		}
		// Recursion
		if (low < j)
			Quicksort(low, j);
		if (i < high)
			Quicksort(i, high);
	}

	void exchange(int i, int j) {
		int temp = data[i];
		data[i] = data[j];
		data[j] = temp;
	}

	// when array.length is less than M, it will be called.
	void insertion_sort(int low, int high) {
		for (int i = low; i <= high; i++) {
            int valueToSort = data[i];
            int j = i;
            while (j > 0 && data[j - 1] > valueToSort) {
            	data[j] = data[j - 1];
                j--;
            }
            data[j] = valueToSort;
        }
		//System.out.println("iSort from " + low + " to "+ high);
	}
};

class MergeSort: public Sort {
public:
	void doSort() {
		cout << "data sorting starts, timing...\n";
		time_t start=time(NULL);

		if (data.size()==0){
			return;
		}

		sortImplementation();
		cout << "N=" << N << endl;
		cout << "------------------------------\n";
		cout << "MergeSort time used: " << time(NULL) - start << endl;
		cout << "------------------------------\n";
		cout << "data sorted.\n";
	}

private:
	void sortImplementation() {
		merge_sort(0, data.size()-1);
	}

	void merge(int p, int r)
	{
		int mid = floor((p + r) / 2);
		int i1 = 0;
		int i2 = p;
		int i3 = mid + 1;

		// Temp array
		int * temp = new int[r-p+1];

		// Merge in sorted form the 2 arrays
		while ( i2 <= mid && i3 <= r )
			if ( data[i2] < data[i3] )
				temp[i1++] = data[i2++];
			else
				temp[i1++] = data[i3++];

		// Merge the remaining elements in left array
		while ( i2 <= mid )
			temp[i1++] = data[i2++];

		// Merge the remaining elements in right array
		while ( i3 <= r )
			temp[i1++] = data[i3++];

		// Move from temp array to master array
		for ( int i = p; i <= r; i++ )
			data[i] = temp[i-p];
	}

	void merge_sort(int p, int r)
	{
		if ( p < r )
		{
			int mid = floor((p + r) / 2);
			merge_sort(p, mid);
			merge_sort(mid + 1, r);
			merge(p, r);
		}
	}
};

class TrivialSort: public Sort {
public:
	void doSort() {
		cout << "data sorting starts, timing...\n";
		time_t start=time(NULL);

		if (data.size()==0){
			return;
		}

		sortImplementation();

		cout << "N=" << N << endl;
		cout << "------------------------------\n";
		cout << "TrivialSort time used: " << time(NULL) - start << endl;
		cout << "------------------------------\n";
		cout << "data sorted.\n";
	}

private:
	void sortImplementation() {
		selection_sort(0, data.size()-1);
	}

	void selection_sort(int p, int r) {
		int a, min;
		vector<int>::iterator minP;
		for (vector<int>::iterator it1 = data.begin(); it1 != data.end(); it1++) {
			a = *it1;
			min = a;
			for (vector<int>::iterator it2 = it1+1; it2 != data.end(); it2++) {
				if(*it2 < min) { min = *it2; minP = it2; }
			}
			*it1 = min;
			*minP = a;
		}
	}
};

int main(int argc, char ** agrv){
	//miniRand mR(51);
	//for(int i=0; i<10; i++) {
		//cout << mR.mRand()  <<" \n";// << mR.mRand() <<" "<< mR.mRand() << endl;
	//}

	//cout << "input n:\n";
	//cin >> Sort.N;

	cout << "\n------------------------------------------------------------\n";
	cout << "IntegerSort:\n";
	IntegerSort IS;
	IS.verifyResult();
	IS.doSort();
	//IS.verifyResult();
	cout << "------------------------------------------------------------\n";

	cout << "\n------------------------------------------------------------\n";
	cout << "QuickSort:\n";
	QuickSort QS;
	QS.verifyResult();
	QS.doSort();
	QS.verifyResult();
	cout << "------------------------------------------------------------\n";

	cout << "\n------------------------------------------------------------\n";
	cout << "MergeSort:\n";
	MergeSort MS;
	MS.verifyResult();
	MS.doSort();
	MS.verifyResult();
	cout << "------------------------------------------------------------\n";

	cout << "\n------------------------------------------------------------\n";
	cout << "TrivialSort:\n";
	TrivialSort TS;
	TS.verifyResult();
	TS.doSort();
	TS.verifyResult();
	cout << "------------------------------------------------------------\n";

	cout << "\nSort Competition Finished.\n";
	return 0;
}

/*
 *
 * Testing results:
 * n=10,000,000:  -O0      -O3       speedup
 * Integer:       2         0         ?
 * Quick:         8         3         2.6
 * Merge:         14        6         2.3


 * n=100,000,000: -O0      -O3       speedup
 * Integer:       11        0        ?
 * Quick:         96        30       3.2
 * Merge:         ?         ?        ?    // mem allocation failed

 * n=100,000:     -O0      -O3       speedup
 * Trivial:       299       19        15

 * */

