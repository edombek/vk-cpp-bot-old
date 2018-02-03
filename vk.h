#ifndef VK_H_INCLUDED
#define VK_H_INCLUDED

namespace vk{
	void init();
	json send(string method, table params = {}, bool sendtoken = true);
	string upload(string path, string peer_id, string type);
	void friends();
}

#endif
