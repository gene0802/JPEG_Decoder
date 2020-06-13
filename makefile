CC:=g++
exe:=main
other = main.cpp parser.cpp reader.cpp data.cpp decoder.cpp

all:$(obj)
	$(CC) $(other) -o $(exe)
%.o:%.c
	$(CC) -c $^ -o $@