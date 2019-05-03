#include "ANNWrapper.h"

#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

ANNWrapper::ANNWrapper(const ANNConfig& config) : 
	m_config(config),
	m_currEpoch(0),
	m_mse(0.f),
	m_randomizer(-1.f, 1.f)
{
	std::vector<unsigned> layers(m_config.m_numLayers, 0);
	
	layers.front()	= m_config.m_numInputs;
	layers.back()	= m_config.m_numOutputs;

	for (size_t i = 1; i < (layers.size() - 1); ++i)
	{
		layers[i] = m_config.m_numNeuronsInHidden;
	}

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

void ANNWrapper::RandomizeWeights()
{
	unsigned totalConnections = fann_get_total_connections(m_ann);
	std::vector<fann_type> vWeights(totalConnections);
	for (auto & w : vWeights)
		w = m_randomizer.GetRandomFloat();
	fann_set_weights(m_ann, &vWeights[0]);
}

void ANNWrapper::SetWeights(const std::vector<fann_type> & weights)
{
	assert(fann_get_total_connections(m_ann) == weights.size());
	fann_set_weights(m_ann, &weights[0]);
}

std::vector<fann_type> ANNWrapper::GetWeights() const
{
	unsigned totalConnections = fann_get_total_connections(m_ann);
	std::vector<fann_type> vWeights(totalConnections);
	fann_get_weights(m_ann, &vWeights[0]);
	return vWeights;
}

std::vector<ANNWrapper::Connection> ANNWrapper::GetConnections()
{
	std::vector<fann_connection> connections(fann_get_total_connections(m_ann));
	fann_get_connection_array(m_ann, &connections[0]);

	std::vector<ANNWrapper::Connection> result;
	result.reserve(connections.size());
	for (auto & elem : connections)
	{
		ANNWrapper::Connection con;
		con.m_fromNeuron	= elem.from_neuron;
		con.m_toNeuron		= elem.to_neuron;
		con.m_weight		= elem.weight;
		con.m_ann			= m_ann;
		result.emplace_back(con);
	}
	return result;
}

std::vector<fann_type> ANNWrapper::Run(std::vector<fann_type> inputs)
{
	fann_type * output = fann_run(m_ann, &inputs[0]);
	std::vector<fann_type> result(output, output + m_config.m_numOutputs);
	return result;
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

void ANNWrapper::Dump() const
{
	unsigned totalConnections = fann_get_total_connections(m_ann);
	if (totalConnections > 0)
	{
		std::vector< fann_connection> vConnections(totalConnections);

		fann_get_connection_array(m_ann, &vConnections[0]);

		for (fann_connection const & connection : vConnections)
		{
			std::cout << "from neuron " << connection.from_neuron << " to neuron " << connection.to_neuron << ": " << connection.weight << std::endl;
		}
	}
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
