#LDFLAGS=-F/Library/Frameworks -framework SDL2 -framework Cocoa -I/System/Library/Frameworks/OpenGL.framework/Headers -framework OpenGL
#CFLAGS=-g -Wall -I/Library/Frameworks/SDL2.framework/Headers  -I/System/Library/Frameworks/OpenGL.framework/Headers
CFLAGS=-g -Wall `sdl2-config --cflags`
LDFLAGS=-g -Wall `sdl2-config --libs `

all: tank server bot

server: server.o
	gcc -o $@ $^ ${LDFLAGS}

tank: engine.o timer.o graphics.o tank.o client.o
	gcc -o $@ $^ ${LDFLAGS} -lm -lSDL2_ttf

bot: engine.o timer.o graphics.o graphe.o bot.o client.o
	gcc -o $@ $^ ${LDFLAGS} -lm -lSDL2_ttf


engine.o: engine.h
graphics.o: engine.h graphics.h timer.h
tank.o: engine.h graphics.h timer.h
timer.o: timer.h
client.o: client.h
server.o: server.h
bot.o: engine.h graphics.h timer.h graphe.h bot.h
graphe.o: graphe.h

.c.o:
	gcc -c ${CFLAGS} $<

clean:
	rm tank *.o *~
