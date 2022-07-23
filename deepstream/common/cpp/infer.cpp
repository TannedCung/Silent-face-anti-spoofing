#include <iostream>
#include <chrono>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <memory>
#include <math.h>
#include <iomanip>

#include <opencv2/opencv.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <cuda_runtime.h>

#include "../csrc/engine.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
    if (argc!=3) {
		cerr << "Usage: " << argv[0] << " engine.plan image.jpg" << endl;
		return 1;
	}

    cout << "Loading engine..." << endl;
    auto engine = sample_onnx::Engine(argv[1]);

    int inputSize[2] = {80, 80};
    auto image = imread(argv[2], IMREAD_COLOR);
    cv::resize(image, image, Size(inputSize[1], inputSize[0]));
    cv::Mat pixels;
	image.convertTo(pixels, CV_32FC3);

    int channels = 3;
	vector<float> img;
	vector<float> data (channels * inputSize[0] * inputSize[1]);

    if (pixels.isContinuous())
		img.assign((float*)pixels.datastart, (float*)pixels.dataend);
	else {
		cerr << "Error reading image " << argv[2] << endl;
		return -1;
	}

    vector<float> mean {104.0, 117.0, 123.0};

    for (int c = 0; c < channels; c++) {
		for (int j = 0, hw = inputSize[0] * inputSize[1]; j < hw; j++) {
			// data[c * hw + j] = (img[channels * j + 2 - c] - mean[c]);
			data[c * hw + j] = 0;
		}
	}

    // Create device buffers
    void *data_d, *output_d;
    // auto data = engine.processInput(string(argv[2]));
    cudaMalloc(&data_d, 3 * inputSize[0] * inputSize[1] * sizeof(float));
	cudaMalloc(&output_d, 3 * sizeof(float));

    // Copy image to device
	size_t dataSize = data.size() * sizeof(float);
	cudaMemcpy(data_d, data.data(), dataSize, cudaMemcpyHostToDevice);

    // Run inference n times
	cout << "Running inference..." << endl;
    const int count = 1000;
	auto start = chrono::steady_clock::now();
 	vector<void *> buffers = { data_d, output_d };
	for (int i = 0; i < count; i++) {
		engine.infer(buffers, 1);
		// for (auto &i:output_d) std::cout << i << std::endl;
	}
    auto stop = chrono::steady_clock::now();
	std::cout <<  "Done infer !\n";
	auto timing = chrono::duration_cast<chrono::duration<double>>(stop - start);
	cout << "Took " << timing.count() / count << " seconds per inference." << endl;

    cudaFree(data_d);

	// // Get back the results
	unique_ptr<float[]> output(new float[512]);
	cudaMemcpy(output.get(), output_d, sizeof(float) * 3, cudaMemcpyDeviceToHost);
	for (int i=0; i<3; i++) {
		// cv::circle( image, cv::Point2f(output[i]*inputSize[1], output[i+1]*inputSize[0]), 2, cv::Scalar( 0, 0, 255 ), cv:: FILLED, cv::LINE_8);
		std::cout << "[" << output[i] << "]" << std::endl;
	}
	// cv::imwrite("/workspace/cpp/lm.jpg", image);
	cudaFree(output_d);

    return 0;
    
}