SRCS = sctp_client.cpp sctp_server_tcp.cpp sctp_server_udp.cpp tcp_client.cpp tcp_epoll_server.cpp tcp_select_server.cpp tcp_server.cpp udp_client.cpp udp_server.cpp udp_domain_server.cpp udp_domain_client.cpp fifo_pipe_writer.cpp fifo_pipe_reader.cpp
PROG = $(SRCS:.cpp=)
DEPS = $(SRCS:.cpp=.d)
OBJS = $(SRCS:.cpp=.o)

CXXFLAGS = -Wall -g
sctp%: LDLIBS = -lsctp

all: $(PROG)

$(PROG): %: %.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.d: %.cpp
	$(CXX) -MM $(CPPFLAGS) $< > $@
	@sed -i 's/^\($*\)\.o:/\1.o $@:/' $@

-include $(DEPS)

clean:
	@$(RM) $(OBJS) $(DEPS) $(PROG) server.sock test.pipe



