CC=gcc# -fsanitize=address
CFLAGS=-std=c++11 -c -Ofast -ftree-vectorize `pkg-config --cflags python2`
LDFLAGS=-lstdc++ -L. -lcurl -lgd -pthread `pkg-config --libs python2` -lboost_python -Wl,-rpath,.
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

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@
clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
