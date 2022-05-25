#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

struct RectVertex {
    cv::Vec2f tl; // top left vertex
    cv::Vec2f br; // bottom right vertex
    float area(void) const { return (br[0] - tl[0]) * (br[1] - tl[1]); }
};

float iou(const RectVertex& rect1, const RectVertex& rect2) {
    float w = std::min(rect1.br[0], rect2.br[0]) - std::max(rect1.tl[0], rect1.tl[0]);
    float h = std::min(rect1.br[1], rect2.br[1]) - std::max(rect1.tl[1], rect2.tl[1]);
    if (w <= 0 || h <= 0) {
        return 0.0;
    }
    return w * h / (rect1.area() + rect2.area() - w * h);
}

bool isOverlap(const RectVertex& rect1, const RectVertex& rect2) {
    float w = std::min(rect1.br[0], rect2.br[0]) - std::max(rect1.tl[0], rect1.tl[0]);
    float h = std::min(rect1.br[1], rect2.br[1]) - std::max(rect1.tl[1], rect2.tl[1]);
    return !(w <= 0 || h <= 0);
}

int main(int argc, char** argv) {
    RectVertex rect1, rect2, rect3, rect4;
    rect1.tl = cv::Vec2f(1, 1);
    rect1.br = cv::Vec2f(2, 2);
    rect2.tl = cv::Vec2f(1.5, 1.5);
    rect2.br = cv::Vec2f(2.5, 2.5);
    rect3.tl = cv::Vec2f(4, 4);
    rect3.br = cv::Vec2f(8, 8);
    rect4.tl = cv::Vec2f(5, 5);
    rect4.br = cv::Vec2f(6, 6);
    std::cout << "iou1: " << iou(rect1, rect2) << ", iou2: " << iou(rect2, rect3) << ", iou3: " << iou(rect3, rect4) << std::endl;
    std::cout << "isOverlap1: " << isOverlap(rect1, rect2) << ", isOverlap2: "
        << isOverlap(rect2, rect3) << ", isOverlap3: " << isOverlap(rect3, rect4) << std::endl;
    return 0;
}