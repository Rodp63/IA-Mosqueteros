#include <algorithm>
#include <iostream>
#include <fstream>
#include <time.h>
// #include <SFML/Graphics.hpp>
#include <map>

enum NumberType {kZero, kOne, kTwo, kThree, kFour, kFive, kSix, kSeven, kEight, kNine};

const float kInf = 1000.0f;
const float kThreshold = 0.5f;
const float kLearningRate = 0.1f;

struct Perceptron {
  std::vector<float> weights;
};

std::multimap<NumberType, std::vector<int>> data;
std::map<NumberType, Perceptron> trained_perceptrons;

void ReadTrainingData(std::string filename="../training_data.txt") {
  std::ifstream data_file (filename);

  NumberType type;
  std::string type_str;

  while (data_file >> type_str) {
    type = (NumberType)stoi(type_str);
    std::vector<int> values(35, 0);

    std::string cur_val;

    for (int i = 0; i < 35; ++i) {
      data_file >> cur_val;
      values[i] = stoi(cur_val);
    }

    data.insert(std::make_pair(type, values));
  }
}

auto CreateTrainingSet(NumberType expected_type) {
  std::vector<std::pair<std::vector<int>, bool>> training_set;

  for (const auto& [key, training_data] : data) {
    bool right_type = (key == expected_type) ? 1 : 0;
    training_set.push_back(std::make_pair(training_data, right_type));
  }

  return training_set;
}

float DotProduct(std::vector<int> values, std::vector<float> weights) {
  float sum = 0.f;

  for (size_t i = 0; i < values.size(); ++i)
    sum += (values[i] * weights[i]);

  return sum;
}

Perceptron Train(std::vector<std::pair<std::vector<int>, bool>>
                 training_set) {
  std::vector<float> weights (training_set.begin()->first.size(), 0);

  while (true) {
    int error_count = 0;

    for (const auto& [input_vector, desired_output] : training_set) {
      auto result = DotProduct(input_vector, weights) > kThreshold;
      int error = desired_output - result;

      if (error) {
        ++error_count;

        for (size_t i = 0; i < input_vector.size(); ++i)
          weights[i] += (kLearningRate * error * input_vector[i]);
      }
    }

    if (error_count == 0) break;
  }

  Perceptron p;
  p.weights = weights;
  return p;
}

void CreateTrainedPerceptrons() {
  ReadTrainingData();

  std::cout << "Training perceptrons... Threshold: " << kThreshold << std::endl;

  for (auto it = data.begin(); it != data.end();
       it = data.upper_bound(it->first))
    trained_perceptrons[it->first] = Train(CreateTrainingSet(it->first));

  std::cout << "Perceptrons trained!" << std::endl;
}

float Recognize(std::vector<int> input_data, Perceptron p) {
  std::vector<float>& weights = p.weights;

  float result = DotProduct(input_data, weights);
  std::cout << "Output: " << result << std::endl;

  return result;
}

NumberType Classify(std::vector<int> input_data) {
  std::vector<std::pair<NumberType, float>> ratings;

  for (const auto& [key, perceptron] : trained_perceptrons) {
    std::cout << "Trying " << key << std::endl;
    ratings.push_back(std::make_pair(key, Recognize(input_data, perceptron)));
  }

  size_t max_idx;
  float max_rating = -kInf;

 for (size_t i = 0; i < ratings.size(); ++i) {
    if (ratings[i].second > max_rating) {
      max_rating = ratings[i].second;
      max_idx = i;
    }
  }

  return ratings[max_idx].first;
}

int main() {
  CreateTrainedPerceptrons();

  std::vector<int> input_data = {
    0, 1, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    1, 1, 1, 1, 1
  };

  NumberType result = Classify(input_data);
  std::cout << "Result: " << result << std::endl;

  // srand(time(NULL));
  // sf::RenderWindow window(sf::VideoMode(520, 450), "OCR Perceptron");
  // window.setFramerateLimit(60);
//
  // while (window.isOpen()) {
  // sf::Event event;
//
  // while (window.pollEvent(event)) {
  // if (event.type == sf::Event::Closed)
  // window.close();
  // }
//
  // ----------- Draw -----------
  // window.clear(sf::Color::Black);
  // window.display();
  // }

  return 0;
}
