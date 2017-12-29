#ifndef DATA_STRUCTURE_HPP
#define DATA_STRUCTURE_HPP

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

	/* -------------------------------- */
	/* ---- MANIPULATING STRUCT TM ---- */
	/* -------------------------------- */

// for comparing times in iterators
bool operator==(const struct std::tm& lhs, const struct std::tm& rhs);
bool operator>(struct std::tm& lhs, struct std::tm& rhs);
bool operator<(struct std::tm& lhs, struct std::tm& rhs);
bool operator>=(struct std::tm& lhs, struct std::tm& rhs);
bool operator<=(struct std::tm& lhs, struct std::tm& rhs);

namespace std
{
	double difftime(struct std::tm& time_end, struct std::tm& time_beg);
	double difftime(const std::string& time_end, const std::string& time_beg);
}



namespace project
{
	
	/* ---------------------------------- */
	/* ---- MANIPULATING TIME SERIES ---- */
	/* ---------------------------------- */
	
	// TIMES SERIES
	namespace TS
	{
		struct std::tm to_date(const std::string& strdate);
		
		// convert a difftime 
		double difftime_to_days(double difftime);
		double difftime_to_years(double difftime); // base ACT/365
		
		// for printing struct std::tm
		std::string to_string(struct std::tm tm);
		
		
		
		
		
		
		class time_series
		{
		public:
		
			// constructors
			time_series(const std::string& name, std::size_t size);
			time_series(const std::string& name, std::ifstream& csv_file);
			
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
			
			std::size_t get_index(std::string date) const;
			std::size_t get_index(struct std::tm tm) const;
			
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
	
	
	
	
	/* -------------------------------- */
	/* ---- MANIPULATING CSV FILES ---- */
	/* -------------------------------- */
	
	// CSV FILES
	namespace csv
	{
		bool is_open(std::ifstream& csv_file);
		void reset(std::ifstream& csv_file);
		
		
		std::size_t count_lines(std::ifstream& csv_file);
		
		
		void print_csv(std::ifstream& csv_file);
		void print_line(std::ifstream& csv_file, std::size_t line);
	}

}



#endif