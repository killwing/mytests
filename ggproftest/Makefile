PROG = main
SRCS = main.cpp
DEPS = $(SRCS:.cpp=.d)
OBJS = $(SRCS:.cpp=.o)

CXXFLAGS = -Wall -g -I/root/sources/google-perftools-1.6/src 
LDLIBS = -L/root/sources/google-perftools-1.6/.libs/ -lprofiler


all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.d: %.cpp
	$(CXX) -MM $(CPPFLAGS) $< > $@
	@sed -i 's/^\($*\)\.o:/\1.o $@:/' $@

-include $(DEPS)

clean:
	@$(RM) $(OBJS) $(DEPS) $(PROG)



