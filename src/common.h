#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED
#include <map>
#include <string>
#include <thread>
#include <vector>

using namespace std;

#include "json.hpp"

using json = nlohmann::json;

typedef vector<string> args;
typedef map<string, string> table;
typedef struct {
    json js;
    char flags;
    int msg_id;
    int user_id;
    string msg;
    int chat_id;
    args words;
} message;

#include "cmd.h"
#include "cmds.h"
#include "fs.h"
#include "lp.h"
#include "modules.h"
#include "msg.h"
#include "net.h"
#include "other.h"
#include "str.h"
#include "vk.h"

#define MAXTHREADS 3
#endif
