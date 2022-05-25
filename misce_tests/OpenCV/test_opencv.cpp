#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <gflags/gflags.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

DEFINE_int32(compression, 9, "The compression level ranges from 0 ~ 9");
DEFINE_string(src_img, "", "The source .png file");
DEFINE_string(dst_img, "dst.png", "The destination .png file");

bool file_exist(const std::string& file) {
    std::ifstream ifs(file);
    return ifs.good();
}

bool test_png_compression() {
    if (FLAGS_src_img.empty()) {
        std::cout << "You must provide the input source file using -src_img option" << std::endl;
        return false;
    }

    if (!file_exist(FLAGS_src_img)) {
        std::cout << "File does not exist: " << FLAGS_src_img << std::endl;
        return false;
    }

    cv::Mat src_img = cv::imread(FLAGS_src_img);
    std::vector<int> compression_params = { cv::IMWRITE_PNG_COMPRESSION, FLAGS_compression };
    cv:imwrite(FLAGS_dst_img, src_img, compression_params);
    return true;
}

bool test_draw_polyline() {
    const int WIDTH = 640;
    const int HEIGHT = 480;
    cv::Mat image = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
    image.setTo(cv::Scalar(100, 0, 0));

    // the point on a circle: P(cos(theta) * radius, sin(theta) * radius)
    const int SNIPPET_NUM = 9;
    const int POINT_NUM = SNIPPET_NUM + 1;
    const int RADIUS = 100;
    std::array<float, POINT_NUM> thetas;
    std::generate(thetas.begin(), thetas.end(), [n = 0]() mutable { return n++ * -3.1415926 / SNIPPET_NUM; });
    std::vector<cv::Point> points(POINT_NUM);

    for (size_t i = 0; i < points.size(); ++i) {
        points[i].x = int(std::cosf(thetas[i]) * RADIUS + WIDTH / 2);
        points[i].y = int(std::sinf(thetas[i]) * RADIUS + HEIGHT / 2);
        // cv::circle(image, points[i], 5, cv::Scalar(0, 0, 255), 2, 8, 0);
    }

    points.clear();
    points.emplace_back(320, 240);
    points.emplace_back(320, 241);
    cv::polylines(image, points, false, cv::Scalar(0, 255, 0), 10, cv::LINE_AA, 0);
    cv::imshow("", image);
    cv::waitKey(0);
    return true;
}

void test_matrix_multiply() {
    // cv::Mat a(4, 1, CV_64FC1);
    // cv::Mat b(1, 4, CV_64FC1);

    cv::Mat a = (cv::Mat_<double>(4, 1) << 1, 2, 3, 4);
    cv::Mat b = (cv::Mat_<double>(1, 4) << 1, 2, 3, 4);
    cv::Mat c = a * b;
    std::cout << "a: \n" << a << "\nb:\n" << b << "\nc:\n" << c << std::endl;
    cv::Mat d = c * 2 + 1;
    std::cout << "d: \n" << d << std::endl;

    cv::Mat ac2(2, 2, CV_32FC2);
    for (int i = 0; i < ac2.rows; ++i) {
        for (int j = 0; j < ac2.cols; ++j) {
            ac2.at<cv::Vec2f>(i, j)[0] = 2 * i + j;
            ac2.at<cv::Vec2f>(i, j)[1] = 2 * i + j;
        }
    }
    cv::Mat bc2(2, 2, CV_32FC2);
    for (int i = 0; i < bc2.rows; ++i) {
        for (int j = 0; j < bc2.cols; ++j) {
            bc2.at<cv::Vec2f>(i, j)[0] = 2 * i + j;
            bc2.at<cv::Vec2f>(i, j)[1] = 2 * i + j;
        }
    }
    std::cout << "\nac2: \n" << ac2 << "\nb2c: \n" << bc2 << std::endl;
    ac2 = ac2.mul(bc2);
    std::cout << "ac2:\n" << ac2 << std::endl;

    // test overflow case
    cv::Mat a1 = (cv::Mat_<uchar>(2, 2) << 251, 252, 253, 254);
    cv::Mat b1 = (cv::Mat_<uchar>(2, 2) << 10, 10, 10, 10);
    std::cout << "a1: \n" << a1 << "\nb1:\n" << b1 << std::endl;
    a1.convertTo(a1, CV_32FC1);
    b1.convertTo(b1, CV_32FC1);
    a1 = a1.mul(b1);
    std::cout << "a1: \n" << a1 << std::endl;
    a1.convertTo(a1, CV_8UC1);
    std::cout << "a1: \n" << a1 << std::endl;

    // test merge
    cv::Mat mergedImage;
    cv::Mat c1 = (cv::Mat_<uchar>(2, 2) << 251, 252, 253, 254);
    std::vector<cv::Mat> c1channels = {c1, c1, c1};
    cv::merge(c1channels, mergedImage);
    std::cout << "mergedImage:\n" << mergedImage << std::endl;
}

