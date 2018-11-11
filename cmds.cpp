//команды тут

#ifndef NO_PYTHON
#include <boost/python.hpp>
#include <Python.h>
#endif

#include "common.h"
#ifdef __linux__
#include <gd.h>
#elif _WIN32
#pragma comment(lib,"libgd.lib")
#include "win32deps/include/gd/gd.h"
#endif
#include <ctime>
#include <random>
#include <mutex>

#define max_msg_size 4000

mutex lockInP;
mutex lockOutP;
mutex lockOutG;

cmd::cmd_table corpcmd;
void cmds::init()
{
	cmd::add("кр", &cmds::corpCreate, true, "создание/переименование корпорации", 0, 1, &corpcmd);
	cmd::add("ад", &cmds::corpAdd, true, "добавление участника", 0, 1, &corpcmd);
	cmd::add("ап", &cmds::corpUp, true, "повышение уровня", 0, 1, &corpcmd);
	cmd::add("зп", &cmds::corpSend, true, "выплата зп", 0, 1, &corpcmd);
	cmd::add("вз", &cmds::corpMAdd, true, "взнос", 0, 1, &corpcmd);
}

void cmds::weather(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "мб город введёшь?";
		return;
	}
	table params =
	{
		{"lang", "ru"},
		{"units", "metric"},
		{"APPID", "ef23e5397af13d705cfb244b33d04561"},
		{"q", str::summ(inMsg->words, 1)}
	};
	json weather = json::parse(net::send("http://api.openweathermap.org/data/2.5/weather", params, false));
	if (weather["main"].is_null())
	{
		(*outMsg)["message"] += "чтота пошло не так, возможно надо ввести город транслитом";
		return;
	}
	//(*outMsg)["message"]+=weather.dump(4);
	string temp = "";
	temp += "погода в " + weather["sys"]["country"].get<string>() + "/" + weather["name"].get<string>() + ":";
	temp += "\n¤" + other::getTime(weather["dt"]) + ":\n";
	temp += "\n•температура: " + to_string((int)weather["main"]["temp"]) + "°C\n•скорость ветра: " + to_string((int)weather["wind"]["speed"]) + "м/с\n•влажность: " + to_string((int)weather["main"]["humidity"]) + "%\n•описание: " + weather["weather"][0]["description"].get<string>() + "\n";
	(*outMsg)["message"] += temp;
}

void cmds::con(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "...";
		return;
	}
	string cmd = str::summ(inMsg->words, 1);
	cmd = str::convertHtml(cmd);
	string comand = "chmod +x cmd-" + to_string(inMsg->msg_id) + ".sh";
	fs::writeData(string("cmd-" + to_string(inMsg->msg_id) + ".sh"), cmd);
	system(comand.c_str());
	comand = "bash ./cmd-" + to_string(inMsg->msg_id) + ".sh > cmd-" + to_string(inMsg->msg_id) + " 2>&1";
	system(comand.c_str());
	cmd = fs::readData(string("cmd-" + to_string(inMsg->msg_id)));
	comand = "rm -rf cmd-" + to_string(inMsg->msg_id) + " cmd-" + to_string(inMsg->msg_id) + ".sh";
	system(comand.c_str());
	string temp = "";
	args out;
	for (unsigned i = 0; i < cmd.size(); i++)
	{
		temp.push_back(cmd[i]);
		if (temp.size() > max_msg_size && (cmd.size() > i + 1 && cmd[i + 1] != '\n'))
		{
			out.push_back(temp);
			temp = "";
		}
	}
	out.push_back(temp);
	temp = "";
	for (unsigned i = 0; i < out.size(); i++)
	{
		if (out.size() > 1)
			(*outMsg)["message"] += "(" + to_string(i + 1) + "/" + to_string(out.size()) + ")\n";
		(*outMsg)["message"] += "\n" + out[i];
		if (out.size() == 1 || i == out.size() - 1)break;
		msg::send((*outMsg));
		other::sleep(10000);
		(*outMsg)["message"] = "";
	}
}

void cmds::upload(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "...";
		return;
	}
	string url = str::summ(inMsg->words, 2);
	string name = inMsg->words[1];
	if (inMsg->words.size() > 2)
		net::download(url, name);
	(*outMsg)["attachment"] += "," + vk::upload(name, (*outMsg)["peer_id"], "doc");
	if (inMsg->words.size() > 2) {
		name = "rm -f " + name;
		system(name.c_str());
	}
	return;
}

void cmds::video(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "а чо ввести запрос для поиска эт лишнее? Я чо Ванга?";
		return;
	}
	table params =
	{
		{"q", str::summ(inMsg->words, 1)},
		{"adult", "0"},
		{"count", "200"},
		{"hd", "1"},
		{"sort", "2"}
	};
	json res = vk::send("video.search", params)["response"]["items"];
	args videos;
	for (unsigned int i = 0; i < res.size(); i++)
	{
		if (res[i]["id"].is_null()) continue;
		string temp = "";
		temp += "video";
		temp += to_string((int)res[i]["owner_id"]);
		temp += "_";
		temp += to_string((int)res[i]["id"]);
		videos.push_back(temp);
	}
	if (videos.size() == 0)
	{
		(*outMsg)["message"] += "нетю такого(";
		return;
	}
	(*outMsg)["attachment"] = "";
	unsigned int index = 0;
	if (videos.size() > 10)
		index = rand() % (videos.size() - 10);
	for (unsigned int i = index; i < videos.size(); i++)
	{
		(*outMsg)["attachment"] += videos[i];
		(*outMsg)["attachment"] += ",";
	}
	(*outMsg)["message"] += "воть\nвсего:";
	(*outMsg)["message"] += to_string(videos.size());
}

