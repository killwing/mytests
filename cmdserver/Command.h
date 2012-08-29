#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>

typedef boost::shared_ptr<Command> CommandPtr;

class Command : public boost::enable_shared_from_this<Command> {
public:
    Command(char type, const std::string& cmd, boost::function<void(char)> clean) 
        : type_(type), cmd_(cmd), clean_(clean) {

    }

    void execute() {
        // doing
        if (next_) {
            Command* next = next_.get();
            next->prev_.reset(); // delete this command
            next->execute();
        } else { // last
            if (clean_) {
                clean_(type_);
            }
        }
    }

    void next(CommandPtr p) {
        next_ = p;
        p->prev_ = shared_from_this();
    }

private:
    char type_;
    std::string cmd_;
    CommandPtr prev_;
    CommandPtr next_;
    boost::function<void(char)> clean_;
};

#endif // COMMAND_H

