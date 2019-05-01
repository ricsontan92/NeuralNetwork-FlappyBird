#include "Randomizer.h"

Randomizer::Randomizer(float minValue, float maxValue) : 
				m_distributionInt((int)minValue, (int)maxValue),
				m_rndGen(m_randDevice() + static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())),
				m_distributionFloat(minValue, maxValue),
				m_minVal(minValue),
				m_maxVal(maxValue)
{
	srand(unsigned(time(NULL)) + reinterpret_cast<unsigned>(this));
}

Randomizer::Randomizer(int minValue, int maxValue) : 
	Randomizer(static_cast<float>(minValue), static_cast<float>(maxValue))
{
}

int Randomizer::GetRandomInt()
{
	return m_distributionInt(m_rndGen);
}

float Randomizer::GetRandomFloat()
{
	switch (rand() % 2)
	{
	case 0:		return m_distributionFloat(m_rndGen);
	default:	return m_minVal + (m_maxVal - m_minVal) * (rand() / static_cast<float>(RAND_MAX));
	}
}