void cmds::f(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "а чо ввести запрос для поиска эт лишнее? Я чо Ванга?";
		return;
	}
	table params =
	{
		{"q", str::summ(inMsg->words, 1)},
		{"adult", "1"},
		{"count", "100"},
		{"offset", "0"},
		{"hd", "1"},
		{"sort", "2"}
	};
	json res1 = vk::send("video.search", params)["response"]["items"];
	params["adult"] = "0";
	json res2;
	for (int i = 0; i < 1000; i += 200)
	{
		params["offset"] = to_string(i);
		json temp = vk::send("video.search", params)["response"]["items"];
		for (unsigned int t = 0; t < temp.size(); t++)
			res2.push_back(temp[i]);
	}
	json res = other::jsonDifferenceArr(res1, res2);
	args videos;
	for (unsigned int i = 0; i < res.size(); i++)
	{
		if (res[i]["id"].is_null()) continue;
		string temp = "";
		temp += "video";
		temp += to_string((int)res[i]["owner_id"]);
		temp += "_";
		temp += to_string((int)res[i]["id"]);
		videos.push_back(temp);
	}
	if (videos.size() == 0)
	{
		(*outMsg)["message"] += "нетю такого(";
		return;
	}
	(*outMsg)["attachment"] = "";
	unsigned int index = 0;
	if (videos.size() > 10)
		index = rand() % (videos.size() - 10);
	for (unsigned int i = index; i < videos.size(); i++)
	{
		(*outMsg)["attachment"] += videos[i];
		(*outMsg)["attachment"] += ",";
	}
	(*outMsg)["message"] += "воть\nвсего:";
	(*outMsg)["message"] += to_string(videos.size());
}

void cmds::doc(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "а чо ввести запрос для поиска эт лишнее? Я чо Ванга?";
		return;
	}
	table params =
	{
		{"q", str::summ(inMsg->words, 1)},
		{"count", "1000"}
	};
	json res = vk::send("docs.search", params)["response"]["items"];
	args docs;
	for (unsigned int i = 0; i < res.size(); i++)
	{
		if (res[i]["id"].is_null()) continue;
		string temp = "";
		temp += "doc";
		temp += to_string((int)res[i]["owner_id"]);
		temp += "_";
		temp += to_string((int)res[i]["id"]);
		docs.push_back(temp);
	}
	if (docs.size() == 0)
	{
		(*outMsg)["message"] += "нетю такого(";
		return;
	}
	(*outMsg)["attachment"] = "";
	unsigned int index = 0;
	if (docs.size() > 10)
		index = rand() % (docs.size() - 10);
	for (unsigned int i = index; i < docs.size(); i++)
	{
		(*outMsg)["attachment"] += docs[i];
		(*outMsg)["attachment"] += ",";
	}
	(*outMsg)["message"] += "воть\nвсего:";
	(*outMsg)["message"] += to_string(docs.size());
}

void cmds::set(cmdArg)
{
	if (inMsg->words.size() < 3)
	{
		(*outMsg)["message"] += "...";
		return;
	}
	module::user::set(inMsg->words[1], str::fromString(inMsg->words[2]));
}

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define MAX4(x,y,z,w) ((MAX((x),(y))) > (MAX((z),(w))) ? (MAX((x),(y))) : (MAX((z),(w))))
#define MIN4(x,y,z,w) ((MIN((x),(y))) < (MIN((z),(w))) ? (MIN((x),(y))) : (MIN((z),(w))))

#define MAXX(x) MAX4(x[0],x[2],x[4],x[6])
#define MINX(x) MIN4(x[0],x[2],x[4],x[6])
#define MAXY(x) MAX4(x[1],x[3],x[5],x[7])
#define MINY(x) MIN4(x[1],x[3],x[5],x[7])

