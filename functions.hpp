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

// for comparing std::tm structures (eg. in iterators)
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
		// returns a maturity in years using difftime overload (ACT/365 basis)
		double maturity(struct std::tm& start, struct std::tm& end);
		
		// normal distribution
		double normal_cdf(double x); // using std::erfc
		double normal_pdf(double x); // using std::atan
		
		// Black-Scholes formulas
		double price_bs(double S, double K, double T, double r, double v, bool call = true);
		double delta_bs(double S, double K, double T, double r, double v, bool call = true);
		double gamma_bs(double S, double K, double T, double r, double v, bool call = true);
		double vega_bs(double S, double K, double T, double r, double v, bool call = true);
		double rho_bs(double S, double K, double T, double r, double v, bool call = true);
		double theta_bs(double S, double K, double T, double r, double v, bool call = true);

	}
	
	
	
	
	/* ---------------------------------- */
	/* ---- MANIPULATING TIME SERIES ---- */
	/* ---------------------------------- */
	
	namespace TS
	{
		// string to std::tm
		struct std::tm to_date(const std::string& strdate);
		
		// convert a difftime (which is in seconds)
		double difftime_to_days(double difftime);
		double difftime_to_years(double difftime); // basis ACT/365
		
		// std::tm to string / for printing struct std::tm
		std::string to_string(struct std::tm tm);
	}
	
	
	
	
	/* -------------------------------- */
	/* ---- MANIPULATING CSV FILES ---- */
	/* -------------------------------- */
	
	// CSV FILES
	namespace csv
	{
		// checks if the file is open (used in all functions using csv)
		void is_open(std::ifstream& csv_file);
		
		// comes back to the beginning of the csv file (after an interation)
		void reset(std::ifstream& csv_file);
		
		// returns the number of lines in the file
		std::size_t count_lines(std::ifstream& csv_file);
		
		// prints the whole csv file
		void print_csv(std::ifstream& csv_file);

		// prints only the requested line from the csv file
		void print_line(std::ifstream& csv_file, std::size_t line); 
	}

}



#endif