#pragma once
#include <string>
#include <iostream>
#include <fstream>
class Logger {
	private:
	std::ofstream log;
	Logger();
	static Logger instance;


	public:
	static void logToFile(std::string str);
};