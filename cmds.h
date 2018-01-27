#ifndef CMDS_H_INCLUDED
#define CMDS_H_INCLUDED

namespace cmds
{
	void weather(message *inMsg, table *outMsg);
	void con(message *inMsg, table *outMsg);
	void upload(message *inMsg, table *outMsg);
	void video(message *inMsg, table *outMsg);
	void f(message *inMsg, table *outMsg);
	void doc(message *inMsg, table *outMsg);
	void unicode(message *inMsg, table *outMsg);
	void ban(message *inMsg, table *outMsg);
	void unban(message *inMsg, table *outMsg);
}

#endif