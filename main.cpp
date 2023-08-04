#include <Novice.h>
#include<Vector3.h>
#include<Matrix_V3math.h>
#include<stdint.h>
#include<cmath>
#include<numbers>
#include<imgui.h>
#include <algorithm>
#include<iostream>
#include<cassert>
#include"overload.h"




struct vec {
	int x;
	int y;
};

struct mouse {
	vec now;
	vec before;
};


struct WorldTransform{
	Vector3 translation;
	Vector3 rotation;
	Vector3 scale;
};

struct Sphere {
	Vector3 center;
	float radius;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

struct Plane
{
	Vector3 normal;
	float distance;
};

struct Triangle {
	Vector3 verties[3];//頂点
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct Bezier {
	Vector3 controllP0;
	Vector3 controllP1;
	Vector3 controllP2;

};

struct Spring
{
	//アンカー
	Vector3 anchor;
	float naturalLength;//自然長
	float stiffness;//ばね定数
	float dampingCoefficient;
};

struct Ball {
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float mass;
	float radius;
	unsigned int color;
};

const char kWindowTitle[] = "LE2A_05_キクチ_タカヤ";

static const int kRowHeight = 20;
static const int lColumnWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + lColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + lColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + lColumnWidth * 3, y, "%s", label);

}
void MatrixScreenPrintf(int x, int y, const Matrix4x4& m, const char* label) {
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * lColumnWidth, y + row * kRowHeight, "%6.02f", m.m[row][column]);
		}
	}
	Novice::ScreenPrintf(x + lColumnWidth * 4, y, "%s", label);
}


const int kWindowWidth = 1280;
const int kWindowHeight = 720;



//グリッド表記
void DrawGrid(const Matrix4x4& vprojection, const Matrix4x4& viewport) {
	const float kGridHalfWidth = 2.0f;//Gridの半分の幅
	const uint32_t kSubdivition = 10;//分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivition);//一つ分の長さ
	//奥から手前に線を引いていく
	for (uint32_t Index = 0; Index <= kSubdivition; ++Index) {
		//上の情報から始点と終点を求める
		Vector3 st, ed;
		st = {
			-kGridHalfWidth,
			0,
			-kGridHalfWidth+(kGridEvery*Index)
		};
		ed = {
			+kGridHalfWidth,
			0,
			-kGridHalfWidth+(kGridEvery*Index)
		};
		//スクリーン座標に変換をかける
		Vector3 Sst, Sed;
		Sst = Transform(Transform(st, vprojection), viewport);
		Sed = Transform(Transform(ed, vprojection), viewport);
		//線を引く
		if (Index == 5) {
			Novice::DrawLine((int)Sst.x, (int)Sst.y, (int)Sed.x, (int)Sed.y, BLACK);
		}
		else {
			Novice::DrawLine((int)Sst.x, (int)Sst.y, (int)Sed.x, (int)Sed.y, WHITE);
		}
	}
	for (uint32_t Index = 0; Index <= kSubdivition; ++Index) {
		//上の情報から始点と終点を求める
		Vector3 st, ed;
		st = {
			-kGridHalfWidth + (kGridEvery * Index),
			0,
			-kGridHalfWidth
		};
		ed = {
			-kGridHalfWidth + (kGridEvery * Index),
			0,
			kGridHalfWidth
		};
		//スクリーン座標に変換をかける
		Vector3 Sst, Sed;
		Sst = Transform(Transform(st, vprojection), viewport);
		Sed = Transform(Transform(ed, vprojection), viewport);
		//線を引く
		if (Index == 5) {
			Novice::DrawLine((int)Sst.x, (int)Sst.y, (int)Sed.x, (int)Sed.y, BLACK);
		}
		else {
			Novice::DrawLine((int)Sst.x, (int)Sst.y, (int)Sed.x, (int)Sed.y, WHITE);
		}
	}
}

