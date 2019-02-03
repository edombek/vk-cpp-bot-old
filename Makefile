CC=g++ -Ofast -ftree-vectorize# -g -DDEBUG-fsanitize=address -fsanitize-recover=address -U_FORTIFY_SOURCE -fno-omit-frame-pointer -fno-common -static-libasan
CFLAGS= -std=c++11 -c
LDFLAGS= -lcurl -lgd -pthread -lopencv_core -lopencv_objdetect -lopencv_photo -lgif -ljpeg -lopencv_imgproc -lopencv_imgcodecs -lm -ldlib -lcblas -llapack -lpng16
INCLUDES= -I/usr/include/opencv4 -Isrc
SOURCES=	\
	src/fs.cpp \
	src/net.cpp \
	src/vk.cpp \
	src/lp.cpp \
	src/str.cpp \
	src/other.cpp \
	src/msg.cpp \
	src/modules.cpp \
	src/cmd.cpp \
	src/cmds.cpp \
	src/FaceSwapper.cpp \
	src/main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=vkbot

ifdef NO_PYTHON
	CFLAGS+= -DNO_PYTHON
else
	INCLUDES+= $(shell pkg-config --cflags python3)
	LDFLAGS+= $(shell pkg-config --libs python3)
	ifdef TERMUX
		LDFLAGS+= -lboost_python36
	else
		LDFLAGS+= -lboost_python3
	endif
	SOURCES+= 	src/py.cpp
endif

ifdef TERMUX
	LDFLAGS+= -latomic
endif

CFLAGS+= -Wno-psabi -Wno-write-strings

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@
clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
