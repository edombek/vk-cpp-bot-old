// longpoll updates
#include "common.h"
#include <iostream>

string server, key, ts, old_ts;

void lp::getServer()
{
	json longpollinfo = vk::send("messages.getLongPollServer");
	server = longpollinfo["response"]["server"];
	key = longpollinfo["response"]["key"];
	if (ts == "")
		ts = to_string((int)longpollinfo["response"]["ts"]);
}

void lp::loop()
{
	getServer();
	while (true)
	{
		table params = {
			{ "key", key },
			{ "ts", ts },
			{ "wait", "90" },//25
			{ "mode", "2" },
			{ "version", "3" },
			{ "act", "a_check" }
		};
		json data = json::parse(net::send("https://" + server, params));
		if (!data["ts"].is_null()) ts = to_string((int)data["ts"]);
		if (!data["failed"].is_null())
		{
			if (!lp::errors(data)) break;
			continue;
		}
		else if (old_ts != ts)
		{
			lp::updates(data["updates"]);
		}
		old_ts = ts;
		other::sleep(50);
	}
}

bool lp::errors(json lp_data)
{
	switch ((int)lp_data["failed"])
	{
	case 1:
		cout << "LongPoll: " << "Updating ts..." << endl;
		break;
	case 2:
		cout << "LongPoll: " << "Key is out of date" << endl;
		lp::getServer();
		break;
	case 3:
		cout << "LongPoll: " << "Information about the user is lost" << endl;
		lp::getServer();
		break;
	case 4:
		cout << "LongPoll: " << "Invalid api version" << endl;
		return false;
		break;
	default:
		cout << "LongPoll: " << "Reconnect" << endl;
		lp::getServer();
		break;
	}
	return true;
}

void lp::updates(json updates)
{
	for (auto update : updates)
	{
		if (update[0].is_null() || update[1].is_null())continue;
		switch ((int)update[0])
		{
		case 4: // message
			msg::in(update);
			break;
		}
	}
}
