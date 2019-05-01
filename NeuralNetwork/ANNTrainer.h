#pragma once

#include "FANN/fann.h"

#include <vector>

class ANNWrapper;

class ANNTrainer
{
public:
	ANNTrainer();
	void InputData(const std::vector<fann_type> & inputs, const std::vector<fann_type> & outputs);
	unsigned GetDataCount() const;

	std::vector<std::vector<fann_type>> GetAllInputs() const;
	std::vector<std::vector<fann_type>> GetAllOutputs() const;
	std::vector<fann_type> NormalizeInput(std::vector<fann_type> inputs) const;

	void Clear();

	// addition operators
	ANNTrainer& operator+=(const ANNTrainer& rhs);
	ANNTrainer operator+(const ANNTrainer& rhs) const;

private:
	struct ANNInfo
	{
		std::vector<fann_type> m_inputs;
		std::vector<fann_type> m_outputs;
	};
	std::vector<ANNInfo> m_annInfo;
	std::vector<fann_type> m_inputMax;
	std::vector<fann_type> m_inputMin;
};