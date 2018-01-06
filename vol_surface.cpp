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
			m_vols.resize(strikes.size() * maturities.size());
		}
		
		
		
		// destructor
		vol_surface::~vol_surface()
		{
			std::cout << "Deletion of vol_suraface object " << get_name() << std::endl;
			// Care: No pointer destroyer!!!
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
		
		// one element of the vol surface
		double vol_surface::get_vol(double strike, double maturity) const
		{
			double vol;
			try
			{
				vol = m_vols[index_maturity(maturity) * m_strikes.size() + index_strike(strike)];
			}
			catch(const char* msg)
			{
				std::cerr << msg << std::endl;
				return 0;
			}
			return vol;
		}
		
		// term structure
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
		
		// skew
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
				std::cout << std::setfill('0') << std::setw(3) << m_strikes[i] << str;
			}
			std::cout << std::endl;
		}
		
		
		void vol_surface::print_maturities(std::string str) const
		{
			for(std::size_t i = 0; i < m_maturities.size(); ++i)
			{
				std::cout << m_maturities[i] << str;
			}
			std::cout << std::endl;
		}
		
		
		
		
		// printing - volatilities
		
		// term structure
		void vol_surface::print_strike(double strike) const 
		{
			std::vector<double> term_structure = get_strike(strike);
			std::cout << "Term structure for strike " << strike << std::endl;
			for(std::size_t i = 0; i < term_structure.size(); ++i)
			{
				std::cout << std::setfill('0') << std::setw(2) << m_maturities[i] << " - ";
				std::cout.setf(std::ios::fixed, std::ios::floatfield);
				std::cout << std::setprecision(4) << term_structure[i] << std::endl;
				std::cout.unsetf(std::ios::floatfield);
			}
		}
		
		
		// skew
		void vol_surface::print_maturity(double maturity) const 
		{
			std::vector<double> skew = get_maturity(maturity);
			std::cout << "Skew for maturity " << maturity << std::endl;
			for(std::size_t i = 0; i < skew.size(); ++i)
			{
				std::cout << std::setfill('0') << std::setw(3) << m_strikes[i] << " - ";
				std::cout.setf(std::ios::fixed, std::ios::floatfield);
				std::cout << std::setprecision(4) << skew[i] << std::endl;
				std::cout.unsetf(std::ios::floatfield);
			}
		}
		
		void vol_surface::print_vol_surface() const
		{
			std::size_t mat = 0;
			std::cout << "Volatility surface" << std::endl << "        ";
			print_strikes("    ");
			for(std::size_t i = 0; i < m_vols.size(); ++i)
			{
				if(i%m_strikes.size() == 0)
				{
					std::cout.unsetf(std::ios::floatfield);
					std::cout << std::endl << std::setfill('0') << std::setw(2) << m_maturities[mat++] << " - ";
					std::cout.setf(std::ios::fixed, std::ios::floatfield);
				}
				std::cout << std::setprecision(4) << m_vols[i] << ' ';
			}
			std::cout.unsetf(std::ios::floatfield);
			std::cout << std::endl << std::endl;
		}
		
		
		
		
		
		
		// modify
		void vol_surface::load_vol_surface()
		{
			for(std::size_t i = 0; i < m_maturities.size(); ++i)
			{
				p_ptf->let_last_range(static_cast<int>(m_maturities[i]));
				for(std::size_t j = 0; j < m_strikes.size(); ++j)
				{
					p_ptf->let_strike(m_strikes[j]);
					m_vols[i * m_strikes.size() + j] = p_ptf->get_implied_vol2();
				}
			}
			std::cout << "vol_surface " << get_name() << " correctly updated" << std::endl;
		}
		
		
		void vol_surface::let_strikes(std::vector<double> strikes)
		{
			m_strikes = strikes;
		}
		
		void vol_surface::let_maturities(std::vector<double> maturities)
		{
			m_maturities = maturities;
		}
		
		void vol_surface::let_ptf(BS::hedged_ptf& ptf)
		{
			std::cout << "On vol_suraface object, target hedged_ptf object changed from " << get_name() << " to " << ptf.get_name() << std::endl;
			p_ptf = &ptf;
		}
		
		
		
		// export the volatility surface in .csv format
		void vol_surface::export_to_csv(std::string path) const
		{
			// set the path and the name of our file
			std::ofstream file(path + std::string("/") + get_name() + std::string("_vol.csv"));
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
			std::cout << "vol_surface " << get_name() << " exported to " << get_name() << "_vol.csv" << std::endl;
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













