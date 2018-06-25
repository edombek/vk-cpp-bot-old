//команды тут

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

void cmds::weather(message *inMsg, table *outMsg)
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

void cmds::con(message *inMsg, table *outMsg)
{
	if (inMsg->words.size() < 2)
	{
		(*outMsg)["message"] += "...";
		return;
	}
	string cmd = str::summ(inMsg->words, 1);
	cmd = str::convertHtml(cmd);
	string comand = "chmod +x cmd-"+to_string(inMsg->msg_id)+".sh";
	fs::writeData(string("cmd-"+to_string(inMsg->msg_id)+".sh"), cmd);
	system(comand.c_str());
	comand = "bash ./cmd-"+to_string(inMsg->msg_id)+".sh > cmd-"+to_string(inMsg->msg_id)+" 2>&1";
	system(comand.c_str());
	cmd = fs::readData(string("cmd-"+to_string(inMsg->msg_id)));
	comand = "rm -rf cmd-"+to_string(inMsg->msg_id)+" cmd-"+to_string(inMsg->msg_id)+".sh";
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

void cmds::upload(message *inMsg, table *outMsg)
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

void cmds::video(message *inMsg, table *outMsg)
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

void cmds::f(message *inMsg, table *outMsg)
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

void cmds::doc(message *inMsg, table *outMsg)
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

void cmds::set(message *inMsg, table *outMsg)
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
void cmds::citata(message *inMsg, table *outMsg)
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
			string n = "avatar." +str::words( w[w.size() - 1], '?')[0];
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
	FILE *in;
	in = fopen("out.png", "wb");
	gdImagePng(outIm, in);
	fclose(in);
	gdImageDestroy(outIm);
	(*outMsg)["attachment"] += "," + vk::upload("out.png", (*outMsg)["peer_id"], "photo");
	lockOutP.unlock();
}

void cmds::execute(message *inMsg, table *outMsg)
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

void cmds::moneysend(message *inMsg, table *outMsg)
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

void cmds::pixel(message *inMsg, table *outMsg)
{
	table params =
	{
		{"message_ids", to_string(inMsg->msg_id)}
	};
	json res = vk::send("messages.getById", params)["response"]["items"][0];
	if (res["attachments"].is_null())
	{
		return;
	}
	params = {};
	json photos;
	string p = "";
	photos =
	{
		{"photos", ""},
		{"photo_sizes", "1"},
		{"extended", "0"}
	};
	for (unsigned i = 0; i < res["attachments"].size(); i++)
	{
		if (res["attachments"][i]["type"] == "photo")
		{
			p += to_string((int)res["attachments"][i]["photo"]["owner_id"]) + "_" + to_string((int)res["attachments"][i]["photo"]["id"]);
			if (!res["attachments"][i]["photo"]["access_key"].is_null())
				p += "_" + res["attachments"][i]["photo"]["access_key"].get<string>();
			p += ",";
		}
	}
	photos["photos"] = p;
	res = vk::send("photos.getById", photos)["response"];
	for (unsigned i = 0; i < res.size(); i++)
	{
		int maxIndex = 0;
		for (unsigned int si = 0; si < res[i]["sizes"].size(); si++)
			if (res[i]["sizes"][si]["width"] > res[i]["sizes"][maxIndex]["width"])
				maxIndex = si;
		string url = res[i]["sizes"][maxIndex]["src"];
		args w = str::words(url, '.');
		string name = "in." + w[w.size() - 1];
		lockInP.lock();
		net::download(url, name);
		lockInP.unlock();
		gdImagePtr im = gdImageCreateFromFile(name.c_str());
		int size = 4 + rand() % 6;
		gdImagePixelate(im, size, GD_PIXELATE_UPPERLEFT);
		lockOutP.lock();
		FILE *out = fopen("out.png", "wb");
		gdImagePng(im, out);
		fclose(out);
		gdImageDestroy(im);
		(*outMsg)["attachment"] += vk::upload("out.png", (*outMsg)["peer_id"], "photo") + ",";
		lockOutP.unlock();
	}
}

map<string, int> mathDat;
mutex mathLock;
void cmds::math(message *inMsg, table *outMsg)
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

void cmds::who(message *inMsg, table *outMsg)
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

void cmds::when(message *inMsg, table *outMsg)
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

