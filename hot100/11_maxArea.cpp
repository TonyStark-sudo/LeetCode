#include <vector>

class Solutiion {
public:

    // 时间复杂度是n2，超时了
    int _maxArea(std::vector<int>& height) {
        int area = 0;
        for (int i = 0; i < height.size(); i++) {
            for (int j = i + 1; j < height.size(); j++) {
                int h = std::min(height[i], height[j]);
                int w = j - i;
                int tmp = h * w;
                if (tmp > area) area = tmp;
            }
        }
        return area;
    }

    // 使用双指针
    int maxArea(std::vector<int>& height) {
        int l = 0, r = height.size() - 1;
        int area = 0;
        while (l < r) {
            int h = std::min(height[l], height[r]);
            int w = r - l;
            if (h * w > area) area = h * w;
            if (h == height[l]) l++;
            else r--;
        }
        return area;
    }
};