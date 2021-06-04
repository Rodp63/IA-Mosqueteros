#include <algorithm>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

enum NumberType {kZero, kOne, kTwo, kThree, kFour, kFive, kSix, kSeven, kEight, kNine};

const float kInf = 1000.0f;
const float kThreshold = 0.5f;
const float kLearningRate = 0.1f;

const int kRows = 8;
const int kCols = 6;
const int kTotal = kRows * kCols;

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
    std::vector<int> values(kTotal, 0);

    std::string cur_val;

    for (int i = 0; i < kTotal; ++i) {
      data_file >> cur_val;
      values[i] = stoi(cur_val);
    }

    data.insert(std::make_pair(type, values));
  }
}

auto CreateTrainingSet(NumberType expected_type) {
  std::vector<std::pair<std::vector<int>, bool>> training_set;

  for (const auto& [key, training_data] : data) {
    bool right_type = (key == expected_type);
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

Perceptron Train(std::vector<std::pair<std::vector<int>, bool>> training_set) {
  std::vector<float> weights (training_set.begin()->first.size(), 0);

  while (true) {
    int error_count = 0;

    for (const auto& [input_vector, desired_output] : training_set) {
      float dp = DotProduct(input_vector, weights);
      bool result = dp > kThreshold;
      int error = desired_output - result;

      if (error) {
        ++error_count;

        // std::cout << "Updated weights:\n";
        for (size_t i = 0; i < input_vector.size(); ++i) {
          weights[i] += (kLearningRate * error * input_vector[i]);
          // std::cout << weights[i] << " ";
        }
        // std::cout << std::endl;
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
       it = data.upper_bound(it->first)) {
    std::cout << "Training neuron " << it->first << std::endl;
    trained_perceptrons[it->first] = Train(CreateTrainingSet(it->first));
  }

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

void PrintPattern(std::vector<int> &pattern) {
  for (size_t i = 0; i < kCols * 2 + 3; ++i)
    std::cout << "-";
  std::cout << std::endl;
  for (size_t i = 0; i < kRows; ++i) {
    std::cout << "| ";
    for (size_t j = 0; j < kCols; ++j) {
      char c = pattern[i*kCols+j] ? 'o' : ' ';
      std::cout << c << " ";
    }
    std::cout << "|" << std::endl;
  }
  for (size_t i = 0; i < kCols * 2 + 3; ++i)
    std::cout << "-";
  std::cout << std::endl;
}

int main() {
  
  sf::RenderWindow window(sf::VideoMode(kCols * 100 + 200,
                                        kRows * 100 + 400), "Perceptron");
  
  std::vector<int> input_data(kTotal, 0);
  std::vector<sf::RectangleShape> grid(kTotal);
  sf::RectangleShape solveButton;
  sf::RectangleShape answerBox;
  sf::Text buttonText;
  sf::Text answerText;
  sf::Font font;
  
  for (size_t i = 0; i < kRows; ++i) {
    for (size_t j = 0; j < kCols; ++j) {
      size_t p = kCols * i + j;
      grid[p].setSize(sf::Vector2f(100, 100));
      grid[p].setFillColor(sf::Color::Black);
      grid[p].setOutlineColor(sf::Color::White);
      grid[p].setOutlineThickness(kCols);
      grid[p].setPosition(100+100*j, 100+100*i);
    }
  }

  solveButton.setSize(sf::Vector2f(200, 100));
  solveButton.setFillColor(sf::Color::Red);
  solveButton.setPosition(100, 1000);
  solveButton.setOutlineColor(sf::Color::White);
  solveButton.setOutlineThickness(5);

  answerBox.setSize(sf::Vector2f(200, 100));
  answerBox.setFillColor(sf::Color::White);
  answerBox.setPosition(500, 1000);
  answerBox.setOutlineColor(sf::Color::Red);
  answerBox.setOutlineThickness(5);

  font.loadFromFile("../arial.ttf");
  
  buttonText.setFont(font);
  buttonText.setString("Guess");
  buttonText.setCharacterSize(55);
  buttonText.setFillColor(sf::Color::White);
  buttonText.setPosition(120, 1010);

  answerText.setFont(font);
  answerText.setCharacterSize(50);
  answerText.setFillColor(sf::Color::Red);
  answerText.setPosition(580, 1020);
  
  CreateTrainedPerceptrons();

  while (window.isOpen()) {
    sf::Event event;
    
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i position = sf::Mouse::getPosition(window);
        for (size_t i = 0; i < kTotal; ++i) {
          sf::Vector2f origin = grid[i].getPosition();
          if (origin.x < position.x &&
              origin.y < position.y &&
              origin.x + grid[i].getSize().x > position.x &&
              origin.y + grid[i].getSize().y > position.y) {
            input_data[i] ^= 1;
            sf::Color NewColor = (input_data[i] ? sf::Color::Yellow : sf::Color::Black);
            grid[i].setFillColor(NewColor);
          }
        }
        sf::Vector2f solvePos = solveButton.getPosition();
        if (solvePos.x < position.x &&
            solvePos.y < position.y &&
            solvePos.x + solveButton.getSize().x > position.x &&
            solvePos.y + solveButton.getSize().y > position.y) {
          std::cout << "Input Pattern" << std::endl;
          PrintPattern(input_data);
          NumberType result = Classify(input_data);
          std::cout << "Result: " << result << std::endl;
          answerText.setString(std::to_string(result));
        }
      }
    }
    
    window.clear();
    for (size_t i = 0; i < kTotal; ++i)
      window.draw(grid[i]);
    window.draw(solveButton);
    window.draw(buttonText);
    window.draw(answerBox);
    window.draw(answerText);
    window.display();
  }

  return 0;
}
