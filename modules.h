#ifndef MODULES_H_INCLUDED
#define MODULES_H_INCLUDED

namespace module
{
	void start();
	void TR(message *inMsg, table *outMsg, long long int *oldbalance);
	void postTR(message *inMsg, table *outMsg, long long int *oldbalance);
	
	namespace money
	{
		void read();
		void save();
		long long int get(string id);
		void add(string id, long long int money);
	}
	
	namespace admin
	{
		void read();
		void save();
		bool get(string id);
		void set(string id, bool admin);
	}
	
	namespace ban
	{
		void read();
		void save();
		bool get(string id);
		void set(string id, bool admin);
	}
}

#endif