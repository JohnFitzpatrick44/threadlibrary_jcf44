#include "thread.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <algorithm>

using namespace std;

// Lock and CV values
int ORDER_LOCK = 1;
int CASHIER_CV = 200000;
int MAKER_CV = 400000;

// Shared variables
vector <int> orderVector;
int aliveCashiers;

// Variables set in main, they do not change
int numCashiers;
int boardCapacity;

char** files;

// Cashier thread for making orders. 
// Given unique IDs, starting at 0, using cashID and ID_LOCK lock.
// They parse their files into a queue. 
// When adding orders, add sandwich number + cashier ID * 1000, so thousands place is cashier ID.
void cashier(void* arg) {

	int myID = (long int) arg - 2;
	queue <int> cashQueue;

	ifstream streamFile(files[myID+2]);	// Parses file
	if(!streamFile) {
		return;
	}

	while(!streamFile.eof()) {	// Parses until it hits end
		char a[8];				// Max sandwich value is 999, which is 3 digits, 8 is plenty
		streamFile >> a;
		if(streamFile.eof()) {	// Must add this check to make sure an empty line won't add a duplicate sandwich
			break;
		}
		cashQueue.push( atoi(a) );	// Adds sandwich number to queue
	}


	thread_lock(ORDER_LOCK);

	while(!cashQueue.empty()) {		// Repeats until out of orders
		while(boardCapacity <= orderVector.size()) {	// Checks if board is at capacity
			thread_wait(ORDER_LOCK, CASHIER_CV);		// Waits on Cashier CV
		}

		int orderID = cashQueue.front() + myID * 1000;
		orderVector.push_back(orderID);					// Places order
		cout << "POSTED: cashier " << myID << " sandwich " << cashQueue.front() << endl; 
		cashQueue.pop();	


		thread_signal(ORDER_LOCK, MAKER_CV);	// Signals maker that an order has been placed
		thread_wait(ORDER_LOCK, orderID);		// Waits for (unique) CV

	}


	aliveCashiers--;	// Decrements alive cashiers to track when all orders are completed


	thread_signal(ORDER_LOCK, MAKER_CV); //NEEDS TO SIGNAL WHEN EXITING

	thread_unlock(ORDER_LOCK);

}

void sandwichMaker(void* arg) {

	int lastSandwich = -1;
	int maxOrders = boardCapacity;

	thread_lock(ORDER_LOCK);


	while(aliveCashiers != 0) {

		maxOrders = min(boardCapacity, aliveCashiers);

		while(orderVector.size() < maxOrders) {
			//thread_broadcast(ORDER_LOCK, CASHIER_CV);
			thread_wait(ORDER_LOCK, MAKER_CV);		// There are no other makers, so if maker is signaled, there is definitely a sandwich to be made
			maxOrders = min(boardCapacity, aliveCashiers);
		}

		if(aliveCashiers == 0) {
			break;
		}

		int diff = 1001;	// 1 more than possible, 999 and -1
		int closest = 0;

		int i;
		for(i = 0; i < orderVector.size(); i++) {
			if(abs(orderVector.at(i)%1000 - lastSandwich) < diff) {
				closest = i;
				diff = abs(orderVector.at(i)%1000 - lastSandwich);
			}
		}

		int orderID = orderVector.at(closest);
		orderVector.erase(orderVector.begin() + closest);
		lastSandwich = orderID%1000;

		cout << "READY: cashier " << orderID/1000 << " sandwich " << orderID%1000 << endl; 
		thread_broadcast(ORDER_LOCK, CASHIER_CV);
		thread_signal(ORDER_LOCK, orderID);

		

	}
	thread_unlock(ORDER_LOCK);

}

void maker(void* arg) {
	
	files = (char **) arg;

	thread_create((thread_startfunc_t) sandwichMaker, (void*) files[1]);
	int i;
	for(i = 2; i < numCashiers + 2; i++) {
		thread_create((thread_startfunc_t) cashier, (void*) i);
	}

	//start_preemptions(true, true, 2);

	
}

int main(int argc, char* argv[]) {
	if(argc < 3) {
		return 1;
	}
	files = argv;
	numCashiers = argc - 2;

	aliveCashiers = numCashiers;
	boardCapacity = atoi(argv[1]);

	thread_libinit((thread_startfunc_t) maker, (void*) argv);

}


