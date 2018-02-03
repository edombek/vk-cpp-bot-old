#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED
#include <map>
#include <vector>
#include <string>
#include <thread>

using namespace std;

#include "json.hpp"

using json = nlohmann::json;

typedef vector<string> args;
typedef map<string, string> table;
typedef struct{
    json js;
    char flags;
    int msg_id;
    int user_id;
    string msg;
    int chat_id;
    args words;
} message;

#include "fs.h"
#include "net.h"
#include "vk.h"
#include "lp.h"
#include "msg.h"
#include "str.h"
#include "other.h"
#include "modules.h"
#include "cmd.h"
#include "cmds.h"
#include "thr.h"

#define botname {"кот", "!", "пуся"}
#endif
