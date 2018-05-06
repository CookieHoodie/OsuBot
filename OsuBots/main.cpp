#include <iostream>
#include <string>
#include <unordered_map>
#include <bitset>

#include "ProcessTools.h"
#include "SigScanner.h"
#include "OsuBot.h"
#include "Beatmap.h"
#include "Input.h"
#include "OsuDbParser.h"

using namespace std;

//const string folderPath = "C:\\Users\\ong\\AppData\\Local\\osu!\\Songs\\";
//const string fileName = "solfa feat. Shimotsuki Haruka - Natsuiro Ramune (Lasse) [Hard].osu";
//const string fileName = "MOSAIC.WAV - Gokusotu Angel Ennmachan (Nattu) [Ennmachan!].osu";
//const string fileName = "Knife Party & Tom Morello - Battle Sirens (RIOT Remix) (Marmowka) [Extra].osu";
//const string fileName = "YUC'e - Future Candy (Nathan) [Sugar Rush].osu";
//const string fileName = "I SEE MONSTAS - Holdin On (Skrillex and Nero Remix) (Sotarks) [Hobbes2's Extreme].osu";
//const string fileName = "Sota Fujimori - polygon (Kaifin) [Lolirii's Collab Expert].osu";
//const string fileName = "xi - Glorious Crown (Monstrata) [FOUR DIMENSIONS].osu";
//const string fileName = "Panda Eyes - ILY (M a r v o l l o) [Fanteer's Final Level].osu";
const string fileName = "Camellia - Feelin Sky (Camellia's 200step Self-remix) (Smoothie World) [Zero Gravity].osu";


//var bx = 0, by = 0, n = pointArray.length - 1; // degree
//
//if (n == 1) { // if linear
//	bx = (1 - t) * pointArray[0].x + t * pointArray[1].x;
//	by = (1 - t) * pointArray[0].y + t * pointArray[1].y;
//}
//else if (n == 2) { // if quadratic
//	bx = (1 - t) * (1 - t) * pointArray[0].x + 2 * (1 - t) * t * pointArray[1].x + t * t * pointArray[2].x;
//	by = (1 - t) * (1 - t) * pointArray[0].y + 2 * (1 - t) * t * pointArray[1].y + t * t * pointArray[2].y;
//}
//else if (n == 3) { // if cubic
//	bx = (1 - t) * (1 - t) * (1 - t) * pointArray[0].x + 3 * (1 - t) * (1 - t) * t * pointArray[1].x + 3 * (1 - t) * t * t * pointArray[2].x + t * t * t * pointArray[3].x;
//	by = (1 - t) * (1 - t) * (1 - t) * pointArray[0].y + 3 * (1 - t) * (1 - t) * t * pointArray[1].y + 3 * (1 - t) * t * t * pointArray[2].y + t * t * t * pointArray[3].y;
//}
//else { // generalized equation
//	for (var i = 0; i <= n; i++) {
//		bx += this.binomialCoef(n, i) * Math.pow(1 - t, n - i) * Math.pow(t, i) * pointArray[i].x;
//		by += this.binomialCoef(n, i) * Math.pow(1 - t, n - i) * Math.pow(t, i) * pointArray[i].y;
//	}
//}
//
//return{ x: bx, y : by }
//	}

//binomialCoef: function(n, k) {
//var r = 1;
//
//if (k > n)
//return 0;
//
//for (var d = 1; d <= k; d++) {
//	r *= n--;
//	r /= d;
//}
//
//return r;
//}

//double binomialCoef(int n, int k) {
//	double r = 1;
//	if (k > n) {
//		return 0;
//	}
//	for (int d = 1; d <= k; d++) {
//		r *= n--;
//		r /= d;
//	}
//	return r;
//}

