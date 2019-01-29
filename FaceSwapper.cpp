#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv/cv_image_abstract.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo.hpp>

#ifdef DEBUG
#include <dlib/gui_widgets.h>
#include <dlib/image_processing/render_face_detections.h>
#endif

using namespace std;

#ifdef DEBUG
#define DLOG(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define DLOG(fmt, ...)
#endif

struct correspondens {
    std::vector<int> index;
};

void faceLandmarkDetection(dlib::array2d<unsigned char>& img, dlib::shape_predictor sp, std::vector<cv::Point2f>& landmark)
{
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    std::vector<dlib::rectangle> dets = detector(img);
    DLOG("Number of faces detected: %lu \n", dets.size());
    dlib::full_object_detection shape = sp(img, dets[0]);
    for (int i = 0; i < shape.num_parts(); ++i) {
        float x = shape.part(i).x();
        float y = shape.part(i).y();
        landmark.push_back(cv::Point2f(x, y));
    }
}

void delaunayTriangulation(const std::vector<cv::Point2f>& hull, std::vector<correspondens>& delaunayTri, cv::Rect rect)
{
    cv::Subdiv2D subdiv(rect);
    for (int it = 0; it < hull.size(); it++) {
        subdiv.insert(hull[it]);
    }
    DLOG("done subdiv add : %lu \n", hull.size());
    std::vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);
    DLOG("traingleList number is %lu \n", triangleList.size());
    for (size_t i = 0; i < triangleList.size(); ++i) {
        std::vector<cv::Point2f> pt;
        correspondens ind;
        cv::Vec6f t = triangleList[i];
        pt.push_back(cv::Point2f(t[0], t[1]));
        pt.push_back(cv::Point2f(t[2], t[3]));
        pt.push_back(cv::Point2f(t[4], t[5]));
        if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2])) {
            int count = 0;
            for (int j = 0; j < 3; ++j)
                for (size_t k = 0; k < hull.size(); k++)
                    if (abs(pt[j].x - hull[k].x) < 1.0 && abs(pt[j].y - hull[k].y) < 1.0) {
                        ind.index.push_back(k);
                        count++;
                    }
            if (count == 3)
                delaunayTri.push_back(ind);
        }
    }
}

void applyAffineTransform(cv::Mat& warpImage, cv::Mat& src, std::vector<cv::Point2f>& srcTri, std::vector<cv::Point2f>& dstTri)
{
    cv::Mat warpMat = getAffineTransform(srcTri, dstTri);
    warpAffine(src, warpImage, warpMat, warpImage.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);
}

void warpTriangle(cv::Mat& img1, cv::Mat& img2, std::vector<cv::Point2f>& t1, std::vector<cv::Point2f>& t2)
{
    cv::Rect r1 = boundingRect(t1);
    cv::Rect r2 = boundingRect(t2);
    std::vector<cv::Point2f> t1Rect, t2Rect;
    std::vector<cv::Point> t2RectInt;
    for (int i = 0; i < 3; i++) {
        t1Rect.push_back(cv::Point2f(t1[i].x - r1.x, t1[i].y - r1.y));
        t2Rect.push_back(cv::Point2f(t2[i].x - r2.x, t2[i].y - r2.y));
        t2RectInt.push_back(cv::Point(t2[i].x - r2.x, t2[i].y - r2.y));
    }
    cv::Mat mask = cv::Mat::zeros(r2.height, r2.width, CV_32FC3);
    fillConvexPoly(mask, t2RectInt, cv::Scalar(1.0, 1.0, 1.0), 16, 0);
    cv::Mat img1Rect;
    img1(r1).copyTo(img1Rect);
    cv::Mat img2Rect = cv::Mat::zeros(r2.height, r2.width, img1Rect.type());
    applyAffineTransform(img2Rect, img1Rect, t1Rect, t2Rect);
    multiply(img2Rect, mask, img2Rect);
    multiply(img2(r2), cv::Scalar(1.0, 1.0, 1.0) - mask, img2(r2));
    img2(r2) = img2(r2) + img2Rect;
}

void time_teack(int* start, const char* event)
{
    printf("time - %s : %.0f ms\n", event, (clock() - *start) * 1000.0 / CLOCKS_PER_SEC);
    *start = clock();
}

void draw_face(const char* name, cv::Mat image, std::vector<cv::Point2f> points)
{
    cv::Mat imageClone = image.clone();
    for (int i = 0; i < points.size(); i++) {
        cv::circle(imageClone, cv::Point(points[i].x, points[i].y), 1, cv::Scalar(0, 0, 0));
    }
    cv::imshow(name, imageClone);
}

void save_face(string out, cv::Mat image, std::vector<cv::Point2f> points)
{
    cv::Mat imageClone = image.clone();
    for (int i = 0; i < points.size(); i++) {
        cv::circle(imageClone, cv::Point(points[i].x, points[i].y), 1, cv::Scalar(0, 0, 0));
    }
    cv::imwrite(out, imageClone);
}

