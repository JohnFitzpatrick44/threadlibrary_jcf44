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
	thread_lock(1);
	thread_lock(2);
	thread_lock(3);
	cout << "1 has 1, 2, 3" << endl;
	
	thread_unlock(1);
	cout << "1 has 2, 3" << endl;
	thread_yield();
	thread_unlock(2);

	thread_unlock(3);


}

void test2(void* arg) {
	cout << "2 runs" << endl;
	thread_lock(1);
	cout << "2 has 1" << endl;
	thread_unlock(1);
}

void test3(void* arg) {
	cout << "3 runs" << endl;
	thread_lock(2);
	cout << "3 has 2" << endl;
	thread_unlock(2);
}

void test4(void* arg) {
	cout << "4 runs" << endl;
	thread_lock(3);
	cout << "4 has 3" << endl;
	thread_unlock(3);
}

void maker(void* arg) {

	thread_create((thread_startfunc_t) test1, NULL);
	thread_create((thread_startfunc_t) test2, NULL);
	thread_create((thread_startfunc_t) test3, NULL);
	thread_create((thread_startfunc_t) test4, NULL);
	
}



int main() {

	thread_libinit((thread_startfunc_t) maker, NULL);
	
}
