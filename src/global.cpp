#include "global.hpp"
#include <iostream>
#include <sstream>

void clearScreen() 
{
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

#ifdef _WIN32

void getKeypressDownInput(char& c) 
{
	std::cin >> c;
}

#else
#  include <termios.h>
#  include <unistd.h>

char getch() 
{
	char buf = 0;
	struct termios old = { 0 };

	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");

	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;

	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");

	if (read(0, &buf, 1) < 0)
		perror("read()");

	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;

	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("tcsetattr ~ICANON");

	return (buf);
}

void getKeypressDownInput(char& c) 
{
	c = getch();
}

#endif

void pause_for_keypress() 
{
	char c{};
	getKeypressDownInput(c);
}

std::string secondsFormat(double sec) 
{
	double second = sec;
	int minute = second / 60;
	int hour = minute / 60;

	second -= minute * 60;
	minute %= 60;
	second = static_cast<int>(second);

	std::ostringstream oss;
	if (hour) 
	{
		oss << hour << "h ";
	}
	if (minute) 
	{
		oss << minute << "m ";
	}
	oss << second << "s";

	return oss.str();
}