void swap(string face, string body, string out)
{
    int start = clock();
    bool v = true;

    //----------------- step 1. load the input two images.
    //----------------------------------
    dlib::array2d<unsigned char> imgDlib1, imgDlib2;
    dlib::load_image(imgDlib1, face);
    dlib::load_image(imgDlib2, body);
    if (v)
        time_teack(&start, "load_image");

    cv::Mat imgCV1 = cv::imread(face);
    cv::Mat imgCV2 = cv::imread(body);
    if (!imgCV1.data || !imgCV2.data) {
        printf("No image data \n");
        return;
    }

    //---------------------- step 2. detect face landmarks
    //-----------------------------------
    dlib::shape_predictor sp;
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> sp;
    if (v)
        time_teack(&start, "deserialize");

    std::vector<cv::Point2f> points1, points2;

    faceLandmarkDetection(imgDlib1, sp, points1);
    if (v)
        time_teack(&start, "faceLandmarkDetection1");
#ifdef DEBUG
    draw_face("Face 1", imgCV1, points1);
    if (v)
        time_teack(&start, "circle1");
#endif
    save_face("face-" + face, imgCV1, points1);

    faceLandmarkDetection(imgDlib2, sp, points2);
    if (v)
        time_teack(&start, "faceLandmarkDetection2");
#ifdef DEBUG
    draw_face("Face 2", imgCV2, points2);
    if (v)
        time_teack(&start, "circle1");
#endif
    save_face("face-" + body, imgCV2, points2);

    //---------------------step 3. find convex hull
    //-------------------------------------------
    cv::Mat imgCV1Warped = imgCV2.clone();
    imgCV1.convertTo(imgCV1, CV_32F);
    imgCV1Warped.convertTo(imgCV1Warped, CV_32F);
    if (v)
        time_teack(&start, "convertWarped");

    std::vector<cv::Point2f> hull1;
    std::vector<cv::Point2f> hull2;
    std::vector<int> hullIndex;

    cv::convexHull(points2, hullIndex, false, false);
    if (v)
        time_teack(&start, "convexHull");

    for (int i = 0; i < hullIndex.size(); i++) {
        hull1.push_back(points1[hullIndex[i]]);
        hull2.push_back(points2[hullIndex[i]]);
    }

    //-----------------------step 4. delaunay triangulation
    //-------------------------------------
    std::vector<correspondens> delaunayTri;
    cv::Rect rect(0, 0, imgCV1Warped.cols, imgCV1Warped.rows);
    delaunayTriangulation(hull2, delaunayTri, rect);
    if (v)
        time_teack(&start, "delaunayTriangulation");

    for (size_t i = 0; i < delaunayTri.size(); ++i) {
        std::vector<cv::Point2f> t1, t2;
        correspondens corpd = delaunayTri[i];
        for (size_t j = 0; j < 3; ++j) {
            t1.push_back(hull1[corpd.index[j]]);
            t2.push_back(hull2[corpd.index[j]]);
        }

        warpTriangle(imgCV1, imgCV1Warped, t1, t2);
    }
    if (v)
        time_teack(&start, "warpTriangle");

    //------------------------step 5. clone seamlessly
    //-----------------------------------------------
    // calculate mask
    std::vector<cv::Point> hull8U;

    for (int i = 0; i < hull2.size(); ++i) {
        cv::Point pt(hull2[i].x, hull2[i].y);
        hull8U.push_back(pt);
    }

    cv::Mat mask = cv::Mat::zeros(imgCV2.rows, imgCV2.cols, imgCV2.depth());
    fillConvexPoly(mask, &hull8U[0], hull8U.size(), cv::Scalar(255, 255, 255));
    if (v)
        time_teack(&start, "fillConvexPoly");

    cv::Rect r = boundingRect(hull2);
    cv::Point center = (r.tl() + r.br()) / 2;

    cv::Mat output;
    imgCV1Warped.convertTo(imgCV1Warped, CV_8UC3);
#ifdef DEBUG
    cv::imshow("imgCV1Warped", imgCV1Warped);
    if (v)
        time_teack(&start, "imgCV1Warped");
#endif
    cv::seamlessClone(imgCV1Warped, imgCV2, mask, center, output, cv::NORMAL_CLONE);
    if (v)
        time_teack(&start, "seamlessClone");

    //------------------------step 6. beauty
    //-----------------------------------------------
    cv::Mat dst;

    int value1 = 3, value2 = 1;

    int dx = value1 * 5;
    double fc = value1 * 12.5;
    int p = 50;
    cv::Mat temp1, temp2, temp3, temp4;

    bilateralFilter(output, temp1, dx, fc, fc);

    temp2 = (temp1 - output + 128);

    GaussianBlur(temp2, temp3, cv::Size(2 * value2 - 1, 2 * value2 - 1), 0, 0);

    temp4 = output + 2 * temp3 - 255;

    dst = (output * (100 - p) + temp4 * p) / 100;
    if (v)
        time_teack(&start, "beauty");

    cv::imwrite(out, dst);
    if (v)
        time_teack(&start, "imwrite");

#ifdef DEBUG
    cv::imshow("Beauty", dst);
    cv::imshow("Face Swapped", output);
    cv::waitKey(0);
    cv::destroyAllWindows();
#endif
}
