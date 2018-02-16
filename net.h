#ifndef NET_H_INCLUDED
#define NET_H_INCLUDED

namespace net{
	void init ();
	string urlEncode(string str);
	string send(string url, string params);
	string send(string url, table param = {}, bool post = true);
	string upload(string url, string filename, string params = "");
	void download(string url, string filename, string params = "");
}

#endif