#define TXT_SIZE 40
#define TITLE_SIZE 30
void cmds::citata(cmdArg)
{
	table params =
	{
		{"message_ids", to_string((int)inMsg->msg_id)}
	};
	json res = vk::send("messages.getById", params)["response"]["items"];
	if (res[0]["fwd_messages"].is_null())
	{
		(*outMsg)["message"] += "...";
		return;
	}
	json out;
	other::fwds(&res[0]["fwd_messages"], &out);
	params =
	{
		{"fields", "photo_100"}
	};

	unsigned int x = 0;
	unsigned int y = 0;
	for (unsigned i = 0; i < out.size(); i++)
	{
		int brect[8];
		params["user_ids"] = to_string((int)out[i]["user_id"]);
		json t = vk::send("users.get", params)["response"][0];
		out[i]["photo"] = t["photo_100"];
		out[i]["name"] = t["first_name"].get<string>() + " " + t["last_name"].get<string>();
		gdImageStringFT(NULL, brect, 0x999999, "./font.ttf", TXT_SIZE, 0, 0, TXT_SIZE, (char*)out[i]["msg"].get<string>().c_str());
		if (i > 0 && out[i]["name"] == out[i - 1]["name"] && out[i]["photo"] == out[i - 1]["photo"] && out[i]["lvl"] == out[i - 1]["lvl"])
			out[i]["y"] = MAXY(brect) + TXT_SIZE;
		else
			out[i]["y"] = MAXY(brect) + 100 + TXT_SIZE;
		out[i]["x"] = MAXX(brect) + out[i]["lvl"].get<int>() * 100 + TXT_SIZE;
		y += out[i]["y"].get<int>();
		gdImageStringFT(NULL, brect, 0x999999, "./font.ttf", TITLE_SIZE, 0, 0, TITLE_SIZE, (char*)out[i]["name"].get<string>().c_str());
		out[i]["tx"] = MAXX(brect) + 100 + TITLE_SIZE + out[i]["lvl"].get<int>() * 100;
		if (out[i]["x"].get<unsigned int>() + TXT_SIZE > x)
			x = out[i]["x"].get<int>() + TXT_SIZE;
		if (out[i]["tx"].get<unsigned int>() + TITLE_SIZE > x)
			x = out[i]["tx"].get<int>() + TITLE_SIZE;
	}
	if ((float)x / y > 10)y = (float)x / 10;
	gdImagePtr outIm = gdImageCreateTrueColor(x, y);
	y = 0;
	for (unsigned int i = 0; i < out.size(); i++)
	{
		if (!(i > 0 && out[i]["name"] == out[i - 1]["name"] && out[i]["photo"] == out[i - 1]["photo"] && out[i]["lvl"] == out[i - 1]["lvl"]))
		{
			args w = str::words(out[i]["photo"].get<string>(), '.');
			string n = "avatar." + str::words(w[w.size() - 1], '?')[0];
			/*gdImageFilledRectangle(outIm, out[i]["lvl"].get<int>()*100+150, y, out[i]["lvl"].get<int>()*100 + out[i]["tx"].get<int>()-50, y+100, gdImageColorClosest(outIm, 50, 50, 50));
			gdImageFilledEllipse(outIm, out[i]["lvl"].get<int>()*100+150, y+50, 100, 100, gdImageColorClosest(outIm, 50, 50, 50));
			gdImageFilledEllipse(outIm, out[i]["lvl"].get<int>()*100 + out[i]["tx"].get<int>()-50, y+50, 100, 100, gdImageColorClosest(outIm, 50, 50, 50));*/
			lockInP.lock();
			net::download(out[i]["photo"], n);
			lockInP.unlock();
			gdImagePtr im = gdImageCreateFromFile(n.c_str());
			gdImageCopy(outIm, im, out[i]["lvl"].get<int>() * 100, y, 0, 0, 100, 100);
			gdImageDestroy(im);
			gdImageStringTTF(outIm, NULL, gdImageColorClosest(outIm, 200, 200, 200), "./font.ttf", TITLE_SIZE, 0, out[i]["lvl"].get<int>() * 100 + 100 + TITLE_SIZE*0.5, y + TITLE_SIZE*0.5 + 50, (char*)out[i]["name"].get<string>().c_str());
			gdImageStringTTF(outIm, NULL, gdImageColorClosest(outIm, 255, 255, 255), "./font.ttf", TXT_SIZE, 0, TXT_SIZE*0.5 + out[i]["lvl"].get<int>() * 100 + TXT_SIZE*0.5, y + 100 + TXT_SIZE*1.5, (char*)out[i]["msg"].get<string>().c_str());
		}
		else
			gdImageStringTTF(outIm, NULL, gdImageColorClosest(outIm, 255, 255, 255), "./font.ttf", TXT_SIZE, 0, TXT_SIZE*0.5 + out[i]["lvl"].get<int>() * 100 + TXT_SIZE*0.5, y + TXT_SIZE*1.5, (char*)out[i]["msg"].get<string>().c_str());
		y += out[i]["y"].get<int>();
	}
	lockOutP.lock();
	gdImageFile(outIm, "out.png");
	gdImageDestroy(outIm);
	(*outMsg)["attachment"] += "," + vk::upload("out.png", (*outMsg)["peer_id"], "photo");
	lockOutP.unlock();
}

void cmds::execute(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "...";
		return;
	}
	string cmd = str::summ(inMsg->words, 1);
	cmd = str::convertHtml(cmd);
	table params =
	{
		{"code", cmd}
	};
	json res = vk::send("execute", params);
	string temp = "";
	string resp = res.dump(4);
	args out;
	for (unsigned i = 0; i < resp.size(); i++)
	{
		temp.push_back(resp[i]);
		if (temp.size() > max_msg_size && (resp.size() > i + 1 && resp[i + 1] != '\n'))
		{
			out.push_back(temp);
			temp = "";
		}
	}
	out.push_back(temp);
	temp = "";
	for (unsigned i = 0; i < out.size(); i++)
	{
		if (out.size() > 1)
			(*outMsg)["message"] += "(" + to_string(i + 1) + "/" + to_string(out.size()) + ")\n";
		(*outMsg)["message"] += "\n" + out[i];
		if (out.size() == 1 || i == out.size() - 1)break;
		msg::send((*outMsg));
		(*outMsg)["message"] = "";
	}
}

void cmds::moneysend(cmdArg)
{
	if (inMsg->words.size() < 3)
	{
		(*outMsg)["message"] += "баланс: " + to_string(module::money::get(to_string(inMsg->user_id))) + "$\n";
		inMsg->words.push_back("0");
		inMsg->words.push_back("0");
	}
	long long int id = str::fromString(inMsg->words[1]);
	long long int m = str::fromString(inMsg->words[2]);
	if (m < 1 || (m > module::money::get(to_string(inMsg->user_id)) && module::user::get(inMsg) < 5))
	{
		(*outMsg)["message"] += "ошибка(\nиспользуйте: $ <id> <$>\nну или у вас недостаточно $)";
		return;
	}
	else (*outMsg)["message"] += "отправил";
	if (module::user::get(inMsg) < 5)
		module::money::add(to_string(inMsg->user_id), 0 - m);
	module::money::add(to_string(id), m);
}

void cmds::pixel(cmdArg)
{
	args res = other::msgPhotos(inMsg);
	for (unsigned i = 0; i < res.size(); i += 2)
	{
		string url = res[i];
		string name = "in." + res[i + 1];
		lockInP.lock();
		net::download(url, name);
		lockInP.unlock();
		gdImagePtr im = gdImageCreateFromFile(name.c_str());
		int size = 4 + rand() % 6;
		gdImagePixelate(im, size, GD_PIXELATE_UPPERLEFT);
		lockOutP.lock();
		gdImageFile(im, "out.png");
		gdImageDestroy(im);
		(*outMsg)["attachment"] += vk::upload("out.png", (*outMsg)["peer_id"], "photo") + ",";
		lockOutP.unlock();
	}
}

