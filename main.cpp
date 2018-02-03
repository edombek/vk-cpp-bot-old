#include "common.h"
#include <iostream>
int main()
{
	thread t(thr::cleaner);
	vk::init();
	other::startTime();
	module::start();
	cmd::init();
	thread friends(vk::friends);
	lp::loop();
	t.join();
	friends.join();
	return 0;
}
