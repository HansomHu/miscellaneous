/**
 * @file traverse_triangle.cpp
 * @brief traverse a triangle in Standard Algorithm, reference to
 *        http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
 * @version 0.1
 * @date 2021-07-05
 *
 */

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

fillBottomFlatTriangle(cv::Mat& mat, cv::Vec2i& v1, cv::Vec2i& v2, cv::Vec2i& v3) {
    float invslope1 = (v2[0] - v1[0]) / (v2[1] - v1[1]);
    float invslope2 = (v3[0] - v1[0]) / (v3[1] - v1[1]);

    float curx1 = v1[0];
    float curx2 = v1[0];

    for (int scanlineY = v1[1]; scanlineY <= v2[1]; scanlineY++) {
        // draw points in one line
        // drawLine((int)curx1, scanlineY, (int)curx2, scanlineY);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

fillTopFlatTriangle(cv::Vec2i& v1, cv::Vec2i& v2, cv::Vec2i& v3) {
    float invslope1 = (v3[0] - v1[0]) / (v3[1] - v1[1]);
    float invslope2 = (v3[0] - v2[0]) / (v3[1] - v2[1]);

    float curx1 = v3[0];
    float curx2 = v3[0];

    for (int scanlineY = v3[1]; scanlineY > v1[1]; scanlineY--) {
        // draw points in one line
        // drawLine((int)curx1, scanlineY, (int)curx2, scanlineY);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void drawTriangle(const cv::Mat& mat, const cv::Vec2i& v1, const cv::Vec2i& v2, const cv::Vec2i& v3) {
    /* at first sort the three vertices by y-coordinate ascending so v1 is the topmost vertice */
    std::vector<cv::Vec2i> sorted = { v1, v2, v3 };
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) { return a[1] < b[1]; });

    /* here we know that v1[1] <= v2[1] <= v3[1] */
    /* check for trivial case of bottom-flat triangle */
    const auto& vt1 = sorted[0];
    const auto& vt2 = sorted[1];
    const auto& vt3 = sorted[2];
    if (vt2[1] == vt3[1]) {
        fillBottomFlatTriangle(vt1, vt2, vt3);
    }
    /* check for trivial case of top-flat triangle */
    else if (vt1[1] == vt2[1]) {
        fillTopFlatTriangle(vt1, vt2, vt3);
    } else {
        /* general case - split the triangle in a topflat and bottom-flat one */
        cv::Vec2i v4 = cv::Vec2i(
            (int)(vt1[0] + ((float)(vt2[1] - vt1[1]) / (float)(vt3[1] - vt1[1])) * (vt3[0] - vt1[0])), vt2[1]);
        fillBottomFlatTriangle(vt1, vt2, v4);
        fillTopFlatTriangle(vt2, v4, vt3);
    }
}

int main(int argc, char** argv) {
    cv::Mat mat = cv::Mat(20, 20, CV_8UC1);
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            // mat.at<uchar>(i, j) = mat.cols * i + j;
            mat.at<uchar>(i, j) = 234;
        }
    }
    std::cout << mat << std::endl;
    // cv::Vec2f a = cv::Vec2f(4, 4);
    // cv::Vec2f b = cv::Vec2f(8, 16);
    // cv::Vec2f c = cv::Vec2f(18, 8);
    cv::Vec2f a = cv::Vec2f(0, 0);
    cv::Vec2f b = cv::Vec2f(0, 19);
    cv::Vec2f c = cv::Vec2f(19, 0);
    traverseTriangle1(mat, a, b, c, clearPixel);
    std::cout << mat << std::endl;
    return 0;
}