CC=gcc
CFLAGS=-std=c++11 -g -c -Ofast -ftree-vectorize `pkg-config --cflags python3`
LDFLAGS=-lstdc++ -L. -lcurl -lgd -pthread `pkg-config --libs python3` -Wl,-rpath,.
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
	cmds.cpp \
	main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=vkbot

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@
clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
