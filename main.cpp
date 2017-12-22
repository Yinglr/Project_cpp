#include "bilu.hpp"

int main(int argc, char* argv[])
{
    // open the data file and count the lines
	std::ifstream data_file("../data.csv", std::ios_base::in);
	std::size_t nb_lines = project::csv::count_lines(data_file);
	
	// time_series object + tests
	project::TS::time_series data("S&P", nb_lines);
	std::cout << data.get_name() << " - " << data.get_size() << std::endl;
	data.load_from_csv(data_file);
	
	// printing tests
	std::size_t i = 743;
	data.print();
	data.print_line(i);
	project::csv::print_line(data_file, i);
	
	std::cout << data.get_line(i).index << " - "
		<< std::put_time(&data.get_line(i).date, "%d/%m/%Y")
		<< " - " << data.get_line(i).value << std::endl;
	
	std::cout << data["11/12/2017"] << std::endl;
	
	// close the data file
	data_file.close();
	
	return 0;
}

