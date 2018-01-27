#include "common.h"
#include <mutex>
#include <iostream>
vector<thread*> thrs;
mutex thrLock;

void thr::cleaner()
{
	while(true)
	{
		thrLock.lock();
		if(thrs.size()&&thrs[0]->joinable())
		{
			thrLock.unlock();
			thrs[0]->join();
			//cout << other::getRealTime()+": exit thread " << thrs[0] << endl;
			thrLock.lock();
			delete thrs[0];
			thrs.erase(thrs.begin());
		}
		thrLock.unlock();
		other::sleep(100);
	}
}

void thr::add(thread *t)
{
	thrLock.lock();
	//cout << other::getRealTime()+": new thread " << t << endl;
	thrs.push_back(t);
	thrLock.unlock();
}