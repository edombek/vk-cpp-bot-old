#include "common.h"
#include <iostream>
#include <cstdlib>
#include <ctime> 
#include <mutex>

void module::start()
{
	srand(time(NULL));
	module::money::read();
	module::admin::read();
	module::ban::read();
}

void module::TR(message *inMsg, table *outMsg, long long int *oldbalance)
{
	*oldbalance = module::money::get(to_string(inMsg->user_id));
}

void module::postTR(message *inMsg, table *outMsg, long long int *oldbalance)
{
	if(*oldbalance != module::money::get(to_string(inMsg->user_id)))
	{
		(*outMsg)["message"]+="<br><br>🐎";
		(*outMsg)["message"]+=to_string(module::money::get(to_string(inMsg->user_id))-*oldbalance);
		(*outMsg)["message"]+="<br>🐎: ";
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

// admin system
#define admins_path "admins.json"
json admins;
mutex adm;
void module::admin::read()
{
	if(fs::exists(admins_path))
	{
		admins = json::parse(fs::readData(admins_path));
	}else
	{
		admins["default"] = false;
		module::admin::save();
	}
}
void module::admin::save()
{
	fs::writeData(admins_path, admins.dump(4));
}
bool module::admin::get(string id)
{
	bool t;
	adm.lock();
	if(admins[id].is_null())
	{
		admins[id] = admins["default"];
	}
	t = admins[id];
	adm.unlock();
	return t;
}
void module::admin::set(string id, bool admin)
{
	adm.lock();
	admins[id] = admin;
	module::admin::save();
	adm.unlock();
}

// ban system
#define bans_path "bans.json"
json bans;
mutex b;
void module::ban::read()
{
	if(fs::exists(bans_path))
	{
		bans = json::parse(fs::readData(bans_path));
	}else
	{
		bans["default"] = false;
		module::ban::save();
	}
}
void module::ban::save()
{
	fs::writeData(bans_path, bans.dump(4));
}
bool module::ban::get(string id)
{
	bool t;
	b.lock();
	if(bans[id].is_null())
	{
		bans[id] = bans["default"];
	}
	t = bans[id];
	b.unlock();
	return t;
}
void module::ban::set(string id, bool ban)
{
	b.lock();
	bans[id] = ban;
	module::ban::save();
	b.unlock();
}