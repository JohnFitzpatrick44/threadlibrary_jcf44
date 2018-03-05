#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include "thread.h"

using namespace std;

int lock1 = 1;
int lock2 = 2;
int cv = 3;

void test1(void* args) {
        thread_lock(lock2);

        thread_lock(lock1);

        thread_signal(lock1, cv);

        thread_unlock(lock1);

        thread_unlock(lock2);
}


void test2(void* args) {
        thread_lock(lock1);
        thread_create((thread_startfunc_t) test1, NULL);
        thread_lock(lock2);
        thread_wait(lock1, cv);
        cout << "failed, not deadlocked" << endl;
        thread_unlock(lock2);
        thread_unlock(lock1);
        cout << "failed, out of deadlock" << endl;
}



int main(){
    thread_libinit((thread_startfunc_t) test2, NULL);
}