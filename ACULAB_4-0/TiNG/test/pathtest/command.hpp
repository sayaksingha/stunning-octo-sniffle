#ifndef INCLUDED_COMMAND_HPP
#define INCLUDED_COMMAND_HPP

class CFG;

class COMMAND {
public:
	COMMAND() { }
		// parse some input. Return a pointer to the next
		// character to be processed, or 0 to indicate
		// parse has failed
	virtual char *parse(CFG *cfg, char *cmd) = 0;
		// show current settings
	virtual void show(CFG *cfg) = 0;
	virtual ~COMMAND() { };
		// name of this type of item
	static const char *const name;
};

#endif
