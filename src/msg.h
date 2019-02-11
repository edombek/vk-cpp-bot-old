#ifndef MSG_H_INCLUDED
#define MSG_H_INCLUDED

void new_msg(json js);

namespace msg {
void init();
void in(json js);
void change(json js);
void treatment(message inMsg);
void decode(json js, message* inMsg);
void func(message* inMsg, table* outMsg);
void send(table outMsg);
bool toMe(message* inMsg);
void setTyping(string id);
unsigned long long int Count();
unsigned long long int CountComplete();
};

#endif // MSG_H_INCLUDED
