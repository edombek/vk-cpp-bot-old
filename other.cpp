#include "common.h"
#ifdef __linux__
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif
#include <ctime>

void other::sleep(int ms)
{
#ifdef __linux__
	usleep(ms * 1000);
#elif _WIN32
	Sleep(ms);
#endif
}

time_t td;
void other::startTime()
{
	td=time(NULL);
}

string other::getTime()
{
	string str = ctime(&td);
	str.resize(str.size()-1);
	return str;
}

string other::getRealTime()
{
	time_t temp = time(NULL);
	string str = ctime(&temp);
	str.resize(str.size()-1);
	return str;
}

string other::getTime(time_t temp)
{
	struct tm *timeinfo = localtime(&temp);
	char buffer[64];
	strftime(buffer, 64,"%Z %I:%M%p",timeinfo);
	return buffer;
}

string other::getDate(time_t temp)
{
	struct tm *timeinfo = localtime(&temp);
	char buffer[64];
	strftime(buffer, 64,"%A, %d %B %Y",timeinfo);
	return buffer;
}

json other::jsonDifferenceArr(json j1, json j2)
{
	json out;
	for(unsigned int i1 = 0; i1 < j1.size(); i1++)
	{
		bool f = true;
		for(unsigned int i2 = 0; i2 < j2.size(); i2++)
		{
			if(j1[i1]==j2[i2])
			{
				f = false;
				break;
			}
		}
		if(f) out.push_back(j1[i1]);
	}
	return out;
}

void other::fwds(json *in, json *out, unsigned int lvl)
{
	for(auto i: *in)
	{
		json t;
		t["msg"]=i["body"];
		t["user_id"]=i["user_id"];
		t["lvl"]=lvl;
		out->push_back(t);
		if(i["fwd_messages"].is_null())continue;
		other::fwds(&i["fwd_messages"], out, lvl+1);
	}
}

string other::getParamOfPath(string path, string p)
{
	string dat = fs::readData(path);
	args lines = str::words(dat, '\n');
	for(auto line: lines)
	{
		args param=str::words(line, ':');
		if(str::at(param[0], p))
			return str::replase(param[1], "  ", " ");
	}
	return "";
}

#include <sys/stat.h>
long long int other::getFileSize(const char * fileName)
{
    struct stat file_stat;
    stat(fileName, &file_stat);
    return file_stat.st_size;
}