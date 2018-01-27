#ifndef STR_H_INCLUDED
#define STR_H_INCLUDED

namespace str
{
	long long int fromString(const std::string& s);
	args words(const std::string &s, char delim = ' ');
	string summ(args words, unsigned int s);
	bool at(string str1, string str2);
	string low(string str);
	string replase(string str, string findstr, string replasestr);
	string convertHtml(string str);
}

#endif
