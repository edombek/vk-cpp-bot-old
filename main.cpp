#include "common.h"
#include <iostream>

bool friendsadd;

int main()
{
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
