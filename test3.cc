#include "thread.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <algorithm>


// TESTS EARLY CREATES, LOCKS, ETC, AND MULTIPLE LIBINITS


using namespace std;

int lock1 = 1;
int lock2 = 2;
int cv = 3;

void test1(void* arg) {

	thread_lock(lock1);
	cout << "1 locked" << endl;
	thread_unlock(lock1);
}

void test2(void* arg) {

	thread_lock(lock1);
	cout << "2 locked" << endl;
	thread_unlock(lock1);
}



void maker(void* arg) {
	
	if(thread_libinit((thread_startfunc_t) maker, NULL) == -1) {
		cout << "Second init failed" << endl;
	}

	thread_create((thread_startfunc_t) test1, NULL);
	thread_create((thread_startfunc_t) test2, NULL);
	
}



int main() {

	if(thread_create((thread_startfunc_t) test1, NULL) == -1) {
		cout << "Create before init failed" << endl;
	}

	if(thread_yield() == -1) {
		cout << "Yield before init failed" << endl;
	}

	if(thread_lock(lock1) == -1) {
		cout << "Lock before init failed" << endl;
	}

	if(thread_unlock(lock1) == -1) {
		cout << "Unlock before init failed" << endl;
	}

	if(thread_wait(lock1, lock2) == -1) {
		cout << "Wait before init failed" << endl;
	}

	if(thread_signal(lock1, lock2) == -1) {
		cout << "Signal before init failed" << endl;
	}

	if(thread_broadcast(lock1, lock2) == -1) {
		cout << "Broadcast before init failed" << endl;
	}

	thread_libinit((thread_startfunc_t) maker, NULL);
	
}
