#ifndef AGENT__
#define AGENT__

#include <string>
#include <iostream>
using namespace std;

// -----------------------------------------------------------
//				class Agent
// -----------------------------------------------------------
class Environment;
class Agent
{
public:
	Agent(int x, int y){
		RobotX_ = 0;
		RobotY_ = 0;
		ObjX_ = x;
		ObjY_ = y;
	}

	enum ActionType
	{
	    actFORWARD,
	    actTURN,
	    actIDLE
	};

	void Perceive(const Environment &env);
	ActionType Think();

private:
	int RobotX_, RobotY_, ObjX_, ObjY_;
};

string ActionStr(Agent::ActionType);

#endif
