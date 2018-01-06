#ifndef TIME_SERIES_HPP
#define TIME_SERIES_HPP

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
	
	/* ---------------------------------- */
	/* ---- MANIPULATING TIME SERIES ---- */
	/* ---------------------------------- */
	
	// TIMES SERIES
	namespace TS
	{
		
		class time_series
		{
		public:
		
			// constructors
			time_series(const std::string& name, std::size_t size); // without loading the data
			time_series(const std::string& name, std::ifstream& csv_file); // directly from a csv file
			
			// destructor
			~time_series();
			
			// import data
			void load_from_csv(std::ifstream& csv_file);
			
			
			// access - general
			std::string get_name() const;
			std::size_t get_size() const;
			
			struct std::tm date_start() const;
			struct std::tm date_end() const;
			
			
			// access - values
			double operator[](std::size_t line) const;
			double operator[](std::string date) const;
			double operator[](struct std::tm date) const;
			
			// access - index
			std::size_t get_index(std::string date) const;
			std::size_t get_index(struct std::tm tm) const;
			
			// acces - dates
			struct std::tm get_date(std::size_t line) const;
			
			
			// returns the closest value (next value / previous value)
			std::size_t approx_index(std::string date, bool next = true) const;
			std::size_t approx_index(struct std::tm tm, bool next = true) const;
			
			// returns the index n periods before / after
			// ex for getting the last 3M period for computing vol:
			// data.shift_months(data.date_end(), 3, false)
			std::size_t shift_months(std::size_t line, int n, bool after = true, bool next = true) const;
			std::size_t shift_months(std::string date, int n, bool after = true, bool next = true) const;
			std::size_t shift_months(struct std::tm tm, int n, bool after = true, bool next = true) const;
			
			std::size_t shift_days(std::size_t line, int n, bool after = true, bool next = true) const;
			std::size_t shift_days(std::string date, int n, bool after = true, bool next = true) const;
			std::size_t shift_days(struct std::tm tm, int n, bool after = true, bool next = true) const;
			
			
			
			// modify - general
			void let_name(std::string name);
			
			
			
			// printing info
			void print_line(std::size_t line) const;
			void print_data() const;
			void print_info() const;
			
			

		
		private:
			
			// data members
			std::string m_name;
			std::vector<struct std::tm> m_dates;
			std::vector<double> m_values;
			
			
			// check line
			bool is_line(std::size_t line) const;
			
		};
		
	}
	
	
	
	
	

}



#endif