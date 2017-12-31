#include "bilu.hpp"
#include "functions.hpp"

namespace project
{
	
	namespace BS
	{
		
		
		double maturity(struct std::tm& start, struct std::tm& end)
		{
			double difftime = std::difftime(start, end);
			return TS::difftime_to_years(difftime); // base ACT/365 for simplicity (see TS::difftime_to_years)
		}
		
		
		
		
		
		
		
		
		
		/* -------------------------------- */
		/* ---- DELTA-HEDGED PORTFOLIO ---- */
		/* -------------------------------- */
		
		// constructors
		hedged_ptf::hedged_ptf(const std::string& name, std::ifstream& csv_file,
							   double strike, double rate, double div)
			: m_ts(name, csv_file), m_strike(strike), m_rate(rate), m_div(div)
		{
			m_start = 1;
			m_end = m_ts.get_size();
		}
		
		
		
		// destructor
		hedged_ptf::~hedged_ptf()
		{
			std::cout << "Deletion of hedged_ptf object " << get_name() << std::endl;
		}
		
		
		
		
		
		
		// access - general
		std::string hedged_ptf::get_name() const
		{
			return m_ts.get_name();
		}
		
		std::size_t hedged_ptf::get_size() const
		{
			return m_ts.get_size();
		}
		
		std::size_t hedged_ptf::get_size_range() const
		{
			return m_end - m_start + 1;
		}
		
		
		// access - values
		double hedged_ptf::get_spot() const
		{
			return m_ts[m_start];
		}
		
		double hedged_ptf::get_maturity() const
		{
			return maturity(m_ts.get_date(m_end), m_ts.get_date(m_start));
		}
		
		double hedged_ptf::get_strike() const
		{
			return m_strike;
		}
		
		double hedged_ptf::get_rate() const
		{
			return m_rate;
		}
		
		double hedged_ptf::get_div() const
		{
			return m_div;
		}
		
		
		
		
		
		
		// access - time_series
		const TS::time_series& hedged_ptf::get_ts() const
		{
			return m_ts;
		}
		
		
		
		
		// access - date range
		std::size_t hedged_ptf::get_start() const
		{
			return m_start;
		}
		
		std::size_t hedged_ptf::get_end() const
		{
			return m_end;
		}
		
		
		
		
		// printing
		void hedged_ptf::print_info() const
		{
			std::cout << std::endl;
			std::cout << "---------------------------------" << std::endl;
			std::cout << "General info on hedged_ptf object " << get_name() << std::endl;
			std::cout << "---------------------------------" << std::endl;
			std::cout << "Nb of elements (range):           " << get_size() << std::endl;
			std::cout << "Nb of elements (interior range):  " << get_size_range() << std::endl;
			std::cout << "Start of total range:             " << TS::to_string(m_ts.get_date(1)) << std::endl;
			std::cout << "Start of interior range:          " << TS::to_string(m_ts.get_date(m_start)) << std::endl;
			std::cout << "End of interior range:            " << TS::to_string(m_ts.get_date(m_end)) << std::endl;
			std::cout << "End of total range:               " << TS::to_string(m_ts.get_date(get_size())) << std::endl;
			std::cout << "---------------------------------" << std::endl;
			std::cout << std::endl;
		}
		
		
		
		
		
		
		
		
		// modify - general
		void hedged_ptf::let_name(std::string name)
		{
			m_ts.let_name(name);
		}
		
		
		// modify - values
		void hedged_ptf::let_strike(double strike)
		{
			if(strike<=0)
			{
				std::cout << "Error: negative strike on portfolio " << get_name() << std::endl;
			}
			else
			{
				std::cout << "Strike of portfolio " << get_name() << " set to " << strike << std::endl;
				m_strike = strike;
			}	
		}
		
		void hedged_ptf::let_rate(double rate)
		{
			// no constraint as rates can actually go negative!
			std::cout << "Rate of portfolio " << get_name() << " set to " << rate << std::endl;
			m_rate = rate;
		}
		
		void hedged_ptf::let_div(double div)
		{
			if(div<=0)
			{
				std::cout << "Error: negative dividends on portfolio " << get_name() << std::endl;
			}
			else
			{
				std::cout << "Dividends of portfolio " << get_name() << " set to " << div << std::endl;
				m_div = div;
			}
		}
		
		
		// modify - date range
		void hedged_ptf::let_start(std::size_t start)
		{
			std::cout << "Start of portfolio " << get_name() << " set to " << start
					<< " (" << TS::to_string(m_ts.get_date(start)) << ")" << std::endl;
			m_start = start;
		}
		
		void hedged_ptf::let_end(std::size_t end)
		{
			std::cout << "End of portfolio " << get_name() << " set to " << end
					<< " (" << TS::to_string(m_ts.get_date(end)) << ")" << std::endl;
			m_end = end;
		}
		
		
		
		
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