map<string, int> mathDat;
mutex mathLock;
void cmds::math(cmdArg)
{
	cmd::easySet(to_string(inMsg->chat_id) + "_" + to_string(inMsg->user_id), "матеша");
	mathLock.lock();
	if (inMsg->words.size() == 2)
	{
		if (mathDat[(*outMsg)["peer_id"]] && mathDat[(*outMsg)["peer_id"]] == str::fromString(inMsg->words[1]))
		{
			(*outMsg)["message"] += "маладца)\n";
			module::money::add(to_string(inMsg->user_id), 5 + rand() % 6);
		}
		else if (mathDat[(*outMsg)["peer_id"]])
			(*outMsg)["message"] += "ответ: " + to_string(mathDat[(*outMsg)["peer_id"]]) + "\n";
	}
	else
		(*outMsg)["message"] += "форма ответа: \"... <ответ>\"\n";
	int a, b;
	a = 0;
	b = 0;
	while (!(a + b))
	{
		a = -100 + rand() % 201;
		b = -100 + rand() % 201;
	}
	mathDat[(*outMsg)["peer_id"]] = a + b;
	if (b < 0)
		(*outMsg)["message"] += to_string(a) + "-" + to_string(-b) + "=?";
	else
		(*outMsg)["message"] += to_string(a) + "+" + to_string(b) + "=?";
	mathLock.unlock();
}

void cmds::who(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "...";
		return;
	}
	if (!inMsg->chat_id)
	{
		(*outMsg)["message"] += "ты не в чате...";
		return;
	}
	table params =
	{
		{"chat_id", to_string(inMsg->chat_id)},
		{"fields", "photo"}
	};
	json res = vk::send("messages.getChatUsers", params)["response"];
	unsigned int i = rand() % res.size();
	string who = str::summ(inMsg->words, 1);
	if (who[who.size() - 1] == '?')
		who.resize(who.size() - 1);
	(*outMsg)["message"] += "Я считаю, что " + who + " - [id" + to_string((int)res[i]["id"]) + "|" + res[i]["first_name"].get<string>() + "]";
}

void cmds::when(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "...";
		return;
	}
	string when = str::summ(inMsg->words, 1);
	if (when[when.size() - 1] == '?')
		when.resize(when.size() - 1);
	(*outMsg)["message"] += "Я считаю, что " + when + " произойдёт " + other::getDate(time(NULL) + rand() % 100000000);
}

void cmds::info(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "...";
		return;
	}
	unsigned int i = rand() % 130;
	if (i > 100)
		i = (i - 100) * 10;
	string info = str::summ(inMsg->words, 1);
	(*outMsg)["message"] += "Вероятность того, что " + info + " - " + to_string(i) + "%";
}

#ifndef NO_PYTHON
#include "py.h"
void cmds::py(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "...";
		return;
	}
	string cmd = str::summ(inMsg->words, 1);
	cmd = str::convertHtml(cmd);
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
	main_module.attr("net_info") = net::getInfo();
	main_module.attr("msg_imgs") = pyF::toPythonList(other::msgPhotos(inMsg));
	py::exec("import sys", main_namespace);
	//py::exec("from cStringIO import StringIO", main_namespace); //python2
	py::exec("from io import StringIO", main_namespace); //python3
	py::exec("sys.stdout = mystdout = StringIO()", main_namespace);
	try
	{
		fs::writeData(to_string(inMsg->msg_id) + ".py", cmd);
		py::exec_file(py::str(to_string(inMsg->msg_id) + ".py"), main_namespace);
		*outMsg = pyF::toTable(py::extract<py::dict>(main_module.attr("outMsg")));
		py::exec("output = str(mystdout.getvalue())", main_namespace);
		cmd = py::extract<string>(main_module.attr("output"));
	}
	catch (py::error_already_set&)
	{
		string err = pyF::error();
		py::exec("output = str(mystdout.getvalue())", main_namespace);
		cmd = py::extract<string>(main_module.attr("output"));
		cmd += "\n" + err;
	}
	system(string("rm -rf " + to_string(inMsg->msg_id) + ".py").c_str());

	string temp = "";
	args out;
	for (unsigned i = 0; i < cmd.size(); i++)
	{
		temp.push_back(cmd[i]);
		if (temp.size() > max_msg_size && (cmd.size() > i + 1 && cmd[i + 1] != '\n'))
		{
			out.push_back(temp);
			temp = "";
		}
	}
	out.push_back(temp);
	temp = "";
	for (unsigned i = 0; i < out.size(); i++)
	{
		if (out.size() > 1)
			(*outMsg)["message"] += "(" + to_string(i + 1) + "/" + to_string(out.size()) + ")\n";
		(*outMsg)["message"] += "\n" + out[i];
		if (out.size() == 1 || i == out.size() - 1)break;
		msg::send((*outMsg));
		(*outMsg)["message"] = "";
	}
}
#endif

void cmds::ip(cmdArg)
{
	json ip = json::parse(net::send("http://ip-api.com/json/" + str::summ(inMsg->words, 1)));
	(*outMsg)["message"] = ip.dump(4);
	if (!ip["message"].is_null())
		(*outMsg)["message"] = "НЕ ВЕРНЫЙ ОЙПЕ";
	if (ip["lon"].is_number() && ip["lat"].is_number())
	{
		(*outMsg)["long"] = to_string((float)ip["lon"]);
		(*outMsg)["lat"] = to_string((float)ip["lat"]);
	}
}

#define sizeGame 5
#define ss "⭕"
const string nums[] = { "0⃣","1⃣","2⃣","3⃣","4⃣","5⃣","6⃣","7⃣","8⃣","9⃣" };
const string levels[2][3] = {
	{"⚪",{(char)4294967280, (char)4294967199, (char)4294967188, (char)4294967224},{(char)4294967280, (char)4294967199, (char)4294967188, (char)4294967222}},
	{"⚪",{(char)4294967280, (char)4294967199, (char)4294967188, (char)4294967225},{(char)4294967280, (char)4294967199, (char)4294967188, (char)4294967223}}
};
typedef struct {
	int users_id[2];
	int user;
	int map[sizeGame][sizeGame][2]; // user/level
	int step;
	unsigned int count = 0;
}game_t;
map<int, game_t*> gameNew = {};
map<int, map<int, game_t*>> gameUsers = {};
mutex gameL;