//円描画
void DrawSphere(const Sphere& S, const Matrix4x4& vpM, const Matrix4x4& viewport, uint32_t color) {
	const uint32_t kSubdivision = 16;//分割数
	const float kLonEvery = ((float)std::numbers::pi * 2.0) /kSubdivision;//経度分割
	const float kLatEvery = (float)std::numbers::pi/ kSubdivision;//緯度一つ分の角度
	//緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -(float)std::numbers::pi / 2.0f + kLatEvery * latIndex;
		//経度の方向に分割0~2pi
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			//worldのabcを求める
			Vector3 a, b, c;
			a = Multiply(S.radius, { cosf(lat) * cosf(lon),sinf(lat),cosf(lat) * sinf(lon) });
			b = Multiply(S.radius, { cosf(lat + kLatEvery) * cosf(lon),sinf(lat + kLatEvery),cosf(lat + kLatEvery) * sinf(lon) });
			c = Multiply(S.radius, { cosf(lat) * cosf(lon + kLonEvery),sinf(lat),cosf(lat) * sinf(lon + kLonEvery) });
			a = Add(a, S.center);
			b = Add(b, S.center);
			c = Add(c, S.center);

			//スクリーンに変換
			Vector3 Wa, Wb, Wc;
			Wa = Transform(Transform(a, vpM), viewport);
			Wb = Transform(Transform(b, vpM), viewport);
			Wc = Transform(Transform(c, vpM), viewport);

			//線を引く
			Novice::DrawLine((int)Wa.x, (int)Wa.y, (int)Wc.x, (int)Wc.y, color);
			Novice::DrawLine((int)Wa.x, (int)Wa.y, (int)Wb.x, (int)Wb.y, color);

		}
	}
}

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y,vector.x,0.0f };
	}
	return{ 0.0f,-vector.z,vector.y };
}
//線描画
void DrawSegment(const Segment& S, const Matrix4x4& vpM, const Matrix4x4& viewport, uint32_t color) {
	Vector3 st, ed;
	st = Transform(Transform(S.origin, vpM), viewport);
	ed = Transform(Transform(Add(S.origin,S.diff), vpM), viewport);

	Novice::DrawLine((int)st.x, (int)st.y, (int)ed.x, (int)ed.y, color);
}
//面描画
void DrawPlane(const Plane& plane, const Matrix4x4& viewprojectionM, const Matrix4x4& viewportM,uint32_t color) {
	Vector3 center = Multiply(plane.distance, plane.normal);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z };
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = { perpendiculars[2].x,-perpendiculars[2].y,-perpendiculars[2].z };

	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewprojectionM), viewportM);
	}
	Novice::DrawLine((int)points[0].x, (int)points[0].y, (int)points[2].x, (int)points[2].y, color);
	Novice::DrawLine((int)points[0].x, (int)points[0].y, (int)points[3].x, (int)points[3].y, color);
	Novice::DrawLine((int)points[1].x, (int)points[1].y, (int)points[2].x, (int)points[2].y, color);
	Novice::DrawLine((int)points[1].x, (int)points[1].y, (int)points[3].x, (int)points[3].y, color);
}
//三角形描画
void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewprojectionM, const Matrix4x4& viewport,uint32_t color) {
	Vector3 reP[3];
	for (int i = 0; i < 3; i++) {
		reP[i] = Transform(Transform(triangle.verties[i], viewprojectionM), viewport);
	}
	Novice::DrawLine((int)reP[0].x, (int)reP[0].y, (int)reP[1].x, (int)reP[1].y, color);
	Novice::DrawLine((int)reP[0].x, (int)reP[0].y, (int)reP[2].x, (int)reP[2].y, color);
	Novice::DrawLine((int)reP[2].x, (int)reP[2].y, (int)reP[1].x, (int)reP[1].y, color);

}

