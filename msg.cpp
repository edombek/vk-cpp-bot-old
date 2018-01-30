#include "common.h"
#include <iostream>
#include <mutex>
#include <thread>

void msg::in(json js){
    message inMsg;
    table outMsg;
    msg::decode(js, &inMsg);
    if(inMsg.msg=="")return;
    if(!msg::toMe(&inMsg))return;
    string id;
    if(inMsg.chat_id)
        id=to_string(inMsg.chat_id);
    else
        id=to_string(inMsg.user_id);
    thread typing(msg::setTyping, id);
    long long int oldbalance;
    module::TR(&inMsg, &outMsg, &oldbalance);
    msg::func(&inMsg, &outMsg);
    if(outMsg["peer_id"]=="")
		return;
	module::postTR(&inMsg, &outMsg, &oldbalance);
	typing.join();
    msg::send(outMsg);
    //other::sleep(300000);
}

void msg::decode(json js, message *inMsg)
{
    inMsg->msg_id=js[1];
    inMsg->flags=(int)js[2];
    inMsg->msg=js[5];
    if(js[3]>2000000000)
    {
        inMsg->chat_id=js[3];
        inMsg->user_id=str::fromString(js[6]["from"]);
    }
    else
    {
        inMsg->chat_id=0;
        inMsg->user_id=js[3];
    }
    inMsg->js=js;
    if(module::ban::get(to_string(inMsg->user_id)) && !module::admin::get(to_string(inMsg->user_id)))
		inMsg->msg="";
    if(inMsg->msg=="")return;
    inMsg->words=str::words(inMsg->msg, ' ');
}

void msg::func(message *inMsg, table *outMsg)
{
    //outMsg["message"]=inMsg.js.dump(4);
    if(inMsg->chat_id)
        (*outMsg)["peer_id"]=to_string(inMsg->chat_id);
    else
        (*outMsg)["peer_id"]=to_string(inMsg->user_id);
	if(!inMsg->words.size())
		inMsg->words.push_back("help");
	cmd::start(inMsg, outMsg, inMsg->words[0]);
}

void msg::send(table outMsg)
{
	outMsg["message"] = str::replase(outMsg["message"], ".", "•");
	outMsg["message"] = str::replase(outMsg["message"], "#", "•");
    vk::send("messages.send", outMsg);
    cout << other::getRealTime()+": send("+outMsg["peer_id"]+"): "+outMsg["message"] << endl;
}

bool msg::toMe(message *inMsg)
{
	for(auto n: botname)
		if(str::low(n)==str::low(inMsg->words[0]))
		{
			inMsg->words.erase(inMsg->words.begin());
			return true;
		}
	return false;
}

void msg::setTyping(string id)
{
	vk::send("messages.setActivity", {
		{"peer_id", id},
		{"type", "typing"}
	});
}