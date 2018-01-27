#ifndef MODULES_H_INCLUDED
#define MODULES_H_INCLUDED

namespace module
{
	void start();
	void TR(message *inMsg, table *outMsg);
	void postTR(message *inMsg, table *outMsg);
	
	namespace money
	{
		void read();
		void save();
		long long int get(string id);
		void add(string id, long long int money);
		vector<pair<string, long long int>> top();
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