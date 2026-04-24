#include "Math.h"

Vector2 Math::CalcAngles(Vector3 from, Vector3 to) {
  const Vector3 dir = to - from;
  const Vector3 dir_sq = dir * dir;

  const float dir_mag = std::sqrtf(dir_sq.x + dir_sq.y + dir_sq.z);

  const float pitch = -std::asin(dir.z / dir_mag) * Math::to_degrees;
  const float yaw = std::atan2f(dir.y, dir.x) * Math::to_degrees;

  return {yaw, pitch};
}

Vector2 Math::CalcAngles(Vector3 from, Vector3 to, Vector2 weapon_sway) {
  const Vector3 dir = to - from;
  const Vector3 dir_sq = dir * dir;

  const float dir_mag = std::sqrtf(dir_sq.x + dir_sq.y + dir_sq.z);

  float pitch = -std::asin(dir.z / dir_mag) * Math::to_degrees;
  float yaw = std::atan2f(dir.y, dir.x) * Math::to_degrees;

  pitch -= weapon_sway.x;
  yaw -= weapon_sway.y;

  return {yaw, pitch};
}

Vector2 Math::CalcAngles(Vector3 from, Vector3 to, float pitch_min, float pitch_max, float yaw_min, float yaw_max) {
  const Vector3 dir = to - from;
  const Vector3 dir_sq = dir * dir;

  const float dir_mag = std::sqrtf(dir_sq.x + dir_sq.y + dir_sq.z);

  float pitch = -std::asin(dir.z / dir_mag) * Math::to_degrees;
  float yaw = std::atan2f(dir.y, dir.x) * Math::to_degrees;

  pitch = std::clamp(pitch, pitch_min, pitch_max);
  yaw = std::clamp(yaw, yaw_min, yaw_max);

  return {yaw, pitch};
}

Vector2 Math::CalcAngles(Vector3 from, Vector3 to, float pitch_min, float pitch_max, float yaw_min, float yaw_max, Vector2 weapon_sway) {
  const Vector3 dir = to - from;
  const Vector3 dir_sq = dir * dir;

  const float dir_mag = std::sqrtf(dir_sq.x + dir_sq.y + dir_sq.z);

  float pitch = -std::asin(dir.z / dir_mag) * Math::to_degrees;
  float yaw = std::atan2f(dir.y, dir.x) * Math::to_degrees;

  pitch -= weapon_sway.x;
  yaw -= weapon_sway.y;

  pitch = std::clamp(pitch, pitch_min, pitch_max);
  yaw = std::clamp(yaw, yaw_min, yaw_max);

  return {yaw, pitch};
}