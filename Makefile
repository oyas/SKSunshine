LIBS          = -lglut -lGLU -lGL -lalut -lopenal
OBJS          = src/Main.o src/assist/assist.o src/Stage/stage1.o src/XMesh/XLoader.o \
				src/Collision/colli.o src/Sound/sound.o src/Stage/stage0.o \
				src/assist/object.o src/Stage/stage2.o src/image/PNGtexture.o \
				src/assist/vector_math.o src/Player/player.o
PROGRAM       = bin/a.out
LIBS_WIN      = -lopengl32 -lfreeglut -lglu32 -lglew32 -lOpenAL32 -lalut
PROGRAM_WIN   = bin/a.exe
WINR          =


all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) -W -Wall $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)


win: $(PROGRAM_WIN)

$(PROGRAM_WIN): $(OBJS)
	$(CXX) -W -Wall $(OBJS) $(LIBS_WIN) $(WINR) -o $(PROGRAM_WIN)


win_r: ;
	rm -f $(PROGRAM_WIN)
	make win WINR=-mwindows


clean:;	rm -f $(OBJS) $(PROGRAM)


# サフィックスルール (.cpp → .o)
.cpp.o:
#	$(CXX) -W -Wall -c $<
#	$(CXX) -g -O0 -c $< -o $@
	$(CXX) -O3 -c $< -o $@

#以下追加の依存関係
src/Main.o: src/assist/assist.h src/XMesh/XLoader.h
src/assist/assist.o: src/assist/assist.h
src/Stage/stage1.o: src/assist/assist.h src/Stage/stage1.h
src/XMesh/XLoader.o: src/XMesh/XLoader.h src/XMesh/XLoader_sub.cpp
src/Collision/colli.o: src/Collision/colli.h
src/Sound/sound.o: src/Sound/sound.h


