# Tanks Game
A 2D network tank game, with several type of tanks, bonuses and bots.

## Compilation
Extract all files of the zip archive, move into the game folder and execute make.

## Execution
### Server
Go into the game folder and execute : 
```
./server number_of_players [enable_fog]
```

number_of_players : an integer meaning the number of players for the game

enable_fog : 1 to enable fog

### Gamer
Go into the game folder and execute :
```
./tank ip_server
```

ip_server : ip address or name of the game server

### Bot
A bot is a computer controlled gamer. Go into the game folder and execute :
```
./bot ip_server
```

ip_server : ip address or name of the game server
