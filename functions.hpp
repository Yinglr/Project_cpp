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

		/* -------------------------------- */
		/* ---- DELTA-HEDGED PORTFOLIO ---- */
		/* -------------------------------- */
		
		
		
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