//команды тут

#include "common.h"
#include <gd.h>
#include <ctime>
#include <random>
#include <mutex>

mutex lockOut;

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
		if(out.size()>1)
			(*outMsg)["message"]+= "("+to_string(i+1)+"/"+to_string(out.size())+")\n";
		(*outMsg)["message"]+= "\n" + out[i];
		if(out.size() == 1 || i == out.size()-1)break;
		msg::send((*outMsg));
		(*outMsg)["message"]= "";
	}
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

void cmds::ban(message *inMsg, table *outMsg)
{
	module::ban::set(inMsg->words[1], true);
}

void cmds::unban(message *inMsg, table *outMsg)
{
	module::ban::set(inMsg->words[1], false);
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
	if(res[0]["fwd_messages"].is_null())
	{
		(*outMsg)["message"]+="...";
		return;
	}
	json out;
	other::fwds(&res[0]["fwd_messages"], &out);
	params =
	{
		{"fields", "photo_100"}
	};
	
	unsigned int x=0;
	unsigned int y=0;
	lockOut.lock();
	for(unsigned i=0; i < out.size();i++)
	{
		int brect[8];
		params["user_ids"]=to_string((int)out[i]["user_id"]);
		json t = vk::send("users.get", params)["response"][0];
		out[i]["photo"] = t["photo_100"];
		out[i]["name"] = t["first_name"].get<string>() + " " + t["last_name"].get<string>();
		gdImageStringFT(NULL, brect, 0x999999, "./font.ttf", TXT_SIZE, 0, 0, TXT_SIZE, (char*)out[i]["msg"].get<string>().c_str());
		if(i>0&&out[i]["name"] == out[i-1]["name"]&&out[i]["photo"] == out[i-1]["photo"]&&out[i]["lvl"] == out[i-1]["lvl"])
			out[i]["y"]=MAXY(brect)+TXT_SIZE;
		else
			out[i]["y"]=MAXY(brect)+100+TXT_SIZE;
		out[i]["x"]=MAXX(brect)+out[i]["lvl"].get<int>()*100+TXT_SIZE;
		y += out[i]["y"].get<int>();
		gdImageStringFT(NULL, brect, 0x999999, "./font.ttf", TITLE_SIZE, 0, 0, TITLE_SIZE, (char*)out[i]["name"].get<string>().c_str());
		out[i]["tx"]=MAXX(brect)+100+TITLE_SIZE+out[i]["lvl"].get<int>()*100;
		if(out[i]["x"].get<unsigned int>()+TXT_SIZE>x)
			x=out[i]["x"].get<int>()+TXT_SIZE;
		if(out[i]["tx"].get<unsigned int>()+TITLE_SIZE>x)
			x=out[i]["tx"].get<int>()+TITLE_SIZE;
	}
	if((float)x/y>10)y=(float)x/10;
	gdImagePtr outIm = gdImageCreateTrueColor(x, y);
	y=0;
	for(unsigned int i=0;i<out.size();i++)
	{
		if(!(i>0&&out[i]["name"] == out[i-1]["name"]&&out[i]["photo"] == out[i-1]["photo"]&&out[i]["lvl"] == out[i-1]["lvl"]))
		{
			args w = str::words(out[i]["photo"].get<string>(), '.');
			string n = "avatar."+w[w.size()-1];
			/*gdImageFilledRectangle(outIm, out[i]["lvl"].get<int>()*100+150, y, out[i]["lvl"].get<int>()*100 + out[i]["tx"].get<int>()-50, y+100, gdImageColorClosest(outIm, 50, 50, 50));
			gdImageFilledEllipse(outIm, out[i]["lvl"].get<int>()*100+150, y+50, 100, 100, gdImageColorClosest(outIm, 50, 50, 50));
			gdImageFilledEllipse(outIm, out[i]["lvl"].get<int>()*100 + out[i]["tx"].get<int>()-50, y+50, 100, 100, gdImageColorClosest(outIm, 50, 50, 50));*/
			net::download(out[i]["photo"], n);
			gdImagePtr im = gdImageCreateFromFile(n.c_str());
			gdImageCopy(outIm, im, out[i]["lvl"].get<int>()*100, y, 0, 0, 100, 100);
			gdImageDestroy(im);
			gdImageStringTTF(outIm, NULL, gdImageColorClosest(outIm, 200, 200, 200), "./font.ttf", TITLE_SIZE, 0, out[i]["lvl"].get<int>()*100 + 100 + TITLE_SIZE*0.5, y+TITLE_SIZE*0.5+50, (char*)out[i]["name"].get<string>().c_str());
			gdImageStringTTF(outIm, NULL, gdImageColorClosest(outIm, 255, 255, 255), "./font.ttf", TXT_SIZE, 0, TXT_SIZE*0.5+out[i]["lvl"].get<int>()*100 + TXT_SIZE*0.5, y+100+TXT_SIZE*1.5, (char*)out[i]["msg"].get<string>().c_str());
		}
		else
			gdImageStringTTF(outIm, NULL, gdImageColorClosest(outIm, 255, 255, 255), "./font.ttf", TXT_SIZE, 0, TXT_SIZE*0.5+out[i]["lvl"].get<int>()*100 + TXT_SIZE*0.5, y+TXT_SIZE*1.5, (char*)out[i]["msg"].get<string>().c_str());
		y+=out[i]["y"].get<int>();
	}
	FILE *in;
	in = fopen("out.png", "wb");
	gdImagePng(outIm, in);
	fclose(in);
	gdImageDestroy(outIm);
	(*outMsg)["attachment"] += ","+vk::upload("out.png", to_string((int)inMsg->msg[3]), "photo");
	lockOut.unlock();
}