//AABB描画
void DrawAABB(const AABB& aabb, const Matrix4x4& vpMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 a, b, c, d, e, f, g, h;

	Vector3 side = Subtract(aabb.max, aabb.min);

	a = Transform(Transform(aabb.min, vpMatrix), viewportMatrix);
	b = Transform(Transform(Add(aabb.min, { side.x,0,0 }), vpMatrix), viewportMatrix);
	c = Transform(Transform(Add(aabb.min, { 0,side.y,0 }), vpMatrix), viewportMatrix);
	d = Transform(Transform(Add(aabb.min, { side.x,side.y,0 }), vpMatrix), viewportMatrix);

	e = Transform(Transform(aabb.max, vpMatrix), viewportMatrix);
	f = Transform(Transform(Add(aabb.max, { -side.x,0,0 }), vpMatrix), viewportMatrix);
	g = Transform(Transform(Add(aabb.max, { 0,-side.y,0 }), vpMatrix), viewportMatrix);
	h = Transform(Transform(Add(aabb.max, { -side.x,-side.y,0 }), vpMatrix), viewportMatrix);


	Novice::DrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, color);
	Novice::DrawLine((int)a.x, (int)a.y, (int)c.x, (int)c.y, color);
	Novice::DrawLine((int)d.x, (int)d.y, (int)b.x, (int)b.y, color);
	Novice::DrawLine((int)d.x, (int)d.y, (int)c.x, (int)c.y, color);

	Novice::DrawLine((int)a.x, (int)a.y, (int)h.x, (int)h.y, color);
	Novice::DrawLine((int)e.x, (int)e.y, (int)d.x, (int)d.y, color);
	Novice::DrawLine((int)b.x, (int)b.y, (int)g.x, (int)g.y, color);
	Novice::DrawLine((int)c.x, (int)c.y, (int)f.x, (int)f.y, color);



	Novice::DrawLine((int)e.x, (int)e.y, (int)f.x, (int)f.y, color);
	Novice::DrawLine((int)e.x, (int)e.y, (int)g.x, (int)g.y, color);
	Novice::DrawLine((int)h.x, (int)h.y, (int)f.x, (int)f.y, color);
	Novice::DrawLine((int)h.x, (int)h.y, (int)g.x, (int)g.y, color);

}
//入れ替わりチェック
void CheckswitchAABB(AABB& a) {
	Vector3 max, min;
	min.x = (std::min)(a.min.x, a.max.x);
	min.y = (std::min)(a.min.y, a.max.y);
	min.z = (std::min)(a.min.z, a.max.z);

	max.x = (std::max)(a.min.x, a.max.x);
	max.y = (std::max)(a.min.y, a.max.y);
	max.z = (std::max)(a.min.z, a.max.z);
	a.max = max;
	a.min = min;

}

Vector3 Leap(const Vector3& v1, const Vector3& v2, float t) {
	return {
		v1.x * (1.0f - t) + v2.x * t,
		v1.y * (1.0f - t) + v2.y * t,
		v1.z * (1.0f - t) + v2.z * t,
	};

}
void DrawBezier(const Bezier& B, const Matrix4x4& VP, Matrix4x4 viewport,uint32_t color) {
	const float kSubdivision = 32;//分割数
	float t = 0;
	//過去座標
	Vector3 bpos = B.controllP0;
	for (int i = 0; i <= 32; i++) {
		t += (1.0f / kSubdivision);
		Vector3 p0p1 = Leap(B.controllP0, B.controllP1, t);
		Vector3 p1p2 = Leap(B.controllP1, B.controllP2, t);
		Vector3 p = Leap(p0p1, p1p2, t);

		Vector3 world = Transform(Transform(p, VP), viewport);
		Vector3 bp = Transform(Transform(bpos, VP), viewport);

		Novice::DrawLine((int)bp.x, (int)bp.y, (int)world.x, (int)world.y, color);
		//過去位置を更新
		bpos = p;
	}
}


