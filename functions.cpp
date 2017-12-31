#include "bilu.hpp"
#include "functions.hpp"

namespace project
{
	
	namespace BS
	{
		
		
		double maturity(struct std::tm& end, struct std::tm& start)
		{
			double difftime = std::difftime(end, start);
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
		void hedged_ptf::let_strike(double strike, bool percent)
		{
			if(strike<=0)
			{
				std::cout << "Error: negative strike on portfolio " << get_name() << std::endl;
			}
			else
			{
				if(percent)
				{
					m_strike = strike * get_spot() / 100.0;
				}
				else
				{
					m_strike = strike;
				}
				std::cout << "Strike of portfolio " << get_name() << " set to " << m_strike << std::endl;
				
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
		void hedged_ptf::let_last_range(std::size_t n, bool next)
		{
			let_start(m_ts.shift_months(m_ts.get_size(),static_cast<int>(n),false));
			let_end(m_ts.get_size());
			
		}
		
		double hedged_ptf::get_pnl(double vol, bool call) const
		{
			// time to maturity
			double mat = get_maturity();
			
			// portfolio
			double value = price_bs(get_spot(), m_strike, mat, m_rate, vol, call);
			double inv_stock = delta_bs(get_spot(), m_strike, mat, m_rate, vol, call);
			double inv_rate = value - get_spot() * inv_stock;
			
			// loop on the range
			for(std::size_t i = 1; i < get_size_range(); ++i)
			{
				// compute current maturity 
				mat = maturity(m_ts.get_date(m_end), m_ts.get_date(m_start + i));
				
				// change in portfolio value
				value += inv_stock * (m_ts[m_start + i] - m_ts[m_start + i - 1])
					   + inv_rate * (std::exp(m_rate * maturity(m_ts.get_date(m_start + i), m_ts.get_date(m_start + i - 1))) - 1);
				
				// new delta 
				if(mat != 0)
					inv_stock = delta_bs(m_ts[m_start + i], m_strike, mat, m_rate, vol, call);
				
				// the rest is invested in the risk-free asset
				inv_rate = value - m_ts[m_start + i] * inv_stock;
				
				// print
				// std::cout << mat << ' ' << m_ts[m_start + i] << ' ' << inv_stock << ' ' << inv_rate << ' ' << value << std::endl;
			}
			
			// std::cout << "final payoff: " << std::max(m_ts[m_end] - m_strike, 0.0) << std::endl;
			double payoff = call ? std::max((m_ts[m_end] - m_strike), 0.0) : std::max((m_strike - m_ts[m_end]), 0.0);
			return value - payoff;
		}
		
		
		double hedged_ptf::get_vol(double precision, double v_low, double v_high) const
		{
			// optimization depending on the moneyness
			bool call = (m_ts[m_end] - m_strike > 0.0) ? true : false;
			// testing if the two bounds have the same signal
			if (get_pnl(v_low,call)*get_pnl(v_high,call)>0)
			{
				std::cout<< "Error in dichotomy method" << std::endl;
			}
			// initialization
			double vol = (v_low + v_high) / 2.0;
			double pnl = get_pnl(vol, call);
			std::size_t count = 0;
			/* 
			// test low spot
			for (int i=0; i<201; i++)
			{
				vol=v_low+i*(v_high-v_low)/200.;
				std::cout<< "Vol: " << vol<<"  PnL: " << get_pnl(vol)<<std::endl;
			} */
			
			
			// dichotomy loop
			while(std::abs(v_high - v_low) >= precision)
			{
				if(++count == 1.0/precision)
				{
					std::cout << "Dichotomy for implied vol did not converge" << std::endl;
					return 0;
				}
				if(pnl > 0)
				{
					v_high = vol;
				}
				else
				{
					v_low = vol;
				}
				vol = (v_low + v_high) / 2.0;
				pnl = get_pnl(vol, call);
			}
			// std::cout << "Dichotomy converged in " << count << " iterations" << std::endl;
			return vol;
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













