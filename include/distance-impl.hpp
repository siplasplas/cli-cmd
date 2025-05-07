#pragma once
#include "distance.h"

namespace cli
{
  INLINE RangeArray::RangeArray() {
  }

  INLINE void RangeArray::setSize(int minIndex, int maxIndex) {
    this->minIndex = minIndex;
    this->maxIndex = maxIndex;
    array = new int[maxIndex - minIndex + 1]();
  }

  INLINE RangeArray::~RangeArray() {
    delete[] array;
  }

  INLINE int &RangeArray::operator[](int index) {
    return array[index - minIndex];
  }

  INLINE void RangeArray::fill(int value) {
    std::fill(array, array + (maxIndex - minIndex + 1), value);
  }

  INLINE Distance::Distance(const std::string &A, const std::string &B) : A(A), B(B) {
    M = (int) A.size();
    N = (int) B.size();
    if (M > N) {
      std::swap(this->A, this->B);
      std::swap(M, N);
      swapped = true;
    }
    fp.setSize(-(M + 1), N + 1);
  }

  INLINE int Distance::choose(int k) {
    int v0 = fp[k - 1] + 1;
    int v1 = fp[k + 1];
    int dir;
    if (v0 > v1) //should be > , not >=
      dir = 1;
    else
      dir = -1;
    int maxV = std::max(v0, v1);
    int sn = snake(k, maxV);
    collection.emplace_back(k, sn, dir);
    return sn;
  }

  INLINE int Distance::snake(int k, int y) {
    int x = y - k;
    while (x < M && y < N && A[x] == B[y]) {
      x++;
      y++;
    }
    return y;
  }

  INLINE int Distance::compare() {
    int Delta = N - M;
    fp.fill(-1);
    int p = -1;
    do {
      p++;
      for (int k = -p; k <= Delta - 1; k++) {
        fp[k] = choose(k);
      }
      for (int k = Delta + p; k >= Delta + 1; k--) {
        fp[k] = choose(k);
      }
      fp[Delta] = choose(Delta);
    } while (fp[Delta] != N);
    return Delta + 2 * p;
  }
}