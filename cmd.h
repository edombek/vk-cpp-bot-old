#ifndef CMD_H_INCLUDED
#define CMD_H_INCLUDED

namespace cmd {
typedef void (*msg_func)(message* inMsg, table* outMsg); // func(inMsg, outMsg)
typedef struct {
    cmd::msg_func func;
    string pyPath;
} ex_t;
typedef struct {
    string info;
    ex_t ex;
    int cost;
    int acess;
    bool disp;
} cmd_dat;
typedef map<string, cmd_dat> cmd_table;

void init();
void add(string command, cmd::msg_func func, bool disp = false, string info = "", int cost = 0, int acess = 1, cmd::cmd_table* cmdptr = NULL);
#ifndef NO_PYTHON
void pyAdd(string command, string pyPath, bool disp = false, string info = "", int cost = 0, int acess = 1);
#endif
void start(message* inMsg, table* outMsg, string command, cmd::cmd_table* cmdptr = NULL);
string helpList(message* inMsg, cmd::cmd_table* cmdptr = NULL);
void easySet(string id, string cmd);
string easyGet(string id);
}

#endif
