#pragma once

#include <ctime>
#include <chrono>
#include <random>
#include <iostream>

class Randomizer
{
public:
	Randomizer(float minValue, float maxValue);
	Randomizer(int minValue, int maxValue);

	int GetRandomInt();
	float GetRandomFloat();

private:
	float									m_minVal, 
											m_maxVal;
	std::random_device						m_randDevice;  
	std::mt19937							m_rndGen; 
	std::uniform_int_distribution<int>		m_distributionInt;
	std::uniform_real_distribution<float>	m_distributionFloat;
};

