#include "common.h"
#include <iostream>
#include <cstdlib>
#include <ctime> 
#include <mutex>
#include <cmath>
#include <ctime>
#include <algorithm>

void module::start()
{
	srand(time(NULL));
	module::money::read();
	module::user::read();
	module::corp::read();
}

void module::TR(message *inMsg, table *outMsg, long long int *oldbalance)
{
	*oldbalance = module::money::get(to_string(inMsg->user_id));
}

void module::postTR(message *inMsg, table *outMsg, long long int *oldbalance)
{
	if (*oldbalance != module::money::get(to_string(inMsg->user_id)))
	{
		(*outMsg)["message"] += "<br><br>$";
		(*outMsg)["message"] += to_string(module::money::get(to_string(inMsg->user_id)) - *oldbalance);
		(*outMsg)["message"] += "<br>$: ";
		(*outMsg)["message"] += to_string(module::money::get(to_string(inMsg->user_id)));
	}
}

// money system
#define moneys_path "moneys.json"
#define START_MIN 100
#define START_MAX 500
json moneys;
mutex mon;
void module::money::read()
{
	if (fs::exists(moneys_path))
	{
		moneys = json::parse(fs::readData(moneys_path));
	}
}

void module::money::save()
{
	fs::writeData(moneys_path, moneys.dump(4));
}

long long int module::money::get(string id)
{
	long long int t;
	mon.lock();
	if (moneys[id].is_null())
	{
		moneys[id] = rand() % (START_MAX - START_MIN + 1) + START_MIN;
		module::money::save();
	}
	t = moneys[id];
	mon.unlock();
	return t;
}

void module::money::add(string id, long long int money)
{
	long long int t = module::money::get(id);
	mon.lock();
	moneys[id] = t + money;
	module::money::save();
	mon.unlock();
}

// user system
#define users_path "users.json"
json users;
mutex uLock;
void module::user::read()
{
	if (fs::exists(users_path))
	{
		users = json::parse(fs::readData(users_path));
	}
	else
	{
		users["default"] = 1;
		users["323871959"] = 5;
		module::user::save();
	}
}
void module::user::save()
{
	fs::writeData(users_path, users.dump(4));
}
int module::user::get(message *inMsg)
{
	int user = 0;
	int chat = 0;
	uLock.lock();
	if (users.find(to_string(inMsg->user_id)) == users.end())
		user = users["default"];
	else
		user = users[to_string(inMsg->user_id)];

	if (users.find(to_string(-inMsg->chat_id)) == users.end())
		chat = users["default"];
	else
		chat = users[to_string(-inMsg->chat_id)];
	uLock.unlock();
	if (chat + user <= 1)
		return 0;
	if (chat > user)
		return chat;
	else
		return user;
}
void module::user::set(string id, int acess)
{
	uLock.lock();
	users[id] = acess;
	if (acess == users["default"])
		users.erase(id);
	module::user::save();
	uLock.unlock();
}

// corp system
#define corps_path "corps.json"
json corps;
mutex cLock;
void module::corp::read()
{
	if (fs::exists(corps_path))
	{
		corps = json::parse(fs::readData(corps_path));
	}
	else
	{
		corps["users"] = {};
		corps["corps"] = {};
		module::corp::save();
	}
}
void module::corp::save()
{
	fs::writeData(corps_path, corps.dump(4));
}

