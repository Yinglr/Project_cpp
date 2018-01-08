#include "time_series.hpp"
#include "hedged_ptf.hpp"
#include "vol_surface.hpp"
#include "functions.hpp"

namespace project
{
	
	namespace BS
	{
		
		
		/* -------------------------------- */
		/* ---- DELTA-HEDGED PORTFOLIO ---- */
		/* -------------------------------- */
		
		// constructors
		hedged_ptf::hedged_ptf(const std::string& name, std::ifstream& csv_file,
							   double strike, double rate, double div)
			: m_ts(name, csv_file), m_strike(strike), m_rate(rate), m_div(div)
		{
			// time_series object are base 1
			m_start = 1; 
			m_end = m_ts.get_size();
			let_strike(strike);
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
			// total size of the time_series object
			return m_ts.get_size();
		}
		
		std::size_t hedged_ptf::get_size_range() const
		{
			// size of the range currently used
			return m_end - m_start + 1;
		}
		
		
		// access - values
		double hedged_ptf::get_spot() const
		{
			return m_ts[m_start];
		}
		
		double hedged_ptf::get_maturity() const
		{
			// maturity of the range currently used
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
		
		// general info on the hedged_ptf object
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
			// usually done in percentage, after setting the date range (not before...)
			if(strike <= 0.0)
			{
				// cannot set a negative strike
				std::cout << "Error: negative strike on portfolio " << get_name() << std::endl;
			}
			else
			{
				// the strike can be either in percentage or in absolute value (not recommended)
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
			// implement a more complex version, using a time_series instance of rates?
		}
		
		void hedged_ptf::let_div(double div)
		{
			// dividend computations currently not implemented
			if(div <= 0.0)
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
			// if out of range
			if((start < 1) | (start >= get_size()))
			{
				std::cout << "Error on portfolio " << get_name() << ": attempted let_start " << start
						<< " is out of its possible range (" << 1 << " - " << get_size()-1 << ")" << std::endl;
			}
			else
			{
				// if start above current end
				if(start >= m_end)
				{
					std::cout << "Error on portfolio " << get_name() << ": attempted let_start " << start
							<< " is equal to or above end (" << m_end << ")" << std::endl;
				}
				else
				{
					// if all good
					std::cout << "Start of portfolio " << get_name() << " set to " << start
							<< " (" << TS::to_string(m_ts.get_date(start)) << ")" << std::endl;
					m_start = start; // let the new start
				}
			}
		}
		
		void hedged_ptf::let_end(std::size_t end)
		{
			// if out of range
			if((end <= 1) | (end > get_size()))
			{
				std::cout << "Error on portfolio " << get_name() << ": attempted let_end " << end
						<< " is out of its possible range (" << 2 << " - " << get_size() << ")" << std::endl;
			}
			else
			{
				// if end below current start
				if(end <= m_start)
				{
					std::cout << "Error on portfolio " << get_name() << ": attempted let_end " << end
							<< " is equal to or below start (" << m_start << ")" << std::endl;
				}
				else
				{
					// if all good
					std::cout << "End of portfolio " << get_name() << " set to " << end
							<< " (" << TS::to_string(m_ts.get_date(end)) << ")" << std::endl;
					m_end = end; // let the new end
				}
			}
		}
		
		void hedged_ptf::let_range(std::size_t start, std::size_t end)
		{
			// we want a range to be at least size 2
			if(start == end)
			{
				std::cout << "Error on portfolio " << get_name() << ": attempted let_range " << start 
						<< " - " << end << " has equal values" << std::endl;
			}
			else if(start > end) // avoid crossing range
			{
				std::cout << "Error on portfolio " << get_name() << ": attempted let_range " << start 
						<< " - " << end << " is crossing" << std::endl;
			}
			else
			{
				// to avoid bugs with let_end and let_start (end below start etc.)
				if(start >= m_end)
				{
					let_end(end);
					let_start(start);
				}
				else
				{
					let_start(start);
					let_end(end);
				}
			}
		}
		
		
		// sets range to last n months (for vol computations)
		void hedged_ptf::let_last_range(std::size_t n, bool next)
		{
			// need static cast to transform std::size_t into int to avoid warnings
			let_start(m_ts.shift_months(m_ts.get_size(), static_cast<int>(n), false));
			let_end(m_ts.get_size());
		}
		
		
		
		
// -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_ //

		
		
		
		// P&L computations
		double hedged_ptf::get_pnl(double vol, bool call) const
		{
			// This method computes the pnl of an autofinancing portfolio
			// that delta-hedges daily the option, and invest the rest in the risk free rate
			
			// time to maturity
			double mat = get_maturity();
			
			// portfolio
			double value = price_bs(get_spot(), m_strike, mat, m_rate, vol, call);
			double inv_stock = delta_bs(get_spot(), m_strike, mat, m_rate, vol, call); // delta
			double inv_rate = value - get_spot() * inv_stock; // risk-free rate investment
			
			// loop on the range
			for(std::size_t i = 1; i < get_size_range(); ++i)
			{
				// compute current maturity 
				mat = maturity(m_ts.get_date(m_end), m_ts.get_date(m_start + i));
				
				// change in portfolio value = change in delta + change in risk-free cash
				value += inv_stock * (m_ts[m_start + i] - m_ts[m_start + i - 1])
					   + inv_rate * (std::exp(m_rate * maturity(m_ts.get_date(m_start + i), m_ts.get_date(m_start + i - 1))) - 1.0);
				
				// new delta 
				if(mat != 0)
					inv_stock = delta_bs(m_ts[m_start + i], m_strike, mat, m_rate, vol, call);
				
				// the rest is invested in the risk-free asset
				inv_rate = value - m_ts[m_start + i] * inv_stock;
				
				// print for debugging
				// std::cout << mat << ' ' << m_ts[m_start + i] << ' ' << inv_stock << ' ' << inv_rate << ' ' << value << std::endl;
			}
			
			double payoff = call ? std::max((m_ts[m_end] - m_strike), 0.0) : std::max((m_strike - m_ts[m_end]), 0.0);
			// std::cout << "final payoff: " << payoff << std::endl;
			return value - payoff;
		}
		
		
		double hedged_ptf::get_delta_pnl(double vol, bool call) const
		{
			// This method does not take into account the interest of risk-free position
			// if rates = 0, this yields the same computations as the normal get_pnl method.
			// otherwise, it is not appropriate for computing breakeven volatility
			// as it returns strictly positive pnl under some circumstances
			// (due to ommitting the positive rates)
			
			// time to maturity
			double mat = get_maturity();
			
			// portfolio
			double value = price_bs(get_spot(), m_strike, mat, m_rate, vol, call);
			double inv_stock = delta_bs(get_spot(), m_strike, mat, m_rate, vol, call); // delta
			
			// loop on the range
			for(std::size_t i = 1; i < get_size_range(); ++i)
			{
				// compute current maturity 
				mat = maturity(m_ts.get_date(m_end), m_ts.get_date(m_start + i));
				
				// pnl from delta hedging (no consideration of cash)
				value += inv_stock * (m_ts[m_start + i] - m_ts[m_start + i - 1]);
					   
				// new delta 
				if(mat != 0)
					inv_stock = delta_bs(m_ts[m_start + i], m_strike, mat, m_rate, vol, call);
				
			}
			
			double payoff = call ? std::max((m_ts[m_end] - m_strike), 0.0) : std::max((m_strike - m_ts[m_end]), 0.0);
			return value - payoff;
		}
		
		
		double hedged_ptf::get_robust_pnl(double vol, bool call) const
		{
			// computing the pnl using the gamma weighted average method
			// This method yields similar results to the get_pnl
			// However a significant difference can be observed on strikes where the option ends
			// close to at the money, because of the high gamma effect near maturity
			
			
			// time to maturity
			double mat = get_maturity();
			double dt, ds; // delta time, delta stock
			
			// portfolio
			double pnl = 0;
			double gamma = gamma_bs(get_spot(), m_strike, mat, m_rate, vol, call);
			
			// loop on the range
			for(std::size_t i = 1; i < get_size_range(); ++i)
			{
				// computations
				mat = maturity(m_ts.get_date(m_end), m_ts.get_date(m_start + i)); // for gamma 
				dt = maturity(m_ts.get_date(m_start + i), m_ts.get_date(m_start + i - 1)); // times vol square
				ds = (m_ts[m_start + i] - m_ts[m_start + i - 1]) / m_ts[m_start + i - 1]; // to square
				
				// change in pnl
				// sum of dollar gamma times realized vol squared minus implied vol squared
				// gamma(i) * S(i)^2 * ((dS(i) / S(i))^2 - vol^2 * dt(i, i+1)) with dS(i) = S(i+1) - S(i)
				pnl += gamma * m_ts[m_start + i - 1] * m_ts[m_start + i - 1] * (ds * ds - vol * vol * dt);
				
				// new gamma 
				if(mat != 0)
					gamma = gamma_bs(m_ts[m_start + i], m_strike, mat, m_rate, vol, call);
				
			}
			
			// return negative pnl as we want the function to be generally increasing with vol (for our dichotomy)
			return -pnl * 0.5;
		}
		
		
		
		// implied vol computations
		double hedged_ptf::get_implied_vol(bool robust_pnl, double tol, double precision, double v_low, double v_high) const
		{
			// In this method we assume that the pnl is in general increasing with implied-volatility (using get_pnl).
			// By analyzing pnl for different implied volatility at lower strike, we found that 
			// the pnl is not exactly monotone.
			// Also, for lower strikes the pnl converge to 0 quickly with the decrease of vol.
			
			// We want to create a method that allows to find the implied-vol 
			// that gives a pnl equal to the tolerance (which has to be very low, 1e-13 for example)
			
			
			// optimization depending on the moneyness (hedging using call or put)
			// in theory it should not change the result for the delta method (and it doesn't when rates are equal to zero)
			// but in practice, it does change marginally because of the discounting effect
			// the results are equal for the gamma method, as gamma is the same for puts and calls
			bool call = (m_ts[m_end] - m_strike > 0.0) ? true : false;
			
			// initialization (midpoint)
			double vol = (v_low + v_high) / 2.0;
			
			// the pnl method depends on the boolean parameter robust_pnl
			double pnl = robust_pnl ? get_robust_pnl(vol, call) : get_pnl(vol, call);
			
			// managing the number of iterations
			std::size_t count = 0, max_iter = static_cast<int>(10 / precision); 
			
			
			// while our volatility range is wider than the required precision
			while(std::abs(v_high - v_low) >= precision)
			{
				// managing the number of iterations
				if(count++ >= max_iter)
				{
					std::cout << "Dichotomy for implied vol did not converge in " << count << " iterations" << std::endl;
					return 0;
				}
				
				// if standardized pnl is superior to tolerance
				if((pnl / get_spot()) > tol) // standardize pnl because pnl is proportional to spot
				{
					v_high = vol; // reduce upper bound
				}
				else
				{
					v_low = vol; // otherwise reduce lower bound
				}
				
				// recompute midpoint and pnl
				vol = (v_low + v_high) / 2.0;
				pnl = robust_pnl ? get_robust_pnl(vol, call) : get_pnl(vol, call); // method depending on robust_pnl
			}
			return vol;
				
		}
		
		
		
		// Old function // Should not use
		double hedged_ptf::get_implied_vol_old(double precision, double v_low, double v_high) const
		{
			// optimization depending on the moneyness
			bool call = (m_ts[m_end] - m_strike > 0.0) ? true : false;
			
			// testing if the two bounds have the same sign
			if (get_pnl(v_low,call)*get_pnl(v_high,call)>0)
			{
				std::cout<< "Error in dichotomy method" << std::endl;
			}
			
			// initialization
			double vol = (v_low + v_high) / 2.0;
			double pnl = get_pnl(vol, call);
			std::size_t count = 0;
			
			/* // test low spot
			for (int i=0; i<201; i++)
			{
				vol=v_low+i*(v_high-v_low)/200.;
				std::cout<< "Vol: " << vol<<"  PnL: " << get_pnl2(vol)<<std::endl;
			}  */
			
			
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
		
		
	}

}













