#include "thread.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <algorithm>


// TESTS IF CONDITION VARIABLES ARE TIED TO LOCKS OR NOT


using namespace std;

int lock1 = 1;
int lock2 = 2;
int cv = 3;

void test1(void* arg) {

	thread_lock(lock1);
	thread_wait(lock1, cv);
	thread_unlock(lock1);
	cout << "One triggered, failed" << endl;
}

void test2(void* arg) {

	thread_lock(lock2);
	thread_wait(lock2, cv);
	thread_unlock(lock2);
	cout << "Two triggered, correct" << endl;
}

void test3(void* arg) {
	thread_yield();
	thread_lock(lock2);
	thread_broadcast(lock2, cv);
	thread_unlock(lock2);
}

void maker(void* arg) {
	
	thread_create((thread_startfunc_t) test1, NULL);
	thread_create((thread_startfunc_t) test2, NULL);
	thread_create((thread_startfunc_t) test3, NULL);
	
}



int main() {

	thread_libinit((thread_startfunc_t) maker, NULL);
	
}
