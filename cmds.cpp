//команды тут

#include "common.h"

void cmds::weather(message *inMsg, table *outMsg)
{
	if(inMsg->words.size() < 2)
	{
		(*outMsg)["message"]+="мб город введёшь?";
		return;
	}
	table params =
	{
		{"lang", "ru"},
		{"units", "metric"},
		{"APPID", "ef23e5397af13d705cfb244b33d04561"},
		{"q", str::summ(inMsg->words, 1)}
	};
	json weather = json::parse(net::send("http://api.openweathermap.org/data/2.5/forecast", params, false));
	if(weather["list"].is_null()&&!weather["list"].size())
	{
		(*outMsg)["message"]+="чтота пошло не так, возможно надо ввести город транслитом";
		return;
	}
	//(*outMsg)["message"]+=weather["list"][0].dump(4);
	string temp = "";
	temp += "погода в "+weather["city"]["country"].get<string>()+"/"+weather["city"]["name"].get<string>()+":";
	weather = weather["list"];
	temp += "\nвремя | температура | скорость ветра | влажность | осадки";
	temp += "\nсейчас "+to_string((int)weather[0]["main"]["temp"])+"°C | "+to_string((int)weather[0]["wind"]["speed"])+"м/с | "+to_string((int)weather[0]["main"]["humidity"])+"% | "+weather[0]["weather"][0]["description"].get<string>();
	for(unsigned int i = 1; i<weather.size(); i++)
		temp += "\n"+other::getTime(weather[i]["dt"])+" | "+to_string((int)weather[i]["main"]["temp"])+"°C | "+to_string((int)weather[i]["wind"]["speed"])+"м/с | "+to_string((int)weather[i]["main"]["humidity"])+"% | "+weather[i]["weather"][0]["description"].get<string>();
	(*outMsg)["message"]+=temp;
}

#define max_size 2000
void cmds::con(message *inMsg, table *outMsg)
{
	if(inMsg->words.size() < 2)
	{
		(*outMsg)["message"]+="...";
		return;
	}
	string cmd = str::summ(inMsg->words, 1);
	cmd = str::replase(cmd, "<br>", "\n");
	cmd = str::convertHtml(cmd);
	string time = other::getRealTime();
	string comand = "chmod +x \""+time+".sh\"";
	fs::writeData(time+".sh", cmd);
	system(comand.c_str());
	comand = "bash \"./"+time+".sh\" > \""+time+"\" 2>&1";
	system(comand.c_str());
	cmd = fs::readData(time);
	comand = "rm -rf \""+time+"\";rm -rf \""+time+".sh\"";
	system(comand.c_str());
	string temp = "";
	args out;
	for(unsigned i = 0; i < cmd.size(); i++)
	{
		temp.push_back(cmd[i]);
		if(temp.size() > max_size && (cmd.size() > i +1 && cmd[i+1]!='\n'))
		{
			out.push_back(temp);
			temp = "";
		}
	}
	out.push_back(temp);
	temp = "";
	for(unsigned i = 0; i < out.size(); i++)
	{
		(*outMsg)["message"]+= "("+to_string(i+1)+"/"+to_string(out.size())+")\n";
		(*outMsg)["message"]+= "\n" + out[i];
		if(out.size() == 1 || i == out.size()-1)break;
		msg::send((*outMsg));
		(*outMsg)["message"]= "";
	}
	return;
}

void cmds::upload(message *inMsg, table *outMsg)
{
	if(inMsg->words.size() < 2)
	{
		(*outMsg)["message"]+="...";
		return;
	}
	string url = str::summ(inMsg->words, 2);
	string name = inMsg->words[1];
	if(inMsg->words.size() > 2)
		net::download(url, name);
	(*outMsg)["attachment"] += ","+vk::upload(name, to_string((int)inMsg->msg[3]), "doc");
	if(inMsg->words.size() > 2){
		name = "rm -f " + name;
		system(name.c_str());
	}
	return;
}