//デバッグ関数
void DebugSphere(Sphere& S,const char*label) {
	ImGui::Begin(label);
	ImGui::DragFloat3("S center", &S.center.x, 0.01f);
	ImGui::DragFloat("S radius", &S.radius, 0.01f);
	ImGui::End();
}
void DebugV3(Vector3& v, const char* label) {
	ImGui::Begin(label);
	ImGui::DragFloat3("V3", &v.x, 0.01f);
	ImGui::End();
}
void DebugAABB(AABB& aabb, const char* label) {
	ImGui::Begin(label);
	ImGui::DragFloat3("max", &aabb.max.x, 0.01f);
	ImGui::DragFloat3("min", &aabb.min.x, 0.01f);
	ImGui::End();
}
void DebugBezier(Bezier& B, const char* label) {
	ImGui::Begin(label);
	ImGui::DragFloat3("p0", &B.controllP0.x, 0.01f);
	ImGui::DragFloat3("p1", &B.controllP1.x, 0.01f);
	ImGui::DragFloat3("p2", &B.controllP2.x, 0.01f);
	ImGui::End();
}


bool CmeraMove(WorldTransform &camera,char key[],mouse m) {
	//原点からカメラへのベクトル
	Vector3 tocamera = Normalize(Subtract(camera.translation, { 0,0,0 }));
	//ホイール操作分移動
	camera.translation = Add(camera.translation, Multiply(-(float)Novice::GetWheel() / 100, tocamera));
	if (key[DIK_UP]) {}

	//クリック中処理
	if (Novice::IsPressMouse(1)) {
		//マウスの過去位置と現在位置から回転方向をそれぞれ算出
		vec hri = {
			m.now.x-m.before.x,//プラスなら右
			m.now.y - m.before.y,//プラスなら下
		};
		if (hri.x == 0 && hri.y == 0) {
			return false;
		}
		//ｘの数値からＺ軸編集
		camera.rotation.y += (float)hri.x * 0.01f;



	}
	m;
	return true;
}
//
Vector3 Project(const Vector3& v1, const Vector3& v2) {
	float dot = Dot(v1, v2);
	float v22 = Length(v2);
	float t = dot / (v22 * v22);
	return Multiply(t, v2);
}
//最近接点
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 project = Project(Subtract(point, segment.origin), segment.diff);
	return Add(segment.origin, project);
}

