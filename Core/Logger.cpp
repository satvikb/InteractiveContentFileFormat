#include "Logger.h"

Logger Logger::instance;

Logger::Logger() {
	//log = new std::ofstream("log.txt");
	log.open("log.txt");
}

void Logger::logToFile(std::string str) {
	instance.log << str << std::endl;
}