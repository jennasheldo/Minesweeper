#include "RandomNum.h"
#include <ctime>

std::mt19937 RandomNum::random(time(0));

 int RandomNum::Int(int min, int max) {
	 std::uniform_int_distribution<int> dist(min, max);
	 return dist(random);
}

 float RandomNum::Float(float min, float max) {
	 std::uniform_real_distribution<float> dist(min, max);
	 return dist(random);
}