void cmds::video(message *inMsg, table *outMsg)
{
	if(inMsg->words.size() < 2)
	{
		(*outMsg)["message"]+="а чо ввести запрос для поиска эт лишнее? Я чо Ванга?";
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
	for(unsigned int i = 0; i < res.size(); i++)
	{
		if(res[i]["id"].is_null()) continue;
		string temp = "";
		temp+="video";
		temp+=to_string((int)res[i]["owner_id"]);
		temp+="_";
		temp+=to_string((int)res[i]["id"]);
		videos.push_back(temp);
	}
	if(videos.size()==0)
	{
		(*outMsg)["message"]+="нетю такого(";
		return;
	}
	(*outMsg)["attachment"]="";
	unsigned int index = 0;
	if(videos.size()>10)
		index = rand() % (videos.size() - 10);
	for(unsigned int i = index; i < videos.size(); i++)
	{
		(*outMsg)["attachment"]+=videos[i];
		(*outMsg)["attachment"]+=",";
	}
	(*outMsg)["message"]+="воть\nвсего:";
	(*outMsg)["message"]+=to_string(videos.size());
}

void cmds::f(message *inMsg, table *outMsg)
{
	if(inMsg->words.size() < 2)
	{
		(*outMsg)["message"]+="а чо ввести запрос для поиска эт лишнее? Я чо Ванга?";
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
	for(int i = 0; i < 1000; i+=200)
	{
		params["offset"] = to_string(i);
		json temp = vk::send("video.search", params)["response"]["items"];
		for(unsigned int t; t<temp.size(); t++)
			res2.push_back(temp[i]);
	}
	json res = other::jsonDifferenceArr(res1, res2);
	args videos;
	for(unsigned int i = 0; i < res.size(); i++)
	{
		if(res[i]["id"].is_null()) continue;
		string temp = "";
		temp+="video";
		temp+=to_string((int)res[i]["owner_id"]);
		temp+="_";
		temp+=to_string((int)res[i]["id"]);
		videos.push_back(temp);
	}
	if(videos.size()==0)
	{
		(*outMsg)["message"]+="нетю такого(";
		return;
	}
	(*outMsg)["attachment"]="";
	unsigned int index = 0;
	if(videos.size()>10)
		index = rand() % (videos.size() - 10);
	for(unsigned int i = index; i < videos.size(); i++)
	{
		(*outMsg)["attachment"]+=videos[i];
		(*outMsg)["attachment"]+=",";
	}
	(*outMsg)["message"]+="воть\nвсего:";
	(*outMsg)["message"]+=to_string(videos.size());
}

void cmds::doc(message *inMsg, table *outMsg)
{
	if(inMsg->words.size() < 2)
	{
		(*outMsg)["message"]+="а чо ввести запрос для поиска эт лишнее? Я чо Ванга?";
		return;
	}
	table params =
	{
		{"q", str::summ(inMsg->words, 1)},
		{"count", "1000"}
	};
	json res = vk::send("docs.search", params)["response"]["items"];
	args docs;
	for(unsigned int i = 0; i < res.size(); i++)
	{
		if(res[i]["id"].is_null()) continue;
		string temp = "";
		temp+="doc";
		temp+=to_string((int)res[i]["owner_id"]);
		temp+="_";
		temp+=to_string((int)res[i]["id"]);
		docs.push_back(temp);
	}
	if(docs.size()==0)
	{
		(*outMsg)["message"]+="нетю такого(";
		return;
	}
	(*outMsg)["attachment"]="";
	unsigned int index = 0;
	if(docs.size()>10)
		index = rand() % (docs.size() - 10);
	for(unsigned int i = index; i < docs.size(); i++)
	{
		(*outMsg)["attachment"]+=docs[i];
		(*outMsg)["attachment"]+=",";
	}
	(*outMsg)["message"]+="воть\nвсего:";
	(*outMsg)["message"]+=to_string(docs.size());
}

void cmds::unicode(message *inMsg, table *outMsg)
{
	string str = str::summ(inMsg->words, 1);
	string out = "";
	for(unsigned int i = 0; i<str.size();i++)
		out+="&#"+to_string((unsigned int)str[i])+";";
	str = out;
	out = "";
	for(unsigned int i = 0; i<str.size();i++)
		out+="&#"+to_string((unsigned int)str[i])+";";
	(*outMsg)["message"]+=out;
}

void cmds::ban(message *inMsg, table *outMsg)
{
	module::ban::set(inMsg->words[1], true);
}

void cmds::unban(message *inMsg, table *outMsg)
{
	module::ban::set(inMsg->words[1], false);
}