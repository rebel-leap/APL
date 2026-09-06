#pragma once
#include "apl/common.h"

namespace apl {
    class NeuralNetwork {
    public:
        NeuralNetwork(const Vector<size_t>& topology);
        Vector<double> feedForward(const Vector<double>& inputs);
        void train(const Vector<Vector<double>>& inputs, const Vector<Vector<double>>& targets, size_t epochs, double learningRate);
    private:
        Vector<size_t> topology;
        Vector<Vector<Vector<double>>> weights;
        Vector<Vector<double>> biases;
        double sigmoid(double x);
        double sigmoidDerivative(double x);
    };
}
