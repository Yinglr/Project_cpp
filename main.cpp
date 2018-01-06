#include "time_series.hpp"
#include "hedged_ptf.hpp"
#include "vol_surface.hpp"
#include "functions.hpp"

std::vector<double> vol_skew(project::BS::hedged_ptf& ptf, const std::vector<double>& strikes);

int main(int argc, char* argv[])
{
    
	// 1. open the datafile
	std::ifstream data_file("../data.csv", std::ios_base::in);
	
	// 2. create a hedged_ptf instance using the datafile
	project::BS::hedged_ptf ptf("S&P", data_file);
	data_file.close();
	
	// 3. setting the ptf + printing infos
	ptf.let_rate(0.00);
	ptf.print_info();
	
	// 4. create a vol_surface instance pointing on ptf
	project::VS::vol_surface vs(ptf);

	// 5. computes the whole volatility surface
	vs.load_vol_surface();
	vs.print_vol_surface();
	
	vs.print_maturity(12); 
	
	// 6. export the results to .csv file
	// vs.export_to_csv(/* optional path */);
	
	
	/* ptf.let_start(12);
	ptf.let_end(264);
	ptf.let_strike(ptf.get_ts()[12]);
	ptf.let_rate(0.01); */
	
	//ptf.print_info();
	//std::cout << ptf.get_maturity() << std::endl;
	/* for (int i=0; i<51;i++)
	{
		double vol=i*(0.2-0.05)/50+0.05;
		std::cout.precision(4);
		std::cout << "Vol: " << vol << ", PnL: "<< ptf.get_pnl(vol) << std::endl;
	} */
	
	/* for (double k=40; k<100; k=k+1)
	{
		ptf.let_strike(k*ptf.get_ts()[12]/100.0);
		std::cout << "The implied-vol for strike " << ptf.get_strike() << " is: " << ptf.get_vol(0.0001) << std::endl;
	} */
	//ptf.let_strike(100.0*ptf.get_ts()[12]/100.0);
	//std::cout << "The implied-vol for strike " << ptf.get_strike() << " is: " << ptf.get_vol(0.0001) << std::endl;
	
	// testing for maturity 1 year
	//ptf.let_start(ptf.get_ts().shift_months(ptf.get_ts().get_size(),12,false));
	//ptf.let_end(ptf.get_ts().get_size());
	//ptf.let_strike(ptf.get_ts()[12]);
	ptf.let_last_range(12);
	ptf.let_rate(0.01);
	ptf.print_info();
	
	std::vector<double> strikes = { 70.0, 80.0, 90.0, 100.0, 110.0, 120.0, 130.0, 140.0, 150.0};
	std::vector<double> skew = vol_skew(ptf,strikes);
	
	for(std::size_t i = 0; i < strikes.size(); ++i)
		std::cout << strikes[i] << " - " << skew[i] << std::endl;
	
	return 0;
	
	/* ptf.let_strike(100.);
	std::cout << ptf.get_implied_vol2() << std::endl; */
}

std::vector<double> vol_skew(project::BS::hedged_ptf& ptf, const std::vector<double>& strikes)
{
	std::vector<double> skew(strikes.size());
	for(std::size_t i = 0; i < strikes.size(); ++i)
	{
		ptf.let_strike(strikes[i]);
		skew[i] = ptf.get_implied_vol2();
	}
	return skew;
}