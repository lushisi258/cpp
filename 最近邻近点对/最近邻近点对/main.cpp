#include <algorithm>
#include <vector>
#include <cmath>
#include <climits>
#include <cfloat>
#include <cstdio>

using namespace std;

class Solution {
public:
	// ���Ƚ��㼯��x�������򣬵õ�Px���ٰ�y�������򣬵õ�Py
	double closestPair(vector<pair<int, int>>& points) {
		int n = points.size();
		vector<pair<int, int>> Px = points, Py = points;
		sort(Px.begin(), Px.end());
		sort(Py.begin(), Py.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
			return a.second < b.second;
			});
		return closestPairRec(Px, Py, 0, n - 1);
	}

private:
	// �ݹ����������
	double closestPairRec(vector<pair<int, int>>& Px, vector<pair<int, int>>& Py, int l, int r) {
		// ����㼯�еĵ����ڵ���3����ֱ�����
		if (r - l <= 3) {
			return bruteForce(Px, l, r);
		}
		int mid = (r + l) / 2;
		// ��Ϊ���������㼯
		vector<pair<int, int>> Qx(Px.begin() + l, Px.begin() + mid + 1);
		vector<pair<int, int>> Rx(Px.begin() + mid + 1, Px.begin() + r + 1);
		// �������������㼯��y����ĵ㼯
		vector<pair<int, int>> Qy, Ry;
		for (const auto& point : Py) {
			if (point.first <= Px[mid].first) {
				Qy.push_back(point);
			}
			else {
				Ry.push_back(point);
			}
		}
		// ������������㼯����С����
		double dl = closestPairRec(Qx, Qy, 0, mid - l);
		double dr = closestPairRec(Rx, Ry, 0, r - mid - 1);
		double d = min(dl, dr);
		// ����ָ���С��d�ĵ����strip����������
		vector<pair<int, int>> strip;
		for (const auto& point : Py) {
			if (abs(point.first - Px[mid].first) < d) {
				strip.push_back(point);
			}
		}
		return min(d, stripClosest(strip, d));
	}

	// ֱ�����������
	double bruteForce(vector<pair<int, int>>& Px, int l, int r) {
		double minDist = DBL_MAX;
		for (int i = l; i <= r; i++) {
			for (int j = i + 1; j <= r; j++) {
				minDist = min(minDist, dist(Px[i], Px[j]));
			}
		}
		return minDist;
	}

	// ����Խ�ָ��ߵ�������
	double stripClosest(vector<pair<int, int>>& strip, double d) {
		double minDist = d;
		sort(strip.begin(), strip.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
			return a.second < b.second;
			});
		for (int i = 0; i < strip.size(); i++) {
			for (int j = i + 1; j < strip.size() && (strip[j].second - strip[i].second) < minDist; j++) {
				minDist = min(minDist, dist(strip[i], strip[j]));
			}
		}
		return minDist;
	}
	/*double stripClosest(vector<pair<int, int>>& strip, double d) {
		double minDist = d;
		for (int i = 0; i < strip.size(); i++) {
			for (int j = i + 1; j < strip.size(); j++) {
				if ((strip[j].second - strip[i].second) < minDist)
				{
					minDist = min(minDist, dist(strip[i], strip[j]));
				}
				else {	
					break;
				}
			}
		}
		return minDist;
	}*/

	// ��������֮��ľ���
	double dist(pair<int, int> p1, pair<int, int> p2) {
		return sqrt(pow(p1.first - p2.first, 2) + pow(p1.second - p2.second, 2));
	}
};

int main() {
	int n;
	int x, y;

	while (scanf("%d", &n) != EOF) {
		Solution solution;
		vector<pair<int, int>> points(n);
		for (int i = 0; i < n; i++) {
			scanf("%d %d", &x, &y);
			points[i] = make_pair(x, y);
		}
		double result = solution.closestPair(points);
		printf("%.4f\n", result);
	}
	return 0;
}