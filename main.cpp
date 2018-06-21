#include "common.h"
#include <iostream>
#include <Python.h>

bool friendsadd;

int main()
{
	Py_Initialize();
	//PyEval_InitThreads();
	//PyEval_SaveThread();
	vk::init();
	other::startTime();
	module::start();
	cmd::init();
        thread friends(vk::friends);
	msg::init();
	lp::loop();
    if(friendsadd)
        friends.join();
	return 0;
}
