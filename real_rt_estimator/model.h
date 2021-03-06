
#ifndef REAL_RT_ESTIMATOR_MODEL_H
#define REAL_RT_ESTIMATOR_MODEL_H


#include <iostream>
#include <pthread.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <jderobot/camera.h>
// #include <jderobot/motors.h>
// #include <jderobot/ptmotors.h>
// #include <jderobot/laser.h>
// #include <jderobot/encoders.h>
// #include <jderobot/ptencoders.h>
#include <jderobot/pointcloud.h>
#include <Eigen/Dense>

// #include <opencv2/core/core.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/highgui/highgui.hpp>
//
//#include "opencv2/opencv.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"

// IMPORTANT: install this lib to use SIFT
#include "opencv2/nonfree/features2d.hpp"

#include <opencv2/legacy/legacy.hpp>

//PCL
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>

#include <algorithm>

//#include <geometry/progeo/Progeo.h>
//#include <progeo/progeo.h>

//#define IMG_X 320
//#define IMG_Y 240
#define N_ESTIMATOR_POINTS 15

#define OUTSTANDING_DISTANCE 100

#define RANSAC_PERC 0.8 // percentage points to choose randomly
#define RANSAC_ITER 10 // number of iterations for ransac

/* Traslation of graphic coordinates to optical coordinates and vice versa */
/*#define GRAPHIC_TO_OPTICAL_X(x,y) (SIFNTSC_ROWS-1-y)
#define GRAPHIC_TO_OPTICAL_Y(x,y) (x)
#define OPTICAL_TO_GRAPHIC_X(x,y) (y)
#define OPTICAL_TO_GRAPHIC_Y(x,y) (SIFNTSC_ROWS-1-x)*/

//#define N_BEST_POINTS_FOR_RT 10

#include "myprogeo.h"


namespace real_rt_estimator {
    class Model {
	public:
		Model();
		virtual ~Model();

	  pthread_mutex_t controlImgRGB;
		pthread_mutex_t controlImgDEPTH;
		pthread_mutex_t controlImgMatches;
    pthread_mutex_t controlPcConverted;
    pthread_mutex_t controlCamera;
    pthread_mutex_t controlVars;
    pthread_mutex_t controlRTMatrix;

		std::vector<jderobot::RGBPoint> get_pc();
		std::vector<jderobot::RGBPoint> get_pc_converted();
    std::vector<jderobot::RGBPoint> get_pc_camera();
    std::vector<jderobot::RGBPoint> get_pc_camera_converted();

	  cv::Mat getImageCameraRGB();
    cv::Mat getImageCameraRGBKeyPoints();
		cv::Mat getImageCameraRGBAux();
    cv::Mat getImageCameraRGBAuxKeyPoints();
	  cv::Mat getImageCameraDEPTH();
    cv::Mat getImageCameraDEPTHKeyPoints();
		cv::Mat getImageCameraDEPTHAux();
    cv::Mat getImageCameraDEPTHAuxKeyPoints();
		cv::Mat getImageCameraRGBMatches();
    cv::Mat getImageCameraDEPTHMatches();

    int getMatchingPoints();
    int getTotalPoints();
    Eigen::Matrix4f getFinalRTMatrix();
    int getEuclideanError();
    int getReprojectionError();

		void createImageRGB(cv::Mat data);
		void createImageRGBAux(cv::Mat data);
		void createImageDEPTH(cv::Mat data);
		void createImageDEPTHAux(cv::Mat data);

	  void createEmptyImageRGB();
 		void createEmptyImageDEPTH();
	  void updateImageRGB(cv::Mat data);
		void updateImageDEPTH(cv::Mat data);
    void updateImages();

    void changeImageAux();

    // New
    bool calculatePoints(cv::String detectionMode, cv::String detectionFilterMode);
    bool calculateMatching(cv::String matchingMode, cv::String matchingFilterMode, int percentagePoints);

		bool estimateRT(cv::String estimateFilterMode);
    bool isEstimated();

    void RotateXAxis();
    void RotateYAxis();
    void moveDownRT();
    void moveUpRT();

