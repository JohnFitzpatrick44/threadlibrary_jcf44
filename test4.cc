#include "thread.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <algorithm>


// TESTS LOCKING / UNLOCKING / WAITING ON LOCKS INCORRECTLY


using namespace std;

int lock1 = 1;
int lock2 = 2;
int cv = 3;

void test1(void* arg) {

	if(thread_unlock(lock1) == -1) {
		cout << "1: Unlocking unacquired lock failed" << endl;
	}

	thread_lock(lock1);
	if(thread_lock(lock1) == -1) {
		cout << "1: Locking already acquired lock failed" << endl;
	}
	thread_yield();
	thread_yield();
	thread_yield();
	thread_unlock(lock1);
}

void test2(void* arg) {

	if(thread_unlock(lock1) == -1) {
		cout << "2: Unlocking unacquired lock failed" << endl;
	}
	
}

void test3(void* arg) {
	if(thread_wait(lock1, cv) == -1) {
		cout << "3: Waiting on unacquired lock failed" << endl;
	}
}

void maker(void* arg) {
	
	thread_create((thread_startfunc_t) test1, NULL);
	thread_create((thread_startfunc_t) test2, NULL);
	thread_create((thread_startfunc_t) test3, NULL);
	
}



int main() {

	thread_libinit((thread_startfunc_t) maker, NULL);
	
}
