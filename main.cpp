#include "time_series.hpp"
#include "hedged_ptf.hpp"
#include "vol_surface.hpp"
#include "functions.hpp"


void print_diff(const std::vector<double>& v1, const std::vector<double>& v2)
{
	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	for(std::size_t i = 0; i < v1.size(); ++i)
	{
		std::cout << std::setfill('0') << std::setw(2) << i+1 << " - ";
		std::cout << std::setprecision(4) << v1[i] - v2[i] << std::endl;
	}
	std::cout.unsetf(std::ios::floatfield);
}


int main(int argc, char* argv[])
{
    
	// 1. open the datafile
	std::ifstream data_file("../data.csv", std::ios_base::in);
	
	// 2. create a hedged_ptf instance using the datafile
	project::BS::hedged_ptf ptf("S&P", data_file);
	data_file.close();
	
	// 3. setting the ptf + printing infos
	ptf.let_rate(0.01);
	ptf.print_info();
	
	// 4. create a vol_surface instance pointing on ptf
	project::VS::vol_surface vs(ptf);
	project::VS::vol_surface vs_robust(ptf);
	
	// this line is for comparison with quoted skew
	// project::VS::vol_surface vs(ptf,std::vector<double> {12},std::vector<double> {30,40,60,80,90,95,97.5,100,102.5,105,110,120,150,200,300});

	
	// 5. computes the whole volatility surface
	vs.load_vol_surface(false); // delta hedging method
	vs_robust.load_vol_surface(true); // gamma pnl method
	
	// 6. printing our results
	vs.print_vol_surface();
	vs_robust.print_vol_surface();
	
	// 7. export the results to .csv file
	vs.export_to_csv(/* optional path */);
	vs_robust.export_to_csv(/* optional path */);
	
	// 8. quickly compare differences (easier in Excel with a heatmap...)
	// print_diff(vs.get_strike(100), vs_robust.get_strike(100));

	
	return 0;
}