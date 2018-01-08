#include "time_series.hpp"
#include "hedged_ptf.hpp"
#include "vol_surface.hpp"
#include "functions.hpp"

namespace project
{
	
	/* ---------------------------------- */
	/* ---- MANIPULATING TIME SERIES ---- */
	/* ---------------------------------- */
	
	namespace TS
	{

		// constructors
		// without loading the data
		time_series::time_series(const std::string& name, std::size_t size)
			: m_name(name), m_dates(size), m_values(size)
		{}
		
		// directly from a csv file
		time_series::time_series(const std::string& name, std::ifstream& csv_file)
			: m_name(name)
		{
			try
			{
				csv::is_open(csv_file); // tries if the file is open
				
				// resize vectors
				std::size_t size = csv::count_lines(csv_file);
				m_dates.resize(size); 
				m_values.resize(size);
				
				// loads the data from the file
				load_from_csv(csv_file);
			}
			catch(const char* msg)
			{
				// error messages
				std::cerr << msg << std::endl;
				std::cout << "Error: time_series object " << m_name << " was not initialized" << std::endl;
			}
		}
		
		//destructor
		time_series::~time_series()
		{
			std::cout << "Deletion of time_series object " << get_name() << std::endl;
		}
		
		// import data
		void time_series::load_from_csv(std::ifstream& csv_file)
		{
			try
			{
				csv::is_open(csv_file); // tries if the file is open
				
				std::size_t csv_size = csv::count_lines(csv_file);
				if(get_size() != csv_size)
				{
					std::cout << "Error: size of csv file (" << csv_size
						<< ") do not match size of time_series object (" << get_size() << ")" << std::endl;
				}
				else
				{
					std::string date, value; // temporary storages
					std::size_t i = 0;
					
					while(csv_file.good())
					{
						std::getline(csv_file, date, ';'); // get the date
						std::getline(csv_file, value, '\n'); // get the value
						if(!date.empty())
						{
							// storing the date
							if(i == 0) // first date has some additionnal characters (pb of .csv file)
								date = date.substr(3);
							
							m_dates[i] = to_date(date);
							
							// storing the value
							m_values[i] = std::atof(value.c_str()); // convert string to double
							i++;
						}
					}
					
					// reset the file
					try
					{
						csv::reset(csv_file);
					}
					catch(const char* msg)
					{
						std::cerr << msg << std::endl;
					}
				
					std::cout << "Data successfully loaded into time_series object " << m_name << std::endl;
				}
			}
			catch(const char* msg)
			{
				std::cerr << msg << std::endl;
				std::cout << "Error: data not loaded in time_series object " << m_name << std::endl;
			}
		}
		
		// access - general
		std::string time_series::get_name() const
		{
			return m_name;
		}
		
		std::size_t time_series::get_size() const
		{
			return m_dates.size();
		}
		
		struct std::tm time_series::date_start() const
		{
			return m_dates[0]; // returns the first date
		}
		
		struct std::tm time_series::date_end() const
		{
			return m_dates[get_size()-1]; // returns the last date
		}
		
		
		// access - values
		
		// by index
		double time_series::operator[](std::size_t line) const
		{
			if(is_line(line))
			{
				return m_values[line-1];
			}
			else
			{
				return 0;
			}
		}
		
		// by date as a string
		double time_series::operator[](std::string date) const
		{
			std::size_t line(get_index(date));
			if(is_line(line))
			{
				return m_values[line-1];
			}
			else
			{
				return 0;
			}
		}
		
		// by date as a std::tm
		double time_series::operator[](struct std::tm date) const
		{
			std::size_t line(get_index(date));
			if(is_line(line))
			{
				return m_values[line-1];
			}
			else
			{
				return 0;
			}
		}
		
		
		// access - index
		std::size_t time_series::get_index(std::string date) const
		{
			return get_index(to_date(date));
		}
		
		std::size_t time_series::get_index(struct std::tm tm) const
		{
			// to use the following find algorithm, we had to implement boolean operators for std::tm
			auto pos = std::find(m_dates.cbegin(), m_dates.cend(), tm);
			std::size_t index = std::distance(m_dates.cbegin(), pos) + 1;
			if(is_line(index)) // if not means we didn't find
			{
				return index;
			}
			else
			{
				std::cout << "Error: date not found" << std::endl;
				return 0;
			}
		} 
		
		
		// acces - dates
		struct std::tm time_series::get_date(std::size_t line) const
		{
			if(is_line(line))
			{
				return m_dates[line-1];
			}
			else
			{
				// if the requested line is out of bounds
				std::cout << "Bad std::tm return" << std::endl;
				struct std::tm tm = {0,0,0,0,0,0,0,0,0};
				return tm;
			}
		}
		
		
		// returns the closest value (next value / previous value)
		std::size_t time_series::approx_index(std::string date, bool next) const
		{
			return approx_index(to_date(date), next);
		}		
		
