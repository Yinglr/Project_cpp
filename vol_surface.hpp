#ifndef VOL_SURFACE_HPP
#define VOL_SURFACE_HPP

// libs of the project

#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace project
{
	
	namespace VS
	{
		
		/* ---------------------------------- */
		/* ---- VOLATILITY SURFACE CLASS ---- */
		/* ---------------------------------- */
		
		// class of the volatility surface
		class vol_surface
		{
		public:
			
			// constructors
			vol_surface(BS::hedged_ptf& ptf,
						std::vector<double> strikes = {50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150},
						std::vector<double> maturities = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
			
			// destructor
			~vol_surface();
			
			
			// access - data members
			std::string get_name() const; // from the pointed ptf
			const std::vector<double>& get_strikes() const;
			const std::vector<double>& get_maturities() const;
			
			
			// access - volatilities
			double get_vol(double strike, double maturity) const;
			std::vector<double> get_strike(double strike) const; // term structure
			std::vector<double> get_maturity(double maturity) const; // skew
			
			
			// printing - general
			// void print_info() const;
			void print_strikes(std::string str = " ") const;
			void print_maturities(std::string str = " ") const;
			
			// printing - volatilities
			void print_strike(double strike) const; // term structure
			void print_maturity(double maturity) const; // skew
			void print_vol_surface() const;
			
			
			// modify
			void load_vol_surface(); // loops with ptf.get_implied_vol()
			
			void let_strikes(std::vector<double> strikes = {50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150});
			void let_maturities(std::vector<double> maturities = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
			
			void let_ptf(BS::hedged_ptf& ptf);
			
			
			// export
			void export_to_csv(std::string path = "..") const; // default path is outside of build
			
			
			
		private:
			
			// data members
			std::vector<double> m_strikes;
			std::vector<double> m_maturities;
			
			// volatility surface
			std::vector<double> m_vols; // vectorized 2d matrix
			
			// hedged_ptf class from which we get the implied vols
			BS::hedged_ptf *p_ptf;
			
			// private methods for getting indices
			std::size_t index_strike(double strike) const;
			std::size_t index_maturity(double maturity) const;
			
			
		};
		
		
		
		
	}

}



#endif