#ifndef CMDS_H_INCLUDED
#define CMDS_H_INCLUDED

#define addCmd(c) void ((c))(message *inMsg, table *outMsg);

namespace cmds
{
	addCmd(weather)
	addCmd(con)
	addCmd(upload)
	addCmd(video)
	addCmd(f)
	addCmd(doc)
	addCmd(set)
	addCmd(citata)
	addCmd(execute)
	addCmd(moneysend)
	addCmd(pixel)
	addCmd(math)
	addCmd(info)
	addCmd(who)
	addCmd(when)
#ifndef NO_PYTHON
	addCmd(py)
#endif
	addCmd(ip)
	addCmd(game)
	addCmd(neon)
	addCmd(vox)
	addCmd(rgb)
	addCmd(art)
#ifndef NO_PYTHON
	addCmd(pyinit)
#endif
	addCmd(ascii)
	addCmd(hsv)
}

#endif
