#include <Novice.h>
#include<Vector3.h>
#include<Matrix_V3math.h>

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


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 scale{ 1.2f,0.79f,-2.1f };
	Vector3 ratate{ 0.4f,1.43f,-0.8f };
	Vector3 translate{ 2.7f,-4.15f,1.57f };
	Matrix4x4 worldM = MakeAffineM(scale, ratate, translate);

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		MatrixScreenPrintf(0, 0, worldM, "World");

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