void cmds::execute(message *inMsg, table *outMsg)
{
	if(inMsg->words.size() < 2)
	{
		(*outMsg)["message"]+="...";
		return;
	}
	string cmd = str::summ(inMsg->words, 1);
	cmd = str::replase(cmd, "<br>", "\n");
	cmd = str::convertHtml(cmd);
	table params =
	{
		{"code", cmd}
	};
	json res = vk::send("execute", params);
	string temp = "";
	string resp=res.dump(4);
	args out;
	for(unsigned i = 0; i < resp.size(); i++)
	{
		temp.push_back(resp[i]);
		if(temp.size() > max_size && (resp.size() > i +1 && resp[i+1]!='\n'))
		{
			out.push_back(temp);
			temp = "";
		}
	}
	out.push_back(temp);
	temp = "";
	for(unsigned i = 0; i < out.size(); i++)
	{
		if(out.size()>1)
			(*outMsg)["message"]+= "("+to_string(i+1)+"/"+to_string(out.size())+")\n";
		(*outMsg)["message"]+= "\n" + out[i];
		if(out.size() == 1 || i == out.size()-1)break;
		msg::send((*outMsg));
		(*outMsg)["message"]= "";
	}
}

void cmds::moneysend(message *inMsg, table *outMsg)
{
	if(inMsg->words.size() < 3)
	{
		inMsg->words.push_back("0");
		inMsg->words.push_back("0");
	}
	long long int id = str::fromString(inMsg->words[1]);
	long long int m = str::fromString(inMsg->words[2]);
	if(m < 1 || m > module::money::get(to_string(inMsg->user_id)))
	{
		(*outMsg)["message"] += "ошибка(\nиспользуйте отправить <id> <$>\nну или у вас недостаточно $";
		return;
	}
	else (*outMsg)["message"] += "отправил";
	module::money::add(to_string(inMsg->user_id), 0-m);
	module::money::add(to_string(id), m);
}

void cmds::pixel(message *inMsg, table *outMsg)
{
	table params =
	{
		{"message_ids", to_string(inMsg->msg_id)}
	};
	json res = vk::send("messages.getById", params)["response"]["items"][0];
	if(res["attachments"].is_null())
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
	for(unsigned i=0;i<res["attachments"].size();i++)
	{
		if(res["attachments"][i]["type"]=="photo")
		{
			p+=to_string((int)res["attachments"][i]["photo"]["owner_id"])+"_"+to_string((int)res["attachments"][i]["photo"]["id"]);
			if(!res["attachments"][i]["photo"]["access_key"].is_null())
				p+="_"+res["attachments"][i]["photo"]["access_key"].get<string>();
			p+=",";
		}
	}
	photos["photos"]=p;
	res = vk::send("photos.getById", photos)["response"];
	for(unsigned i=0;i<res.size();i++)
	{
		lockOut.lock();
		string url = res[i]["sizes"][res[i]["sizes"].size()-1]["src"];
		args w = str::words(url, '.');
		string name = "in-"+other::getRealTime()+"."+w[w.size()-1];
		net::download(url, name);
		gdImagePtr im = gdImageCreateFromFile(name.c_str());
		int size = 4+rand()%6;
		gdImagePixelate(im, size, GD_PIXELATE_UPPERLEFT);
		FILE *out = fopen("out.png", "wb");
		gdImagePng(im, out);
		fclose(out);
		gdImageDestroy(im);
		(*outMsg)["attachment"] += vk::upload("out.png", to_string((int)inMsg->msg[3]), "photo") + ",";
		lockOut.unlock();
	}
}