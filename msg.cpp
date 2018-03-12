#include "common.h"
#include <iostream>
#include <mutex>
#include <thread>

mutex msgLock;
unsigned long long int msgCount=0;
unsigned long long int msgCountComplete=0;

void msg::in(json js){
    message inMsg;
    table outMsg;
    msg::decode(js, &inMsg);
    msgLock.lock();
    msgCount++;
    msgLock.unlock();
    if(inMsg.msg==""||(inMsg.flags&0x02))return;
    if(!msg::toMe(&inMsg))return;
    thr::add(new thread(msg::treatment, inMsg, outMsg));
}

void msg::treatment(message inMsg, table outMsg)
{
	string id;
    if(inMsg.chat_id)
        id=to_string(inMsg.chat_id+2000000000);
    else
        id=to_string(inMsg.user_id);
    //thread typing(msg::setTyping, id);
    long long int oldbalance;
    module::TR(&inMsg, &outMsg, &oldbalance);
    msg::func(&inMsg, &outMsg);
	module::postTR(&inMsg, &outMsg, &oldbalance);
	msgLock.lock();
	msgCountComplete++;
	msgLock.unlock();
	//typing.join();
	msg::send(outMsg);
}

void msg::decode(json js, message *inMsg)
{
    inMsg->msg_id=js[1];
    inMsg->flags=(int)js[2];
    if(!js[5].is_null())inMsg->msg=js[5];
    if(js[3]>2000000000)
    {
        inMsg->chat_id=(int)js[3]-2000000000;
        if(!js[6].is_null())inMsg->user_id=str::fromString(js[6]["from"]);
    }
    else
    {
        inMsg->chat_id=0;
        inMsg->user_id=js[3];
    }
    inMsg->js=js;
    if(!module::user::get(inMsg))
		inMsg->msg="";
    if(inMsg->msg=="")return;
    inMsg->words=str::words(inMsg->msg, ' ');
}

void msg::func(message *inMsg, table *outMsg)
{
    //outMsg["message"]=inMsg.js.dump(4);
    if(inMsg->chat_id)
        (*outMsg)["peer_id"]=to_string(inMsg->chat_id+2000000000);
    else
        (*outMsg)["peer_id"]=to_string(inMsg->user_id);
	if(!inMsg->words.size())
		inMsg->words.push_back("help");
#ifdef forwardmessages
	(*outMsg)["forward_messages"]=to_string(inMsg->msg_id);
#endif
	cmd::start(inMsg, outMsg, inMsg->words[0]);
}

void msg::send(table outMsg)
{
	if(outMsg["peer_id"]=="")return;
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
	if(cmd::easyGet(to_string(inMsg->chat_id)+"_"+to_string(inMsg->user_id))!="")
        return true;
	if(!inMsg->chat_id)
		return true;
	return false;
}

void msg::setTyping(string id)
{
	vk::send("messages.setActivity", {
		{"peer_id", id},
		{"type", "typing"}
	});
}

unsigned long long int msg::Count()
{
	msgLock.lock();
	unsigned long long int temp = msgCount;
	msgLock.unlock();
	return temp;
}
unsigned long long int msg::CountComplete()
{
	msgLock.lock();
	unsigned long long int temp = msgCountComplete;
	msgLock.unlock();
	return temp;
}