bool module::corp::add(string name, string id)
{
	cLock.lock();
	if (corps["corps"].find(name) != corps["corps"].end())
	{
		cLock.unlock();
		return false;
	}
	if (corps["users"][id].is_null()) // create
	{
		corps["users"][id] = name;
		corps["corps"][name]["admin_id"] = id;
		corps["corps"][name]["users"] = { id };
		corps["corps"][name]["lvl"] = 1;
		corps["corps"][name]["money"] = 0;
		corps["corps"][name]["money_time"] = int(time(NULL) / 60);
	}
	else
	{
		if (corps["corps"][corps["users"][id].get<string>()]["admin_id"] == id) // rename
		{
			corps["corps"][name] = corps["corps"][corps["users"][id].get<string>()];
			corps["corps"].erase(corps["users"][id].get<string>());
			corps["users"][id] = name;
			for(auto i: corps["corps"][name]["users"])
				corps["users"][i.get<string>()] = name;
		}
		else // leave and create
		{
			corps["corps"][corps["users"][id].get<string>()].erase(id);
			corps["users"][id] = name;
			corps["corps"][name]["admin_id"] = id;
			corps["corps"][name]["users"] = { id };
			corps["corps"][name]["lvl"] = 1;
			corps["corps"][name]["money_time"] = int(time(NULL) / 60);
		}
	}
	module::corp::save();
	cLock.unlock();
	return true;
}

void module::corp::leave(message *inMsg)
{
	cLock.lock();
	if (corps["users"][to_string(inMsg->user_id)].is_null())
	{
		cLock.unlock();
		return;
	}
	string name = corps["users"][to_string(inMsg->user_id)];
	if (corps["corps"][name]["admin_id"] == to_string(inMsg->user_id)) // drop corp
	{
		for (auto id : corps["corps"][name]["users"])
		{
			corps["users"].erase(id.get<string>());
		}
		corps["corps"].erase(name);
	}
	else
	{
		string id = to_string(inMsg->user_id);
		corps["users"].erase(id);
		corps["corps"][name]["users"].erase(find(corps["corps"][name]["users"].begin(), corps["corps"][name]["users"].end(), id));
	}
	module::corp::save();
	cLock.unlock();
}

#define mCoff 1
#define costUp 500
string module::corp::get(message *inMsg)
{
	cLock.lock();
	if (!corps["users"][to_string(inMsg->user_id)].is_null())
	{
		string msg = "";
		string name = corps["users"][to_string(inMsg->user_id)];
		module::corp::money(name);
		msg += "Корп.: " + name + "\n";
		msg += "Участников: " + to_string(corps["corps"][name]["users"].size()) + "\n";
		msg += "Бюджет: " + to_string(corps["corps"][name]["money"].get<unsigned long int>()) + "$\n";
		msg += "Уровень: " + to_string(corps["corps"][name]["lvl"].get<int>()) + "\n";
		msg += "Заработок: " + to_string(int(mCoff * (int)sqrt(64 * corps["corps"][name]["lvl"].get<int>()) * pow(0.95, corps["corps"][name]["users"].size()) * corps["corps"][name]["users"].size())) + "$/мин\n";
		msg += "Повысить можно за: " + to_string(costUp * corps["corps"][name]["lvl"].get<int>()) + "$\n";
		cLock.unlock();
		return msg;
	}
	cLock.unlock();
	return "";
}

void module::corp::money(string name)
{
	int t = time(NULL) / 60;
	corps["corps"][name]["money"] = corps["corps"][name]["money"].get<unsigned long int>() + int(mCoff * (int)sqrt(64 * corps["corps"][name]["lvl"].get<int>()) * pow(0.95, corps["corps"][name]["users"].size()) * corps["corps"][name]["users"].size()) * (t - corps["corps"][name]["money_time"].get<int>());
	corps["corps"][name]["money_time"] = t;
	module::corp::save();
}

bool module::corp::addUser(message *inMsg)
{
	cLock.lock();
	if (corps["users"][to_string(inMsg->user_id)].is_null())
	{
		cLock.unlock();
		return false;
	}
	string name = corps["users"][to_string(inMsg->user_id)];
	if(corps["corps"][name]["admin_id"] != to_string(inMsg->user_id))
	{
		cLock.unlock();
		return false;
	}
	json resp = vk::send("messages.getById", { {"message_ids", to_string(inMsg->msg_id)} })["response"]["items"][0];
	if (resp["fwd_messages"].is_null())
	{
		cLock.unlock();
		return false;
	}
	money(name);
	bool f = false;
	for(auto m: resp["fwd_messages"])
		if (m["user_id"].is_number())
		{
			string id = to_string(m["user_id"].get<int>());
			if (corps["users"][id].is_null())
			{
				f = true;
				corps["users"][id] = name;
				corps["corps"][name]["users"].push_back(id);
			}
		}
	module::corp::save();
	cLock.unlock();
	return f;
}

