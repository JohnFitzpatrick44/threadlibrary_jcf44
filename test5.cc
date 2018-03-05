#include "thread.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <algorithm>

using namespace std;


// TESTS HANDOFF LOCKS OR NOT

int order = 0;

void test1(void* arg) {
	cout << "1 runs" << endl;
}

void test2(void* arg) {
	cout << "2 runs" << endl;
}

void test3(void* arg) {
	cout << "3 runs" << endl;
	if(thread_create((thread_startfunc_t) test2, NULL) == -1) {
		cout << "Creating thread in a thread failed" << endl;
	}
}

void maker(void* arg) {

	thread_create((thread_startfunc_t) test1, NULL);
	thread_create((thread_startfunc_t) test3, NULL);
	
}



int main() {

	thread_libinit((thread_startfunc_t) maker, NULL);
	
}