void gameNewMap(game_t *t)
{
	t->user = 1;
	t->users_id[0] = 0;
	t->users_id[1] = 0;
	t->step = 0;
	for (int y = 0; y < sizeGame; y++)
		for (int x = 0; x < sizeGame; x++)
		{
			t->map[y][x][0] = 0;
			t->map[y][x][1] = 0;
		}
	t->map[0][0][0] = 0;
	t->map[0][0][1] = 1;
	t->map[sizeGame - 1][sizeGame - 1][0] = 1;
	t->map[sizeGame - 1][sizeGame - 1][1] = 1;
}

void gameUplevel(game_t *t, int x, int y, bool replase = false, int step = sizeGame)
{
	if (x >= sizeGame || y >= sizeGame || x < 0 || y < 0 || step == 0)
		return;
	if (t->map[x][y][1] >= 2 && (replase || t->map[x][y][0] == t->user)) //если она взрывается
	{
		t->map[x][y][1] = 0;
		t->map[x][y][0] = 0;
		gameUplevel(t, x, y + 1, true, step - 1);
		//gameUplevel(t, x+1, y+1, true, step-1);
		gameUplevel(t, x + 1, y, true, step - 1);
		//gameUplevel(t, x+1, y-1, true, step-1);
		gameUplevel(t, x, y - 1, true, step - 1);
		//gameUplevel(t, x-1, y-1, true, step-1);
		gameUplevel(t, x - 1, y, true, step - 1);
		//gameUplevel(t, x+1, y-1, true, step-1);
	}
	else
	{
		if (replase || t->map[x][y][0] == t->user || t->map[x][y][1] == 0)
		{
			t->map[x][y][1]++;
			t->map[x][y][0] = t->user;
		}
	}
}

bool gameWin(game_t *t)
{
	bool s = true;
	for (int y = 0; y < sizeGame; y++)
		for (int x = 0; x < sizeGame; x++)
			if (t->map[x][y][0] != t->user && t->map[x][y][1] != 0)
				s = false;
	return s;
}

void gameDeleteMap(game_t *t, int chat_id)
{
	if (t->users_id[0])
	{
		gameUsers[chat_id].erase(gameUsers[chat_id].find(t->users_id[0]));
		cmd::easySet(to_string(chat_id) + "_" + to_string(t->users_id[0]), "");
	}
	if (t->users_id[1])
	{
		gameUsers[chat_id].erase(gameUsers[chat_id].find(t->users_id[1]));
		cmd::easySet(to_string(chat_id) + "_" + to_string(t->users_id[1]), "");
	}
	gameNew[chat_id] = NULL;
	delete t;
}

void cmds::game(cmdArg)
{
	if (!inMsg->chat_id)
	{
		(*outMsg)["message"] += "ты не в чате...";
		return;
	}
	gameL.lock();
	cmd::easySet(to_string(inMsg->chat_id) + "_" + to_string(inMsg->user_id), "гейм");
	game_t *t;
	if (gameUsers[inMsg->chat_id].find(inMsg->user_id) != gameUsers[inMsg->chat_id].end() && (gameUsers[inMsg->chat_id].find(inMsg->user_id))->second->users_id[1]) // проверяем на полностью созданную игру
	{
		t = gameUsers[inMsg->chat_id].find(inMsg->user_id)->second;
		if (inMsg->words.size() <= 1)
		{
			(*outMsg)["message"] += "игра остановлена";
			gameDeleteMap(t, inMsg->chat_id);
			gameL.unlock();
			return;
		}
		if (inMsg->words.size() <= 3)
		{
			inMsg->words.push_back("0");
			inMsg->words.push_back("0");
		}
	}
	else
	{
		if (gameNew[inMsg->chat_id] && !gameNew[inMsg->chat_id]->users_id[1]) //ожидает второго игрока
		{
			if (inMsg->user_id == gameNew[inMsg->chat_id]->users_id[0])
			{
				(*outMsg)["message"] += "так нельзя";
				gameL.unlock();
				return;
			}
			gameNew[inMsg->chat_id]->users_id[1] = inMsg->user_id;
			gameUsers[inMsg->chat_id][inMsg->user_id] = gameNew[inMsg->chat_id];
			t = gameUsers[inMsg->chat_id].find(inMsg->user_id)->second;
		}
		else
		{
			t = new game_t;
			gameNewMap(t);
			t->users_id[0] = inMsg->user_id;
			gameUsers[inMsg->chat_id][inMsg->user_id] = t;
			gameNew[inMsg->chat_id] = t;
			(*outMsg)["message"] += "создана игра, ожидаем игроков";
			gameL.unlock();
			return;
		}
	}

	if (inMsg->user_id == t->users_id[t->user] || t->step == 0)
	{
		if (t->step)
			gameUplevel(t, str::fromString(inMsg->words[1]), str::fromString(inMsg->words[2]));
		t->step++;
		(*outMsg)["message"] += ss;
		for (int i = 0; i < sizeGame; i++)
			(*outMsg)["message"] += nums[i];
		(*outMsg)["message"] += "\n";
		for (int y = 0; y < sizeGame; y++)
		{
			(*outMsg)["message"] += nums[y];
			for (int x = 0; x < sizeGame; x++)
				(*outMsg)["message"] += levels[t->map[x][y][0]][t->map[x][y][1]];
			(*outMsg)["message"] += "\n";
		}
		t->count++;
		msg::send((*outMsg));

		if (gameWin(t))
		{
			if (t->user)
			{
				(*outMsg)["message"] = "выйграл второй игрок";
			}
			else
			{
				(*outMsg)["message"] = "выйграл первый игрок\n";
			}
			(*outMsg)["message"] += "за " + to_string(t->count - 1) + " ходов";
			module::money::add(to_string(t->users_id[t->user]), 100);
			gameDeleteMap(t, inMsg->chat_id);
		}
		else if (t->user)
		{
			t->user = 0;
			(*outMsg)["message"] = "ходит первый игрок";
		}
		else
		{
			t->user = 1;
			(*outMsg)["message"] = "ходит второй игрок";
		}
	}
	else
		(*outMsg)["message"] = "не твой ход!";
	gameL.unlock();
	other::sleep(1000);
}

