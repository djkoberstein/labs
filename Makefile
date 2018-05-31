a.out : main.cpp
	g++ -g -Wall -Wextra -std=c++11 main.cpp -lpthread -lboost_system 