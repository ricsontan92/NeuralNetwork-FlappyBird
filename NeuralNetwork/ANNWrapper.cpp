#include "ANNWrapper.h"

#include "ANNTrainer.h"

#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

ANNWrapper::ANNWrapper(const ANNConfig& config) : 
	m_config(config),
	m_annTrainer(std::make_unique<ANNTrainer>()),
	m_currEpoch(0),
	m_mse(0.f)
{
	std::vector<unsigned> layers(m_config.m_numLayers, 0);
	layers.front() = m_config.m_numInputs;
	layers.back() = m_config.m_numOutputs;
	for (size_t i = 1; i < (layers.size() - 1); ++i)
		layers[i] = m_config.m_numNeuronsInHidden;

	m_ann = fann_create_standard_array(m_config.m_numLayers, &layers[0]);

	fann_set_activation_function_hidden(m_ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(m_ann, FANN_SIGMOID_SYMMETRIC);

	fann_set_user_data(m_ann, this);
	fann_set_callback(m_ann, MyANNCallback);
}

ANNWrapper::~ANNWrapper()
{
	fann_destroy(m_ann);
}

std::vector<fann_type> ANNWrapper::Run(std::vector<fann_type> inputs)
{
	std::vector<fann_type> result;
	inputs = m_annTrainer->NormalizeInput(inputs);
	fann_type * output = fann_run(m_ann, &inputs[0]);
	for (int i = 0; i < m_config.m_numOutputs; ++i)
		result.push_back(output[i]);
	return result;
}

void ANNWrapper::Train()
{
	auto allOutputs = m_annTrainer->GetAllOutputs();
	auto allInputs	= m_annTrainer->GetAllInputs();

	PreprocessData(allInputs, allOutputs);

	int failures	= 0, successes = 0;

	std::vector<std::vector<fann_type>> successIns, successOuts;
	std::vector<std::vector<fann_type>> failureIns, failureOuts;

	for (unsigned i = 0; i < allInputs.size(); ++i)
	{
		if (allOutputs[i][0] == -1.f)
		{
			failures++;
			failureIns.push_back(allInputs[i]);
			failureOuts.push_back(allOutputs[i]);
		}
		else
		{
			successIns.push_back(allInputs[i]);
			successOuts.push_back(allOutputs[i]);
			successes++;
		}
	}

	std::cout << "Total Samples(Before Preprocessing): " << m_annTrainer->GetDataCount() << std::endl;
	std::cout << "Total Samples(After Preprocessing): " << allInputs.size() << std::endl;
	std::cout << "Failures: "	<< failures << std::endl;
	std::cout << "Successes: "	<< successes << std::endl;

	TrainingData data;
	data.m_inputs = std::move(allInputs);
	data.m_outputs = std::move(allOutputs);

	TrainFromData(&data);

	m_annTrainer->Clear();
}

void ANNWrapper::SetTrainer(std::unique_ptr<ANNTrainer>& trainerData)
{
	m_annTrainer = std::move(trainerData);
}

void ANNWrapper::SetTrainer(std::unique_ptr<ANNTrainer>&& trainerData)
{
	m_annTrainer = std::move(trainerData);
}

ANNTrainer& ANNWrapper::GetTrainer()
{
	return *m_annTrainer;
}

unsigned ANNWrapper::GetCurrentEpoch() const
{
	return m_currEpoch;
}

unsigned ANNWrapper::GetMaxEpoch() const
{
	return m_config.m_maxEpochs;
}

float ANNWrapper::GetCurrentMSE() const
{
	return m_mse;
}

float ANNWrapper::GetDesiredMSE() const
{
	return m_config.m_maxErr;
}

void ANNWrapper::TrainFromData(TrainingData * trainingData)
{
	if (trainingData == nullptr)
	{
		return;
	}

	assert(trainingData->m_inputs.size() == trainingData->m_outputs.size());
	
	std::vector<fann_type*>	forInput(trainingData->m_inputs.size(), nullptr),
							forOutput(trainingData->m_outputs.size(), nullptr);

	for (unsigned i = 0; i < trainingData->m_inputs.size(); ++i)
	{
		forInput[i]		= &(trainingData->m_inputs[i])[0];
		forOutput[i]	= &(trainingData->m_outputs[i])[0];
	}

	fann_train_data data;
	data.input			= &forInput[0];
	data.output			= &forOutput[0];
	data.num_data		= trainingData->m_outputs.size();
	data.num_input		= m_config.m_numInputs;
	data.num_output		= m_config.m_numOutputs;

	fann_train_on_data(m_ann, &data, m_config.m_maxEpochs, 1, m_config.m_maxErr);
}

int FANN_API ANNWrapper::MyANNCallback(	struct fann *ann, struct fann_train_data *train,
										unsigned int max_epochs,
										unsigned int epochs_between_reports,
										float desired_error, unsigned int epochs)
{
	ANNWrapper* annWrapper = reinterpret_cast<ANNWrapper*>(fann_get_user_data(ann));
	
	bool isRunning = true;

	if (annWrapper->m_epochCallback)
		isRunning = (annWrapper->m_epochCallback)->operator()(epochs);

	float mse = fann_get_MSE(ann);
	bool shouldPrintReport = epochs == 1;
	shouldPrintReport = shouldPrintReport || (epochs % annWrapper->m_config.m_epochsBtwnReports) == 0;
	shouldPrintReport = shouldPrintReport || mse <= desired_error;
	if(shouldPrintReport)
		printf("Epochs     %8d. MSE: %.5f. Desired-MSE: %.5f. Number of fails: %d\n", epochs, mse, desired_error, fann_get_bit_fail(ann));

	annWrapper->m_currEpoch = epochs;
	annWrapper->m_mse = mse;

	return isRunning ? 0 : -1;
}

void ANNWrapper::PreprocessData(std::vector<std::vector<fann_type>>& inputs, std::vector<std::vector<fann_type>>& outputs) const
{
	std::cout << "Preprocessing data..." << std::endl;

	// remove similar data
	std::vector<std::pair<InputVec, OutputVec>> vData(inputs.size(), std::make_pair<InputVec, OutputVec>({}, {}));
	
	for (unsigned i = 0; i < inputs.size(); ++i)
	{
		vData[i].first	= std::move(inputs[i]);
		vData[i].second = std::move(outputs[i]);
	}

	std::sort(vData.begin(), vData.end(), [](const DataPair& l, const DataPair& r) 
	{
		for (unsigned i = 0; i < l.first.size(); ++i)
		{
			if (l.first[i] < r.first[i])
				return true;
			else if (l.first[i] > r.first[i])
				return false;
		}
		return false;
	});

	// preprocessing starts here
	RemoveSimilarData(vData);

	inputs.clear();
	outputs.clear();

	ShuffleData(vData);
	for (auto & data : vData)
	{
		inputs.emplace_back(std::move(data.first));
		outputs.emplace_back(std::move(data.second));
	}
}

void ANNWrapper::RemoveSimilarData(std::vector<std::pair<InputVec, OutputVec>>& sortedData) const
{
	std::cout << "\t* Remove Similar Data..." << std::endl;

	for (auto it = sortedData.begin() + 1; it != sortedData.end();)
	{
		InputVec & prev = (it - 1)->first;
		InputVec & curr = (it - 0)->first;

		float totalDiff = 0.f;

		for (unsigned k = 0; k < curr.size(); ++k)
		{
			float diff = (curr[k] - prev[k]);
			totalDiff += diff * diff;
		}

		if (totalDiff <= 0.001f)
			it = sortedData.erase(it);
		else
			it++;
	}
}

void ANNWrapper::ShuffleData(std::vector<std::pair<InputVec, OutputVec>>& sortedData) const
{
	std::random_shuffle(sortedData.begin(), sortedData.end());
}