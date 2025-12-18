// 独自計算ライブラリ
#include "mymath.h"
#include <cmath>
// radian→degree
float DegToRad(float deg) {
    return deg * PI / 180.0f;		// 角度をdegreeからradianに変換
}

// degree→radian
float RadToDeg(float rad) {
    return rad * 180.0f / PI;		// 角度をradianからdegreeに変換
}



// 当たり判定用。2つのboxが当たったかを判定
// 当たっていたら1, 当たっていなかったら0を返す
int IsHitBox(
    int x1, int y1, int w1, int h1,		// ひとつめのbox 左上(x,y), 大きさw,h
    int x2, int y2, int w2, int h2		// ふたつめのbox 左上(x,y), 大きさw,h
) {
    if(x1 < x2 + w2 && x2 < x1 + w1		// x方向の判定
        && y1 < y2 + h2 && y2 < y1 + h1		// y方向の判定
        && w1 != 0 && h1 != 0           // box1の大きさがある
        && w2 != 0 && h2 != 0           // box2の大きさがある
        )
    {
        // 2つのboxは当たっている
        return 1;
    }

    // 2つのboxは当たっていない
    return 0;
}


// 当たり判定用。2つの円が当たったかを判定
// 当たっていたら1, 当たっていなかったら0を返す
int IsHitCircle(
    int x1, int y1, int r1,		// ひとつめのcircle 中心(x,y), 半径r
    int x2, int y2, int r2		// ふたつめのcircle 中心(x,y), 半径r
) {
    int w, h, r;
    w = x1 - x2;
    h = y1 - y2;
    r = r1 + r2;
    if(r * r > w * w + h * h) {
        return 1;
    }
    return 0;
}

// ベクトル関連関数

// ベクトルの正規化
VECTOR VecNormalize(const VECTOR& v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len <= 1e-6f) return VGet(0.0f, 0.0f, 0.0f);
    return VGet(v.x / len, v.y / len, v.z / len);
}

// ベクトルとスカラーの乗算
VECTOR VecScale(const VECTOR& v, float s)
{
    return VGet(v.x * s, v.y * s, v.z * s);
}

// ベクトルの加算
VECTOR VecAdd(const VECTOR& a, const VECTOR& b)
{
    return VGet(a.x + b.x, a.y + b.y, a.z + b.z);
}

// ベクトルの減算
VECTOR VecSub(const VECTOR& a, const VECTOR& b)
{
    return VGet(a.x - b.x, a.y - b.y, a.z - b.z);
}

// ベクトルの外積
VECTOR VecCross(const VECTOR& a, const VECTOR& b)
{
    return VGet(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}





#define TwoPI   (PI * 2.0f)
#define PIOver2 (PI / 2.0f)

// イージング
float EasingLinear(float cnt, float start, float end, float frames) {
    return (end - start) * cnt / frames + start;
}
float EasingInQuad(float cnt, float start, float end, float frames) {
    cnt /= frames;
    return (end - start) * cnt * cnt + start;
}
float EasingOutQuad(float cnt, float start, float end, float frames) {
    cnt /= frames;
    return -(end - start) * cnt * (cnt - 2) + start;
}
float EasingInOutQuad(float cnt, float start, float end, float frames) {
    cnt /= frames / 2.0;
    if(cnt < 1) {
        return (end - start) / 2.0 * cnt * cnt + start;
    }
    cnt--;
    return -(end - start) / 2.0 * (cnt * (cnt - 2) - 1) + start;
}
float EasingInCubic(float cnt, float start, float end, float frames) {
    cnt /= frames;
    return (end - start) * cnt * cnt * cnt + start;
}
float EasingOutCubic(float cnt, float start, float end, float frames) {
    cnt /= frames;
    cnt--;
    return (end - start) * (cnt * cnt * cnt + 1) + start;
}
float EasingInOutCubic(float cnt, float start, float end, float frames) {
    cnt /= frames / 2.0;
    if(cnt < 1) {
        return (end - start) / 2.0 * cnt * cnt * cnt + start;
    }
    cnt -= 2;
    return (end - start) / 2.0 * (cnt * cnt * cnt + 2) + start;
}
float EasingInQuart(float cnt, float start, float end, float frames) {
    cnt /= frames;
    return (end - start) * cnt * cnt * cnt * cnt + start;
}
float EasingOutQuart(float cnt, float start, float end, float frames) {
    cnt /= frames;
    cnt--;
    return -(end - start) * (cnt * cnt * cnt * cnt - 1) + start;
}
float EasingInOutQuart(float cnt, float start, float end, float frames) {
    cnt /= frames / 2.0;
    if(cnt < 1) {
        return (end - start) / 2.0 * cnt * cnt * cnt * cnt + start;
    }
    cnt -= 2;
    return -(end - start) / 2.0 * (cnt * cnt * cnt * cnt - 2) + start;
}
float EasingInQuint(float cnt, float start, float end, float frames) {
    cnt /= frames;
    return (end - start) * cnt * cnt * cnt * cnt * cnt + start;
}
float EasingOutQuint(float cnt, float start, float end, float frames) {
    cnt /= frames;
    cnt--;
    return (end - start) * (cnt * cnt * cnt * cnt * cnt + 1) + start;
}
float EasingInOutQuint(float cnt, float start, float end, float frames) {
    cnt /= frames / 2.0;
    if(cnt < 1) {
        return (end - start) / 2.0 * cnt * cnt * cnt * cnt * cnt + start;
    }
    cnt -= 2;
    return (end - start) / 2.0 * (cnt * cnt * cnt * cnt * cnt + 2) + start;
}
float EasingInSine(float cnt, float start, float end, float frames) {
    return -(end - start) * cos(cnt / frames * (PIOver2)) + end + start;
}
float EasingOutSine(float cnt, float start, float end, float frames) {
    return (end - start) * sin(cnt / frames * PIOver2) + start;
}
float EasingInOutSine(float cnt, float start, float end, float frames) {
    return -(end - start) / 2.0 * (cos(PI * cnt / frames) - 1) + start;
}
float EasingInExpo(float cnt, float start, float end, float frames) {
    return (end - start) * pow(2.0, 10 * (cnt / frames - 1)) + start;
}
float EasingOutExpo(float cnt, float start, float end, float frames) {
    return (end - start) * (-pow(2.0, -10 * cnt / frames) + 1) + start;
}
float EasingInOutExpo(float cnt, float start, float end, float frames) {
    cnt /= frames / 2.0;
    if(cnt < 1) {
        return (end - start) / 2.0 * pow(2.0, 10 * (cnt - 1)) + start;
    }
    cnt--;
    return (end - start) / 2.0 * (-pow(2.0, -10 * cnt) + 2) + start;
}
float EasingInCirc(float cnt, float start, float end, float frames) {
    cnt /= frames;
    return -(end - start) * (sqrt(1 - cnt * cnt) - 1) + start;
}
float EasingOutCirc(float cnt, float start, float end, float frames) {
    cnt /= frames;
    cnt--;
    return (end - start) * sqrt(1 - cnt * cnt) + start;
}
float EasingInOutCirc(float cnt, float start, float end, float frames) {
    cnt /= frames / 2.0;
    if(cnt < 1) {
        return -(end - start) / 2.0 * (sqrt(1 - cnt * cnt) - 1) + start;
    }
    cnt -= 2;
    return (end - start) / 2.0 * (sqrt(1 - cnt * cnt) + 1) + start;
}


// 範囲内を返す
float Clamp(float min_n, float max_n, float n) {
    if(n <= min_n) { return min_n; }
    if(n >= max_n) { return max_n; }
    return n;
}
