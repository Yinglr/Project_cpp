#include "time_series.hpp"
#include "hedged_ptf.hpp"
#include "vol_surface.hpp"
#include "functions.hpp"


	/* -------------------------------- */
	/* ---- MANIPULATING STRUCT TM ---- */
	/* -------------------------------- */

// for comparing std::tm structures (eg. in iterators)
bool operator==(const struct std::tm& lhs, const struct std::tm& rhs)
{
	return std::tie(lhs.tm_year, lhs.tm_mon, lhs.tm_mday) ==
		   std::tie(rhs.tm_year, rhs.tm_mon, rhs.tm_mday);
}

bool operator>(struct std::tm& lhs, struct std::tm& rhs)
{
	return (std::difftime(lhs, rhs) > 0.0);
}

bool operator<(struct std::tm& lhs, struct std::tm& rhs)
{
	return (std::difftime(lhs, rhs) < 0.0);
}

bool operator>=(struct std::tm& lhs, struct std::tm& rhs)
{
	return ((lhs == rhs) | (lhs > rhs));
}

bool operator<=(struct std::tm& lhs, struct std::tm& rhs)
{
	return ((lhs == rhs) | (lhs < rhs));
}


// overloads for std::tm
namespace std
{
	double difftime(struct std::tm& time_end, struct std::tm& time_beg)
	{
		return difftime(std::mktime(&time_end), std::mktime(&time_beg));
	}
	
	double difftime(const std::string& time_end, const std::string& time_beg)
	{
		return difftime(project::TS::to_date(time_end), project::TS::to_date(time_beg));
	}
}



namespace project
{
	
	namespace BS
	{
		
		// returns a maturity in years using difftime overload (ACT/365 basis)
		double maturity(struct std::tm& end, struct std::tm& start)
		{
			double difftime = std::difftime(end, start);
			return TS::difftime_to_years(difftime); // base ACT/365 for simplicity (see TS::difftime_to_years)
		}
		
		
		/* ------------------------------- */
		/* ---- GAUSSIAN DISTRIBUTION ---- */
		/* ------------------------------- */
		
		// normal cumulative distribution
		double normal_cdf(double x)
		{
		   // using existing std error function
		   return 0.5 * std::erfc(-x / std::sqrt(2.0));
		}
		
		// normal probability distribution
		double normal_pdf(double x)
		{
		   // using existing std atan function
		   return std::exp(-x * x * 0.5) / std::sqrt(8.0 * std::atan(1.0));
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
				return normal_cdf(d) - 1; // from call-put parity
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

	
	/* ---------------------------------- */
	/* ---- MANIPULATING TIME SERIES ---- */
	/* ---------------------------------- */
	
	namespace TS
	{
		// string to std::tm function
		struct std::tm to_date(const std::string& strdate)
		{
			struct std::tm tm;
			std::istringstream datestream(strdate);
			datestream >> std::get_time(&tm, "%d/%m/%Y");
			
			// necessary sets to zero so mktime works
			tm.tm_sec = 0;
			tm.tm_min = 0;
			tm.tm_hour = 0;
			return tm;
		}
		
		
		
		// convert a difftime (which is in seconds)
		double difftime_to_days(double difftime)
		{
			return difftime / 3600 / 24;
		}
		
		double difftime_to_years(double difftime)
		{
			return difftime / 3600 / 24 / 365; // basis ACT/365
		}
		
		
		
		// std::tm to string function (for printing struct std::tm)
		std::string to_string(struct std::tm tm)
		{
			std::ostringstream stream;
			stream << std::put_time(&tm, "%d/%m/%Y");
			return stream.str();
		}
	}
	
	
	
	
	/* -------------------------------- */
	/* ---- MANIPULATING CSV FILES ---- */
	/* -------------------------------- */
	
	namespace csv
	{
		// checks if the file is open (used in all functions using csv)
		void is_open(std::ifstream& csv_file)
		{
			if(!csv_file.is_open())
			{
				throw "Error: argument file not open!";
			}
		}
		
		// comes back to the beginning of the csv file (after an interation)
		void reset(std::ifstream& csv_file)
		{
			try
			{
				is_open(csv_file); // tries if the file is open
				
				csv_file.clear();
				csv_file.seekg(0, std::ios::beg);
			}
			catch(const char* msg)
			{
				std::cerr << msg << std::endl;
				std::cout << "Error: file could not be reset!" << std::endl;
			}
		}
		
		// returns the number of lines
		std::size_t count_lines(std::ifstream& csv_file)
		{
			try
			{
				is_open(csv_file); // tries if the file is open
				
				csv_file.unsetf(std::ios_base::skipws);
				std::size_t nb_lines = std::count(std::istream_iterator<char>(csv_file), std::istream_iterator<char>(), '\n');
				csv_file.setf(std::ios_base::skipws);
				
				// reset the file
				try
				{
					reset(csv_file);
				}
				catch(const char* msg)
				{
					std::cerr << msg << std::endl;
				}
				
				return nb_lines;
			}
			catch(const char* msg)
			{
				std::cerr << msg << std::endl;
				return 0;
			}
		}
		
		// prints the whole csv file
		void print_csv(std::ifstream& csv_file)
		{
			try
			{
				is_open(csv_file); // tries if the file is open
				
				std::string text;
				std::size_t i = 0;
				
				std::cout << "Content of the CSV file:" << std::endl;
				while(csv_file.good())
				{
					std::getline(csv_file, text, '\n');
					if(!text.empty())
						std::cout << ++i << " - " << text << std::endl;
				}
				
				// reset the file
				try
				{
					reset(csv_file);
				}
				catch(const char* msg)
				{
					std::cerr << msg << std::endl;
				}
			}
			catch(const char* msg)
			{
				std::cerr << msg << std::endl;
			}
		}
		
		// prints only the requested line from the csv file
		void print_line(std::ifstream& csv_file, std::size_t line)
		{
			try
			{
				is_open(csv_file); // tries if the file is open
				
				std::string text;
				std::size_t i = 0;
				
				while(csv_file.good() && (i++ < line))
					std::getline(csv_file, text, '\n');
				
				if(line==1) // first date has some additionnal characters
					text = text.substr(3);
				
				std::cout << line << " - " << text << std::endl;
				
				// reset the file
				try
				{
					reset(csv_file);
				}
				catch(const char* msg)
				{
					std::cerr << msg << std::endl;
				}
			}
			catch(const char* msg)
			{
				std::cerr << msg << std::endl;
			}
		}
	}

}













