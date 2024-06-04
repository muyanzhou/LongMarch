#include "long_march.h"
#include "random"

using namespace long_march;

int main() {
  geometry::Field<double> field(11, 11, 11, 1.0, {-5.0, -5.0, -5.0}, 1);
  data_structure::LinearGridView<double> grid_view(field.grid());
  geometry::Field<double, decltype(grid_view)> field_view(
      1.0, {-5.0, -5.0, -5.0}, grid_view);

  for (int i = 0; i < field.width(); i++) {
    for (int j = 0; j < field.height(); j++) {
      for (int k = 0; k < field.depth(); k++) {
        field(i, j, k) = field.get_position(i, j, k).sum();
      }
    }
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(-5, 5);

  for (int i = 0; i < 100000; i++) {
    geometry::Vector3<float> pos(dis(gen), dis(gen), dis(gen));
    auto field_result = field(pos);
    auto field_view_result = field_view(pos);
    double predicted_result = pos.cast<double>().sum();
    std::cout << "Field: " << field_result
              << ", FieldView: " << field_view_result
              << ", Predicted: " << predicted_result << std::endl;
  }

  return 0;
}
