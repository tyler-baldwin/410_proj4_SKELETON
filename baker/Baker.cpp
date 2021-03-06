#include <mutex>
#include <condition_variable>
#include<thread>

#include "../includes/externs.h"
#include "../includes/baker.h"
using namespace std;
//has private id
//ID is just a number used to identify this particular baker
//(used with PRINT statements)
Baker::Baker(int id) :
		id(id) {
}
//deconstruct
Baker::~Baker() {
}
//bake, box and append to anOrder.boxes vector
//if order has 13 donuts there should be 2 boxes
//1 with 12 donuts, 1 with 1 donut
void Baker::bake_and_box(ORDER &anOrder) {
	//Tyler W
	int numBox = (anOrder.number_donuts / 12);
	if (anOrder.number_donuts % 12 != 0)
		numBox += 1;

	int numDon = anOrder.number_donuts;

	DONUT d;
	d.myType = GLAZED;
	for (int i = 0; i < numBox; i++) {
		Box b;
		while (numDon > 0 && b.addDonut(d)) {
			--numDon;
		}
		anOrder.boxes.push_back(b);
	}
}

//as long as there are orders in order_in_Q then
//for each order:
//	create box(es) filled with number of donuts in the order
//  then place finished order on order_outvector
//  if waiter is finished (b_WaiterIsFinished) then
//  finish up remaining orders in order_in_Q and exit
//
//You will use cv_order_inQ to be notified by waiter
//when either order_in_Q.size() > 0 or b_WaiterIsFinished == true
//hint: wait for something to be in order_in_Q or b_WaiterIsFinished == true
void Baker::beBaker() {
	//combined effort
	{
		unique_lock<mutex> lck(mutex_order_inQ);
		while (!b_WaiterIsFinished)
			cv_order_inQ.wait(lck);
		cv_order_inQ.notify_all();
	}
	while (true) {
		//if we have consumed all orders
		if (order_in_Q.empty() && b_WaiterIsFinished == true) {
		break;
		}

		while (order_in_Q.empty() && b_WaiterIsFinished == false) {
			unique_lock<mutex> lck(mutex_order_inQ);
			cv_order_inQ.wait(lck);
		}

		unique_lock<mutex> lckin(mutex_order_inQ);

		//bake & box something
		if (!order_in_Q.empty()) {

			//get next
			ORDER ord = order_in_Q.front();
			order_in_Q.pop();

			lckin.unlock();
			bake_and_box(ord);

			unique_lock<mutex> lckout(mutex_order_outQ);
			order_out_Vector.push_back(ord);
			lckout.unlock();
		}

	}
	cv_order_inQ.notify_all();
}

