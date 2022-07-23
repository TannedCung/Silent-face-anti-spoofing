#include <algorithm> // for transform
#include <functional> // for plus
#include <vector>
#include <iostream>

void print_vector(std::vector<int> vector){
  std::cout << "[";
  for (auto &i:vector){
    std::cout << i << ", "; 
  }
  std::cout << "]";
}

template <typename T>
std::vector<T> vector_add(std::vector<T>v1, std::vector<T>v2){
  std::vector<T> sum;
  std::transform(v1.begin(), v1.end(), v2.begin(), std::back_inserter(sum), std::plus<T>()); 
  return sum;
}

int main(){
  std::vector<int> v1 = {0,0,0,1};
  std::vector<int> v2 = {4,5,6};
  // std::vector<int> v3 = {1,2,3};
  v2 = vector_add<int>(v1,v2);
  std::cout << "v1: "; print_vector(v1); std::cout << std::endl;
  std::cout << "v2: "; print_vector(v2); std::cout << std::endl;
  // std::cout << "v3: "; print_vector(v3); std::cout << std::endl;
}