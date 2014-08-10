APPNAME = compress

LFLAGS = 
DFLAGS = -g -Wno-unused-result
EXT =

OBJECT_FILES = compressor.o main.o

ifdef MINGW
LFLAGS += -lmingw32 -mwindows -lSDL2main -static-libgcc -static-libstdc++
DFLAGS += -DMINGW -DNO_STDIO_REDIRECT
EXT = .exe
endif

APP = $(APPNAME)$(EXT)

all: $(APP)

rebuild: clean $(APP)

.cpp.o:
	$(CXX) -c $< $(DFLAGS)

$(APP): $(OBJECT_FILES)
	$(CXX) $(OBJECT_FILES) -o $(APP) $(LFLAGS) $(DFLAGS)

run: $(APP) test.txt
	./$(APP) test.txt test.cmp
	./$(APP) -x test.cmp out.txt
	diff test.txt out.txt

clean:
	rm -f *~ *.o $(APP) Thumbs.db *.stackdump test.cmp
