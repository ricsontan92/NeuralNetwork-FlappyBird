#include "ANNTrainer.h"

ANNTrainer::ANNTrainer()
{

}

void ANNTrainer::InputData(const std::vector<fann_type> & inputs, const std::vector<fann_type> & outputs)
{
	// insert if data is different from previous
	ANNInfo info;
	info.m_inputs = inputs;
	info.m_outputs = outputs;

	if (m_inputMax.empty()) 
		m_inputMax = inputs;
	if (m_inputMin.empty())
		m_inputMin = inputs;

	for (unsigned i = 0; i < inputs.size(); ++i)
	{
		m_inputMax[i] = max(m_inputMax[i], inputs[i]);
		m_inputMin[i] = min(m_inputMin[i], inputs[i]);
	}

	m_annInfo.emplace_back(std::move(info));
}

unsigned ANNTrainer::GetDataCount() const
{
	return static_cast<unsigned>(m_annInfo.size());
}

std::vector<std::vector<fann_type>> ANNTrainer::GetAllInputs() const
{
	std::vector<std::vector<fann_type>> result;
	for (const ANNInfo & info : m_annInfo)
	{
		result.push_back(NormalizeInput(info.m_inputs));
	}
	return result;
}

std::vector<std::vector<fann_type>> ANNTrainer::GetAllOutputs() const
{
	std::vector<std::vector<fann_type>> result;
	for (const ANNInfo & info : m_annInfo)
	{
		result.push_back({});
		for (fann_type const & output : info.m_outputs)
		{
			result.back().push_back(output);
		}
	}
	return result;
}

std::vector<fann_type> ANNTrainer::NormalizeInput(std::vector<fann_type> inputs) const
{
	for (unsigned i = 0; i < inputs.size(); ++i)
	{
		float range = m_inputMax[i] - m_inputMin[i];
		if (range <= FLT_EPSILON)
		{
			inputs[i] = 1.f;
		}
		else
		{
			inputs[i] = 2.f * (inputs[i] - m_inputMin[i]) / range - 1.f;
		}
	}
	return inputs;
}

void ANNTrainer::Clear()
{
	m_annInfo.clear();
}

ANNTrainer& ANNTrainer::operator+=(const ANNTrainer& rhs)
{
	if (GetDataCount() == 0)
	{
		m_inputMax	 = rhs.m_inputMax;
		m_inputMin	= rhs.m_inputMin;
		m_annInfo	= rhs.m_annInfo;
	}
	else
	{
		m_annInfo.insert(m_annInfo.end(), rhs.m_annInfo.begin(), rhs.m_annInfo.end());

		for (unsigned i = 0; i < rhs.m_inputMax.size(); ++i)
		{
			m_inputMax[i] = max(m_inputMax[i], rhs.m_inputMax[i]);
			m_inputMin[i] = min(m_inputMin[i], rhs.m_inputMin[i]);
		}
	}
	return *this;
}

ANNTrainer ANNTrainer::operator+(const ANNTrainer& rhs) const
{
	ANNTrainer copy = *this;
	copy += rhs;
	return copy;
}