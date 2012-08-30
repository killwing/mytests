#include "CmdDispatcher.h"
#include "CmdChannel.h"

void
CmdDispatcher::dispatch(char type, const std::string& cmd) {
    CmdChannels::iterator it = channels_.find(type);
    if (it == channels_.end()) { // new type
        boost::shared_ptr<CmdChannel> p(new CmdChannel(lock_, cond_));
        channels_.insert(CmdChannels::value_type(type, p));
        p->push(cmd);
    } else {
        it->second->push(cmd);
    }
}

