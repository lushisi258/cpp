#include <algorithm>
#include <vector>
#include <cmath>
#include <climits>
#include <cfloat>
#include <cstdio>

using namespace std;

class Solution {
public:
	// 首先将点集按x坐标排序，得到Px，再按y坐标排序，得到Py
	int closestPair(vector<pair<int, int>>& points) {
		int n = points.size();
		vector<pair<int, int>> Px = points, Py = points;
		sort(Px.begin(), Px.end());
		sort(Py.begin(), Py.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
			return a.second < b.second;
			});
		return closestPairRec(Px, Py, 0, n - 1);
	}

private:
	// 递归求解最近点对
	int closestPairRec(vector<pair<int, int>>& Px, vector<pair<int, int>>& Py, int l, int r) {
		// 如果点集中的点少于等于3个，直接求解
		if (r - l <= 3) {
			return bruteForce(Px, l, r);
		}
		// 分为左右两个点集
		int mid = (r + l) / 2;
		// 分为左右两个点集
		int Qx_l = l, Qx_r = mid;
		int Rx_l = mid + 1, Rx_r = r;
		// 创建左右两个点集按y排序的点集
		int midX = Px[mid].first;
		vector<pair<int, int>> Qy, Ry;
		for (const auto& point : Py) {
			if (point.first <= midX) {
				Qy.push_back(point);
			}
			else {
				Ry.push_back(point);
			}
		}
		// 求出左右两个点集的最小距离
		int dl = closestPairRec(Px, Qy, Qx_l, Qx_r);
		int dr = closestPairRec(Px, Ry, Rx_l, Rx_r);
		int d = min(dl, dr);
		// 距离分割线小于d的点加入strip集进行排序
		vector<pair<int, int>> strip;
		for (const auto& point : Py) {
			if (abs(point.first - Px[mid].first) < d) {
				strip.push_back(point);
			}
		}
		return min(d, stripClosest(strip, d));
	}

	// 直接求解最近点对
	int bruteForce(vector<pair<int, int>>& Px, int l, int r) {
		int minDist = INT_MAX;
		for (int i = l; i <= r; i++) {
			for (int j = i + 1; j <= r; j++) {
				minDist = min(minDist, dist(Px[i], Px[j]));
			}
		}
		return minDist;
	}

	// 求解跨越分割线的最近点对
	int stripClosest(vector<pair<int, int>>& strip, double d) {
		int minDist = d;
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

	// 计算两点之间的距离的平方
	int dist(pair<int, int> p1, pair<int, int> p2) {
		return pow(p1.first - p2.first, 2) + pow(p1.second - p2.second, 2);
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
		int result = solution.closestPair(points);
		double dist = sqrt(result);
		printf("%.4f\n", dist);
	}
	return 0;
}