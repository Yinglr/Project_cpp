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


	/* -------------------------------- */
	/* ---- MANIPULATING STRUCT TM ---- */
	/* -------------------------------- */

// for comparing times in iterators
bool operator==(const struct std::tm& lhs, const struct std::tm& rhs);
bool operator>(struct std::tm& lhs, struct std::tm& rhs);
bool operator<(struct std::tm& lhs, struct std::tm& rhs);
bool operator>=(struct std::tm& lhs, struct std::tm& rhs);
bool operator<=(struct std::tm& lhs, struct std::tm& rhs);


// overloads for std::tm
namespace std
{
	double difftime(struct std::tm& time_end, struct std::tm& time_beg);
	double difftime(const std::string& time_end, const std::string& time_beg);
}



namespace project
{
	
	
	/* -------------------------------- */
	/* ---- BLACK-SCHOLES FORMULAS ---- */
	/* -------------------------------- */	
	
	namespace BS
	{
		double maturity(struct std::tm& start, struct std::tm& end);
		
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
	
	
	
	
	/* ---------------------------------- */
	/* ---- MANIPULATING TIME SERIES ---- */
	/* ---------------------------------- */
	
	namespace TS
	{
		struct std::tm to_date(const std::string& strdate);
		
		// convert a difftime 
		double difftime_to_days(double difftime);
		double difftime_to_years(double difftime); // base ACT/365
		
		// for printing struct std::tm
		std::string to_string(struct std::tm tm);
	}
	
	
	
	
	/* -------------------------------- */
	/* ---- MANIPULATING CSV FILES ---- */
	/* -------------------------------- */
	
	// CSV FILES
	namespace csv
	{
		bool is_open(std::ifstream& csv_file);
		void reset(std::ifstream& csv_file);
		
		
		std::size_t count_lines(std::ifstream& csv_file);
		
		
		void print_csv(std::ifstream& csv_file);
		void print_line(std::ifstream& csv_file, std::size_t line);
	}

}



#endif