#pragma once
#include "iostream"
#include "spdlog/spdlog.h"

namespace grassland {

std::string GetTimestamp();

void LogInfo(const std::string &message);

void LogWarning(const std::string &message);

void LogError(const std::string &message);

template <class... Args>
void LogInfo(const std::string &message, Args &&...args) {
  spdlog::info(message, std::forward<Args>(args)...);
}

template <class... Args>
void LogWarning(const std::string &message, Args &&...args) {
  spdlog::warn(message, std::forward<Args>(args)...);
}

template <class... Args>
void LogError(const std::string &message, Args &&...args) {
  spdlog::error(message, std::forward<Args>(args)...);
}
}  // namespace grassland
