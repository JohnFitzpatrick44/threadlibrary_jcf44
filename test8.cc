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
	thread_lock(1);
	cout << "1 runs" << endl;
	if(thread_signal(1,1) == -1) {
		cout << "signal failed" << endl;
	}
	thread_unlock(1);
}

void test2(void* arg) {
	if(thread_lock(1) == -1) {
		cout << "lock failed" << endl;
	}
	cout << "2 runs" << endl;
	if(thread_create((thread_startfunc_t) test1, NULL) == -1) {
		cout << "create failed" << endl;
	}

	if(thread_wait(1,1) == -1) {
		cout << "wait failed" << endl;
	}
	cout << "done waiting" << endl;

	if(thread_broadcast(1,1) == -1) {
		cout << "broadcast failed" << endl;
	}

	if(thread_yield() == -1) {
		cout << "yield failed" << endl;
	}

	if(thread_unlock(1) == -1) {
		cout << "unlock failed" << endl;
	}
}



void maker(void* arg) {

	thread_create((thread_startfunc_t) test2, NULL);
	
}



int main() {

	thread_libinit((thread_startfunc_t) maker, NULL);
	
}
