#pragma once

#include "FANN/fann.h"

#include <vector>
#include <memory>

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

	ANNWrapper(const ANNConfig& config);
	~ANNWrapper();

	std::vector<fann_type> Run(std::vector<fann_type> inputs);
	template<unsigned N> std::vector<fann_type> Run(const fann_type (&inputs)[N]);
	void Train();

	void SetTrainer(std::unique_ptr<ANNTrainer>& trainerData);
	void SetTrainer(std::unique_ptr<ANNTrainer>&& trainerData);
	template<typename F, typename C> void SetEpochCallback(F fnc, C* fncClass);
	ANNTrainer& GetTrainer();
	unsigned GetCurrentEpoch() const;
	unsigned GetMaxEpoch() const;
	float GetCurrentMSE() const;
	float GetDesiredMSE() const;

	struct TrainingData
	{
		std::vector<std::vector<fann_type>> m_inputs;
		std::vector<std::vector<fann_type>> m_outputs;
		void AddData()
		{
			m_inputs.push_back({});
			m_outputs.push_back({});
		}
	};
	void TrainFromData(TrainingData * trainingData);

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
	using InputVec	= std::vector<fann_type>;
	using OutputVec = std::vector<fann_type>;
	using DataPair	= std::pair<InputVec, OutputVec>;

	void PreprocessData(std::vector<std::vector<fann_type>>& inputs, std::vector<std::vector<fann_type>>& outputs) const;
	void RemoveSimilarData(std::vector<std::pair<InputVec, OutputVec>>& sortedData) const;
	void ShuffleData(std::vector<std::pair<InputVec, OutputVec>>& sortedData) const;

	friend class ANNTrainer;
	struct fann *				m_ann;
	ANNConfig					m_config;
	std::unique_ptr<ANNTrainer> m_annTrainer;
	std::unique_ptr<Concept>	m_epochCallback;

	float						m_mse;
	unsigned					m_currEpoch;
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