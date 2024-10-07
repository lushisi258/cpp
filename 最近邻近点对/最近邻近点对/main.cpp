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
	// �ݹ����������
	int closestPairRec(vector<pair<int, int>>& Px, vector<pair<int, int>>& Py, int l, int r) {
		// ����㼯�еĵ����ڵ���3����ֱ�����
		if (r - l <= 3) {
			return bruteForce(Px, l, r);
		}
		// ��Ϊ���������㼯
		int mid = (r + l) / 2;
		// ��Ϊ���������㼯
		int Qx_l = l, Qx_r = mid;
		int Rx_l = mid + 1, Rx_r = r;
		// �������������㼯��y����ĵ㼯
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
		// ������������㼯����С����
		int dl = closestPairRec(Px, Qy, Qx_l, Qx_r);
		int dr = closestPairRec(Px, Ry, Rx_l, Rx_r);
		int d = min(dl, dr);
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
	int bruteForce(vector<pair<int, int>>& Px, int l, int r) {
		int minDist = INT_MAX;
		for (int i = l; i <= r; i++) {
			for (int j = i + 1; j <= r; j++) {
				minDist = min(minDist, dist(Px[i], Px[j]));
			}
		}
		return minDist;
	}

	// ����Խ�ָ��ߵ�������
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

	// ��������֮��ľ����ƽ��
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