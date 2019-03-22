FLAGS = -Wall -Werror
CPP_FILES = src/main.cpp src/wst_to_ws.cpp src/ws_to_wst.cpp src/code_gen.cpp

main: $(CPP_FILES)
	g++ -o wswac $(CPP_FILES) $(FLAGS)

clean:
	rm wswac