    void moveCamera();

/*
// 	    jderobot::EncodersDataPtr encodersData;
// 	    jderobot::LaserDataPtr laserData;
// 	    jderobot::ImageDataPtr imageData1; // Contains the image info
// 	    jderobot::ImageDataPtr imageData2; // Contains the image info
// 	    jderobot::PTEncodersDataPtr PTencodersData1;
// 	    jderobot::PTEncodersDataPtr PTecondersData2;
//             jderobot::PTMotorsData* PTmotorsData1;
//             jderobot::PTMotorsData* PTmotorsData2;

	    cv::Mat image1;	// Image camera1 processed to manipulate with openCV
	    cv::Mat image2; // Image camera2 processed to manipulate with openCV


// 	    bool guiVisible;
// 	    bool iterationControlActivated;
	    //Variables used in NavigationAlgorithm
// 	    int sentido;
// 	    int accion;*/

	private:
	    //Variables used in NavigationAlgorithm
// 	    cv::Mat imageCamera1;
// 	    cv::Mat imageCamera2;

	    //typedef std::vector<cv::KeyPoint> vectorkp;

		cv::Mat currentImageRGB;
    cv::Mat imageRGB;
    cv::Mat imageRGB_kp;
		cv::Mat imageRGB_aux;
    cv::Mat imageRGB_aux_kp;
    cv::Mat currentImageDEPTH;
		cv::Mat imageDEPTH;
    cv::Mat imageDEPTH_gray;
    cv::Mat imageDEPTH_kp;
		cv::Mat imageDEPTH_aux;
    cv::Mat imageDEPTH_aux_gray;
    cv::Mat imageDEPTH_aux_kp;
		cv::Mat imageRGBMatches;
    cv::Mat imageDEPTHMatches;
    cv::Mat imageGray;
    cv::Mat imageGray_aux;

		cv::Mat temp_imageRGB;
		cv::Mat temp_imageRGB_aux;
		cv::Mat temp_imageDEPTH;
		cv::Mat temp_imageDEPTH_aux;

    cv::Mat descriptors_n, descriptors_n_aux;
    std::vector<cv::KeyPoint> keypoints_n, keypoints_n_aux;

    cv::Mat* distance;
    cv::Mat* distance_aux;

    bool isChangeImageAux;

		real_rt_estimator::myprogeo *mypro;

		int sift_points;

		struct myMatch {
			int matchNum;
			double matchDistance;
			double matchAprox;
		};
    std::vector<myMatch> myMatches;

    struct myPoint {
      int x;
      int y;
      jderobot::RGBPoint rgbPoint;
    };
    std::vector<myPoint> myPrevPoints;
    std::vector<myPoint> myNewPoints;


		cv::Mat dataRGB;
		cv::Mat dataDEPTH;

		/* It is supposed that camera parameters are defined */
		//TPinHoleCamera TPHcamera1;
		//TPinHoleCamera TPHcamaraDepth;

		Eigen::Matrix4f RT_final;
    jderobot::RGBPoint p_aux;

		//std::vector<cv::KeyPoint> vkp1;
		//std::vector<cv::KeyPoint> vkp2;

		std::vector<jderobot::RGBPoint> v_rgbp, v_rgbp_aux;
		std::vector<jderobot::RGBPoint> pc, pc_converted, pc_camera, pc_camera_converted;

    bool _firstIteration;
    int iterationCloud;

    int matchingPoints;
    int totalPoints;
    int euclidean_err;
    int reprojection_err;

    bool _finishedOk;

    // private methods
    void updateImageRGBAux();
		void updateImageDEPTHAux();

		//pcl::PointCloud<pcl::PointXYZRGBA>::Ptr pc;
		//pcl::PointCloud<pcl::PointXYZRGBA>::Ptr pc_converted;

		void doSift(/*jderobot::ImageDataPtr img1, jderobot::ImageDataPtr img2*/);
		void getPoints();



		static bool sortByDistance(const cv::DMatch &lhs, const cv::DMatch &rhs);


    void moveCamera(Eigen::Matrix4f RT_estimate);


		jderobot::RGBPoint getPoints3D(int x, int y, cv::Mat* imgRGB, cv::Mat* imgDepth);
    bool isBorderPoint(int x, int y, cv::Mat* imgDepth);
    jderobot::RGBPoint findPoint(int x, int y, std::vector<myPoint> points);


    };//class
} // namespace
#endif /*REAL_RT_ESTIMATOR_MODEL_H*/
