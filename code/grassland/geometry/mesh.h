#pragma once
#include "fstream"
#include "grassland/geometry/geometry_utils.h"

namespace grassland::geometry {
template <typename Scalar = float>
class Mesh {
 public:
  Mesh(size_t num_vertices = 0,
       size_t num_indices = 0,
       const uint32_t *indices = nullptr,
       const Vector3<Scalar> *positions = nullptr,
       const Vector3<Scalar> *normals = nullptr,
       const Vector3<Scalar> *tangents = nullptr,
       const Vector2<Scalar> *tex_coords = nullptr);

  size_t NumVertices() const {
    return num_vertices_;
  }
  size_t NumIndices() const {
    return num_indices_;
  }

  Vector3<Scalar> *Positions() {
    return positions_.data();
  }
  Vector3<Scalar> *Normals() {
    if (normals_.empty())
      return nullptr;
    return normals_.data();
  }
  Vector3<Scalar> *Tangents() {
    if (tangents_.empty())
      return nullptr;
    return tangents_.data();
  }
  Vector2<Scalar> *TexCoords() {
    if (tex_coords_.empty())
      return nullptr;
    return tex_coords_.data();
  }
  float *Signals() {
    if (signals_.empty())
      return nullptr;
    return signals_.data();
  }
  uint32_t *Indices() {
    return indices_.data();
  }

  const Vector3<Scalar> *Positions() const {
    return positions_.data();
  }
  const Vector3<Scalar> *Normals() const {
    if (normals_.empty())
      return nullptr;
    return normals_.data();
  }
  const Vector3<Scalar> *Tangents() const {
    if (tangents_.empty())
      return nullptr;
    return tangents_.data();
  }
  const Vector2<Scalar> *TexCoords() const {
    if (tex_coords_.empty())
      return nullptr;
    return tex_coords_.data();
  }
  const float *Signals() const {
    if (signals_.empty())
      return nullptr;
    return signals_.data();
  }
  const uint32_t *Indices() const {
    return indices_.data();
  }

  int SaveObjFile(const std::string &filename) const;

  int SplitVertices();

  int MergeVertices();

  int GenerateNormals(
      Scalar merging_threshold =
          0.8f);  // if all the face normals on a vertex's pairwise dot product
                  // larger than merging_threshold, then merge them

