#include "common.h"
#include <iostream>
int main()
{
	thread t(thr::cleaner);
	vk::init();
	other::startTime();
	module::start();
	cmd::init();
	lp::loop();
	t.join();
	return 0;
}