//POINT bezierCurve(vector<CurvePointsS> curvePoints, float t) {
//	double bx = 0;
//	double by = 0;
//	int n = curvePoints.size() - 1;
//	/*if (n == 1) {
//		bx = (1 - t) * curvePoints.at(0).x + t * curvePoints.at(1).x;
//		by = (1 - t) * curvePoints.at(0).y + t * curvePoints.at(1).y;
//	}
//	else if (n == 2) {
//		bx = (1 - t) * (1 - t) * curvePoints.at(0).x + 2 * (1 - t) * t * curvePoints.at(1).x + t * t * curvePoints.at(2).x;
//		by = (1 - t) * (1 - t) * curvePoints.at(0).y + 2 * (1 - t) * t * curvePoints.at(1).y + t * t * curvePoints.at(2).y;
//	}
//	else if (n == 3) {
//		bx = (1 - t) * (1 - t) * (1 - t) * curvePoints.at(0).x + 3 * (1 - t) * (1 - t) * t * curvePoints.at(1).x + 3 * (1 - t) * t * t * curvePoints.at(2).x + t * t * t * curvePoints.at(3).x;
//		by = (1 - t) * (1 - t) * (1 - t) * curvePoints.at(0).y + 3 * (1 - t) * (1 - t) * t * curvePoints.at(1).y + 3 * (1 - t) * t * t * curvePoints.at(2).y + t * t * t * curvePoints.at(3).y;
//	}
//	else {*/
//		for (int i = 0; i <= n; i++) {
//			bx += binomialCoef(n, i) * pow(1 - t, n - i) * pow(t, i) * curvePoints.at(i).x;
//			by += binomialCoef(n, i) * pow(1 - t, n - i) * pow(t, i) * curvePoints.at(i).y;
//		}
//	//}
//	POINT p;
//	p.x = bx;
//	p.y = by;
//	return p;
//}
//else { // generalized equation
//	for (var i = 0; i <= n; i++) {
//		bx += this.binomialCoef(n, i) * Math.pow(1 - t, n - i) * Math.pow(t, i) * pointArray[i].x;
//		by += this.binomialCoef(n, i) * Math.pow(1 - t, n - i) * Math.pow(t, i) * pointArray[i].y;
//	}

POINT scalePoints(int x, int y) {
	POINT p;
	p.x = x * 1.60156250 + 274;
	p.y = y * 1.60208333 + 88;
	return p;
}

class Vector2
{
public:
	//MEMBERS
	float x;
	float y;

	//CONSTRUCTORS
	Vector2(void) : x(0), y(0) { }
	Vector2(float xValue, float yValue) : x(xValue), y(yValue) { }

	//DECONSTRUCTOR
	~Vector2(void) { };

	//METHODS

	float Length() const { return sqrt(x * x + y * y); }
	float LengthSquared() const { return x * x + y * y; }
	/*float Distance(const Vector2 & v) const;
	float DistanceSquared(const Vector2 & v) const;
	float Dot(const Vector2 & v) const;
	float Cross(const Vector2 & v) const;

	Vector2 & Normal();
	Vector2 & Normalize();*/

	//ASSINGMENT AND EQUALITY OPERATIONS
	inline Vector2 & Vector2::operator = (const Vector2 & v) { x = v.x; y = v.y; return *this; }
	inline Vector2 & Vector2::operator = (const float & f) { x = f; y = f; return *this; }
	inline Vector2 & Vector2::operator - (void) { x = -x; y = -y; return *this; }
	inline bool Vector2::operator == (const Vector2 & v) const { return (x == v.x) && (y == v.y); }
	inline bool Vector2::operator != (const Vector2 & v) const { return (x != v.x) || (y != v.y); }

	//VECTOR2 TO VECTOR2 OPERATIONS
	inline const Vector2 Vector2::operator + (const Vector2 & v) const { return Vector2(x + v.x, y + v.y); }
	inline const Vector2 Vector2::operator - (const Vector2 & v) const { return Vector2(x - v.x, y - v.y); }
	inline const Vector2 Vector2::operator * (const Vector2 & v) const { return Vector2(x * v.x, y * v.y); }
	inline const Vector2 Vector2::operator / (const Vector2 & v) const { return Vector2(x / v.x, y / v.y); }

	//VECTOR2 TO THIS OPERATIONS
	inline Vector2 & Vector2::operator += (const Vector2 & v) { x += v.x; y += v.y; return *this; }
	inline Vector2 & Vector2::operator -= (const Vector2 & v) { x -= v.x; y -= v.y; return *this; }
	inline Vector2 & Vector2::operator *= (const Vector2 & v) { x *= v.x; y *= v.y; return *this; }
	inline Vector2 & Vector2::operator /= (const Vector2 & v) { x /= v.x; y /= v.y; return *this; }