#define dColor 8
void cmds::neon(cmdArg)
{
	(*outMsg)["message"] = "started";
	args res = other::msgPhotos(inMsg);
	for (unsigned i = 0; i < res.size(); i += 2)
	{
		string url = res[i];
		string name = "in." + res[i + 1];
		lockInP.lock();
		net::download(url, name);
		lockInP.unlock();

		std::chrono::time_point<std::chrono::system_clock> begin, end;
		begin = std::chrono::system_clock::now();

		gdImagePtr im = gdImageCreateFromFile(name.c_str());

		string msg_id = to_string(vk::send("messages.send", (*outMsg))["response"].get<int>());
		for (unsigned int xc = 0; xc < im->sx; xc++)
			for (unsigned int yc = 0; yc < im->sy; yc++)
			{
				int color = gdImageGetPixel(im, xc, yc);
				gdImageSetPixel(im, xc, yc, gdImageColorClosest(im, gdTrueColorGetRed(color) - gdTrueColorGetRed(color) % dColor + dColor / 2, gdTrueColorGetGreen(color) - gdTrueColorGetGreen(color) % dColor + dColor / 2, gdTrueColorGetBlue(color) - gdTrueColorGetBlue(color) % dColor + dColor / 2));
			}
		gdImageEmboss(im);
		gdImageContrast(im, -500);
		//gdImagePtr outIm = gdImageCopyGaussianBlurred(im, 4, -1.0);
		gdImageGaussianBlur(im);
		lockOutP.lock();
		gdImageFile(im, "out.png");
		//gdImageDestroy(outIm);
		gdImageDestroy(im);
		string ph = vk::upload("out.png", (*outMsg)["peer_id"], "photo");
		lockOutP.unlock();
		end = std::chrono::system_clock::now();
		unsigned int t = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000;
		vk::send("messages.edit", { {"message_id", msg_id}, {"message", to_string(t) + " сек."}, {"peer_id", (*outMsg)["peer_id"]}, {"attachment", ph} });
	}
	(*outMsg)["message"] = "";
}

#define TPAUSE 0.1 //pause in sec
#define ofset 8 //in bites
mutex voxLock;

struct wav_header_t
{
	char chunkID[4]; //"RIFF" = 0x46464952
	unsigned long chunkSize; //28 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
	char format[4]; //"WAVE" = 0x45564157
	char subchunk1ID[4]; //"fmt " = 0x20746D66
	unsigned long subchunk1Size; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	//[WORD wExtraFormatBytes;]
	//[Extra format bytes]
};
struct chunk_t
{
	char ID[4]; //"data" = 0x61746164
	unsigned long size;  //Chunk data bytes
};
void cmds::vox(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "...";
		return;
	}
	voxLock.lock();
	unsigned long size = 0;
	string data = "";
	wav_header_t wavHeader;
	chunk_t wavChunk;
	for (int i = 1; i < inMsg->words.size(); i++)
	{
		bool pause = true;
		if (inMsg->words[i][0] == '-')
		{
			pause = false;
			inMsg->words[i].erase(inMsg->words[i].begin());
		}
		string path = "vox/" + inMsg->words[i] + ".wav";
		FILE *wavFile = fopen(path.c_str(), "rb");
		if (wavFile == NULL)continue;
		fread(&wavHeader, sizeof(wav_header_t), 1, wavFile);
		char *offset = new char[wavHeader.subchunk1Size - 16];
		fread(offset, wavHeader.subchunk1Size - 16, 1, wavFile);
		delete[] offset;
		fread(&wavChunk, sizeof(chunk_t), 1, wavFile);
		char *dataIn = new char[wavChunk.size];
		fread(dataIn, wavChunk.size, 1, wavFile);
		fclose(wavFile);
		size += wavChunk.size;
		if (pause)
		{
			size += TPAUSE*wavHeader.byteRate;
			for (int s = 0; s < TPAUSE*wavHeader.byteRate; s++)
				data += data[data.size() - 1];
		}
		for (int s = ofset; s < wavChunk.size - ofset; s++)
			data += dataIn[s];
		delete[] dataIn;
	}
	if (data == "")
	{
		voxLock.unlock();
		(*outMsg)["message"] += "...";
		return;
	}
	size += TPAUSE*wavHeader.byteRate;
	for (int s = 0; s < TPAUSE*wavHeader.byteRate; s++)
		data += data[data.size() - 1];
	wavChunk.size = size;
	FILE *wavFile = fopen("audiomsg.wav", "wb");
	wavHeader.subchunk1Size = 16;
	fwrite(&wavHeader, sizeof(wav_header_t), 1, wavFile);
	fwrite(&wavChunk, sizeof(chunk_t), 1, wavFile);
	fwrite(&data[0], size, 1, wavFile);
	fclose(wavFile);
	(*outMsg)["attachment"] += "," + vk::upload("audiomsg.wav", (*outMsg)["peer_id"], "audio_message");
	voxLock.unlock();
}

#define deltaS 0.01
void cmds::rgb(cmdArg)
{
	args res = other::msgPhotos(inMsg);
	for (unsigned i = 0; i < res.size(); i += 2)
	{
		string url = res[i];
		string name = "in." + res[i + 1];
		lockInP.lock();
		net::download(url, name);
		lockInP.unlock();
		gdImagePtr im = gdImageCreateFromFile(name.c_str());
		gdImagePtr outIm = gdImageCreateTrueColor(im->sx*(1 - deltaS), im->sy*(1 - deltaS));
		for (unsigned int xc = 0; xc < im->sx; xc++)
			for (unsigned int yc = 0; yc < im->sy; yc++)
				gdImageSetPixel(outIm, xc, yc, gdImageColorClosest(outIm, gdTrueColorGetRed(gdImageGetPixel(im, xc + im->sx*deltaS, yc)), gdTrueColorGetGreen(gdImageGetPixel(im, xc, yc + im->sy*deltaS)), gdTrueColorGetBlue(gdImageGetPixel(im, xc + im->sx*deltaS, yc + im->sy*deltaS))));
		gdImageCopyMerge(outIm, im, 0, 0, 0, 0, im->sx, im->sy, 50);
		gdImageDestroy(im);
		lockOutP.lock();
		gdImageFile(outIm, "out.png");
		gdImageDestroy(outIm);
		(*outMsg)["attachment"] += vk::upload("out.png", (*outMsg)["peer_id"], "photo") + ",";
		lockOutP.unlock();
	}
}

