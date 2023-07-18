#pragma once
#include<Vector3.h>
#include<Matrix4x4.h>

//クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);

//投資投影
Matrix4x4 MakePerspectiveFovM(float fovY, float aspectRatio, float nearClip, float farClip);
//正射影行列
Matrix4x4 MakeOrthographicM(float left, float top, float right, float bottom, float nearClip, float farClip);
//ビューポート変換行列
Matrix4x4 MakeViewportM(float left, float top, float width, float height, float minDepth, float maxDepth);


//アフィン
Matrix4x4 MakeAffineM(const Vector3& S, const Vector3& R, const Vector3& T);

//平行移動
Matrix4x4 MakeTranslateMatrix(const Vector3& T);
//拡縮
Matrix4x4 MakeScaleMatrix(const Vector3& S);
//座標変換
Vector3 Transform(const Vector3& v, const Matrix4x4& m);

//x軸回転行列
Matrix4x4 MakeRotateXM(float radian);
//y軸回転行列
Matrix4x4 MakeRotateYM(float radian);
//z軸回転行列
Matrix4x4 MakeRotateZM(float radian);

Matrix4x4 MakeAllRotateM(Vector3 rotate);


//加算
Vector3 Add(const Vector3& v1, const Vector3& v2);

//減算　
Vector3 Subtract(const Vector3& v1, const Vector3& v2);

//スカラー倍
Vector3 Multiply(float ccalar, const Vector3& v);

//内積
float Dot(const Vector3& v1, const Vector3& v2);

//長さ
float Length(const Vector3& v);

//正規化
Vector3 Normalize(const Vector3& v);

//加算
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
//減算
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);
//積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
//転置行列
Matrix4x4 Transpose(const Matrix4x4& m);
//単位行列の作成
Matrix4x4 MakeIdentity4x4();
