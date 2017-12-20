#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include "bilu.hpp"

int bilu(int argc, char* argv[])
{
    
	std::ifstream datafile("data.csv");
	
 	if(datafile.is_open())
		std::cout << "ERROR: file open\n" << std::endl;
	
	std::string teste;
	
	
	
	std::getline(ip,teste,',');
	
	std::cout << teste << std::endl;
	
	
	ip.close();
	
	
	
	
	return 0;
}

