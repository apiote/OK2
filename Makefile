all: ok2.cpp
	g++ -std=c++11 ok2.cpp -o ok2
g: ok2.cpp
	g++ -std=c++11 -g ok2.cpp -o ok2
clean:
	rm ok2
