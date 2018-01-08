#include "time_series.hpp"
#include "hedged_ptf.hpp"
#include "vol_surface.hpp"
#include "functions.hpp"

namespace project
{
	
	namespace VS
	{
		
		/* ---------------------------------- */
		/* ---- VOLATILITY SURFACE CLASS ---- */
		/* ---------------------------------- */
		
		// constructors
		vol_surface::vol_surface(BS::hedged_ptf& ptf, std::vector<double> maturities, std::vector<double> strikes)
			: p_ptf(&ptf), m_strikes(strikes), m_maturities(maturities)
		{
			m_robust_pnl = false; // by default, we want the delta P&L
			m_vols.resize(strikes.size() * maturities.size());
		}
		
		
		
		// destructor
		vol_surface::~vol_surface()
		{
			std::cout << "Deletion of vol_suraface object " << get_name() << std::endl;
			// Care: No pointer destroyer!!! It would create a seg fault as the hedged_ptf is independent
		}
		
		
		
		
		// access - data members
		std::string vol_surface::get_name() const
		{
			return p_ptf->get_name();
		}
		
		const std::vector<double>& vol_surface::get_strikes() const
		{
			return m_strikes;
		}
		
		const std::vector<double>& vol_surface::get_maturities() const
		{
			return m_maturities;
		}
		
		
		
		// access - volatilities
		
		// returns one element of the vol surface
		double vol_surface::get_vol(double strike, double maturity) const
		{
			double vol;
			try
			{
				// first dimension: strikes // second dimension: maturities
				vol = m_vols[index_maturity(maturity) * m_strikes.size() + index_strike(strike)];
			}
			catch(const char* msg)
			{
				std::cerr << msg << std::endl;
				return 0;
			}
			return vol;
		}
		
		// term structure (for a given strike)
		std::vector<double> vol_surface::get_strike(double strike) const 
		{
			std::vector<double> term_structure(m_maturities.size()); // has to be declared outside
			try
			{
				// fill the term_structure vector looping through maturities
				for(std::size_t i = 0; i < m_maturities.size(); ++i)
				{
					term_structure[i] = m_vols[i * m_strikes.size() + index_strike(strike)];
				}
			}
			catch(const char* msg)
			{
				std::cerr << msg << std::endl;
				return std::vector<double> {0};
			}
			return term_structure;
		}
		
		// skew (for a given maturity)
		std::vector<double> vol_surface::get_maturity(double maturity) const 
		{
			std::vector<double> skew(m_strikes.size()); // has to be declared outside
			try
			{
				// fill the skew vector looping through strikes
				for(std::size_t i = 0; i < m_strikes.size(); ++i)
				{
					skew[i] = m_vols[index_maturity(maturity) * m_strikes.size() + i];
				}
			}
			catch(const char* msg)
			{
				std::cerr << msg << std::endl;
				return std::vector<double> {0};
			}
			return skew;
		}
		
		
		
		
		
		// printing - general
		void vol_surface::print_strikes(std::string str) const
		{
			for(std::size_t i = 0; i < m_strikes.size(); ++i)
			{
				// formatting: padding strikes with leading zeros
				// the optional argument str is for spacing
				std::cout << std::setfill('0') << std::setw(3) << m_strikes[i] << str;
			}
			std::cout << std::endl;
		}
		
		
		void vol_surface::print_maturities(std::string str) const
		{
			for(std::size_t i = 0; i < m_maturities.size(); ++i)
			{
				// the optional argument str is for spacing
				std::cout << m_maturities[i] << str;
			}
			std::cout << std::endl;
		}
		
		
		
		
		// printing - volatilities
		
		// term structure (for a given strike)
		void vol_surface::print_strike(double strike) const 
		{
			std::vector<double> term_structure = get_strike(strike);
			std::cout << "Term structure for strike " << strike << std::endl;
			for(std::size_t i = 0; i < term_structure.size(); ++i)
			{
				// formatting: padding strikes with a leading zero if needed
				std::cout << std::setfill('0') << std::setw(2) << m_maturities[i] << " - ";
				
				// we want a standard printing
				std::cout.setf(std::ios::fixed, std::ios::floatfield);
				std::cout << std::setprecision(4) << term_structure[i] << std::endl;
				std::cout.unsetf(std::ios::floatfield);
			}
		}
		
		
		// skew (for a given maturity)
		void vol_surface::print_maturity(double maturity) const 
		{
			std::vector<double> skew = get_maturity(maturity);
			std::cout << "Skew for maturity " << maturity << std::endl;
			for(std::size_t i = 0; i < skew.size(); ++i)
			{
				// formatting: padding strikes with a leading zero if needed
				std::cout << std::setfill('0') << std::setw(3) << m_strikes[i] << " - ";
				
				// we want a standard printing
				std::cout.setf(std::ios::fixed, std::ios::floatfield);
				std::cout << std::setprecision(4) << skew[i] << std::endl;
				std::cout.unsetf(std::ios::floatfield);
			}
		}
		
		
		// printing the whole surface as a table (inverted but whatever)
		void vol_surface::print_vol_surface() const
		{
			std::size_t mat = 0;
			// initial formatting
			std::string method = m_robust_pnl ? "(Black-Scholes Robustness formula)" : "(Delta Hedging Portfolio)";
			std::cout << "Volatility surface " << method << std::endl << "        ";
			print_strikes("    ");
			// loop on all the surface
			for(std::size_t i = 0; i < m_vols.size(); ++i)
			{
				// when we've done all the strikes, we go to the next line (new maturity)
				if(i%m_strikes.size() == 0)
				{
					// formatting: padding strikes with a leading zero if needed
					std::cout.unsetf(std::ios::floatfield);
					std::cout << std::endl << std::setfill('0') << std::setw(2) << m_maturities[mat++] << " - ";
					std::cout.setf(std::ios::fixed, std::ios::floatfield);
				}
				// standardized volatility formatting
				std::cout << std::setprecision(4) << m_vols[i] << ' ';
			}
			std::cout.unsetf(std::ios::floatfield);
			std::cout << std::endl << std::endl;
		}
		
		
		// current pnl computation method
		void vol_surface::print_current_method() const
		{
			std::string method = m_robust_pnl ? "Black-Scholes Robustness formula" : "Delta Hedging Portfolio";
			std::cout << "Current PnL computation method: " << method << std::endl;
		}
		
		
		
		
		// modify
		