		std::size_t time_series::approx_index(struct std::tm tm, bool next) const
		{
			// non-converging cases
			if( ((next == true) & (tm > date_end())) | ((next == false) & (tm < date_start())) )
			{
				std::cout << "Error: call out of bounds of time_series object " << m_name << std::endl;
				return 0;
			}
			
			// extreme cases
			if(tm > date_end())
				return get_size();
			if(tm < date_start())
				return 1;
			
			// general code
			int incr = next ? 1 : -1; // increment positive if we want the next closest value
			while(get_index(tm) == 0)
				tm.tm_mday += incr;
			return get_index(tm);
		}
		
		
		
		// returns the index n months before / after
		std::size_t time_series::shift_months(std::size_t line, int n, bool after, bool next) const
		{
			return shift_months(m_dates[line-1], n, after, next);
		}
		
		std::size_t time_series::shift_months(std::string date, int n, bool after, bool next) const
		{
			return shift_months(to_date(date), n, after, next);
		}
		
		std::size_t time_series::shift_months(struct std::tm tm, int n, bool after, bool next) const
		{
			// if we want the date after then we will do +n otherwise -n
			int incr = after ? 1 : -1;
			tm.tm_mon += n * incr;
			return approx_index(tm, next);
		}
		
		// returns the index n days before / after
		std::size_t time_series::shift_days(std::size_t line, int n, bool after, bool next) const
		{
			return shift_days(m_dates[line-1], n, after, next);
		}
		
		std::size_t time_series::shift_days(std::string date, int n, bool after, bool next) const
		{
			return shift_days(to_date(date), n, after, next);
		}
		
		std::size_t time_series::shift_days(struct std::tm tm, int n, bool after, bool next) const
		{
			// if we want the date after then we will do +n otherwise -n
			int incr = after ? 1 : -1;
			tm.tm_mday += n * incr; // same principle as for months but with days
			return approx_index(tm, next);
		}
		
		
		
		// modify - general
		void time_series::let_name(std::string name)
		{
			// let_name is used by other classes: hedged_ptf and vol_surface
			std::cout << "time_series object " << m_name << " renamed " << name << std::endl;
			m_name = name; 
		}
		
		
		
		
		
		
		
		
		// printing info
		void time_series::print_line(std::size_t line) const
		{
			if(is_line(line))
			{
				std::cout << line << " - " << to_string(m_dates[line-1])
						<< " - " << m_values[line-1] << std::endl;
			}
			// error message is printed through is_line if line out of bounds
		}
		
		void time_series::print_data() const
		{
			for(std::size_t i = 1; i <= get_size(); ++i)
				print_line(i);
		}
		
		
		// general info on the class
		void time_series::print_info() const
		{
			std::cout << std::endl;
			std::cout << "----------------------------------" << std::endl;
			std::cout << "General info on time_series object " << m_name << std::endl;
			std::cout << "----------------------------------" << std::endl;
			std::cout << "Number of elements: " << get_size() << std::endl;
			// min max dates
			std::cout << "Date range: " << to_string(date_start()) << " - "
						<< to_string(date_end()) << std::endl;
			// min max values
			std::cout << "Values range: " << *std::min_element(m_values.cbegin(), m_values.cend()) << " - "
						<< *std::max_element(m_values.cbegin(), m_values.cend()) << std::endl;
			// average value
			std::cout << "Values average: " << std::accumulate(m_values.cbegin(), m_values.cend(), 0.0) / get_size() << std::endl;
			std::cout << "----------------------------------" << std::endl;
			std::cout << std::endl;
		}
		
		

		
		
		
		
		
		
		// check line (kind of exception management)
		// could have been done with throwing exceptions...
		bool time_series::is_line(std::size_t line) const
		{
			if((line > get_size()) || (line <= 0))
			{
				std::cout << "Error: call out of bounds of time_series object " << m_name << std::endl;
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	
	
	
	
	

}













