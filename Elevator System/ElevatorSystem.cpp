#include "ElevatorSystem.h"

Elevator::Elevator() {
	this->currentFloor = 0;
	this->maxWeight = 0;
	this->currentWeight = 0;
	this->nextCmd = false;
}

Elevator::Elevator(std::string name, int maxWeight) {
	this->name = name;
	this->status = "stationary 0\n";
	this->currentFloor = 0;
	this->maxWeight = maxWeight;
	this->currentWeight = 0;
	this->nextCmd = false;
}

std::string Elevator::getName() { return name; }

std::string Elevator::getStatus() { return status; }

void Elevator::call(int floorId) {

	using namespace std::literals::chrono_literals;

	if (floorId == currentFloor)
		return;

	bool direction = floorId > currentFloor ? true : false;

	if (direction)
		status = "moving-up " + std::to_string(currentFloor) + ' ' + std::to_string(floorId) + ' ' + std::to_string(currentWeight) + "\n";
	else
		status = "moving-down " + std::to_string(currentFloor) + ' ' + std::to_string(floorId) + ' ' + std::to_string(currentWeight) + "\n";

	while (currentFloor != floorId) {
		//_sleep(5000);
		std::this_thread::sleep_for(5s);

		if (direction)
			currentFloor++;
		else
			currentFloor--;

		if (direction)
			status = "moving-up " + std::to_string(currentFloor) + ' ' + std::to_string(floorId) + ' ' + std::to_string(currentWeight) + "\n";
		else
			status = "moving-down " + std::to_string(currentFloor) + ' ' + std::to_string(floorId) + ' ' + std::to_string(currentWeight) + "\n";

	}

	status = "stationary " + std::to_string(currentFloor) + "\n";

	commands.pop();
}

void Elevator::pushCommand(std::string command) {
	this->commands.push(command);
}

std::queue<std::string> Elevator::getCommands() {
	return this->commands;
}

void Elevator::nextCommand() {
	if (!commands.empty()) {
		nextCmd = true;
		ElevatorSystem::execute(commands.front());
	}
}

int Elevator::getMaxWeight() {
	return this->maxWeight;
}

int Elevator::getCurrentWeight() {
	return this->currentWeight;
}

void Elevator::setCurrentWeight(int weight) {
	this->currentWeight += weight;
}

bool Elevator::getNextCmd() {
	return this->nextCmd;
}

void Elevator::setNextCmd(bool cmdStatus) {
	this->nextCmd = cmdStatus;
}


/////// ELEVATOR SYSTEM /////////


ElevatorSystem::ElevatorSystem() {}

bool ElevatorSystem::exist(std::string elvName) {
	if (elevators.find(elvName) == elevators.end())
		return false;
	return true;
}

std::string ElevatorSystem::execute(std::string command) {
	std::istringstream ss(command);
	std::string cmd;

	ss >> cmd;

	if (cmd == "add-elevator") {
		std::string elvName;
		int weight;

		ss >> elvName >> weight;

		if (weight <= 1 || weight > WEIGHT) {
			return "failure1\n";
		}

		elevators[elvName] = Elevator(elvName, weight);

		return "success\n";
	}
	else if (cmd == "status") {
		std::string elvName;

		ss >> elvName;

		if (!exist(elvName))
			return "there is no elevator with that name\n";

		return elevators[elvName].getStatus();
	}
	else if (cmd == "call-elevator") {
		std::string elvName;
		int floorId;

		ss >> elvName >> floorId;

		if (!exist(elvName))
			return "there is no elevator with that name\n";

		if (floorId > FLOORS)
			return "the highest floor is " + std::to_string(FLOORS) + "!\n";
		else if (floorId < 0)
			return "the lowest floor is 0!\n";

		if(!elevators[elvName].getNextCmd())
			elevators[elvName].pushCommand(command);

		if (elevators[elvName].getCommands().size() == 1 || elevators[elvName].getNextCmd()) {
			std::thread runningElv(&Elevator::call, &elevators[elvName], floorId);
			runningElv.detach();
		}

		elevators[elvName].setNextCmd(false);
		
		return "success\n";
	}
	else if (cmd == "continue") {
		std::string elvName;

		ss >> elvName;

		if (!exist(elvName))
			return "there is no elevator with that name\n";

		if (elevators[elvName].getStatus().substr(0, 10) == "stationary")
			elevators[elvName].nextCommand();
		else
			return "failure\n";

		return "success\n";
	}
	else if (cmd == "enter-elevator") {
		std::string  elvName;
		int weightIn;
		std::vector<int> floors;
		int floor;
		bool firstCommand = false;

		ss >> elvName >> weightIn;

		if (!exist(elvName))
			return "there is no elevator with that name\n";

		if (elevators[elvName].getStatus().substr(0, 10) != "stationary")
			return "the elevator is moving!\n";

		if (weightIn > elevators[elvName].getMaxWeight())
			return "weight limit!\n";

		if (elevators[elvName].getCommands().empty())
			firstCommand = true;

		while (ss >> floor) {
			floors.push_back(floor);
		}

		if (floors.empty())
			return "there are no floors!\n";

		//std::sort(floors.begin(), floors.end());

		for (int i : floors) {
			elevators[elvName].pushCommand("call-elevator " + elvName + ' ' + std::to_string(i));
		}

		elevators[elvName].setCurrentWeight(weightIn);

		if (firstCommand) {
			std::thread runningElv(&Elevator::call, &elevators[elvName], floors[0]);
			runningElv.detach();
		}

		return "success\n";
	}
	else if (cmd == "exit-elevator") {
		std::string  elvName;
		int weightOut;

		ss >> elvName >> weightOut;

		if (!exist(elvName))
			return "there is no elevator with that name\n";

		if (weightOut < 1 || weightOut > elevators[elvName].getCurrentWeight()) {
			return "weight is out of bounds!\n";
		}

		if (elevators[elvName].getStatus().substr(0, 10) == "stationary")
			elevators[elvName].setCurrentWeight(-weightOut);
		else
			return "the elevator is moving!\n";

		return "success\n";
	}

	return "command not found!\n";
}

std::unordered_map<std::string, Elevator> ElevatorSystem::elevators;