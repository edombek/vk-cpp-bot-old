#include "common.h"
#include <string> 
#include <codecvt>
#include <locale> 
#include <sstream>

#ifdef __linux__ 
	std::locale const utf8("en_US.UTF-8");
#elif _WIN32
	std::locale const utf8("rus");
#endif

long long int str::fromString(const std::string& s) 
{
  std::istringstream iss(s);
  long long int res;
  iss >> res;
  return res;
}

args &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

args str::words(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

auto converter = new std::wstring_convert<std::codecvt_utf8<wchar_t> >();
// Convert UTF-8 byte string to wstring
std::wstring to_wstring(std::string const& s) 
{
	wstring data = converter->from_bytes(s);
	return data;
}

std::string to_string(std::wstring const& s) {
	string data = converter->to_bytes(s);
	return data;
}

string str::summ(args words, unsigned int s)
{
	string r = "";
	if(s+1>words.size()) return r;
	for(unsigned int i = s; i < words.size(); i++)
	{
		r += words[i];
		r+= " ";
	}
	r.resize(r.size()-1);
	return r;
}

bool str::at(string str1, string str2)
{
	str1=str::low(str1);
	str2=str::low(str2);
	return strstr(str1.c_str(), str2.c_str());
}

string str::low(string str)
{
	auto ss = to_wstring(str);
	for (auto& c : ss) {
		c = std::tolower(c, utf8);
	}
	return to_string(ss);
}

string str::replase(string str, string findstr, string replasestr)
{
	string::size_type index;
	while((index = str.find(findstr))!=std::string::npos) 
	{
		str.replace(index, findstr.size(), replasestr);
	}
	return str;
}

string str::convertHtml(string str)
{
	str = str::replase(str, "&amp;", "&");
	str = str::replase(str, "&quot;", "\"");
	str = str::replase(str, "&gt;", ">");
	str = str::replase(str, "&lt;", "<");
	return str;
}