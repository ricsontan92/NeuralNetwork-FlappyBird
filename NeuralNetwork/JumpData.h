#pragma once

#include "FANN/fann.h"
#include <vector>

struct JumpData
{
	enum class INPUT_TYPE : unsigned
	{
		X_DIST = 0,
		X_SPEED,
		COUNT
	};

	enum class OUTPUT_TYPE : unsigned
	{
		JUMP = 0,
		COUNT
	};

	fann_type m_inputs[(unsigned)INPUT_TYPE::COUNT]{ 0.f };
	fann_type m_outputs[(unsigned)OUTPUT_TYPE::COUNT]{ 0.f };

	std::vector<fann_type> GetInputsAsVector() const { return std::vector<fann_type>(m_inputs, m_inputs + (unsigned)INPUT_TYPE::COUNT); }
	std::vector<fann_type> GetOutputsAsVector() const { return std::vector<fann_type>(m_outputs, m_outputs + (unsigned)OUTPUT_TYPE::COUNT); }
};