	//SCALER TO VECTOR2 OPERATIONS
	inline const Vector2 Vector2::operator + (float v) const { return Vector2(x + v, y + v); }
	inline const Vector2 Vector2::operator - (float v) const { return Vector2(x - v, y - v); }
	inline const Vector2 Vector2::operator * (float v) const { return Vector2(x * v, y * v); }
	inline const Vector2 Vector2::operator / (float v) const { return Vector2(x / v, y / v); }

	//SCALER TO THIS OPERATIONS
	inline Vector2 & Vector2::operator += (float v) { x += v; y += v; return *this; }
	inline Vector2 & Vector2::operator -= (float v) { x -= v; y -= v; return *this; }
	inline Vector2 & Vector2::operator *= (float v) { x *= v; y *= v; return *this; }
	inline Vector2 & Vector2::operator /= (float v) { x /= v; y /= v; return *this; }
};

vector<Vector2> subdivisionBuffer1;
vector<Vector2> subdivisionBuffer2;
const float tolerance = 0.25f;
const float tolerance_sq = tolerance * tolerance;

bool isFlatEnough(vector<Vector2> controlPoints)
{
	for (int i = 1; i < controlPoints.size() - 1; i++)
		if ((controlPoints.at(i-1) - controlPoints.at(i) * 2.0f + controlPoints.at(i+1)).LengthSquared() > tolerance_sq * 4)
			return false;

	return true;
}

void subdivide(vector<Vector2> controlPoints, vector<Vector2> &l, vector<Vector2> &r, int count) {
	vector<Vector2> midPoints = subdivisionBuffer1;
	//int count = controlPoints.size();
	for (int i = 0; i < count; ++i) {
		midPoints[i] = controlPoints.at(i);
	}

	for (int i = 0; i < count; i++) {
		l[i] = midPoints.at(0);
		r[count - i - 1] = midPoints[count - i - 1];

		for (int j = 0; j < count - i - 1; j++) {
			midPoints.at(j) = (midPoints.at(j) + midPoints.at(j + 1)) / 2;
		}
	}
}

void approximate(vector<Vector2> controlPoints, vector<Vector2> &output, int count) {
	vector<Vector2> l = subdivisionBuffer2;
	vector<Vector2> r = subdivisionBuffer1;
	//int count = controlPoints.size();
	subdivide(controlPoints, l, r, count);
	for (int i = 0; i < count - 1; ++i) {
		l.at(count + 1) = r.at(i + 1);
	}

	output.push_back(controlPoints.at(0));
	for (int i = 1; i < count - 1; ++i) {
		int index = 2 * i;
		Vector2 p = (l.at(index - 1) + l.at(index) * 2 + l.at(index + 1)) * 0.25f;
		output.push_back(p);
	}
}