#define dColor2 64
void cmds::art(cmdArg)
{
	args res = other::msgPhotos(inMsg);
	for (unsigned i = 0; i < res.size(); i += 2)
	{
		string url = res[i];
		string name = "in." + res[i + 1];
		lockInP.lock();
		net::download(url, name);
		gdImagePtr in = gdImageCreateFromFile(name.c_str());
		lockInP.unlock();
		gdImagePtr im = gdImageCopyGaussianBlurred(in, 15, -1.0);
		for (unsigned int xc = 0; xc < im->sx; xc++)
			for (unsigned int yc = 0; yc < im->sy; yc++)
			{
				int color = gdImageGetPixel(im, xc, yc);
				gdImageSetPixel(im, xc, yc, gdImageColorClosest(im, gdTrueColorGetRed(color) - gdTrueColorGetRed(color) % dColor2 + dColor2 / 2, gdTrueColorGetGreen(color) - gdTrueColorGetGreen(color) % dColor2 + dColor2 / 2, gdTrueColorGetBlue(color) - gdTrueColorGetBlue(color) % dColor2 + dColor2 / 2));
			}
		gdImageMeanRemoval(im);
		lockOutP.lock();
		gdImageFile(im, "out.png");
		gdImageDestroy(im);
		gdImageDestroy(in);
		(*outMsg)["attachment"] += vk::upload("out.png", (*outMsg)["peer_id"], "photo") + ",";
		lockOutP.unlock();
	}
}

#ifndef NO_PYTHON
void cmds::pyinit(cmdArg)
{
	cmd::init();
	(*outMsg)["message"] = "done";
}
#endif

#include <gdfonts.h>
#define fsx 6
#define fsy 12
#define lenS "128"
const string gscale("$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/|()1{}[]?-_+~<>i!lI;:, ");
void cmds::ascii(cmdArg)
{
	args res = other::msgPhotos(inMsg);
	for (unsigned i = 0; i < res.size(); i += 2)
	{
		string url = res[i];
		string name = "in." + res[i + 1];
		lockInP.lock();
		net::download(url, name);
		gdImagePtr in = gdImageCreateFromFile(name.c_str());
		lockInP.unlock();
		inMsg->words.push_back(lenS);
		unsigned int w = str::fromString(inMsg->words[1]);
		if (w > 1024)
			w = 1024;
		if (w < 2)
			w = 2;
		unsigned int h = w*in->sy / in->sx*fsx / fsy;
		if (h < 2)
			h = 2;
		gdImageSetInterpolationMethod(in, GD_BILINEAR_FIXED);
		gdImagePtr out = gdImageScale(in, w, h);
		gdImageDestroy(in);
		gdImageGrayScale(out);
		bool dark = false;
		if (gdTrueColorGetRed(gdImageGetTrueColorPixel(out, 0, 0)) < 128)
			dark = true;
		gdImagePtr im = gdImageCreateTrueColor(w*fsx, h*fsy);
		string s = "";
		for (unsigned int y = 0; y < out->sy; y++)
		{
			for (unsigned int x = 0; x < out->sx; x++)
			{
				int c = gdImageGetTrueColorPixel(out, x, y);
				if (dark)
					c = gscale.size() - (gdTrueColorGetRed(c)*gscale.size() / 255);
				else
					c = gdTrueColorGetRed(c)*(gscale.size() - 1) / 255;
				gdImageChar(im, gdFontSmall, x*fsx, y*fsy, (unsigned char)gscale[c], 0xFFFFFF);
				s += gscale[c];
			}
			s += "\n";
		}
		gdImageDestroy(out);
		if (!dark)
			gdImageNegate(im);
		lockOutP.lock();
		fs::writeData("out.txt", s);
		gdImageFile(im, "out.png");
		gdImageDestroy(im);
		(*outMsg)["attachment"] += vk::upload("out.png", (*outMsg)["peer_id"], "photo") + ",";
		(*outMsg)["attachment"] += vk::upload("out.txt", (*outMsg)["peer_id"], "doc") + ",";
		lockOutP.unlock();
	}
}

typedef struct {
	double r;       // a fraction between 0 and 1
	double g;       // a fraction between 0 and 1
	double b;       // a fraction between 0 and 1
} rgb_t;

typedef struct {
	double h;       // angle in degrees
	double s;       // a fraction between 0 and 1
	double v;       // a fraction between 0 and 1
} hsv_t;

hsv_t rgb2hsv(rgb_t in)
{
	hsv_t out;
	double min, max, delta;

	min = in.r < in.g ? in.r : in.g;
	min = min < in.b ? min : in.b;

	max = in.r > in.g ? in.r : in.g;
	max = max > in.b ? max : in.b;

	out.v = max;                                // v
	delta = max - min;
	if (delta < 0.00001)
	{
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return out;
	}
	if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		out.s = (delta / max);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0              
		// s = 0, h is undefined
		out.s = 0.0;
		out.h = NAN;                            // its now undefined
		return out;
	}
	if (in.r >= max)                           // > is bogus, just keeps compilor happy
		out.h = (in.g - in.b) / delta;        // between yellow & magenta
	else
		if (in.g >= max)
			out.h = 2.0 + (in.b - in.r) / delta;  // between cyan & yellow
		else
			out.h = 4.0 + (in.r - in.g) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if (out.h < 0.0)
		out.h += 360.0;

	return out;
}


