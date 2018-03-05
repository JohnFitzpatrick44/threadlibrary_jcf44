#include "thread.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <algorithm>

// TESTS IF A THREAD CAN EFFECTIVELY HOLD MULTIPLE LOCKS

using namespace std;

int lock1 = 1;
int lock2 = 2;
int cv = 3;

void test1(void* arg) {

	thread_lock(lock1);
	cout << "1 has lock 1" << endl;
	thread_lock(lock2);
	cout << "1 has both locks" << endl;
	cout << "1 yields" << endl;
	thread_yield();
	thread_unlock(lock2);
	cout << "1 has lock 1" << endl;
	cout << "1 yields" << endl;
	thread_yield();
	thread_unlock(lock1);
	cout << "1 is exiting" << endl;
}

void test2(void* arg) {

	thread_lock(lock2);
	cout << "2 has lock 2" << endl;
	cout << "2 yields" << endl;
	thread_yield();
	thread_unlock(lock2);
	cout << "2 is exiting" << endl;
}

void test3(void* arg) {
	thread_lock(lock1);
	cout << "3 has lock 1" << endl;
	cout << "3 yields" << endl;
	thread_yield();
	thread_unlock(lock1);
	cout << "3 is exiting" << endl;
}

void maker(void* arg) {
	
	thread_create((thread_startfunc_t) test1, NULL);
	thread_create((thread_startfunc_t) test2, NULL);
	thread_create((thread_startfunc_t) test3, NULL);
	
}



int main() {

	thread_libinit((thread_startfunc_t) maker, NULL);
	
}
