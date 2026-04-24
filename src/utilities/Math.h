#pragma once

#include "../pch.h"

namespace Math {
constexpr float to_degrees = 180 / std::numbers::pi_v<float>;

Vector2 CalcAngles(Vector3 from, Vector3 to);
Vector2 CalcAngles(Vector3 from, Vector3 to, float pitch_min, float pitch_max, float yaw_min, float yaw_max);
Vector2 CalcAngles(Vector3 from, Vector3 to, float pitch_min, float pitch_max, float yaw_min, float yaw_max, Vector2 weapon_swap);
Vector2 CalcAngles(Vector3 from, Vector3 to, Vector2 weapon_sway);
}