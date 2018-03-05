#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "interrupt.h"
#include "thread.h"
#include <queue>
#include <deque>
#include <map>
using namespace std;

struct thread { 
    ucontext_t* u_ptr; 
    bool completed; 
};

static deque <thread*> ready;
static queue <thread*> blocked;
static map <int, void*> waitingQueues;
static map <unsigned int, int> lockMap; 
static bool initiated = false;
static ucontext_t* prevContext;

static thread * currentThread = NULL;
static thread * mainThread = NULL;



int start(thread_startfunc_t func, void *arg){

    if(!initiated) {
    	interrupt_enable();
    	return -1;
    }

    interrupt_enable();
    func(arg);
    interrupt_disable();

    currentThread->completed = true;
    if(swapcontext(currentThread->u_ptr, mainThread->u_ptr) == -1) {
    	interrupt_enable();
    	return -1;
    }
    interrupt_enable();
   	return 0;
}


extern int thread_libinit(thread_startfunc_t func, void *arg) {
	interrupt_disable();

    if(initiated) {
    	interrupt_enable();
    	return -1;
    }

    initiated = true;

    thread * first = NULL;

    try{
        first = new thread;
        first->u_ptr = new ucontext_t;
        first->completed = false;
    }
    catch (std::bad_alloc& ba){
        interrupt_enable();
        return -1;
    }

    if(first == NULL || getcontext(first->u_ptr) == -1){
        interrupt_enable();
        return -1;
    }

    //doing the ucontext things
    try{
	    first->u_ptr->uc_stack.ss_sp =  new char[STACK_SIZE];
	    first->u_ptr->uc_stack.ss_size = STACK_SIZE;
	    first->u_ptr->uc_stack.ss_flags = 0;
	    first->u_ptr->uc_link = NULL;
    }
    catch (std::bad_alloc& ba){
        interrupt_enable();
        return -1;
    }

    makecontext(first->u_ptr, (void (*)()) start, 2, func, arg);

    thread * main = NULL;

    try{
	    main = new thread;
	    main->u_ptr = new ucontext_t;
	    main->completed = false;
    }
    catch (std::bad_alloc& ba){
        interrupt_enable();
        return -1;
    }
    
    if(getcontext(main->u_ptr) == -1){
        interrupt_enable();
        return -1;
    }

    //doing the ucontext things

    try{
	    main->u_ptr->uc_stack.ss_sp =  new char[STACK_SIZE];
	    main->u_ptr->uc_stack.ss_size = STACK_SIZE;
	    main->u_ptr->uc_stack.ss_flags = 0;
	    main->u_ptr->uc_link = NULL;
    }
    catch (std::bad_alloc& ba){
        interrupt_enable();
        return -1;
    }

    mainThread = main;

    if(ready.size() != 0) {
        interrupt_enable();
        return -1;
    }

    ready.push_back(first);

    while(ready.size()>0){

        thread * next = ready.front();
        ready.pop_front();

        currentThread = next;

        if(swapcontext(mainThread->u_ptr, next->u_ptr) == -1) { 
            interrupt_enable(); 
            return -1; 
        } 

        if(currentThread->completed == true){
            delete (char*) currentThread->u_ptr->uc_stack.ss_sp;
            delete currentThread->u_ptr;
            delete currentThread;
        }

    }
    
    delete (char*) mainThread->u_ptr->uc_stack.ss_sp;
    delete mainThread->u_ptr;
    delete mainThread;

    // clear queues

    while(blocked.size() > 0) {
   		delete blocked.front();
    	blocked.pop();
    }

    map<int, void*>::iterator it;
	for ( it = waitingQueues.begin(); it != waitingQueues.end(); it++ ) {
	     delete (queue<thread*> *) (it->second);
	}


    interrupt_enable();
    cout << "Thread library exiting.\n";
    exit(0);
}

extern int thread_create(thread_startfunc_t func, void *arg) {
    interrupt_disable();

    if(!initiated) {
        interrupt_enable();
        return -1;
    }

    thread * newThread = NULL;

    try{
	    newThread = new thread;
	    newThread->u_ptr = new ucontext_t;
	    newThread->completed = false;
    }
    catch (std::bad_alloc& ba){
        interrupt_enable();
        return -1;
    }

    if(getcontext(newThread->u_ptr) == -1) {
        interrupt_enable();
        return -1;
    }

    try{
	    newThread->u_ptr->uc_stack.ss_sp = new char[STACK_SIZE];
	    newThread->u_ptr->uc_stack.ss_size = STACK_SIZE;
	    newThread->u_ptr->uc_stack.ss_flags = 0;
	    newThread->u_ptr->uc_link = NULL;
    }
    catch (std::bad_alloc& ba){
        interrupt_enable();
        return -1;
    }

    makecontext(newThread->u_ptr, (void (*)()) start, 2, func, arg);

    ready.push_back(newThread);

    interrupt_enable();
}

