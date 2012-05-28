SRCS = cpu.cpp heap.cpp
PROG = $(SRCS:.cpp=)
DEPS = $(SRCS:.cpp=.d)
OBJS = $(SRCS:.cpp=.o)

CXXFLAGS = -Wall -g
LDLIBS = -lpthread

all: $(PROG)

cpu: cpu.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -lprofiler -o $@

heap: heap.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -ltcmalloc -o $@

%.d: %.cpp
	$(CXX) -MM $(CPPFLAGS) $< > $@
	@sed -i 's/^\($*\)\.o:/\1.o $@:/' $@

-include $(DEPS)

clean:
	@$(RM) $(OBJS) $(DEPS) $(PROG)



