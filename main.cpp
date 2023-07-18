#include <Novice.h>
#include<Vector3.h>
#include<Matrix_V3math.h>
#include<stdint.h>
#include<cmath>
#include<numbers>
#include<imgui.h>


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
	uint32_t color = WHITE;
	
#pragma region 授業
	Sphere S1{
		.center{0,0,0},
		.radius{0.5},
	};
	Sphere S2{
		.center{0,-2,0},
		.radius{0.5},
	};
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
		DebugSphere(S1,"S1");
		DebugSphere(S2, "S2");

		if (InCollision(S1, S2)) {
			color = RED;
		}
		else {
			color = WHITE;
		}

#pragma endregion

		///
		/// ↑更新処理ここまで
		///
		DrawGrid(VP, viewportM);
		///
		/// ↓描画処理ここから
		///
		
		DrawSphere(S1, VP, viewportM, color);
		DrawSphere(S2, VP, viewportM, color);

		
		
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
