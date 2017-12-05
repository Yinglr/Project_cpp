#include <iostream>
#include "vector_utils.hpp"
#include "additional_exercises.hpp"

int main(int argc, char* argv[])
{
    std::vector<double> v(10,1.);
	v[2]=4.5;
	v[4]=5;
	//dauphine::printVector(v);
	//dauphine::Mean(v);
	//dauphine::test1(v);
	std::vector<double> v2={1,2,3,4};
	//dauphine::extend_vector(v2);
	//dauphine::append_vector(v,v2);
	//dauphine::printVector2(v2);
	//dauphine::append_vector(v,v2);
	//dauphine::printVector3(v);
	//dauphine::Mean2(v);
	//std::cout<<dauphine::multiple(v)<<std::endl;
	dauphine::extend_vector2(v2);
	return 0;
}

