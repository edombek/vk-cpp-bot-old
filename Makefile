CC=gcc# -fsanitize=address -Ofast -ftree-vectorize
CFLAGS=-std=c++11 -c  $(shell pkg-config --cflags python3) -I json/include -I json/include/nlohmann/ -Wno-psabi
LDFLAGS=-lstdc++ -L. -lcurl -lgd -pthread $(shell pkg-config --libs python3) -Wl,-rpath,.
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
	LDFLAGS+= -lboost_python36 -latomic
else
	LDFLAGS+= -lboost_python-py36
endif

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@
clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