 private:
  std::vector<Vector3<Scalar>> positions_;
  std::vector<Vector3<Scalar>> normals_;
  std::vector<Vector3<Scalar>> tangents_;
  std::vector<Vector2<Scalar>> tex_coords_;
  std::vector<float> signals_;
  std::vector<uint32_t> indices_;
  size_t num_vertices_{0};
  size_t num_indices_{0};
};

template <typename Scalar>
int Mesh<Scalar>::MergeVertices() {
  std::vector<Vector3<Scalar>> new_positions;
  std::vector<Vector3<Scalar>> new_normals;
  std::vector<Vector3<Scalar>> new_tangents;
  std::vector<Vector2<Scalar>> new_tex_coords;
  std::vector<float> new_signals;
  auto vector_comparator = [](const Vector3<Scalar> &a,
                              const Vector3<Scalar> &b) {
    if (a[0] != b[0]) {
      return a[0] < b[0];
    }
    if (a[1] != b[1]) {
      return a[1] < b[1];
    }
    return a[2] < b[2];
  };
  auto vertex_index_comparator = [this, vector_comparator](uint32_t a,
                                                           uint32_t b) {
    if (positions_[a] != positions_[b]) {
      return vector_comparator(positions_[a], positions_[b]);
    }
    if (!normals_.empty() && normals_[a] != normals_[b]) {
      return vector_comparator(normals_[a], normals_[b]);
    }
    if (!tangents_.empty() && tangents_[a] != tangents_[b]) {
      return vector_comparator(tangents_[a], tangents_[b]);
    }
    if (!tex_coords_.empty() && tex_coords_[a] != tex_coords_[b]) {
      return tex_coords_[a][0] < tex_coords_[b][0] ||
             (tex_coords_[a][0] == tex_coords_[b][0] &&
              tex_coords_[a][1] < tex_coords_[b][1]);
    }
    if (!signals_.empty() && signals_[a] != signals_[b]) {
      return signals_[a] < signals_[b];
    }
    return false;
  };
  std::map<uint32_t, uint32_t, decltype(vertex_index_comparator)> vertex_map(
      vertex_index_comparator);
  std::vector<uint32_t> index_map;

  for (size_t i = 0; i < num_vertices_; i++) {
    if (vertex_map.find(i) == vertex_map.end()) {
      new_positions.push_back(positions_[i]);
      if (!normals_.empty()) {
        new_normals.push_back(normals_[i]);
      }
      if (!tangents_.empty()) {
        new_tangents.push_back(tangents_[i]);
      }
      if (!tex_coords_.empty()) {
        new_tex_coords.push_back(tex_coords_[i]);
      }
      if (!signals_.empty()) {
        new_signals.push_back(signals_[i]);
      }
      vertex_map[i] = new_positions.size() - 1;
      index_map.push_back(new_positions.size() - 1);
    } else {
      index_map.push_back(vertex_map[i]);
    }
  }

  positions_ = new_positions;
  normals_ = new_normals;
  tangents_ = new_tangents;
  tex_coords_ = new_tex_coords;
  signals_ = new_signals;

  for (size_t i = 0; i < num_indices_; i++) {
    indices_[i] = index_map[indices_[i]];
  }

  num_vertices_ = new_positions.size();

  return 0;
}

template <typename Scalar>
Mesh<Scalar>::Mesh(size_t num_vertices,
                   size_t num_indices,
                   const uint32_t *indices,
                   const Vector3<Scalar> *positions,
                   const Vector3<Scalar> *normals,
                   const Vector3<Scalar> *tangents,
                   const Vector2<Scalar> *tex_coords) {
  num_vertices_ = num_vertices;
  num_indices_ = num_indices;
  indices_.resize(num_indices);
  if (indices) {
    std::copy(indices, indices + num_indices, indices_.begin());
  }
  positions_.resize(num_vertices);
  if (positions) {
    std::copy(positions, positions + num_vertices, positions_.begin());
  }
  if (normals) {
    normals_.resize(num_vertices);
    std::copy(normals, normals + num_vertices, normals_.begin());
    if (tangents) {
      tangents_.resize(num_vertices);
      std::copy(tangents, tangents + num_vertices, tangents_.begin());
      signals_.resize(num_vertices);
      std::fill(signals_.begin(), signals_.end(), 1.0f);
    }
  }
  if (tex_coords) {
    tex_coords_.resize(num_vertices);
    std::copy(tex_coords, tex_coords + num_vertices, tex_coords_.begin());
  }
}

template <typename Scalar>
int Mesh<Scalar>::SaveObjFile(const std::string &filename) const {
  std::ofstream file(filename);
  if (!file.is_open()) {
    return -1;
  }
  for (size_t i = 0; i < num_vertices_; i++) {
    file << "v " << positions_[i][0] << " " << positions_[i][1] << " "
         << positions_[i][2] << std::endl;
  }
  if (!normals_.empty()) {
    for (size_t i = 0; i < num_vertices_; i++) {
      file << "vn " << normals_[i][0] << " " << normals_[i][1] << " "
           << normals_[i][2] << std::endl;
    }
  }
  if (!tex_coords_.empty()) {
    for (size_t i = 0; i < num_vertices_; i++) {
      file << "vt " << tex_coords_[i][0] << " " << tex_coords_[i][1]
           << std::endl;
    }
  }
  auto output_face_index = [this, &file](size_t i) {
    file << i;
    file << "/";
    if (!tex_coords_.empty()) {
      file << i;
    }
    file << "/";
    if (!normals_.empty()) {
      file << i;
    }
  };
  for (size_t i = 0; i < num_indices_; i += 3) {
    file << "f ";
    output_face_index(indices_[i] + 1);
    file << " ";
    output_face_index(indices_[i + 1] + 1);
    file << " ";
    output_face_index(indices_[i + 2] + 1);
    file << std::endl;
  }
  return 0;
}

template <typename Scalar>
int Mesh<Scalar>::SplitVertices() {
  std::vector<Vector3<Scalar>> new_positions;
  std::vector<Vector3<Scalar>> new_normals;
  std::vector<Vector3<Scalar>> new_tangents;
  std::vector<Vector2<Scalar>> new_tex_coords;
  std::vector<float> new_signals;

  for (size_t i = 0; i < num_indices_; i++) {
    new_positions.push_back(positions_[indices_[i]]);
    if (!normals_.empty()) {
      new_normals.push_back(normals_[indices_[i]]);
    }
    if (!tangents_.empty()) {
      new_tangents.push_back(tangents_[indices_[i]]);
      new_signals.push_back(signals_[indices_[i]]);
    }
    if (!tex_coords_.empty()) {
      new_tex_coords.push_back(tex_coords_[indices_[i]]);
    }
    indices_[i] = i;
  }

  positions_ = new_positions;
  normals_ = new_normals;
  tangents_ = new_tangents;
  tex_coords_ = new_tex_coords;
  signals_ = new_signals;
  num_vertices_ = num_indices_;

  return 0;
}

template <typename Scalar>
int Mesh<Scalar>::GenerateNormals(Scalar merging_threshold) {
  std::vector<std::vector<Vector3<Scalar>>> vertex_normals(num_vertices_);
  std::vector<Vector3<Scalar>> weighted_normals(num_vertices_,
                                                Vector3<Scalar>::Zero());
  std::vector<Scalar> weights(num_vertices_, 0);
  std::vector<bool> merge(num_vertices_, true);

  for (size_t i = 0; i < num_indices_; i += 3) {
    Vector3<Scalar> v0 = positions_[indices_[i]];
    Vector3<Scalar> v1 = positions_[indices_[i + 1]];
    Vector3<Scalar> v2 = positions_[indices_[i + 2]];
    Vector3<Scalar> normal = (v1 - v0).cross(v2 - v0);
    if (normal.norm() < Eps<Scalar>()) {
      continue;
    }
    normal.normalize();
    Scalar angles[3];
    angles[0] = acos((v1 - v0).normalized().dot((v2 - v0).normalized()));
    angles[1] = acos((v2 - v1).normalized().dot((v0 - v1).normalized()));
    angles[2] = acos((v0 - v2).normalized().dot((v1 - v2).normalized()));
    for (size_t j = 0; j < 3; j++) {
      vertex_normals[indices_[i + j]].push_back(normal);
      weighted_normals[indices_[i + j]] += angles[j] * normal;
      weights[indices_[i + j]] += angles[j];
    }
  }

  std::vector<Vector3<Scalar>> new_positions = positions_;
  std::vector<Vector3<Scalar>> new_normals(num_vertices_);
  std::vector<Vector2<Scalar>> new_tex_coords = tex_coords_;
  std::vector<uint32_t> split_count(num_vertices_, 0);
  std::vector<uint32_t> new_indices;

  for (size_t i = 0; i < num_vertices_; i++) {
    for (size_t j = 0; j < vertex_normals[i].size(); j++) {
      for (size_t k = j + 1; k < vertex_normals[i].size(); k++) {
        if (vertex_normals[i][j].dot(vertex_normals[i][k]) <
            merging_threshold) {
          merge[i] = false;
          break;
        }
      }
      if (!merge[i]) {
        break;
      }
    }

    if (merge[i]) {
      if (weights[i] > Eps<Scalar>()) {
        new_normals[i] = (weighted_normals[i] / weights[i]).normalized();
      }
    }
  }

  for (size_t i = 0; i < num_indices_; i++) {
    uint32_t vi = indices_[i];
    if (merge[vi]) {
      new_indices.push_back(vi);
    } else {
      if (split_count[vi] == 0) {
        new_normals[vi] = vertex_normals[vi][0];
        new_indices.push_back(vi);
      } else {
        new_positions.push_back(positions_[vi]);
        new_normals.push_back(vertex_normals[vi][split_count[vi]]);
        if (!tex_coords_.empty()) {
          new_tex_coords.push_back(tex_coords_[vi]);
        }
        new_indices.push_back(new_positions.size() - 1);
      }
      split_count[vi]++;
    }
  }

  positions_ = new_positions;
  normals_ = new_normals;
  tex_coords_ = new_tex_coords;
  indices_ = new_indices;

  tangents_.clear();
  signals_.clear();
  num_vertices_ = positions_.size();
  num_indices_ = indices_.size();

  return 0;
}

}  // namespace grassland::geometry