void test_getGuassianKernel() {
    int ksize = 2880;
    float sigma = 1108.8;
    cv::Point2f center(503.064392, 500.512238);
    auto kernelCol = cv::getGaussianKernel(ksize, sigma, CV_32F) * center.x;
    auto kernelRow = cv::getGaussianKernel(ksize, sigma, CV_32F) * center.y;
    std::cout << kernelCol << std::endl;
}

void test_findContours() {
    cv::Mat img = cv::imread("../face.png", cv::IMREAD_UNCHANGED);
    std::vector<cv::Mat> channels;
    cv::split(img, channels);
    cv::Mat gray = channels[3];
    // 高斯模糊，将一些紧邻的碎片合成一个
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 5);
    cv::threshold(gray, gray, 20, 20, cv::THRESH_BINARY);
    // cv::cvtColor(channels[3], gray, cv::COLOR_BGRA2GRAY);
    // cv::imwrite("lipOpenbak.png", img);
    // cv::imwrite("lipOpenGray.jpg", gray);
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(gray, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    cv::Mat contoursImg = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
    drawContours(contoursImg, contours, -1, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
    std::vector<cv::Rect2f> bbox(contours.size());
    for (int i = 0; i < contours.size(); ++i) {
        bbox[i] = cv::boundingRect(contours[i]);
        cv::rectangle(contoursImg, bbox[i].tl(), bbox[i].br(), cv::Scalar(0, 255, 0), 1);
    }
    // cv::Mat contoursImg = gray;
    cv::imwrite("contourface.png", contoursImg);
    cv::imwrite("grayface.png", gray);
}

double* getGaussianArray(int arr_size, double sigma) {
    const double pi = 3.14159265359;
    int i;
    double* array = new double[arr_size];
    int center_i = arr_size / 2;
    double sum = 0.0f;
    double sigma_square_2 = (2.0f * sigma * sigma);
    for (i = 0; i < arr_size; i++) {
        array[i] = (1.0f / pi * sigma_square_2) * exp(-(1.0f) * (((i - center_i) * (i - center_i)) / sigma_square_2));
        sum += array[i];
    }
    // [2-2] 归一化求权值
    for (i = 0; i < arr_size; i++) { array[i] /= sum; }
    return array;
}

void gaussian(cv::Mat& src, int kernelSize, double sigma) {
    double* gaussianArray = getGaussianArray(kernelSize, sigma);
    int center = kernelSize / 2;
    cv::Mat temp = src.clone();
    int channels = src.channels();
    // X方向
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            for (int c = 0; c < channels; ++c) {
                double sum = 0.0;
                for (int k = -center; k <= center; k++) {
                    if (j + k < 0 || j + k >= src.cols)
                        continue;
                    sum += src.ptr<uchar>(i)[(j + k) * channels + c] * gaussianArray[k + center];
                }
                // 放入中间结果
                temp.ptr<uchar>(i)[j * channels + c] = MAX(MIN(sum, 255), 0);
            }
        }
    }
    // Y方向
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            for (int c = 0; c < channels; ++c) {
                double sum = 0.0;
                for (int k = -center; k <= center; ++k) {
                    if (i + k < 0 || i + k >= src.rows)
                        continue;
                    sum += temp.ptr<uchar>(i + k)[j * channels + c] * gaussianArray[k + center];
                }
                // 此时可以修改原图像数据了
                src.ptr<uchar>(i)[j * channels + c] = MAX(MIN(sum, 255), 0);
            }
        }
    }
    delete[] gaussianArray;
}
void test_gaussian_blur() {
    cv::Mat img = cv::imread("../img_rendered.png", cv::IMREAD_UNCHANGED);
    gaussian(img, 55, 20);
    cv::imwrite("../img_rendered_gaussian_blur.png", img);
}

void test_gaussian_blur_on_rect() {
    cv::Mat img = cv::imread("../img_rendered.png", cv::IMREAD_UNCHANGED);
    cv::Mat roi = img(cv::Rect(350, 860, 200, 200));
    cv::GaussianBlur(roi, roi, cv::Size(13, 13), 0);
    cv::rectangle(img, cv::Rect(350, 860, 200, 200), cv::Scalar(255, 0, 0));
    cv::imwrite("../img_rendered_gaussian_blur_on_rect.png", img);
}

void test_exception_catch() {
    try {
        cv::Mat img = cv::Mat::zeros(0, -1, CV_8UC4);
        // cv::Mat img = cv::Mat::zeros(999999998, 999999998, CV_8UC4);
        std::cout << "should throw an exception" << std::endl;
    } catch (cv::Exception e) {
        const char* err_msg = e.what();
        std::cout << "exception caught: " << err_msg << std::endl;
    }
}

void test_png_read_and_write() {
    cv::Mat organ = cv::imread("../flaw_organ_mask.png");
    cv::imwrite("organ.png", organ);
}

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    // test_png_compression();
    // test_draw_polyline();
    // test_matrix_multiply();
    // test_getGuassianKernel();
    // test_findContours();
    // test_gaussian_blur();
    // test_exception_catch();
    // test_png_read_and_write();
    test_gaussian_blur_on_rect();
    return 0;
}
