#include "apl/nn.h"
#include <random>
#include <cmath>

namespace apl {
    NeuralNetwork::NeuralNetwork(const Vector<size_t>& topology) : topology(topology) {
        std::mt19937 gen(42);
        std::uniform_real_distribution<> dis(-1.0, 1.0);
        for (size_t i = 1; i < topology.size(); i++) {
            Vector<Vector<double>> layerWeights;
            for (size_t j = 0; j < topology[i]; j++) {
                Vector<double> neuronWeights;
                for (size_t k = 0; k < topology[i - 1]; k++) neuronWeights.push_back(dis(gen));
                layerWeights.push_back(neuronWeights);
            }
            weights.push_back(layerWeights);
            Vector<double> layerBiases;
            for (size_t j = 0; j < topology[i]; j++) layerBiases.push_back(dis(gen));
            biases.push_back(layerBiases);
        }
    }

    double NeuralNetwork::sigmoid(double x) { return 1.0 / (1.0 + std::exp(-x)); }
    double NeuralNetwork::sigmoidDerivative(double x) { double s = sigmoid(x); return s * (1.0 - s); }

    Vector<double> NeuralNetwork::feedForward(const Vector<double>& inputs) {
        Vector<double> activations = inputs;
        for (size_t i = 0; i < weights.size(); i++) {
            Vector<double> newActivations;
            for (size_t j = 0; j < weights[i].size(); j++) {
                double sum = biases[i][j];
                for (size_t k = 0; k < weights[i][j].size(); k++) sum += activations[k] * weights[i][j][k];
                newActivations.push_back(sigmoid(sum));
            }
            activations = newActivations;
        }
        return activations;
    }

    void NeuralNetwork::train(const Vector<Vector<double>>& inputs, const Vector<Vector<double>>& targets, size_t epochs, double learningRate) {
        for (size_t epoch = 0; epoch < epochs; epoch++) {
            for (size_t i = 0; i < inputs.size(); i++) {
                Vector<double> output = feedForward(inputs[i]);
            }
        }
    }
}
