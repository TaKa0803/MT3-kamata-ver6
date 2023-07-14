#include<Matrix_V3math.h>
#include<math.h>
#include<cassert>
//加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	return{
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z,
	};
}

//減算　
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	return{
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z,
	};
}

//スカラー倍
Vector3 Multiply(float scalar, const Vector3& v) {
	return{
		v.x* scalar,
		v.y* scalar,
		v.z* scalar,
	};
}

//内積
float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

//長さ
float Length(const Vector3& v) {
	return sqrtf(Dot(v, v));
}

//正規化
Vector3 Normalize(const Vector3& v) {
	float leng = Length(v);
	assert(leng != 0);
	return{
		v.x / leng,
		v.y / leng,
		v.z / leng,
	};
}