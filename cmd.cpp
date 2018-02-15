#include "common.h"
#include <iostream>
cmd::cmd_table cmd_d;

void help(message *inMsg, table *outMsg)
{
	(*outMsg)["message"]+="команды\n"+cmd::helpList(inMsg)+"\n\nРазработчик: [id323871959|EVGESHAd]";
}

void cmd::init()
{
	// comand func print discripion cost admin
	cmd::add("help", &help, false, "help", 0, false);
	cmd::add("погода", &cmds::weather, true, "погодка", 2, false);
	cmd::add("con", &cmds::con, true, "консолька)", 0, true);
	cmd::add("u", &cmds::upload, true, "выгрузить в контач", 0, true);
	cmd::add("видосы", &cmds::video, false, "видосы", 0, true);
	cmd::add("f", &cmds::f, false, "видосы с правками)", 0, true);
	cmd::add("-f", &cmds::f, false, "видосы с правками)", 0, true);
	cmd::add("доки", &cmds::doc, true, "доки", 0, true);
	cmd::add("ban", &cmds::ban, true, "ban", 0, true);
	cmd::add("unban", &cmds::unban, true, "unban", 0, true);
	cmd::add("цитата", &cmds::citata, true, "создать цитату", 5, false);
	cmd::add("$", &cmds::moneysend, true, "отправить $", 0, false);
	cmd::add("exe", &cmds::execute, true, "api", 0, true);
	cmd::add("pix", &cmds::pixel, true, "пиксельарт из атачмента)", 2, false);
	cmd::add("матеша", &cmds::math, true, "заработок", 0, false);
	cmd::add("i", &cmds::test, true, "info", 0, false);
	cmd::add("кто", &cmds::who, true, "рандом в чате", 0, false);
	cmd::add("инфа", &cmds::info, true, "вероятности)", 0, false);
	cmd::add("py", &cmds::py, true, "python 2.7", 0, true);
}

void cmd::add(string command, cmd::msg_func func, bool disp, string info, int cost, bool admin_cmd)
{
	cmd_d[str::low(command)].func=func;
	cmd_d[str::low(command)].disp=disp;
	cmd_d[str::low(command)].info=info;
	cmd_d[str::low(command)].cost=cost;
	cmd_d[str::low(command)].admin_cmd=admin_cmd;
}

void cmd::start(message *inMsg, table *outMsg, string command)
{
	command = str::replase(str::replase(str::low(command), "#", "[*]"), ".", "[*]");
	if(cmd_d.find(command)->first != "")
	{
		if(module::money::get(to_string(inMsg->user_id))<cmd_d[command].cost)
		{
			(*outMsg)["message"] += "чот тебе $ нехватаит";
			return;
		}
		if(!module::admin::get(to_string(inMsg->user_id))&cmd_d[command].admin_cmd)
		{
			(*outMsg)["message"] += "и куды это мы лезем?";
			return;
		}
		module::money::add(to_string(inMsg->user_id), 0-cmd_d[command].cost);
		cmd_d[command].func(inMsg, outMsg);
	}
	else
	{
		(*outMsg)["message"]=(*outMsg)["message"]+"незнаю такого"+"("+command+")";
	}
	return;
}

string cmd::helpList(message *inMsg)
{
	string out = "";
	for(auto cmds: cmd_d)
	{
		if(!cmds.second.disp&!module::admin::get(to_string(inMsg->user_id)))continue;
		if(!module::admin::get(to_string(inMsg->user_id))&cmds.second.admin_cmd) continue;
		out+="\n - \"";
		out+=str::low(cmds.first);
		out+="\" - ";
		out+=cmds.second.info;
		if(cmds.second.cost)
		{
			out+=" - ";
			out+=to_string(cmds.second.cost);
			out+="$";
		}
	}
	return out;
}
