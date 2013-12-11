PROG = hello
SRCS = main.cpp a.cpp
DEPS = $(SRCS:.cpp=.d)
OBJS = $(SRCS:.cpp=.o)

CXX = clang++
CXXFLAGS = -std=c++1y -stdlib=libc++

all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.d: %.cpp
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< > $@
	@sed -i 's/^\($*\)\.o:/\1.o $@:/' $@

-include $(DEPS)

clean:
	@$(RM) $(OBJS) $(DEPS) $(PROG)



