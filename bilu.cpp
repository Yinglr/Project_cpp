#include "bilu.hpp"
#include "functions.hpp"

	/* -------------------------------- */
	/* ---- MANIPULATING STRUCT TM ---- */
	/* -------------------------------- */

// for comparing times in iterators
bool operator==(const struct std::tm& lhs, const struct std::tm& rhs)
{
	return std::tie(lhs.tm_year, lhs.tm_mon, lhs.tm_mday) ==
		   std::tie(rhs.tm_year, rhs.tm_mon, rhs.tm_mday);
}

bool operator>(struct std::tm& lhs, struct std::tm& rhs)
{
	return (std::difftime(lhs, rhs) > 0);
}

bool operator<(struct std::tm& lhs, struct std::tm& rhs)
{
	return (std::difftime(lhs, rhs) < 0);
}

bool operator>=(struct std::tm& lhs, struct std::tm& rhs)
{
	return ((lhs == rhs) | (lhs > rhs));
}

bool operator<=(struct std::tm& lhs, struct std::tm& rhs)
{
	return ((lhs == rhs) | (lhs < rhs));
}


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
	
	/* ---------------------------------- */
	/* ---- MANIPULATING TIME SERIES ---- */
	/* ---------------------------------- */
	
	namespace TS
	{
		// string to date function
		struct std::tm to_date(const std::string& strdate)
		{
			struct std::tm dt;
			std::istringstream datestream(strdate);
			datestream >> std::get_time(&dt, "%d/%m/%Y");
			// necessary sets to zero so mktime works
			dt.tm_sec = 0;
			dt.tm_min = 0;
			dt.tm_hour = 0;
			return dt;
		}
		
		

		
		// constructors
		time_series::time_series(const std::string& name, std::size_t size)
			: m_name(name), m_dates(size), m_values(size)
		{}
		
		time_series::time_series(const std::string& name, std::ifstream& csv_file)
			: m_name(name)
		{
			std::size_t size = csv::count_lines(csv_file);
			m_dates.resize(size); 
			m_values.resize(size);
			load_from_csv(csv_file);
		}
		
		//destructor
		time_series::~time_series()
		{
			std::cout << "Deletion of time_series object " << get_name() << std::endl;
		}
		
		// import data
		void time_series::load_from_csv(std::ifstream& csv_file)
		{
			if(csv::is_open(csv_file))
			{
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
						std::getline(csv_file, date, ';');
						std::getline(csv_file, value, '\n');
						if(!date.empty())
						{
							// storing the date
							if(i==0) // first date has some additionnal characters
								date = date.substr(3);
							
							m_dates[i] = to_date(date);
							// storing the value
							m_values[i] = std::atof(value.c_str());
							i++;
						}
					}
					csv::reset(csv_file);
					std::cout << "Data successfully loaded into time_series object " << m_name << std::endl;
				}
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
			return m_dates[0];
		}
		
		struct std::tm time_series::date_end() const
		{
			return m_dates[get_size()-1];
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
		
		// by date as a tm
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
		
		
		
		std::size_t time_series::get_index(std::string date) const
		{
			return get_index(to_date(date));
		}
		
		std::size_t time_series::get_index(struct std::tm tm) const
		{
			auto pos = std::find(m_dates.cbegin(), m_dates.cend(), tm);
			std::size_t index = std::distance(m_dates.cbegin(), pos) + 1;
			if(is_line(index))
			{
				return index;
			}
			else
			{
				std::cout << "Error: date not found" << std::endl;
				return 0;
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
			int incr = next ? 1 : -1;
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
			int incr = after ? 1 : -1;
			tm.tm_mday += n * incr;
			return approx_index(tm, next);
		}
		
		
		// useless atm
		time_point time_series::get_line(std::size_t line) const
		{
			if(is_line(line))
			{
				time_point tp = {line, m_dates[line-1], m_values[line-1]};
				return tp;
			}
			else
			{
				time_point tp;
				std::istringstream datestream("01/01/2000");
				datestream >> std::get_time(&tp.date, "%d/%m/%Y");
				return tp;
			}
		}
		
		
		// printing info
		void time_series::print_line(std::size_t line) const
		{
			if(is_line(line))
			{
				std::cout << line << " - " << std::put_time(&m_dates[line-1], "%d/%m/%Y")
						<< " - " << m_values[line-1] << std::endl;
			}
		}
		
		void time_series::print_data() const
		{
			for(std::size_t i=1;i<=get_size();++i)
				print_line(i);
		}
		
		void time_series::print_info() const
		{
			std::cout << std::endl;
			std::cout << "----------------------------------" << std::endl;
			std::cout << "General info on time_series object " << m_name << std::endl;
			std::cout << "----------------------------------" << std::endl;
			std::cout << "Number of elements: " << get_size() << std::endl;
			std::cout << "Date range: " << std::put_time(&date_start(), "%d/%m/%Y") << " - "
						<< std::put_time(&date_end(), "%d/%m/%Y") << std::endl;
			std::cout << "Values range: " << *std::min_element(m_values.cbegin(), m_values.cend()) << " - "
						<< *std::max_element(m_values.cbegin(), m_values.cend()) << std::endl;
			std::cout << "Values average: " << std::accumulate(m_values.cbegin(), m_values.cend(), 0.0) / get_size() << std::endl;
			std::cout << "----------------------------------" << std::endl;
			std::cout << std::endl;
		}
		
		
		// modify - general
		void time_series::let_name(std::string name)
		{
			std::cout << "time_series object " << m_name << " renamed " << name << std::endl;
			m_name = name;
		}
		
		
		
		
		
		
		
		// check line
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
	
	
	
	
	/* -------------------------------- */
	/* ---- MANIPULATING CSV FILES ---- */
	/* -------------------------------- */
	
	namespace csv
	{
		// checks if the file is open (used in all functions using csv)
		bool is_open(std::ifstream& csv_file)
		{
			if(!csv_file.is_open())
			{
				std::cout << "Error: argument file not open" << std::endl;
				return false;
			}
			else
			{
				return true;
			}
		}
		
		// comes back to the beginning of the csv file (after an interation)
		void reset(std::ifstream& csv_file)
		{
			if(is_open(csv_file))
			{
				csv_file.clear();
				csv_file.seekg(0, std::ios::beg);
			}
		}
		
		// returns the number of lines
		std::size_t count_lines(std::ifstream& csv_file)
		{
			if(is_open(csv_file))
			{
				csv_file.unsetf(std::ios_base::skipws);
				std::size_t nb_lines = std::count(std::istream_iterator<char>(csv_file), std::istream_iterator<char>(), '\n');
				csv_file.setf(std::ios_base::skipws);
				reset(csv_file);
				return nb_lines;
			}
			else
			{
				return 0;
			}
		}
		
		// prints the whole csv file
		void print_csv(std::ifstream& csv_file)
		{
			if(is_open(csv_file))
			{
				std::string text;
				std::size_t i = 0;
				
				std::cout << "Content of the CSV file:" << std::endl;
				while(csv_file.good())
				{
					std::getline(csv_file, text, '\n');
					if(!text.empty())
						std::cout << ++i << " - " << text << std::endl;
				}
				reset(csv_file);
			}
		}
		
		// prints only the requested line from the csv file
		void print_line(std::ifstream& csv_file, std::size_t line)
		{
			if(is_open(csv_file))
			{
				std::string text;
				std::size_t i = 0;
				while(csv_file.good() && (i++ < line))
					std::getline(csv_file, text, '\n');
				
				if(line==1) // first date has some additionnal characters
					text = text.substr(3);
				
				std::cout << line << " - " << text << std::endl;
				reset(csv_file);
			}
		}
	}

}













