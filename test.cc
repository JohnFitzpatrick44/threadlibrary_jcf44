#include "thread.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <algorithm>

using namespace std;

int order = 0;

void testThread(void* arg) {

	thread_lock(order);
	thread_yield();
	cout << (long int) arg << endl;
	thread_unlock(order);
}

void maker(void* arg) {
	long int i;
	for(i = 0; i < (long int) arg; i++) {
		thread_create((thread_startfunc_t) testThread, (void*) i);
	}
	
}



int main(int argc, char* argv[]) {

	int index = atoi(argv[1]);

	thread_libinit((thread_startfunc_t) maker, (void*) index);
}
