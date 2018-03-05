#include "thread.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <sys/time.h>

using namespace std;


// TESTS HANDOFF LOCKS OR NOT

int order = 0;

void test1(void* arg) {
	cout << rand() << endl;
}

void test2(void* arg) {

	
}

void test3(void* arg) {
	
}

void maker(void* arg) {

	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	srand(ms);
	thread_create((thread_startfunc_t) test1, NULL);
	thread_create((thread_startfunc_t) test2, NULL);
	thread_create((thread_startfunc_t) test3, NULL);
	
}



int main() {

	thread_libinit((thread_startfunc_t) maker, NULL);
	
}
