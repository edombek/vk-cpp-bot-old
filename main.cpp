#include "common.h"
#include <iostream>
int main()
{
	thread t(thr::cleaner);
	vk::init();
	other::startTime();
	module::start();
	cmd::init();
#ifdef friendsadd
	thread friends(vk::friends);
#endif
	lp::loop();
	t.join();
#ifdef friendsadd
	friends.join();
#endif
	return 0;
}