extern int thread_yield(void) {

    interrupt_disable();

    if(!initiated) {
        interrupt_enable();
        return -1;
    }

    ready.push_back(currentThread);
    //thread * next = ready.front();
    //ready.pop();

	if(swapcontext(currentThread->u_ptr, mainThread->u_ptr) == -1) {	// USED TO BE NEXT
		interrupt_enable();
		return -1;
	}
	interrupt_enable();
	//currentThread = next;
	return 0;


}

extern int thread_lock(unsigned int lock) {
    interrupt_disable(); 

	if(!initiated) {
    	interrupt_enable();
    	return -1;
    }

    map<unsigned int, int>::iterator it = lockMap.find(lock);

    if(it==lockMap.end()) {  
        lockMap.insert(pair<unsigned int, int>(lock, 1));
    } else if(it->second == 0) {
    	lockMap.at(lock) = 1;
    }
 

	else { 
        //interrupt_enable(); 
 
        blocked.push(currentThread); 
        //thread* next = ready.front(); 
        //ready.pop(); 
        //swapcontexts 
        if(swapcontext(currentThread->u_ptr, mainThread->u_ptr) == -1) { 
            interrupt_enable(); 
            return -1; 
        }  
     
    } 
    interrupt_enable();
    return 0;
}


extern int thread_unlock(unsigned int lock) {
    interrupt_disable(); 

	if(!initiated) {
    	interrupt_enable();
    	return -1;
    }

    map<unsigned int, int>::iterator it = lockMap.find(lock);

    if(it == lockMap.end()){ 
        interrupt_enable(); 
        return -1; 
    } 
    else { 
        lockMap.at(lock)=0; 
     
        if(!blocked.empty()){ 
            ready.push_front(blocked.front());
            blocked.pop(); 
        } 
    }
    interrupt_enable();
    return 0;
}


extern int thread_wait(unsigned int lock, unsigned int cond) {

    interrupt_disable();

	if(!initiated) {
    	interrupt_enable();
    	return -1;
    }

    map<int, void*>::iterator it = waitingQueues.find(cond);

    queue<thread*> * cv_queue;

    if (it == waitingQueues.end()) {
    	queue <thread*> * newQueue = NULL;
        try{
        	newQueue = new queue <thread*>;
        	waitingQueues[cond] = newQueue;
        }
        catch (std::bad_alloc& ba){
        	interrupt_enable();
        	return -1;
        }


        waitingQueues.insert(pair<int, void*>(cond, newQueue));
        cv_queue = newQueue;
    } else {

        cv_queue = (queue<thread*> *)it->second;

    }

    (*cv_queue).push(currentThread);

	interrupt_enable();
	thread_unlock(lock);
	interrupt_disable();
	if(swapcontext(currentThread->u_ptr, mainThread->u_ptr) == -1) {
		interrupt_enable();
		return -1;
	}
	interrupt_enable();
	thread_lock(lock);

	return 0;

}
extern int thread_signal(unsigned int lock, unsigned int cond) {
    interrupt_disable();

	if(!initiated) {
    	interrupt_enable();
    	return -1;
    }

    map<int, void*>::iterator it = waitingQueues.find(cond);

    if(it == waitingQueues.end()) {
        interrupt_enable();
        return 0;
    }

    queue<thread*> * cv_queue = (queue<thread*>*)it->second;

	if ((*cv_queue).size() == 0) {
		interrupt_enable();
    	return 0;
	}

    ready.push_back( (*cv_queue).front() );
    (*cv_queue).pop();

    interrupt_enable();
    return 0;
}


extern int thread_broadcast(unsigned int lock, unsigned int cond) {
    interrupt_disable();

    if(!initiated) {
        interrupt_enable();
        return -1;
    }

    map<int, void*>::iterator it = waitingQueues.find(cond);

    if(it == waitingQueues.end()) {
        interrupt_enable();
        return 0;
    }

    queue<thread*> * cv_queue = (queue<thread*>*)it->second;

    while((*cv_queue).size() != 0) {
        ready.push_back( (*cv_queue).front() );
        (*cv_queue).pop();
    }

    interrupt_enable();
    return 0;
}
