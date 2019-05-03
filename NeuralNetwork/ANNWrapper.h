#pragma once

#include "FANN/fann.h"

#include <vector>
#include <memory>

#include "Randomizer.h"

class ANNTrainer;

class ANNWrapper
{
public:
	struct ANNConfig
	{
		int			m_numInputs;
		int			m_numOutputs;
		int			m_numLayers;
		int			m_numNeuronsInHidden;
		float		m_maxErr;
		unsigned	m_maxEpochs;
		unsigned	m_epochsBtwnReports;
	};

	class Connection
	{
	public:
		unsigned GetFromNeuron() const	{ return m_fromNeuron;	}
		unsigned GetToNeuron() const	{ return m_toNeuron;	}
		float GetWeight() const			{ return m_weight;		}
		void SetWeight(float set)		{ m_weight = set; fann_set_weight(m_ann, m_fromNeuron, m_toNeuron, set); }
	private:
		friend class ANNWrapper;
		struct fann *	m_ann;
		unsigned		m_fromNeuron;
		unsigned		m_toNeuron;
		float			m_weight;
	};

	ANNWrapper(const ANNConfig& config);
	~ANNWrapper();

	void RandomizeWeights();
	void SetWeights(const std::vector<fann_type> & weights);
	std::vector<Connection> GetConnections();
	std::vector<fann_type> GetWeights() const;

	std::vector<fann_type> Run(std::vector<fann_type> inputs);
	template<unsigned N> std::vector<fann_type> Run(const fann_type (&inputs)[N]);

	template<typename F, typename C> void SetEpochCallback(F fnc, C* fncClass);

	unsigned	GetCurrentEpoch() const;
	unsigned	GetMaxEpoch() const;
	float		GetCurrentMSE() const;
	float		GetDesiredMSE() const;

	void		Dump() const;

private:
	struct Concept
	{
		virtual bool operator()(unsigned) = 0;
	};

	template<typename F, typename C>
	struct Model : Concept
	{
		Model(F f, C c) : m_function(f), m_fncClass(c) {}

		F m_function;
		C m_fncClass;
		bool operator()(unsigned currEpoch)
		{
			return (m_fncClass->*m_function)(currEpoch);
		}
	};
	static int FANN_API MyANNCallback(	struct fann *ann, struct fann_train_data *train,
										unsigned int max_epochs,
										unsigned int epochs_between_reports,
										float desired_error, unsigned int epochs);
private:
	struct fann *				m_ann;
	ANNConfig					m_config;
	std::unique_ptr<Concept>	m_epochCallback;
	float						m_mse;
	unsigned					m_currEpoch;
	Randomizer					m_randomizer;
};

template<unsigned N>
std::vector<fann_type> ANNWrapper::Run(const fann_type(&inputs)[N])
{
	return Run(std::vector<fann_type>(inputs, inputs + N));
}

template<typename F, typename C>
void ANNWrapper::SetEpochCallback(F fnc, C* fncClass)
{
	m_epochCallback = std::make_unique<Model<F, C*>>(fnc, fncClass);
}