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

// for comparing times in iterators
bool operator==(const std::tm& lhs, const std::tm& rhs);


namespace project
{
	
	/* ---------------------------------- */
	/* ---- MANIPULATING TIME SERIES ---- */
	/* ---------------------------------- */
	
	// TIMES SERIES
	namespace TS
	{
		struct std::tm to_date(const std::string& strdate);
		
		
		struct time_point
		{
			size_t index=0;
			struct std::tm date;
			double value=0;
		};
		
		class time_series
		{
		public:
		
			// constructors
			time_series(std::string name, std::size_t size);
			time_series(std::string name, std::ifstream& csv_file);
			
			// destructor
			~time_series();
			
			// import data
			void load_from_csv(std::ifstream& csv_file);
			
			
			// access - general
			std::string get_name() const;
			std::size_t get_size() const;
			
			// access - values
			double operator[](std::size_t line) const;
			double operator[](std::string date) const;
			double operator[](struct std::tm date) const;
			
			std::size_t get_index(std::string date) const;
			std::size_t get_index(struct std::tm tm) const;
			
			// useless
			time_point get_line(std::size_t line) const;

			
			// printing info
			void print_line(std::size_t line) const;
			void print_data() const;
			void print_info() const;
			
			
			// modify - general
			void let_name(std::string name);
		
		
		private:
			
			// data members
			std::string m_name;
			std::vector<std::tm> m_dates;
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