#ifndef DATA_STRUCTURE_HPP
#define DATA_STRUCTURE_HPP

// libs of the project

#include <algorithm>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <tuple>


bool operator==(const std::tm& lhs, const std::tm& rhs);


namespace project
{
	// TIMES SERIES
	namespace TS
	{
		struct std::tm to_date(const std::string & strdate);
		
		
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
			
			// import data
			void load_from_csv(std::ifstream& csv_file);
			
			
			// access - general
			std::string get_name() const;
			std::size_t get_size() const;
			
			// access - values
			double operator[](std::size_t line) const;
			double operator[](std::string date) const;
			double operator[](std::tm date) const;
			
			std::size_t get_index(std::string date) const;
			std::size_t get_index(struct std::tm tm) const;
			
			
			time_point get_line(std::size_t line) const;

			
			// printing info
			void print_line(std::size_t line) const;
			void time_series::print() const;
			
			
			
			// void print_info() const;
		
		private:
			
			// data members
			std::string m_name;
			std::vector<std::tm> m_dates;
			std::vector<double> m_values;
			
			
			// check line
			bool is_line(std::size_t line) const;
		};
		
	}
	
	
	
	
	
	
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