bool module::corp::dropUser(message *inMsg)
{
	cLock.lock();
	if (corps["users"][to_string(inMsg->user_id)].is_null())
	{
		cLock.unlock();
		return false;
	}
	string name = corps["users"][to_string(inMsg->user_id)];
	if (corps["corps"][name]["admin_id"] != to_string(inMsg->user_id))
	{
		cLock.unlock();
		return false;
	}
	json resp = vk::send("messages.getById", { { "message_ids", to_string(inMsg->msg_id) } })["response"]["items"][0];
	if (resp["fwd_messages"].is_null())
	{
		cLock.unlock();
		return false;
	}
	money(name);
	bool f = false;
	for (auto m : resp["fwd_messages"])
		if (m["user_id"].is_number())
		{
			string id = to_string(m["user_id"].get<int>());
			if (!corps["users"][id].is_null())
			{
				f = true;
				if (corps["users"][id].get<string>() == name)
				{
					corps["users"].erase(id);
					corps["corps"][name]["users"].erase(find(corps["corps"][name]["users"].begin(), corps["corps"][name]["users"].end(), id));
				}
			}
		}
	module::corp::save();
	cLock.unlock();
	return f;
}

bool module::corp::up(message *inMsg)
{
	cLock.lock();
	if (corps["users"][to_string(inMsg->user_id)].is_null())
	{
		cLock.unlock();
		return false;
	}
	string name = corps["users"][to_string(inMsg->user_id)];
	module::corp::money(name);
	if (corps["corps"][name]["money"].get<unsigned long int>() < costUp * corps["corps"][name]["lvl"].get<int>())
	{
		cLock.unlock();
		return false;
	}
	corps["corps"][name]["money"] = corps["corps"][name]["money"].get<unsigned long int>() - costUp * corps["corps"][name]["lvl"].get<int>();
	corps["corps"][name]["lvl"] = corps["corps"][name]["lvl"].get<int>() + 1;
	module::corp::save();
	cLock.unlock();
	return true;
}

int module::corp::moneysend(message *inMsg)
{
	cLock.lock();
	if (corps["users"][to_string(inMsg->user_id)].is_null())
	{
		cLock.unlock();
		return 0;
	}
	string name = corps["users"][to_string(inMsg->user_id)];
	if (corps["corps"][name]["admin_id"] != to_string(inMsg->user_id))
	{
		cLock.unlock();
		return 0;
	}
	module::corp::money(name);
	int sended = corps["corps"][name]["money"].get<unsigned long int>() / corps["corps"][name]["users"].size();
	corps["corps"][name]["money"] = corps["corps"][name]["money"].get<unsigned long int>() - sended * corps["corps"][name]["users"].size();
	for (auto id : corps["corps"][name]["users"])
		module::money::add(id, sended);
	module::corp::save();
	cLock.unlock();
	return sended;
}

void module::corp::moneyad(message * inMsg, long long int cost)
{
	cLock.lock();
	if(cost < 0 || module::money::get(to_string(inMsg->user_id)) < cost)
	{
		cLock.unlock();
		return;
	}
	string name = corps["users"][to_string(inMsg->user_id)];
	module::money::add(to_string(inMsg->user_id), -cost);
	corps["corps"][name]["money"] = corps["corps"][name]["money"].get<unsigned long int>() + cost;
	module::corp::save();
	cLock.unlock();
}
