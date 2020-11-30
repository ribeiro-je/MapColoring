
BIN := brain

SRCS := $(wildcard *.cc)
OBJS := $(SRCS:.cc=.o)
HDRS := $(wildcard *.hh)

CFLAGS := -g $(shell pkg-config --cflags gtk+-3.0 x11 gazebo opencv)
LIBS   := $(shell pkg-config --libs gtk+-3.0 x11 gazebo opencv)

all:
	make -j $(BIN)

$(BIN): $(OBJS)
	g++ $(CFLAGS) -o $(BIN) $(OBJS) $(LIBS)

%.o: %.cc $(HDRS)
	g++ -c $(CFLAGS) -o $@ $<

clean:
	rm -f *.o $(BIN)

prereqs:
	sudo apt install libopencv-dev opencv-doc libgtk-3-dev

.PHONY: clean all