rgb_t hsv2rgb(hsv_t in)
{
	double hh, p, q, t, ff;
	long i;
	rgb_t out;

	if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}
	hh = in.h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	switch (i) {
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;
	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}
	return out;
}

#define dDegress 15
void cmds::hsv(cmdArg)
{
	args res = other::msgPhotos(inMsg);
	for (unsigned i = 0; i < res.size(); i += 2)
	{
		string url = res[i];
		string name = "in." + res[i + 1];
		lockInP.lock();
		net::download(url, name);
		gdImagePtr in = gdImageCreateFromFile(name.c_str());
		lockInP.unlock();
		inMsg->words.push_back(lenS);
		unsigned int w = str::fromString(inMsg->words[1]);
		if (w > 2048)
			w = 2048;
		if (w < 64)
			w = 64;
		unsigned int h = w*in->sy / in->sx;
		if (h < 64)
			h = 64;
		gdImageSetInterpolationMethod(in, GD_BILINEAR_FIXED);
		gdImagePtr im = gdImageScale(in, w, h);
		gdImageDestroy(in);

		lockOutG.lock();
		FILE *out = fopen("out.gif", "wb");
		gdImageGifAnimBegin(im, out, 0, 0);
		gdImagePtr frame = gdImageCreateTrueColor(w, h);
		for (unsigned int d = 0; d < 360; d += dDegress)
		{
			for (unsigned int y = 0; y < h; y++)
				for (unsigned int x = 0; x < w; x++)
				{
					int color = gdImageGetPixel(im, x, y);
					hsv_t cHsv = rgb2hsv({ gdTrueColorGetRed(color) / 255.0, gdTrueColorGetGreen(color) / 255.0, gdTrueColorGetBlue(color) / 255.0 });
					cHsv.h += d;
					if (cHsv.h > 360)
						cHsv.h -= 360;
					rgb_t cRgb = hsv2rgb(cHsv);
					gdImageSetPixel(frame, x, y, gdImageColorClosest(im, cRgb.r * 255, cRgb.g * 255, cRgb.b * 255));
				}
			//gdImageFile(frame, (to_string(d) + ".png").c_str());
			gdImageGifAnimAdd(frame, out, 1, 0, 0, 1, 1, NULL);
		}
		gdImageDestroy(frame);
		gdImageGifAnimEnd(out);
		fclose(out);
		(*outMsg)["attachment"] += vk::upload("out.gif", (*outMsg)["peer_id"], "doc") + ",";
		lockOutG.unlock();

		gdImageDestroy(im);
	}
}

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
using namespace cv;
#ifdef _WIN32
#pragma comment(lib,"opencv_core2413.lib")
#pragma comment(lib,"opencv_objdetect2413.lib")
#pragma comment(lib,"opencv_highgui2413.lib")
#pragma comment(lib,"opencv_imgproc2413.lib")
#endif
void cmds::face(cmdArg)
{
	args res = other::msgPhotos(inMsg);
	for (unsigned i = 0; i < res.size(); i += 2)
	{
		string url = res[i];
		string name = to_string(inMsg->msg_id) + "-" + to_string(i) + "." + res[i + 1];
		net::download(url, name);

		gdImagePtr in = gdImageCreateFromFile(name.c_str());

		cv::CascadeClassifier cascade;
		cascade.load("./haarcascade_frontalface_default.xml");
		Mat img = imread(name, CV_LOAD_IMAGE_COLOR);
		Mat gray;
		cv::cvtColor(img, gray, COLOR_BGR2GRAY);
		cv::equalizeHist(gray, gray);
		vector<Rect> faces;
		cascade.detectMultiScale(gray, faces, 1.1, 5, 0 | CASCADE_SCALE_IMAGE, Size(16, 16));
		for (size_t i = 0; i < faces.size(); i++)
		{
			gdImageRectangle(in, faces[i].x, faces[i].y, faces[i].x + faces[i].width, faces[i].y + faces[i].height, gdImageColorClosest(in, 255, 0, 0));
		}

		name = "out-" + name;
		gdImageFile(in, name.c_str());
		gdImageDestroy(in);
		(*outMsg)["attachment"] += vk::upload(name, (*outMsg)["peer_id"], "photo") + ",";
	}
}

void cmds::corp(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += module::corp::get(inMsg) + "\n\nРабота с корпорациями(подкоманда):";
		(*outMsg)["message"] += cmd::helpList(inMsg, &corpcmd);
		return;
	}
	inMsg->words.erase(inMsg->words.begin());
	cmd::start(inMsg, outMsg, inMsg->words[0], &corpcmd);
}

void cmds::corpCreate(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "Ничего не хочешь уточнить?";
		return;
	}
	string name = str::summ(inMsg->words, 1);
	if (module::corp::add(name, to_string(inMsg->user_id)))
		(*outMsg)["message"] += "Выполнено!";
	else
		(*outMsg)["message"] += "корпорация уже существует";
}

void cmds::corpAdd(cmdArg)
{
	if (module::corp::addUser(inMsg))
		(*outMsg)["message"] += "добавлен!";
	else
		(*outMsg)["message"] += "невозможно";
}

void cmds::corpUp(cmdArg)
{
	if (module::corp::up(inMsg))
		(*outMsg)["message"] += "повысил!";
	else
		(*outMsg)["message"] += "невозможно";
}

void cmds::corpSend(cmdArg)
{
	int m = module::corp::moneysend(inMsg);
	if (m)
		(*outMsg)["message"] += "Каждому участнику выплаченно по: " + to_string(m) + "$";
	else
		(*outMsg)["message"] += "невозможно";
}

void cmds::corpMAdd(cmdArg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "неуказал переисляемое значение!";
		return;
	}
	int m = str::fromString(inMsg->words[1]);
	module::corp::moneyad(inMsg, m);
	(*outMsg)["message"] += "внесено!";
}