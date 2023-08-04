#pragma once
#include"Vector3.h"
#include"Matrix_V3math.h"

//オーバーロード演算子

//ベクトル
//加算
Vector3 operator+(const Vector3& a, const Vector3& b) { return Add(a, b); }
//減算
Vector3 operator-(const Vector3& a, const Vector3& b) { return Subtract(a, b); }
//スカラー倍
Vector3 operator*(float a, const Vector3 b) { return Multiply(a, b); }
//スカラー倍(反転
Vector3 operator*(const Vector3& b, float a) { return Multiply(a, b); }
//割る
Vector3 operator/(const Vector3& v, float s) { return Multiply(1.0f / s, v); }

//行列
//加算
Matrix4x4 operator+(const Matrix4x4& a, const Matrix4x4& b) { return Add(a, b); }
//減算
Matrix4x4 operator-(const Matrix4x4& a, const Matrix4x4& b) { return Subtract(a, b); }
//
Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b) { return Multiply(a, b); }

//単項演算子
Vector3 operator-(const Vector3& v) { return{ -v.x,-v.y,-v.z }; }
Vector3 operator+(const Vector3& v) { return v; }