		// load the volatility surface using ptf.get_implied_vol() method.
		void vol_surface::load_vol_surface(bool robust_pnl)
		{
			m_robust_pnl = robust_pnl;
			// outside loop on maturities
			for(std::size_t i = 0; i < m_maturities.size(); ++i)
			{
				// set maturity (important to do it before setting the strike as it is in %)
				p_ptf->let_last_range(static_cast<int>(m_maturities[i]));
				// inside loop on strikes
				for(std::size_t j = 0; j < m_strikes.size(); ++j)
				{
					// set strike
					p_ptf->let_strike(m_strikes[j]);
					// compute breakeven volatility
					m_vols[i * m_strikes.size() + j] = p_ptf->get_implied_vol(m_robust_pnl); 
				}
			}
			// depending on the method for PnL computation
			std::string method = m_robust_pnl ? " (using Black-Scholes Robustness formula)" : "";
			std::cout << "vol_surface " << get_name() << " correctly updated" << method << std::endl;
		}
		
		
		void vol_surface::let_strikes(std::vector<double> strikes)
		{
			m_strikes = strikes;
			print_strikes();
			std::cout << "New strikes correctly set: " << std::endl;
			// erase the old implied volatilities and resize the vector
			m_vols.clear();
			m_vols.resize(m_strikes.size() * m_maturities.size());
		}
		
		void vol_surface::let_maturities(std::vector<double> maturities)
		{
			m_maturities = maturities;
			std::cout << "New maturities correctly set: " << std::endl;
			print_maturities();
			// erase the old implied volatilities and resize the vector
			m_vols.clear();
			m_vols.resize(m_strikes.size() * m_maturities.size());
		}
		
		// changing the reference portfolio
		void vol_surface::let_ptf(BS::hedged_ptf& ptf)
		{
			std::cout << "On vol_suraface object, target hedged_ptf object changed from " << get_name() << " to " << ptf.get_name() << std::endl;
			p_ptf = &ptf;
		}
		
		
		
		// export the volatility surface in .csv format
		void vol_surface::export_to_csv(std::string path) const
		{
			// name of the file depends on the method used for PnL computation
			std::string method = m_robust_pnl ? "_robust" : "";
			
			// set the path and the name of our file
			std::ofstream file(path + get_name() + method + std::string("_vol.csv"));
			std::size_t mat = 0;
			
			// export first line (strikes)
			file << "Maturities\\Strikes;";
			for(std::size_t i = 0; i < m_strikes.size(); ++i)
			{
				file << m_strikes[i] << ';';
			}
			
			// export rest of the file
			for(std::size_t i = 0; i < m_vols.size(); ++i)
			{
				if(i%m_strikes.size() == 0)
				{
					file << '\n' << m_maturities[mat++] << ';';
				}
				file << m_vols[i] << ';';
			}
			// final message
			std::cout << "vol_surface " << get_name() << " exported to " << get_name() << method << "_vol.csv" << std::endl;
			file.close();
		}
		
		
		
		
		
		
		
		
		// private methods for getting indices
		std::size_t vol_surface::index_strike(double strike) const
		{
			auto pos = std::find(m_strikes.cbegin(), m_strikes.cend(), strike);
			if(pos == m_strikes.cend())
			{
				throw "Strike not found!";
			}
			else
			{
				return std::distance(m_strikes.cbegin(), pos);
			}
		}
		
		std::size_t vol_surface::index_maturity(double maturity) const
		{
			auto pos = std::find(m_maturities.cbegin(), m_maturities.cend(), maturity);
			if(pos == m_maturities.cend())
			{
				throw "Maturity not found!";
			}
			else
			{
				return std::distance(m_maturities.cbegin(), pos);
			}
		}
		
		
	}

}













