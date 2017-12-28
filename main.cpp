#include "bilu.hpp"
#include "functions.hpp"

int main(int argc, char* argv[])
{
    // time series object
	std::ifstream data_file("../data.csv", std::ios_base::in);
	project::TS::time_series data("S&P", data_file);
	data_file.close();
	
	data.print_info();
	
	std::cout << data.shift_months(data.date_end(), 3, false) << std::endl;
	std::cout << data.shift_days(data.date_end(), 91, false) << std::endl;
	
	
	
	return 0;
}

