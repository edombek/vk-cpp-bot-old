#include "common.h"
#include <iostream>
#include "py.h"

bool friendsadd;

int main()
{
	PyMainThread m;
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
