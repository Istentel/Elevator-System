#pragma once
#include <string>
#include <unordered_map>
#include <sstream>
#include <queue>
#include <thread>

#define FLOORS 30
#define WEIGHT 10000

class Elevator {
	std::string name;
	std::string status;
	std::queue<std::string> commands;
	int currentFloor;
	int maxWeight;
	int currentWeight;
	bool nextCmd;

public:

	Elevator(); 

	Elevator(std::string name, int maxWeight);

	//getters and setters

	std::string getName();

	std::string getStatus();

	void pushCommand(std::string command);

	std::queue<std::string> getCommands();

	int getMaxWeight();

	int getCurrentWeight();

	void setCurrentWeight(int weight);

	bool getNextCmd();

	void setNextCmd(bool cmdStatus);

	//methods

	void nextCommand();

	void call(int floorId);
};

class ElevatorSystem {
	static std::unordered_map<std::string, Elevator> elevators;

	static bool exist(std::string);
public:

	friend class Elevator;

	ElevatorSystem();

	static std::string addCommand(std::string command);

	static std::string execute(std::string command);

};
