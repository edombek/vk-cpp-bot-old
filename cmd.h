#ifndef CMD_H_INCLUDED
#define CMD_H_INCLUDED

namespace cmd
{
	typedef void(*msg_func)(message *inMsg, table *outMsg); // func(inMsg, outMsg)
	typedef struct{
		string info;
		cmd::msg_func func;
		int cost;
		int acess;
		bool disp;
	}cmd_dat;
	typedef map<string, cmd_dat> cmd_table;

	void init();
	void add(string command, cmd::msg_func func, bool disp=false, string info="", int cost=0, int acess=1);
	void start(message *inMsg, table *outMsg, string command);
	string helpList(message *inMsg);
    void easySet(string id, string cmd);
    string easyGet(string id);
}

#endif