void cmds::info(message *inMsg, table *outMsg)
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

#include "py.h"
void cmds::py(message *inMsg, table *outMsg)
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
	py::exec("import sys", main_namespace);
	//py::exec("from cStringIO import StringIO", main_namespace); //python2
	py::exec("from io import StringIO", main_namespace); //python3
	py::exec("sys.stdout = mystdout = StringIO()", main_namespace);
	try
	{
		fs::writeData(to_string(inMsg->msg_id)+".py", cmd);
		py::exec_file(py::str(to_string(inMsg->msg_id)+".py"), main_namespace);
		*outMsg = pyF::toTable(py::extract<py::dict>(main_module.attr("outMsg")));
		py::exec("output = str(mystdout.getvalue())", main_namespace);
		cmd = py::extract<string>(main_module.attr("output"));
	}
	catch(py::error_already_set&)
	{
		cmd = pyF::error();
	}
	system(string("rm -rf "+to_string(inMsg->msg_id)+".py").c_str());

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

void cmds::ip(message *inMsg, table *outMsg)
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

void cmds::game(message *inMsg, table *outMsg)
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

		msg::send((*outMsg));

		if (gameWin(t))
		{
			if (t->user)
			{
				(*outMsg)["message"] = "выйграл второй игрок";
			}
			else
			{
				(*outMsg)["message"] = "выйграл первый игрок";
			}
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

int delta(gdImagePtr im, int x, int y, int r)
{
	int rr = r*r;
	long long summ[3] = { 0, 0, 0 };
	int colors[3];
	unsigned int c = 0;
	for (int xc = x - r; xc <= x + r; xc++)
	{
		for (int yc = y - r; yc <= y + r; yc++)
		{
			if (rr >= (xc - x)*(xc - x) + (yc - y)*(yc - y) && xc >= 0 && yc >= 0 && xc < im->sx&&yc < im->sy)
			{
				int color = gdImageGetTrueColorPixel(im, xc, yc);
				summ[0] += gdTrueColorGetRed(color);
				summ[1] += gdTrueColorGetGreen(color);
				summ[2] += gdTrueColorGetBlue(color);
				c++;
			}
		}
	}
	int color = gdImageGetTrueColorPixel(im, x, y);
	colors[0] = gdTrueColorGetRed(color) - summ[0] / c;
	colors[1] = gdTrueColorGetGreen(color) - summ[1] / c;
	colors[2] = gdTrueColorGetBlue(color) - summ[2] / c;
	for (int i = 0; i < 3; i++)
		if (colors[i] < 0)colors[i] = 0;
	return gdImageColorClosest(im, colors[0], colors[1], colors[2]);
}

#define minC 0.2
#define radD 20
#define radG 2

void cmds::neon(message *inMsg, table *outMsg)
{
	table params =
	{
		{"message_ids", to_string(inMsg->msg_id)}
	};
	json res = vk::send("messages.getById", params)["response"]["items"][0];
	if (res["attachments"].is_null())
	{
		return;
	}
	params = {};
	json photos;
	string p = "";
	photos =
	{
		{"photos", ""},
		{"photo_sizes", "1"},
		{"extended", "0"}
	};
	for (unsigned i = 0; i < res["attachments"].size(); i++)
	{
		if (res["attachments"][i]["type"] == "photo")
		{
			p += to_string((int)res["attachments"][i]["photo"]["owner_id"]) + "_" + to_string((int)res["attachments"][i]["photo"]["id"]);
			if (!res["attachments"][i]["photo"]["access_key"].is_null())
				p += "_" + res["attachments"][i]["photo"]["access_key"].get<string>();
			p += ",";
		}
	}
	photos["photos"] = p;
	res = vk::send("photos.getById", photos)["response"];
	for (unsigned i = 0; i < res.size(); i++)
	{
		std::chrono::time_point<std::chrono::system_clock> begin, end;
		begin = std::chrono::system_clock::now();
		int maxIndex = 0;
		for (unsigned int si = 0; si < res[i]["sizes"].size(); si++)
			if (res[i]["sizes"][si]["width"] > res[i]["sizes"][maxIndex]["width"])
				maxIndex = si;
		string url = res[i]["sizes"][maxIndex]["src"];
		args w = str::words(url, '.');
		string name = "in." + w[w.size() - 1];
		lockInP.lock();
		net::download(url, name);
		lockInP.unlock();

		gdImagePtr im = gdImageCreateFromFile(name.c_str());
		gdImagePtr outIm = gdImageCreateTrueColor(im->sx, im->sy);

		int max = 0;
		for (int xc = 0; xc < im->sx; xc++)
			for (int yc = 0; yc < im->sy; yc++)
			{
				int color = delta(im, xc, yc, radD);
				int colors[3];
				colors[0] = gdTrueColorGetRed(color);
				colors[1] = gdTrueColorGetGreen(color);
				colors[2] = gdTrueColorGetBlue(color);
				for (int i = 0; i < 3; i++)
				{
					if (colors[i] > max)max = colors[i];
					gdImageSetPixel(outIm, xc, yc, gdImageColorClosest(im, colors[0], colors[1], colors[2]));
				}
			}
		int minColor = max*minC;
		for (int xc = 0; xc < im->sx; xc++)
			for (int yc = 0; yc < im->sy; yc++)
			{
				int color = gdImageGetPixel(outIm, xc, yc);
				int colors[3];
				colors[0] = gdTrueColorGetRed(color);
				colors[1] = gdTrueColorGetGreen(color);
				colors[2] = gdTrueColorGetBlue(color);
				for (int i = 0; i < 3; i++)
					if (colors[i] < minColor) colors[i] = 0;
					else colors[i] = 255;
					gdImageSetPixel(outIm, xc, yc, gdImageColorClosest(im, colors[0], colors[1], colors[2]));
			}
		gdImagePtr result = gdImageCopyGaussianBlurred(outIm, radG, -1.0);
		if (result)
		{
			gdImageDestroy(outIm);
			outIm = result;
		}

		lockOutP.lock();
		FILE *f;
		f = fopen("out.png", "w");
		gdImagePng(outIm, f);
		fclose(f);
		gdImageDestroy(outIm);
		gdImageDestroy(im);
		(*outMsg)["attachment"] = vk::upload("out.png", (*outMsg)["peer_id"], "photo");
		lockOutP.unlock();
		end = std::chrono::system_clock::now();
		unsigned int t = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000;
		(*outMsg)["message"] += to_string(t) + " сек.";
		msg::send((*outMsg));
		(*outMsg)["message"] = "";
		(*outMsg)["attachment"] = "";
	}
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
void cmds::vox(message *inMsg, table *outMsg)
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
void cmds::rgb(message *inMsg, table *outMsg)
{
	table params =
	{
		{"message_ids", to_string(inMsg->msg_id)}
	};
	json res = vk::send("messages.getById", params)["response"]["items"][0];
	if (res["attachments"].is_null())
	{
		return;
	}
	params = {};
	json photos;
	string p = "";
	photos =
	{
		{"photos", ""},
		{"photo_sizes", "1"},
		{"extended", "0"}
	};
	for (unsigned i = 0; i < res["attachments"].size(); i++)
	{
		if (res["attachments"][i]["type"] == "photo")
		{
			p += to_string((int)res["attachments"][i]["photo"]["owner_id"]) + "_" + to_string((int)res["attachments"][i]["photo"]["id"]);
			if (!res["attachments"][i]["photo"]["access_key"].is_null())
				p += "_" + res["attachments"][i]["photo"]["access_key"].get<string>();
			p += ",";
		}
	}
	photos["photos"] = p;
	res = vk::send("photos.getById", photos)["response"];
	for (unsigned i = 0; i < res.size(); i++)
	{
		int maxIndex = 0;
		for (unsigned int si = 0; si < res[i]["sizes"].size(); si++)
			if (res[i]["sizes"][si]["width"] > res[i]["sizes"][maxIndex]["width"])
				maxIndex = si;
		string url = res[i]["sizes"][maxIndex]["src"];
		args w = str::words(url, '.');
		string name = "in." + w[w.size() - 1];
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
		FILE *out = fopen("out.png", "wb");
		gdImagePng(outIm, out);
		fclose(out);
		gdImageDestroy(outIm);
		(*outMsg)["attachment"] += vk::upload("out.png", (*outMsg)["peer_id"], "photo") + ",";
		lockOutP.unlock();
	}
}

void cmds::pyinit(message *inMsg, table *outMsg)
{
    cmd::init();
    (*outMsg)["message"] = "done";
}
