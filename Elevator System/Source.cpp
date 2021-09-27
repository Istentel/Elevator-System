#include <iostream>
#include "ElevatorSystem.h"

using namespace std;

int main() {
	ElevatorSystem system = ElevatorSystem();

	string command;
	while (getline(cin, command)) {
		if (command == "exit")
			break;

		cout << system.execute(command);
	}

}