#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

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
	
	namespace BS
	{

		
		double maturity(struct std::tm& start, struct std::tm& end);
		
		
		
		
		/* -------------------------------- */
		/* ---- DELTA-HEDGED PORTFOLIO ---- */
		/* -------------------------------- */
		
		// class of the delta-hedged portfolio we will manipulate
		class hedged_ptf
		{
		public:
			
			// constructors
			hedged_ptf(const std::string& name, std::ifstream& csv_file,
					   double strike = 100, double rate = 0, double div = 0);
			
			// destructor
			~hedged_ptf();
			
			// access - general
			std::string get_name() const;
			std::size_t get_size() const;
			std::size_t get_size_range() const; // between start and end

			
			// access - values
			double get_spot() const;
			double get_maturity() const;
			double get_strike() const;
			double get_rate() const;
			double get_div() const;
			
			// access - time_series
			const TS::time_series& get_ts() const;
			
			// access - date range
			std::size_t get_start() const;
			std::size_t get_end() const;
			
			
			// printing
			void print_info() const;
			// void print_range() const;
			
			
			
			// modify - general
			void let_name(std::string name);
			
			// modify - values
			void let_strike(double strike);
			void let_rate(double rate);
			void let_div(double div);
			
			// modify - date range
			// !!!!!
			// a faire : gérer le cas où ça se croise // cas out of range
			// !!!!!
			void let_start(std::size_t start);
			void let_end(std::size_t end);
			// void let_range(std::size_t start, std::size_t end);
			
			
			// // sets range to last n months (for vol computations)
			// void let_last_range(std::size_t n, bool next = true)); // uses shift_months
			
			
			// // P&L computations
			// double get_pnl(double vol) const;
			// double get_vol() const;
			
			
		private:
			
			// data members
			
			// parameters
			double m_strike;
			double m_rate;
			double m_div;
			
			// time_series
			TS::time_series m_ts;
			std::size_t m_start;
			std::size_t m_end;
			
			
		};

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		/* -------------------------------- */
		/* ---- BLACK-SCHOLES FORMULAS ---- */
		/* -------------------------------- */
		
		// normal distribution
		double normal_cdf(double x);
		double normal_pdf(double x);
		
		// Black-Scholes formulas
		double price_bs(double S, double K, double T, double r, double v, bool call = true);
		double delta_bs(double S, double K, double T, double r, double v, bool call = true);
		double gamma_bs(double S, double K, double T, double r, double v);
		double vega_bs(double S, double K, double T, double r, double v);
		double rho_bs(double S, double K, double T, double r, double v, bool call = true);
		double theta_bs(double S, double K, double T, double r, double v, bool call = true);

	}

}



#endif