//円と円
bool InCollision(const Sphere& s1, const Sphere& s2) {
	Vector3 a = {
		s1.center.x - s2.center.x,
		s1.center.y - s2.center.y,
		s1.center.z - s2.center.z,
	};
	float Hlong = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
	if (Hlong <= (s1.radius + s2.radius)) {
		return true;
	}
	return false;
}
//円と面
bool InCollision(const Sphere& sphere, const Plane& plane) {
	Vector3 v1 = sphere.center;
	Vector3 v2 = Multiply(plane.distance, plane.normal);
	if (Length(Subtract(v1, v2)) <= sphere.radius) {
		return true;
	}
	return false;
}
//線分内に点があるか
bool PointInLine(const Vector3 a, const Vector3 LS, const Vector3 LE) {
	//x軸	
	if (LS.x == LE.x) {//垂直
		//設置点aが垂直線の中ではないならfalse
		if (LS.x != a.x) {
			return false;
		}
	}
	else if (LS.x < LE.x) {//LS.xがLE.xより小さい場合の範囲計算
		//xの線の範囲にaがいなければfalse
		if (a.x < LS.x || LE.x < a.x) {
			return false;
		}
	}
	else if (LS.x > LE.x) {//LS.xがLE.xより大きい場合の範囲計算
		//線の範囲内にaがいなければfalse
		if (a.x < LE.x || LS.x < a.x) {
			return false;
		}
	}

	//y軸	
	if (LS.y == LE.y) {//垂直
		//設置点aが垂直線の中ではないならfalse
		if (LS.y != a.y) {
			return false;
		}
	}
	else if (LS.y < LE.y) {//LS.xがLE.xより小さい場合の範囲計算
		//xの線の範囲にaがいなければfalse
		if (a.y < LS.y || LE.y < a.y) {
			return false;
		}
	}
	else if (LS.y > LE.y) {//LS.xがLE.xより大きい場合の範囲計算
		//線の範囲内にaがいなければfalse
		if (a.y < LE.y || LS.y < a.y) {
			return false;
		}
	}

	//z軸	
	if (LS.z == LE.z) {//垂直
		//設置点aが垂直線の中ではないならfalse
		if (LS.z != a.z) {
			return false;
		}
	}
	else if (LS.z < LE.z) {//LS.xがLE.xより小さい場合の範囲計算
		//xの線の範囲にaがいなければfalse
		if (a.z < LS.z || LE.z < a.z) {
			return false;
		}
	}
	else if (LS.z > LE.z) {//LS.xがLE.xより大きい場合の範囲計算
		//線の範囲内にaがいなければfalse
		if (a.z < LE.z || LS.z < a.z) {
			return false;
		}
	}

	return true;
}
//線と面
bool InCollision(const Segment& segment, const Plane& plane, const Matrix4x4& vpM, const Matrix4x4& viewport) {
	//法線と線の内積
	float dot = Dot(plane.normal, segment.diff);
	if (dot == 0.0f) {
		return false;
	}
	else {
		//tを求める
		float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

		Vector3 a = Add(segment.origin, Multiply(t, segment.diff));
		Sphere S = { .center = a,.radius = 0.01f, };
		DrawSphere(S, vpM, viewport,WHITE);


		ImGui::Begin("S");
		ImGui::Text("%4.1f/%4.1f/%4.1f", a.x, a.y, a.z);
		ImGui::End();

		Vector3 LS = segment.origin;
		Vector3 LE = Add(segment.origin, segment.diff);

		if (PointInLine(a, LS, LE)) {
			return true;
		}
		else {
			return false;
		}
	}
}
//線と三角形
bool InCollision(const Segment& segment, const Triangle& triangle) {
	//三角形の法線
	Vector3 AB = Subtract(triangle.verties[1], triangle.verties[0]);
	Vector3 AC = Subtract(triangle.verties[2], triangle.verties[0]);
	Vector3 normal = Normalize(Cross(AB, AC));
	//線と三角形の法線の内積
	float dot = Dot(normal, segment.diff);
	if (dot == 0.0f) {
		return false;
	}
	else {
		//tを求める
		float t(0 - Dot(segment.origin, normal) / dot);

		Vector3 a = Add(segment.origin, Multiply(t, segment.diff));

		Vector3 LS = segment.origin;
		Vector3 LE = Add(segment.origin, segment.diff);
		//線の接点が線分内にあるなら
		if (PointInLine(a, LS, LE)) {

			Vector3 v01 = Subtract(triangle.verties[1], triangle.verties[0]);
			Vector3 v12 = Subtract(triangle.verties[2], triangle.verties[1]);
			Vector3 v20 = Subtract(triangle.verties[0], triangle.verties[2]);

			Vector3 v1p = Subtract(a, triangle.verties[1]);
			Vector3 v2p = Subtract(a, triangle.verties[2]);
			Vector3 v0p = Subtract(a, triangle.verties[0]);

			Vector3 ross01 = Cross(v01, v0p);
			Vector3 ross12 = Cross(v12, v1p);
			Vector3 ross20 = Cross(v20, v2p);

			if (Dot(ross01, normal) >= 0.0f && Dot(ross12, normal) >= 0.0f && Dot(ross20, normal) >= 0.0f) {
				return true;
			}
		}
	}
	return false;
}
//AABB同士
bool InCollision(const AABB& a, const AABB& b) {

	if ((a.min.x <= b.max.x && a.max.x >= b.min.x) &&//x軸
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z)
		) {
		return true;
	}
	return false;
}
//AABBと円
bool InCollision(const AABB& a, const Sphere& s, const Matrix4x4& vpMatrix, const Matrix4x4& viewportMatrix) {
	//最近接点を求める
	Vector3 closestPoint{ std::clamp(s.center.x,a.min.x,a.max.x),
	std::clamp(s.center.y,a.min.y,a.max.y),
	std::clamp(s.center.z,a.min.z,a.max.z)
	};
	Sphere S{
		.center = closestPoint,
		.radius = 0.01f,
	};
	DrawSphere(S, vpMatrix, viewportMatrix, WHITE);

	Vector3 V = Subtract(closestPoint, s.center);

	//最近接点と球の中心との距離を求める
	float dis = Length(V);
	//距離が半径よりも小さければ衝突
	if (dis <= s.radius) { return true; }
	return false;
}

