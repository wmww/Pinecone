# I'm totally aware how shitty this makefile is
# I use the IDE Code::Blocks; this is only for people who don't have that
# I'll write a proper makefile at some point

build:
	g++ -Wall -std=c++11 -O2 src/*.cpp src/Actions/*.cpp src/utils/*.cpp -o pinecone
