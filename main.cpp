#include "bilu.hpp"
#include "functions.hpp"

int main(int argc, char* argv[])
{
    // open the data file and count the lines
	std::ifstream data_file("../data.csv", std::ios_base::in);
	
	// time_series object + tests
	project::TS::time_series data("S&P", data_file);
	std::cout << data.get_name() << " - " << data.get_size() << std::endl;
	
	// printing tests
	std::size_t i = 743;
	data.print_line(i);
	project::csv::print_line(data_file, i);
	

	std::cout << data["13/12/2017"] << std::endl;
	
	// close the data file
	data_file.close();
	
	data.let_name("bilo");
	std::cout << data.get_name() << " - " << data.get_size() << std::endl;
	
	data.print_info();
	
	return 0;
}

