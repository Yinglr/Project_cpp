#include "bilu.hpp"
#include "functions.hpp"

int main(int argc, char* argv[])
{
    // time series object
	std::ifstream data_file("../data.csv", std::ios_base::in);
	project::BS::hedged_ptf ptf("S&P", data_file);
	data_file.close();
	
	
	
	ptf.let_start(12);
	ptf.let_end(264);
	
	
	
	ptf.print_info();
	std::cout << ptf.get_maturity() << std::endl;
	
	
	
	
	
	return 0;
}