float ParameterT(Plane P,Segment S) {
	//法線と線の内積
	float dot = Dot(P.normal, S.diff);
	if (dot == 0.0f) {
		return 0;
	}

	
	//tを求める
	float t = (P.distance - Dot(S.origin, P.normal)) / dot;

	return t;
}

bool InCollision(const AABB& AA, const Segment& S, const Matrix4x4& vpMatrix, const Matrix4x4& viewportMatrix) {

	//assert(S.diff == { 0,0,0 });

	//Xplaneと線の点
	Vector3 bx, by, bz;

	bx = { 1,0,0 };
	by = { 0,1,0 };
	bz = { 0,0,1 };
	/*
	float tXmin = ParameterT({ bx,AA.min.x }, S);
	float tXmax = ParameterT({ bx,AA.max.x }, S);

	float tYmin = ParameterT({ by,AA.min.y }, S);
	float tYmax = ParameterT({ by,AA.max.y }, S);

	float tZmin = ParameterT({ bz,AA.min.z }, S);
	float tZmax = ParameterT({ bz,AA.max.z }, S);


	Vector3 a1,a2, b1,b2, c1,c2;

	a1 = Add(S.origin, Multiply(tXmin, S.diff));
	a2 = Add(S.origin, Multiply(tXmax, S.diff));

	b1 = Add(S.origin, Multiply(tYmin, S.diff));
	b2 = Add(S.origin, Multiply(tYmax, S.diff));

	c1 = Add(S.origin, Multiply(tZmin, S.diff));
	c2 = Add(S.origin, Multiply(tZmax, S.diff));

	Vector3 LS = S.origin;
	Vector3 LE = Add(S.origin, S.diff);

	if (PointInLine(a1, LS, LE) || PointInLine(a2, LS, LE) || PointInLine(b1, LS, LE) || PointInLine(b2, LS, LE) || PointInLine(c1, LS, LE) || PointInLine(c2, LS, LE)) {
		return true;
	}
	*/

	
	//tを求める
	float tXmin = (AA.min.x - S.origin.x) / S.diff.x;
	float tYmin = (AA.min.y - S.origin.y) / S.diff.y;
	float tZmin = (AA.min.z - S.origin.z) / S.diff.z;

	float tXmax = (AA.max.x - S.origin.x) /S.diff.x;
	float tYmax = (AA.max.y - S.origin.y) / S.diff.y;
	float tZmax = (AA.max.z - S.origin.z) / S.diff.z;


	float tNearX = min(tXmin, tXmax);
	float tNearY = min(tYmin, tYmax);
	float tNearZ = min(tZmin, tZmax);

	float tFarX = max(tXmin, tXmax);
	float tFarY = max(tYmin, tYmax);
	float tFarZ = max(tZmin, tZmax);


	float tmin = max(max(tNearX, tNearY), tNearZ);
	float tmax = min(min(tFarX, tFarY), tFarZ);

	Vector3 a = Add(S.origin, Multiply(tmin, S.diff));
	Sphere SA = { .center = a,.radius = 0.01f, };
	DrawSphere(SA, vpMatrix, viewportMatrix, WHITE);

	Vector3 b = Add(S.origin, Multiply(tmax, S.diff));
	Sphere Sp = { .center = b,.radius = 0.01f, };
	DrawSphere(Sp, vpMatrix, viewportMatrix, WHITE);

	Vector3 LS, LE;
	LS = S.origin;
	LE = S.origin + S.diff;
	if (tmin <= tmax) {
		if (PointInLine(a, LS, LE) || PointInLine(b, LS, LE)) {
			//衝突
			return true;
		}
		
	}
	
	/*
	//最近接点
	Vector3 a = Add(S.origin, Multiply(t, S.diff));
	Sphere Sph = {
		.center{a},
		.radius = 0.01f,
	};
	DrawSphere(Sph, VP, Vport, WHITE);

	//線分内に点があるかチェック
	if (PointInLine(a, AA.min, AA.max)) {
		return true;//早期リターン
	}
	*/
	return false;
}


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};
	//マウス
	mouse m = {
		.now{0,0},
		.before{0,0}
	};
	WorldTransform camera = {
		.translation{0,1.9f,-6.49f},
		.rotation{0.26f,0,0},
		.scale{1,1,1}
	};
	//uint32_t color = WHITE;
	
