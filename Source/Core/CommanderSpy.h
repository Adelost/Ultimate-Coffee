#pragma once

class Commander;
class Command;

//friend class of "Commander"
class CommanderSpy
{
private:
	Commander* m_commander;

public:
	CommanderSpy(Commander* commander);
	~CommanderSpy(void);

	std::vector<Command*>* getCommands();
};
