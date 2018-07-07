#include "common.h"
#include <iostream>
#include <mutex>
#include "py.h"
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
	cmd_d = {};
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
	cmd::add("кто", &cmds::who, true, "рандом в чате", 0, 1);
	cmd::add("когда", &cmds::when, true, "когда чтолибо произойдёт", 0, 1);
	cmd::add("инфа", &cmds::info, true, "вероятности)", 0, 1);
	cmd::add("py", &cmds::py, true, "python 3", 0, 3);
	cmd::add("ip", &cmds::ip, true, "вычисляет по ойпе", 0, 1);
	cmd::add("гейм", &cmds::game, true, "нукер", 0, 1);
	cmd::add("neon", &cmds::neon, true, "арт с неоновой обработкой", 5, 1);
	cmd::add("vox", &cmds::vox, true, "vox из HL", 1, 1);
	cmd::add("rgb", &cmds::rgb, true, "смещает изображение поканально", 1, 1);
	cmd::add("art", &cmds::art, true, "арт из фото", 1, 1);
	cmd::add("pyinit", &cmds::pyinit, true, "re init py cmds", 0, 5);

	//py init
	PySubThread sub;
	try
	{
		py::object main_module = py::import("__main__");
		py::object main_namespace = main_module.attr("__dict__");
		main_module.attr("init") = cmd::pyAdd;
		py::exec_file("py/init.py", main_namespace);
	}
	catch(py::error_already_set const &)
	{
		PyErr_Print();
	}
}

void cmd::add(string command, cmd::msg_func func, bool disp, string info, int cost, int acess)
{
	cmd_d[str::low(command)].ex.func = func;
	cmd_d[str::low(command)].ex.pyPath = "";
	cmd_d[str::low(command)].disp = disp;
	cmd_d[str::low(command)].info = info;
	cmd_d[str::low(command)].cost = cost;
	cmd_d[str::low(command)].acess = acess;
}

void cmd::pyAdd(string command, string pyPath, bool disp, string info, int cost, int acess)
{
	cmd_d[str::low(command)].ex.func = NULL;
	cmd_d[str::low(command)].ex.pyPath = pyPath;
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
	if (t != "" && cmd_d.find(command) == cmd_d.cend())
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
		cout << other::getRealTime() + ": start(" + to_string(inMsg->user_id) + "/" + to_string(inMsg->chat_id) + "): " + command << endl;
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
		if(cmd_d[command].ex.func!=NULL)
			cmd_d[command].ex.func(inMsg, outMsg);
		else
		{
			//py execute script
			PySubThread sub;
			py::object main_module = py::import("__main__");
			py::object main_namespace = main_module.attr("__dict__");
			main_module.attr("outMsg") = pyF::toPythonDict(*outMsg);
			main_module.attr("chat_id") = inMsg->chat_id;
			main_module.attr("user_id") = inMsg->user_id;
			main_module.attr("msg_id") = inMsg->msg_id;
			main_module.attr("msg_flags") = inMsg->flags;
			main_module.attr("msg") = str::summ(inMsg->words, 1);
			main_module.attr("lp_msg") = inMsg->js.dump(4);
			main_module.attr("money_add") = module::money::add;
			main_module.attr("money_get") = module::money::get;
			main_module.attr("user_set") = module::user::set;
			main_module.attr("user_get") = pyF::user_get;
			main_module.attr("msg_count") = msg::Count;
			main_module.attr("msg_countComplete") = msg::CountComplete;
			main_module.attr("getStartTime") = pyF::getTime;
			main_module.attr("vk_upload") = vk::upload;
			main_module.attr("vk_send") = pyF::vk_send;
			main_module.attr("net_send") = pyF::net_send;
			main_module.attr("net_upload") = net::upload;
			main_module.attr("net_download") = net::download;
			main_module.attr("msg_imgs") = pyF::toPythonList(other::msgPhotos(inMsg));
			try
			{
				py::exec_file(py::str("py/" + cmd_d[command].ex.pyPath), main_namespace);
				*outMsg = pyF::toTable(py::extract<py::dict>(main_module.attr("outMsg")));
			}
			catch(py::error_already_set&)
			{
				(*outMsg)["message"]+=pyF::error();
			}
		}
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
