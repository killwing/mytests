PROG = hello
SRCS = main.cpp a.cpp
DEPS = $(SRCS:.cpp=.d)
OBJS = $(SRCS:.cpp=.o)

CXX = clang++
CXXFLAGS = -std=c++11 -stdlib=libc++
LDLIBS = -lpthread

all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.d: %.cpp
	$(CXX) -MM $(CPPFLAGS) $< > $@
	@sed -i 's/^\($*\)\.o:/\1.o $@:/' $@

-include $(DEPS)

clean:
	@$(RM) $(OBJS) $(DEPS) $(PROG)



