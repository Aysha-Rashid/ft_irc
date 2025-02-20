
#ifndef	COMMANDS_H
#define COMMANDS_H

class Commands
{
public:
	Commands() = delete;
	Commands(const Commands& other) = delete;
	Commands&	operator=(Commands& rhs) = delete;
	~Commands() = delete;

	static void kick(/*Client& client*/);
	static void invite(/**/);
	static void topic(/**/);
	static void mode(/**/);
};

#endif
