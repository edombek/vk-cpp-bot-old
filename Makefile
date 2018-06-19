CC=gcc# -fsanitize=address
CFLAGS=-std=c++11 -c -Ofast -ftree-vectorize $(shell pkg-config --cflags python2)
LDFLAGS=-lstdc++ -L. -lcurl -lgd -pthread $(shell pkg-config --libs python2) -Wl,-rpath,.
INCLUDES=
SOURCES=	\
	fs.cpp \
	net.cpp \
	vk.cpp \
	lp.cpp \
	str.cpp \
	other.cpp \
	msg.cpp \
	modules.cpp \
	cmd.cpp \
	py.cpp \
	cmds.cpp \
	main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=vkbot

ifdef TERMUX
	LDFLAGS+= -lboost_python27
else
	LDFLAGS+= -lboost_python
endif

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@
clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
