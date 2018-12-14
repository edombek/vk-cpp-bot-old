CC=gcc  -Ofast -ftree-vectorize # -fsanitize=addres
CFLAGS=-std=c++11 -c -I json/include -I /usr/include/opencv4
LDFLAGS=-lstdc++ -L. -lcurl -lgd -pthread -lopencv_core -lopencv_objdetect -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lm
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

ifdef NO_PYTHON
	CFLAGS+= -DNO_PYTHON
else
	CFLAGS+= $(shell pkg-config --cflags python3)
	LDFLAGS+= $(shell pkg-config --libs python3)
	ifdef TERMUX
		LDFLAGS+= -lboost_python36
	else
		LDFLAGS+= -lboost_python3
	endif
	SOURCES+= 	py.cpp
endif

ifdef TERMUX
	LDFLAGS+= -latomic
endif

CFLAGS+= -Wno-psabi
LDFLAGS+= -Wl,-rpath,.

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@
clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
