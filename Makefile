CXX=g++
LD=g++
CFLAGS=
LIBS=-lSDL -lSDLmain -lSDL_image -lGL -lGLU
CPP_FILES=$(wildcard *.cpp)
OBJ_FILES=$(patsubst %.cpp,%.o,$(CPP_FILES))

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

oemfoeland: $(OBJ_FILES)
	$(LD) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	@rm *.o; rm oemfoeland
