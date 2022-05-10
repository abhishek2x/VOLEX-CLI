CPP_FILES = main.cpp file_sys.cpp cli.cpp utils.cpp
H_FILES = file_sys.h cli.h utils.h

all: pa3

pa3: $(CPP_FILES) $(H_FILES)
	g++ -o $@ $(CPP_FILES)