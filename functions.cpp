#include "bilu.hpp"
#include "functions.hpp"

namespace project
{
	
	namespace BS
	{
		
		
		
		/* -------------------------------- */
		/* ---- DELTA-HEDGED PORTFOLIO ---- */
		/* -------------------------------- */
		
		
		
		/* ------------------------------- */
		/* ---- GAUSSIAN DISTRIBUTION ---- */
		/* ------------------------------- */
		
		// normal cumulative distribution
		double normal_cdf(double x)
		{
		   return 0.5 * std::erfc(-x / std::sqrt(2));
		}
		
		// normal probability distribution
		double normal_pdf(double x)
		{
		   return std::exp(-x * x * 0.5) / std::sqrt(8 * std::atan(1.0));
		}
		
		/* -------------------------------- */
		/* ---- BLACK-SCHOLES FORMULAS ---- */
		/* -------------------------------- */
		
		// Black-Scholes Call Price
		double price_bs(double S, double K, double T, double r, double v, bool call)
		{
			double d = (std::log(S / K) + T * (r + 0.5 * v * v)) / (v * std::sqrt(T));
			double price = S * normal_cdf(d) - std::exp(-r * T) * K * normal_cdf(d - v * std::sqrt(T));
			if(call)
				return price;
			else
				return price - S + K * std::exp(-r * T); // from call-put parity
		}
		
		// Black-Scholes Delta
		double delta_bs(double S, double K, double T, double r, double v, bool call)
		{
			double d = (std::log(S / K) + T * (r + 0.5 * v * v)) / (v * std::sqrt(T));
			if(call)
				return normal_cdf(d);
			else
				return normal_cdf(d) - 1;
		}

		// Black-Scholes Gamma
		double gamma_bs(double S, double K, double T, double r, double v)
		{
			double d = (std::log(S / K) + T * (r + 0.5 * v * v)) / (v * std::sqrt(T));
			return normal_pdf(d) / S / v / std::sqrt(T);
		}

		// Black-Scholes Vega
		double vega_bs(double S, double K, double T, double r, double v)
		{
			double d = (std::log(S / K) + T * (r + 0.5 * v * v)) / (v * std::sqrt(T));
			return S * normal_pdf(d) * std::sqrt(T);
		}

		// Black-Scholes Rho
		double rho_bs(double S, double K, double T, double r, double v, bool call)
		{
			double d = (std::log(S / K) + T * (r + 0.5 * v * v)) / (v * std::sqrt(T));
			if(call)
				return T * K * std::exp(-r * T) * normal_cdf(d - v * std::sqrt(T));
			else
				return -T * K * std::exp(-r * T) * normal_cdf(v * std::sqrt(T) - d);
		}
		
		// Black-Scholes Theta
		double theta_bs(double S, double K, double T, double r, double v, bool call)
		{
			double d = (std::log(S / K) + T * (r + 0.5 * v * v)) / (v * std::sqrt(T));
			if(call)
				return -S * normal_pdf(d) * v / 2 / std::sqrt(T) - r * K * std::exp(-r * T) * normal_cdf(d - v * std::sqrt(T));
			else
				return -S * normal_pdf(d) * v / 2 / std::sqrt(T) + r * K * std::exp(-r * T) * normal_cdf(v * std::sqrt(T) - d);
		}
		
	}

}













