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

	thread_lock(order);
	thread_yield();
	cout << "1" << endl;
	thread_unlock(order);
}

void test2(void* arg) {

	thread_lock(order);
	cout << "2" << endl;
	thread_unlock(order);
}

void test3(void* arg) {
	thread_yield();
	thread_lock(order);
	cout << "3" << endl;
	thread_unlock(order);
}

void maker(void* arg) {
	
	thread_create((thread_startfunc_t) test1, NULL);
	thread_create((thread_startfunc_t) test2, NULL);
	thread_create((thread_startfunc_t) test3, NULL);
	
}



int main() {

	thread_libinit((thread_startfunc_t) maker, NULL);
	
}
