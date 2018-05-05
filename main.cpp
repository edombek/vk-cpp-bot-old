#include "common.h"
#include <iostream>

int main()
{
	vk::init();
	other::startTime();
	module::start();
	cmd::init();
#ifdef friendsadd
	thread friends(vk::friends);
#endif
	msg::init();
	lp::loop();
#ifdef friendsadd
	friends.join();
#endif
	return 0;
}