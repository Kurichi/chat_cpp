CXX = g++ -std=c++11

CXXFLAGS = -Wall -g

INCDIRS = -I./

server: ./server/server_main.cpp ./server/server.cpp ./server/server.hpp
	$(CXX) $(CXXFLAGS) $(INCDIRS) -o ./server/server ./server/server_main.cpp -pthread

client: ./client/client_main.cpp ./client/client.cpp ./client/client.hpp
	$(CXX) $(CXXFLAGS) $(INCDIRS) -o ./client/client ./client/client_main.cpp -pthread