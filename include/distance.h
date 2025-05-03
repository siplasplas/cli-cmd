#pragma once
#include <tuple>
#include <vector>
#include <string>

namespace cli
{
  class RangeArray {
  private:
    int minIndex;
    int maxIndex;
    int *array;
  public:
    RangeArray();
    ~RangeArray();
    int &operator[](int index);
    void setSize(int minIndex, int maxIndex);
    void fill(int value);
  };

  /**
* Implementation based on:
* "An O(NP) Sequence Comparison Algorithm"
* Sun Wu, Udi Manber, Gene Myers
* 1989, Department of Computer Science, University of Arizona
*/
  class Distance {
    int snake(int k, int y);
    RangeArray fp;
    std::vector<std::tuple<int, int, int>> collection;
    int choose(int k);
  public:
    std::string A;
    std::string B;
    int M;
    int N;
    bool swapped;
    Distance(const std::string &A, const std::string &B);
    int compare();
  };
}
