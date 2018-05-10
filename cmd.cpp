#include "common.h"
#include <iostream>
#include <mutex>
cmd::cmd_table cmd_d;

/*
права доступа:
0 в бане
1 пользователь
2 доверенный
3 кодер
4
5 админ
*/

void help(message *inMsg, table *outMsg)
{
	(*outMsg)["message"] += "ваш уровень доступа: " + to_string(module::user::get(inMsg)) + "\n";
	(*outMsg)["message"] += "ВНИМАНИЕ!!! КОМАНДЫ ТРЕБУЮЩИЕ ОТВЕТА БУДУТ ЖДАТЬ ОТВЕТ И ВОСПРИНИМАТЬ ЗА НЕГО ЛЮБОЕ СООБЩЕНИЕ, ВЫХОДИТЕ ИЗ ЭТОГО РЕЖИМА КОМАНДОЙ \"exit\"\n\n";
	(*outMsg)["message"] += "команды\n" + cmd::helpList(inMsg) + "\n";
}

void cmd::init()
{
	// comand func print discripion cost cess
	cmd::add("help", &help, false, "help", 0, 1);
	cmd::add("погода", &cmds::weather, true, "погодка", 2, 1);
	cmd::add("con", &cmds::con, true, "консолька)", 0, 5);
	cmd::add("u", &cmds::upload, true, "выгрузить в контач", 0, 3);
	cmd::add("видосы", &cmds::video, false, "видосы", 0, 1);
	cmd::add("f", &cmds::f, false, "видосы с правками)", 0, 2);
	cmd::add("-f", &cmds::f, false, "видосы с правками)", 0, 2);
	cmd::add("доки", &cmds::doc, true, "доки", 0, 2);
	cmd::add("set", &cmds::set, true, "acess set", 0, 5);
	cmd::add("цитата", &cmds::citata, true, "создать цитату", 5, 1);
	cmd::add("$", &cmds::moneysend, true, "отправить $", 0, 1);
	cmd::add("exe", &cmds::execute, true, "api", 0, 5);
	cmd::add("pix", &cmds::pixel, true, "пиксельарт из атачмента)", 2, 1);
	cmd::add("матеша", &cmds::math, true, "заработок", 0, 1);
	cmd::add("i", &cmds::test, true, "info", 0, 1);
	cmd::add("кто", &cmds::who, true, "рандом в чате", 0, 1);
	cmd::add("когда", &cmds::when, true, "когда чтолибо произойдёт", 0, 1);
	cmd::add("инфа", &cmds::info, true, "вероятности)", 0, 1);
	cmd::add("py", &cmds::py, true, "python 3", 0, 3);
	cmd::add("ip", &cmds::ip, true, "вычисляет по ойпе", 0, 1);
	cmd::add("гейм", &cmds::game, true, "нукер", 0, 1);
	cmd::add("neon", &cmds::neon, true, "арт с неоновой обработкой", 5, 1);
	cmd::add("vox", &cmds::vox, true, "vox из HL", 1, 1);
	cmd::add("rgb", &cmds::rgb, true, "смещает изображение поканально", 1, 1);
}

void cmd::add(string command, cmd::msg_func func, bool disp, string info, int cost, int acess)
{
	cmd_d[str::low(command)].func = func;
	cmd_d[str::low(command)].disp = disp;
	cmd_d[str::low(command)].info = info;
	cmd_d[str::low(command)].cost = cost;
	cmd_d[str::low(command)].acess = acess;
}

void cmd::start(message *inMsg, table *outMsg, string command)
{
	if (str::low(command) == "exit")
	{
		cmd::easySet(to_string(inMsg->chat_id) + "_" + to_string(inMsg->user_id), "");
		(*outMsg)["message"] += "вышел";
		return;
	}
	string t = cmd::easyGet(to_string(inMsg->chat_id) + "_" + to_string(inMsg->user_id));
	if (t != "" && cmd_d.find(command)->first == "")
	{
		command = t;
		args temp;
		temp.push_back(t);
		for (auto ar : inMsg->words)
			temp.push_back(ar);
		inMsg->words = temp;
	}
	else
		command = str::low(command);
	if (cmd_d.find(command) != cmd_d.cend())
	{
		if (module::money::get(to_string(inMsg->user_id)) < cmd_d[command].cost)
		{
			(*outMsg)["message"] += "чот тебе $ нехватаит";
			return;
		}
		if (module::user::get(inMsg) < cmd_d[command].acess)
		{
			(*outMsg)["message"] += "и куды это мы лезем?";
			return;
		}
		cmd_d[command].func(inMsg, outMsg);
		module::money::add(to_string(inMsg->user_id), 0 - cmd_d[command].cost);
	}
	else
	{
		(*outMsg)["message"] = (*outMsg)["message"] + "незнаю такого" + "(" + command + "), введите команду help и уточните";
		//(*outMsg)["peer_id"]="";
	}

	if (module::user::get(inMsg) < 2)
	{
		(*outMsg)["message"] = str::replase((*outMsg)["message"], ". ", "@#$%&");
		(*outMsg)["message"] = str::replase(str::replase((*outMsg)["message"], "&#", "-"), ".", "-");
		(*outMsg)["message"] = str::replase((*outMsg)["message"], "@#$%&", ". ");
	}
	return;
}

string cmd::helpList(message *inMsg)
{
	string out = "";
	for (auto cmds : cmd_d)
	{
		if (!cmds.second.disp&&module::user::get(inMsg) != 5)continue;
		if (module::user::get(inMsg) < cmds.second.acess) continue;
		out += "\n - \"";
		out += str::low(cmds.first);
		out += "\" - ";
		out += cmds.second.info;
		if (cmds.second.cost)
		{
			out += " - ";
			out += to_string(cmds.second.cost);
			out += "$";
		}
	}
	return out;
}

map<string, string> easyCmd;
mutex easyLock;

void cmd::easySet(string id, string cmd)
{
	easyLock.lock();
	if (cmd == "")
		easyCmd.erase(id);
	else
		easyCmd[id] = cmd;
	easyLock.unlock();
}

string cmd::easyGet(string id)
{
	string t;
	easyLock.lock();
	if (easyCmd.find(id)->first != "")
		t = easyCmd[id];
	else
		t = "";
	easyLock.unlock();
	return t;
}