int main() {
	POINT a1; a1.x = 173; a1.y = 353;
	POINT a2; a2.x = 129; a2.y=268;
	POINT a3; a3.x = 153; a3.y= 186;
	POINT a4; a4.x = 228; a4.y= 171;
	POINT a5; a5.x = 282;a5.y= 186;
	
	POINT b1; b1.x = 282; b1.y = 196;
	POINT b2; b2.x = 218; b2.y = 121;
	POINT b3; b3.x = 93; b3.y = 174;
	POINT b4; b4.x = 47; b4.y = 68;
	POINT b5; b5.x = 75; b5.y = -21;
	POINT b6 ; b6.x = 210; b6.y = -27;
	POINT b7; b7.x = 291; b7.y = 70;
	POINT b8 ; b8.x = 181; b8.y = 139;
	POINT b9; b9.x = 78; b9.y = 157;

	//vector<Vector2> subControlPoints;
	//vector<vector<Vector2>> controlPoints; // vector<Vector2>, not this
	//subControlPoints.push_back(Vector2(a1.x, a1.y));
	//subControlPoints.push_back(Vector2(a2.x, a2.y));
	//subControlPoints.push_back(Vector2(a3.x, a3.y));
	//subControlPoints.push_back(Vector2(a4.x, a4.y));
	//controlPoints.push_back(subControlPoints);
	//subControlPoints.clear();
	//subControlPoints.push_back(Vector2(a5.x, a5.y));
	//subControlPoints.push_back(Vector2(b1.x, b1.y));
	//subControlPoints.push_back(Vector2(b2.x, b2.y));
	//subControlPoints.push_back(Vector2(b3.x, b3.y));
	//subControlPoints.push_back(Vector2(b4.x, b4.y));
	//subControlPoints.push_back(Vector2(b5.x, b5.y));
	//subControlPoints.push_back(Vector2(b6.x, b6.y));
	//subControlPoints.push_back(Vector2(b7.x, b7.y));
	//subControlPoints.push_back(Vector2(b8.x, b8.y));
	//subControlPoints.push_back(Vector2(b9.x, b9.y));
	//controlPoints.push_back(subControlPoints);
	
	
	//vector<Vector2> controlPoints; // vector<Vector2>, not this
	//controlPoints.push_back(Vector2(a1.x, a1.y));
	//controlPoints.push_back(Vector2(a2.x, a2.y));
	//controlPoints.push_back(Vector2(a3.x, a3.y));
	//controlPoints.push_back(Vector2(a4.x, a4.y));
	//controlPoints.push_back(Vector2(a5.x, a5.y));
	//controlPoints.push_back(Vector2(b1.x, b1.y));
	//controlPoints.push_back(Vector2(b2.x, b2.y));
	//controlPoints.push_back(Vector2(b3.x, b3.y));
	//controlPoints.push_back(Vector2(b4.x, b4.y));
	//controlPoints.push_back(Vector2(b5.x, b5.y));
	//controlPoints.push_back(Vector2(b6.x, b6.y));
	//controlPoints.push_back(Vector2(b7.x, b7.y));
	//controlPoints.push_back(Vector2(b8.x, b8.y));
	//controlPoints.push_back(Vector2(b9.x, b9.y));
	//int count = controlPoints.size();
	//subdivisionBuffer1.resize(count);
	//subdivisionBuffer2.resize(count * 2 - 1);
	//try {
	//	
	//	vector<Vector2> output;
	//	
	//	if (count != 0) {
	//		vector<vector<Vector2>> toFlatten;
	//		vector<vector<Vector2>> freeBuffers;
	//		toFlatten.push_back(controlPoints);
	//		vector<Vector2> leftChild = subdivisionBuffer2;
	//		while (!toFlatten.empty()) {
	//			vector<Vector2> parent = toFlatten.back();
	//			toFlatten.pop_back();
	//			if (isFlatEnough(parent)) {
	//				approximate(parent, output, count);
	//				freeBuffers.push_back(parent);
	//				continue;
	//			}
	//			vector<Vector2> rightChild;
	//			if (freeBuffers.size() > 0) {
	//				rightChild = freeBuffers.back();
	//				freeBuffers.pop_back();
	//			}
	//			else {
	//				rightChild = vector<Vector2>(count);
	//			}
	//			subdivide(parent, leftChild, rightChild, count);
	//			for (int i = 0; i < count; ++i)
	//				parent.at(i) = leftChild.at(i);

	//			toFlatten.push_back(rightChild);
	//			toFlatten.push_back(parent);
	//		}
	//		output.push_back(controlPoints.at(count - 1));
	//	}

	//	for (auto v : output) {
	//		cout << v.x << ", " << v.y << endl;
	//		POINT p = scalePoints(v.x, v.y);
	//		SetCursorPos(p.x, p.y);
	//		Sleep(100);
	//	}
	//	
	//}
	//catch (const std::runtime_error& re)
	//{
	//	// speciffic handling for runtime_error
	//	std::cerr << "Runtime error: " << re.what() << std::endl;
	//}
	//catch (const std::exception& ex)
	//{
	//	// speciffic handling for all exceptions extending std::exception, except
	//	// std::runtime_error which is handled explicitly
	//	std::cerr << "Error occurred: " << ex.what() << std::endl;
	//}
	//catch (...)
	//{
	//	// catch any other errors (that we have no information about)
	//	std::cerr << "Unknown failure occurred. Possible memory corruption" << std::endl;
	//}

















	wchar_t* processName = L"osu!.exe";
	//POINT pq; 
	//pq.x = 274; pq.y = 232;
	//POINT pw; 
	//pw.x = 315; pw.y = 317;
	//POINT pe; 
	//pe.x = 425; pe.y = 286;
	///*pq = scalePoints(274, 232);
	//pw = scalePoints(315, 317);
	//pe = scalePoints(425, 286);*/
	//auto q = CurvePointsS(pq.x, pq.y);
	//auto w = CurvePointsS(pw.x, pw.y);
	//auto e = CurvePointsS(pe.x, pe.y);

	//POINT pa;
	//POINT pb;
	//POINT pc;
	//POINT pd;
	//POINT pe;
	//pa.x = 85; pa.y = 298;
	//pb.x = 43; pb.y = 248;
	//pc.x = 66; pc.y = 173;
	//pd.x = 74; pd.y = 214;
	//pe.x = 106; pe.y = 258;
	////auto q = CurvePointsS(pa.x, pa.y);
	//auto w = CurvePointsS(pb.x, pb.y);
	//auto e = CurvePointsS(pc.x, pc.y);
	//auto r = CurvePointsS(pd.x, pd.y);
	////auto t = CurvePointsS(pe.x, pe.y);

	//vector<CurvePointsS> v;
	////v.push_back(q);v.push_back(w);v.push_back(e);
	////v.push_back(q);
	//v.push_back(w); v.push_back(e); v.push_back(r); 
	////v.push_back(t); 
	////for (int i = 0; i < 3; i++) {
	////	for (float t = 0; t <= 1; t += 0.05) {
	////		POINT p = bezierCurve(v, t);
	////		POINT c = scalePoints(p.x, p.y);
	////		//POINT c = p;
	////		SetCursorPos(c.x, c.y);
	////		Sleep(100);
	////	}
	////}
	//Sleep(2000);
	//POINT c = scalePoints(pa.x, pa.y);
	//SetCursorPos(c.x, c.y);
	//Sleep(1000);
	// c = scalePoints(pb.x, pb.y);
	//SetCursorPos(c.x, c.y); Sleep(1000);
	// c = scalePoints(pc.x, pc.y);
	//SetCursorPos(c.x, c.y); Sleep(1000);
	// c = scalePoints(pd.x, pd.y);
	//SetCursorPos(c.x, c.y); Sleep(1000);
	// c = scalePoints(pe.x, pe.y);
	//SetCursorPos(c.x, c.y); Sleep(1000);
	
	//173:353 | 129 : 268 | 153 : 186 | 228 : 171 | 282 : 186 | 282 : 186 | 218 : 121 | 93 : 174 | 47 : 68 | 75 : -21 | 210 : -27 | 291 : 70 | 181 : 139 | 78 : 157
	//vector<vector<CurvePointsS>> externalV;
	//vector<CurvePointsS> curvePointsV;
	///*POINT a1 = scalePoints(173, 353);
	//POINT a2 = scalePoints(129, 268);
	//POINT a3 = scalePoints(153, 186);
	//POINT a4 = scalePoints(228, 171);
	//POINT a5 = scalePoints(282, 186);*/
	//POINT a1; a1.x = 173; a1.y = 353;
	//POINT a2; a2.x = 129; a2.y=268;
	//POINT a3; a3.x = 153; a3.y= 186;
	//POINT a4; a4.x = 228; a4.y= 171;
	//POINT a5; a5.x = 282;a5.y= 186;
	//curvePointsV.push_back(CurvePointsS(a1.x, a1.y));
	//curvePointsV.push_back(CurvePointsS(a2.x, a2.y));
	//curvePointsV.push_back(CurvePointsS(a3.x, a3.y));
	//curvePointsV.push_back(CurvePointsS(a4.x, a4.y));
	//curvePointsV.push_back(CurvePointsS(a5.x, a5.y));
	//externalV.push_back(curvePointsV);
	//curvePointsV.clear();
	//
	///*POINT b1 = scalePoints(282, 186);
	//POINT b2 = scalePoints(218, 121);
	//POINT b3 = scalePoints(93, 174);
	//POINT b4 = scalePoints(47, 68);
	//POINT b5 = scalePoints(75, -21);
	//POINT b6 = scalePoints(210, -27);
	//POINT b7 = scalePoints(291, 70);
	//POINT b8 = scalePoints(181, 139);
	//POINT b9 = scalePoints(78, 157);*/
	//POINT b1; b1.x = 282; b1.y = 196;
	//POINT b2; b2.x = 218; b2.y = 121;
	//POINT b3; b3.x = 93; b3.y = 174;
	//POINT b4; b4.x = 47; b4.y = 68;
	//POINT b5; b5.x = 75; b5.y = -21;
	//POINT b6 ; b6.x = 210; b6.y = -27;
	//POINT b7; b7.x = 291; b7.y = 70;
	//POINT b8 ; b8.x = 181; b8.y = 139;
	//POINT b9; b9.x = 78; b9.y = 157;
	//curvePointsV.push_back(CurvePointsS(b1.x, b1.y));
	//curvePointsV.push_back(CurvePointsS(b2.x, b2.y));
	//curvePointsV.push_back(CurvePointsS(b3.x, b3.y));
	//curvePointsV.push_back(CurvePointsS(b4.x, b4.y));
	//curvePointsV.push_back(CurvePointsS(b5.x, b5.y));
	//curvePointsV.push_back(CurvePointsS(b6.x, b6.y));
	//curvePointsV.push_back(CurvePointsS(b7.x, b7.y));
	//curvePointsV.push_back(CurvePointsS(b8.x, b8.y));
	//curvePointsV.push_back(CurvePointsS(b9.x, b9.y));
	//externalV.push_back(curvePointsV);
	//curvePointsV.clear();

	//// TODO: account for t of connected curves
	//int repeat = 1;
	//bool reverse = false;
	///*float pixelLength = 750;
	//float sliderMultiplier = 2.0;
	//int beatDuration = 300;
	//int duration = pixelLength / (100.0 * sliderMultiplier) * beatDuration * repeat;
	//int nanosecPerDistance = (duration * 1000000) / (pixelLength * externalV.size() );*/
	//
	//double totalDistance = 0;
	//vector<POINT> T;
	//vector<double> D;
	//for (auto vec : externalV) {
	//	for (float t = 0; t <= 1; t += 0.005) {
	//		POINT p = Input::bezierCurve(vec, t);
	//		T.push_back(p);
	//		if (T.size() > 1) {
	//			auto lastPoint = T.at(T.size() - 2);
	//			auto distance = sqrt(pow(p.y - lastPoint.y, 2) + pow(p.x - lastPoint.x, 2));
	//			D.push_back(distance);
	//			totalDistance += distance;
	//			//cout << distance << endl;
	//		}
	//	}
	//}
	//cout << totalDistance << endl;
	//vector<POINT> N;
	//
	//for (int i = 0; i < D.size(); i++) {
	//	auto d = D.at(i);
	//	while (d >= 1) {
	//		auto last = T.at(i);
	//		auto now = T.at(i + 1);
	//		POINT v;
	//		v.x = now.x - last.x;
	//		v.y = now.y - last.y;
	//		POINT multi;
	//		multi.x = v.x / (sqrt(v.x * v.x + v.y * v.y));
	//		multi.y = v.y / (sqrt(v.x * v.x + v.y * v.y));
	//		POINT newN;
	//		newN.x = last.x + multi.x;
	//		newN.y = last.y + multi.y;
	//		N.push_back(newN);
	//		d--;
	//	}
	//}
	//		
	//	
	//

	///*for (int i = 0; i < repeat; i++) {
	//	if (!reverse) {
	//		for (int j = 0; j < externalV.size(); j++) {
	//			auto v = externalV.at(j);
	//			for (float t = 0; t <= 1; t += 0.05) {
	//				POINT p = Input::bezierCurve(v, t);
	//				SetCursorPos(p.x, p.y);
	//				Sleep(100);
	//			}
	//		}
	//		reverse = true;
	//	}
	//	else {
	//		for (int j = externalV.size(); j-- > 0;) {
	//			auto v = externalV.at(j);
	//			for (float t = 1; t >= 0; t -= 0.05) {
	//				POINT p = Input::bezierCurve(v, t);
	//				SetCursorPos(p.x, p.y);
	//				Sleep(100);
	//			}
	//		}
	//		reverse = false;
	//	}
	//}*/
	//
	//
	//int duration = 3000;
	//auto nanoSec = duration * 1000000 / N.size();
	//for (auto n : N) {
	//	auto t_start = chrono::high_resolution_clock::now();
	//	n = scalePoints(n.x, n.y);
	//	SetCursorPos(n.x, n.y);
	//	while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanoSec) {}
	//}



	//DWORD processID = ProcessTools::getProcessID(processName);
	//if (processID != NULL) {
	//	//char* pattern = "\xDB\x5D\xE8\x8B\x45\xE8\xA3";
	//	// pattern to find currentAudioTimeInstance
	//	/*const unsigned char pattern[] = { 0x75, 0x26, 0xDD, 0x05 };
	//	const char* mask = "xxxx";
	//	const int offset = -5; 
	//	
	//	HANDLE osuHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processID);
	//	DWORD ad = SigScanner::findPattern(osuHandle, pattern, mask, offset);
	//	cout << ad << endl;
	//	DWORD address;
	//	ReadProcessMemory(osuHandle, (LPCVOID)ad, &address, sizeof DWORD, nullptr);
	//	cout << address << endl;*/
	//	// address to instruction that writes to the currentAudioTime
	//	//DWORD currentAudioTimeInstructionAddress = SigScanner::findPattern(osuHandle, pattern, mask, offset);
	//	//if (currentAudioTimeInstructionAddress != NULL) {
	//	//	// address that stores the currentAudioTime value
	//	//	DWORD currentAudioTimeAddress = NULL;
	//	//	ReadProcessMemory(osuHandle, (LPCVOID)currentAudioTimeInstructionAddress, &currentAudioTimeAddress, sizeof DWORD, nullptr);
	//	//	if (currentAudioTimeAddress != NULL) {
	//	//		// value of currentAudioTime from the previous address
	//	//		// if failed, value = -1
	//	//		int currentAudioTime = -1;
	//	//		ReadProcessMemory(osuHandle, (LPCVOID)currentAudioTimeAddress, &currentAudioTimeAddress, sizeof(int), nullptr);
	//	//		std::cout << currentAudioTimeAddress << std::endl;
	//	//	}
	//	//}
	//	//CloseHandle(osuHandle);
	//	
	//	HWND windowHandle = ProcessTools::getWindowHandle(processID);
	//	/*string title = ProcessTools::getWindowTextString(windowHandle);
	//	while (true) {
	//		if (title != "osu!") {
	//			cout << title << endl;
	//			string new_title = title.substr(title.find_first_of('-') + 2);
	//			cout << new_title << endl;
	//			string osuTitle = new_title + ".osu";
	//			cout << osuTitle << endl;
	//		}
	//		title = ProcessTools::getWindowTextString(windowHandle);
	//		Sleep(1000);
	//	}*/
	//	

	//	//while (true) {
	//	//	GetCursorPos(&p);
	//	//	cout << "Screen coordinates: "<<p.x << ", " << p.y << endl;
	//	//	ScreenToClient(windowHandle, &p); // position of cursor relative to the window
	//	//	cout <<"Coordinates relative to the window: "<< p.x << ", " << p.y << endl;
	//	//	Sleep(500);
	//	//}
	//	
	//}
	//else {
	//	std::cout << "Failed to get processID" << std::endl;
	//}
	try {
		OsuBot bot = OsuBot(processName);
		//bot.loadBeatmap(fileName);
		bot.start();
	}
	catch (OsuBotException e) {
		cerr << "OsuBotException: " << e.what() << endl;
	}
	catch (const std::runtime_error& re)
	{
		// speciffic handling for runtime_error
		std::cerr << "Runtime error: " << re.what() << std::endl;
	}
	catch (const std::exception& ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		std::cerr << "Error occurred: " << ex.what() << std::endl;
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		std::cerr << "Unknown failure occurred. Possible memory corruption" << std::endl;
	}

	// TODO: FindWindowA that has osu! inside;

	//Beatmap test = Beatmap(fileName);
	//if (test.allSet) {
	//	/*cout << test.General.audioLeadIn << endl;
	//	cout << test.General.stackLeniency << endl << endl;

	//	cout << test.Difficulty.hpDrainRate << endl;
	//	cout << test.Difficulty.circleSize << endl;
	//	cout << test.Difficulty.overallDifficulty << endl;
	//	cout << test.Difficulty.approachRate << endl;
	//	cout << test.Difficulty.sliderMultiplier << endl;
	//	cout << test.Difficulty.sliderTickRate << endl << endl;*/

	//	/*for (auto timingPoint : test.TimingPoints) {
	//		cout << timingPoint.offset << ", " << timingPoint.relativeMPB << ", "<< timingPoint.realMPB << ", " << timingPoint.meter << endl;
	//	}*/

	//	//for (auto hitObject : test.HitObjects) {
	//	//	//cout << hitObject.x << ',' << hitObject.y << endl;

	//	//	switch (hitObject.type) {
	//	//	case HitObject::TypeE::slider: {
	//	//		cout << hitObject.sliderType << '|';
	//	//		for (auto curvePointsV : hitObject.CurvePoints) {
	//	//			for (auto point : curvePointsV) {
	//	//				cout << point.x << ':' << point.y << '|';
	//	//			}
	//	//		}
	//	//		cout << hitObject.repeat << ',';
	//	//		cout << hitObject.pixelLength << ", "<< hitObject.sliderDuration<<  ", total points: " << hitObject.totalNotRepeatedCurvePoints <<endl;
	//	//		break;
	//	//	}
	//	//	case HitObject::TypeE::spinner: {
	//	//		cout << hitObject.spinnerEndTime << endl;
	//	//		break;
	//	//	}
	//	//	}
	//	//}
	//	/*for (auto hitObject : test.HitObjects) {
	//		if (hitObject.type == HitObject::TypeE::slider && hitObject.sliderType == 'B' && hitObject.x == 138 && hitObject.y == 323) {
	//			POINT c; c.x = 274; c.y = 88;
	//			Input::sliderMove(hitObject.CurvePoints, hitObject.sliderDuration, hitObject.repeat, hitObject.pixelLength, 1.60156250, 1.60208333, c);
	//		}
	//	}*/
	//	/*for (auto hitObject : test.HitObjects) {
	//		if (hitObject.type == HitObject::TypeE::slider && hitObject.sliderType == 'P') {
	//			POINT c; c.x = 274; c.y = 88;
	//			Input::sliderCircleMove(hitObject, 1.60156250, 1.60208333, c);
	//		}
	//	}*/
	//}

//unordered_map<string, vector<string>> dict;
//dict.insert({ "testing" , vector<string>(1, "testing value") });

//cout << dict["testing"].at(0) << endl;

//ifstream infile;
string pathName = "C:\\Users\\ong\\AppData\\Local\\osu!\\osu!.db";
//unsigned int version;
//unsigned int folderCount;
//bool accountUnlocked;
//char stringPresent[1];
//infile.open(pathName, ios::binary | ios::in);
//if (infile) {
//	infile.read((char*)&version, sizeof(version));
//	infile.read((char*)&folderCount, sizeof(folderCount));
//	infile.read((char*)&accountUnlocked, sizeof(accountUnlocked));
//	infile.seekg(8, ios::cur);
//	infile.read(stringPresent, sizeof(stringPresent));
//	cout << version << endl;
//	cout << folderCount << endl;
//	cout << accountUnlocked << endl;
//	if (stringPresent[0] == 0x0b) {
//		//cout << stringPresentHex.str() << endl;
//		unsigned int result = 0;
//		int shift = 0;
//		char bufferByte[1];
//		while (true) {
//			infile.read(bufferByte, sizeof(bufferByte));
//			// & 127 gets last 7 bits
//			result |= (bufferByte[0] & 127) << shift;
//			// & 128 gets 1st bit
//			// if 1st bit == 0, the decode process ends
//			if (!(bufferByte[0] & (unsigned int)128)) {
//				break;
//			}
//			shift += 7;
//		}
//		char* playerName = new char[result + 1];  // Allocate n ints and save ptr in a.
//		infile.read(playerName, result);
//		playerName[result] = '\0';
//		string playerNameStr(playerName);
//		cout << playerNameStr << endl;
//		delete[] playerName;  // When done, free memory pointed to by a
//		int numberOfBeatmaps;
//		infile.read((char*)&numberOfBeatmaps, sizeof(numberOfBeatmaps));
//		cout << numberOfBeatmaps << endl;
//	}
//	
//	//cout << x[0] << endl;
//}
//infile.close();

//OsuDbParser osuDb = OsuDbParser(pathName);
//string test = "\xE7\x81\xB0\xE3\x81\xA8\xE5\xB9\xBB\xE6\x83\xB3\xE3\x81\xAE\xE3\x82\xB0\xE3\x83\xAA\xE3\x83\xA0\xE3\x82\xAC\xE3\x83\xAB";
//string test2 = u8"灰と幻想のグリムガル";
//cout << test << endl;
//cout << test2 << endl;
}