#pragma region 授業
	Spring spring{};
	spring.anchor = { 0.0f,0.0f,0.0f };
	spring.naturalLength = 1.0f;
	spring.stiffness = 100.0f;
	spring.dampingCoefficient = 2.0f;

	Ball ball{};
	ball.position = { 1.2f,0.0f,0.0f };
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = BLUE;

	float deltaTime = 1.0f / 60.0f;
#pragma endregion

	

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		m.before = m.now;
		Novice::GetHitKeyStateAll(keys);
		Novice::GetMousePosition(&m.now.x, &m.now.y);

		CmeraMove(camera, keys,m);
		
		ImGui::Begin("Wnd");
		ImGui::DragFloat3("C trans", &camera.translation.x, 0.01f);
		ImGui::DragFloat3("C rotate", &camera.rotation.x, 0.01f);
		ImGui::End();

		
		///
		/// ↓更新処理ここから
		///
		//Matrix4x4 worldM = MakeAffineM({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 cameraM = MakeAffineM(camera.scale,camera.rotation,camera.translation);
		Matrix4x4 viewM = Inverse(cameraM);

		Matrix4x4 projectionM = MakePerspectiveFovM(0.45f,float(kWindowWidth)/float(kWindowHeight),0.1f,100.0f);
		//Matrix4x4 WVP =  Multiply(worldM,Multiply (viewM,projectionM));
		Matrix4x4 viewportM = MakeViewportM(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
		Matrix4x4 VP = Multiply(viewM, projectionM);
		
#pragma region jugyou
		ImGui::Begin("Window");
		

		ImGui::End();
#pragma endregion
		Vector3 diff = ball.position - spring.anchor;
		float length = Length(diff);
		if (length != 0.0f) {
			Vector3 direction = Normalize(diff);
			Vector3 restPosition = spring.anchor + direction * spring.naturalLength;
			Vector3 displacement = length * (ball.position - restPosition);
			Vector3 restoringForce = -spring.stiffness * displacement;
			//減衰抵抗を計算する
			Vector3 dampingForce = -spring.dampingCoefficient * ball.velocity;
			Vector3 force = restoringForce + dampingForce;
			ball.acceleration = force / ball.mass;
		}
		ball.velocity =Add(ball.velocity,ball.acceleration * deltaTime);
		ball.position =Add(ball.position, ball.velocity * deltaTime);
		
		Sphere s{
			.center = ball.position,
			.radius = ball.radius,
		};


		///
		/// ↑更新処理ここまで
		///
		DrawGrid(VP, viewportM);

		DrawSphere(s, VP, viewportM, ball.color);

		Vector3 st = Transform(Transform(spring.anchor, VP), viewportM);
		Vector3 ed = Transform(Transform(ball.position, VP), viewportM);

		Novice::DrawLine((int)st.x, (int)st.y, (int)ed.x, (int)ed.y, WHITE);
		///
		/// ↓描画処理ここから
		///
		

		
		
		
		

		
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
