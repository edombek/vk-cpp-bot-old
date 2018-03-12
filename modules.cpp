#include "common.h"
#include <iostream>
#include <cstdlib>
#include <ctime> 
#include <mutex>

void module::start()
{
	srand(time(NULL));
	module::money::read();
	module::user::read();
}

void module::TR(message *inMsg, table *outMsg, long long int *oldbalance)
{
	*oldbalance = module::money::get(to_string(inMsg->user_id));
}

void module::postTR(message *inMsg, table *outMsg, long long int *oldbalance)
{
	if(*oldbalance != module::money::get(to_string(inMsg->user_id)))
	{
		(*outMsg)["message"]+="<br><br>$";
		(*outMsg)["message"]+=to_string(module::money::get(to_string(inMsg->user_id))-*oldbalance);
		(*outMsg)["message"]+="<br>$: ";
		(*outMsg)["message"]+=to_string(module::money::get(to_string(inMsg->user_id)));
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
	if(fs::exists(moneys_path))
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
	if(moneys[id].is_null())
	{
		moneys[id] = rand() % (START_MAX - START_MIN +1 ) + START_MIN;
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
	if(fs::exists(users_path))
	{
		users = json::parse(fs::readData(users_path));
	}else
	{
		users["default"] = 1;
		module::user::save();
	}
}
void module::user::save()
{
	fs::writeData(users_path, users.dump(4));
}
int module::user::get(message *inMsg)
{
	int user=0;
	int chat=0;
	uLock.lock();
	if(users.find(to_string(inMsg->user_id))==users.end())
		user = users["default"];
	else
		user = users[to_string(inMsg->user_id)];
	
	if(users.find(to_string(-inMsg->chat_id))==users.end())
		chat = users["default"];
	else
		chat = users[to_string(-inMsg->chat_id)];
	uLock.unlock();
	if(chat+user<=1)
		return 0;
	if(chat>user)
		return chat;
	else
		return user;
}
void module::user::set(string id, int acess)
{
	uLock.lock();
	users[id] = acess;
	if(acess==users["default"])
		users.erase(id);
	module::user::save();
	uLock.unlock();
}