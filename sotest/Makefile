PROG = main liba.so libb.so libl.so
SRCS = main.cpp a.cpp b.cpp l.cpp
DEPS = $(SRCS:.cpp=.d)
OBJS = $(SRCS:.cpp=.o)

CXXFLAGS = -Wall -g -fPIC
lib%: LDFLAGS = -shared
main: LDFLAGS = -ldl

all: $(PROG)

main: main.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

liba.so libb.so libl.so: lib%.so: %.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.d: %.cpp
	$(CXX) -MM $(CPPFLAGS) $< > $@
	@sed -i 's/^\($*\)\.o:/\1.o $@:/' $@

-include $(DEPS)

clean:
	@$(RM) $(OBJS) $(DEPS) $(PROG)



