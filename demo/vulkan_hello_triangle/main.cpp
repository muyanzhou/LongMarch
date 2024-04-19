#include "iostream"
#include "long_march.h"
#include "memory"

template <class ContentType>
class LifeTimeTracker {
 public:
  template <class... Args>
  LifeTimeTracker(Args &&...args) : content(std::forward<Args>(args)...) {
    std::cout << "LifeTimeTracker constructor: " << content << std::endl;
  }
  ~LifeTimeTracker() {
    std::cout << "LifeTimeTracker destructor: " << content << std::endl;
  }

 private:
  ContentType content;
};

void CreateInt(grassland::utils::double_ptr<LifeTimeTracker<int>> ppInt,
               int value) {
  ppInt = new LifeTimeTracker<int>(value);
}

int main() {
  std::shared_ptr<LifeTimeTracker<int>> pInt;
  std::unique_ptr<LifeTimeTracker<int>> upInt;
  LifeTimeTracker<int> *rawInt = nullptr;
  {
    CreateInt(&pInt, 20);
    CreateInt(&upInt, 30);
    CreateInt(&rawInt, 10);
    delete rawInt;
  }
}
