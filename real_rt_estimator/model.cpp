#include "model.h"

#define w 400

//Shared memory class storage of shared resources of the component

namespace real_rt_estimator {
	Model::Model() {

		//pthread_mutex_t controlImgRGB = PTHREAD_MUTEX_INITIALIZER;
		//pthread_mutex_t controlImgDEPTH = PTHREAD_MUTEX_INITIALIZER;

		/*xmlReader(&TPHcamaraDepth, "camDepth");
		TPHcamaraDepth.position.H=1;
		TPHcamaraDepth.foa.H=1;
		update_camera_matrix(&TPHcamaraDepth);
		display_camerainfo(TPHcamaraDepth);*/


		this->_firstIteration = true;
		this->iterationCloud = 0;

    this->matchingPoints = 0;
    this->totalPoints = 0;

		this->isChangeImageAux = true;

		this->mypro= new real_rt_estimator::myprogeo();
		char c_null[0];
		this->mypro->load_cam(c_null,0,320, 240);

		//pcl::PointCloud<pcl::PointXYZRGBA>::Ptr pc2(new pcl::PointCloud<pcl::PointXYZRGBA>);
		//pcl::PointCloud<pcl::PointXYZRGBA>::Ptr pc_aux2(new pcl::PointCloud<pcl::PointXYZRGBA>);
		//this->pc = pc2;
		//this->pc_converted = pc_aux2;

		this->sift_points = 0;

		//this->pc->points.resize(0);


		//this->pc_converted->points.resize(0);

		this->RT_final << 1, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1;
		std::cout << this->RT_final;


		// Set camera TODO: COger de la configuración de progeo
		this->pc_camera.resize(0);
		this->p_aux.x=0;
		this->p_aux.y=0;
		this->p_aux.z=0;
		this->pc_camera.push_back(this->p_aux);
		this->p_aux.x=0;
		this->p_aux.y=700;
		this->p_aux.z=0;
		this->pc_camera.push_back(this->p_aux);
		this->p_aux.x=-150;
		this->p_aux.y=350;
		this->p_aux.z=-150;
		this->pc_camera.push_back(this->p_aux);
		this->p_aux.x=-150;
		this->p_aux.y=350;
		this->p_aux.z=150;
		this->pc_camera.push_back(this->p_aux);
		this->p_aux.x=150;
		this->p_aux.y=350;
		this->p_aux.z=150;
		this->pc_camera.push_back(this->p_aux);
		this->p_aux.x=150;
		this->p_aux.y=350;
		this->p_aux.z=-150;
		this->pc_camera.push_back(this->p_aux);

 		// TODO: Cambiar ocnstantes de tamaño por el que venga al inicializar el ojeto
		this->distance = new cv::Mat(cv::Size(320, 240),CV_32FC1,cv::Scalar(0,0,0));
		this->distance_aux = new cv::Mat(cv::Size(320, 240),CV_32FC1,cv::Scalar(0,0,0));

    _finishedOk = true;
	}

	Model::~Model() {}

	// Get Methods
	std::vector<jderobot::RGBPoint> Model::get_pc() {
		return this->pc;
	}
	std::vector<jderobot::RGBPoint> Model::get_pc_converted() {
		//pthread_mutex_lock(&this->controlPcConverted);
		return this->pc_converted;
		//pthread_mutex_unlock(&this->controlPcConverted);
	}

	std::vector<jderobot::RGBPoint> Model::get_pc_camera() {
		return this->pc_camera;
	}

	std::vector<jderobot::RGBPoint> Model::get_pc_camera_converted() {
    std::vector<jderobot::RGBPoint> pc_camera_converted_aux;
    pthread_mutex_lock(&this->controlCamera);
    pc_camera_converted_aux = this->pc_camera_converted;
    pthread_mutex_unlock(&this->controlCamera);
		return pc_camera_converted_aux;
	}

	// Gets Images from GUI
	cv::Mat Model::getImageCameraRGB() {
		/*pthread_mutex_lock(&this->controlImgRGB);
		cv::Mat result;
		returnthis->imageRGB.copyTo(result);
		pthread_mutex_unlock(&this->controlImgRGB);
		return result;*/
		return this->imageRGB;
	}
	cv::Mat Model::getImageCameraRGBKeyPoints() {
		return this->imageRGB_kp;
	}
	cv::Mat Model::getImageCameraRGBAux(){
		/*pthread_mutex_lock(&this->controlImgRGB);
		cv::Mat result;
		this->imageRGB_aux.copyTo(result);
		pthread_mutex_unlock(&this->controlImgRGB);
		return result;*/
		return this->imageRGB_aux;
	}
	cv::Mat Model::getImageCameraRGBAuxKeyPoints() {
		return this->imageRGB_aux_kp;
	}
	cv::Mat Model::getImageCameraDEPTH() {
		/*pthread_mutex_lock(&this->controlImgDEPTH);
		cv::Mat result;
		this->imageDEPTH.copyTo(result);
		pthread_mutex_unlock(&this->controlImgDEPTH);
		return result;*/
		return this->imageDEPTH_gray;
	}
	cv::Mat Model::getImageCameraDEPTHKeyPoints() {
		return this->imageDEPTH_kp;
	}
	cv::Mat Model::getImageCameraDEPTHAux() {
		/*pthread_mutex_lock(&this->controlImgDEPTH);
		cv::Mat result;
		this->imageDEPTH_aux.copyTo(result);
		pthread_mutex_unlock(&this->controlImgDEPTH);
		return result;*/
		return this->imageDEPTH_aux_gray;
	}
	cv::Mat Model::getImageCameraDEPTHAuxKeyPoints() {
		return this->imageDEPTH_kp;
	}
	cv::Mat Model::getImageCameraRGBMatches() {
		pthread_mutex_lock(&this->controlImgMatches);
		cv::Mat result;
		this->imageRGBMatches.copyTo(result);
		pthread_mutex_unlock(&this->controlImgMatches);
		return result;
	}
	cv::Mat Model::getImageCameraDEPTHMatches() {
		pthread_mutex_lock(&this->controlImgMatches);
		cv::Mat result;
		this->imageDEPTHMatches.copyTo(result);
		pthread_mutex_unlock(&this->controlImgMatches);
		return result;
	}

  int Model::getMatchingPoints() {
    int matchingPoints_aux;
    pthread_mutex_lock(&this->controlVars);
    matchingPoints_aux = this->matchingPoints;
    pthread_mutex_unlock(&this->controlVars);
    return matchingPoints_aux;
  }
  int Model::getTotalPoints() {
    int totalPoints_aux;
    pthread_mutex_lock(&this->controlVars);
    totalPoints_aux = this->totalPoints;
    pthread_mutex_unlock(&this->controlVars);
    return totalPoints_aux;
  }
  Eigen::Matrix4f Model::getFinalRTMatrix() {
    Eigen::Matrix4f matrix;
    pthread_mutex_lock(&this->controlRTMatrix);
    matrix = this->RT_final;
    pthread_mutex_unlock(&this->controlRTMatrix);
    return matrix;
  }
  int Model::getEuclideanError() {
    int error;
    pthread_mutex_lock(&this->controlVars);
    error = this->euclidean_err;
    pthread_mutex_unlock(&this->controlVars);
    return error;
  }
  int Model::getReprojectionError() {
    return 0;
  }


	void Model::createImageRGB(cv::Mat data) {
		this->dataRGB = data;
		pthread_mutex_lock(&this->controlImgRGB);
		imageRGB.create(data.rows, data.cols, CV_8UC3);
		pthread_mutex_unlock(&this->controlImgRGB);
		imageRGB_aux.create(data.rows, data.cols, CV_8UC3);
	}

	void Model::createImageRGBAux(cv::Mat data) {
		//pthread_mutex_lock(&this->controlImgRGB);
		imageRGB_aux.create(data.rows, data.cols, CV_8UC3);
		//pthread_mutex_unlock(&this->controlImgRGB);
	}

	void Model::createImageDEPTH(cv::Mat data) {
		this->dataDEPTH = data;
		pthread_mutex_lock(&this->controlImgRGB);
		imageDEPTH.create(data.rows, data.cols, CV_8UC3);
		pthread_mutex_unlock(&this->controlImgRGB);
		imageDEPTH_aux.create(data.rows, data.cols, CV_8UC3);
	}

	void Model::createImageDEPTHAux(cv::Mat data) {
		//pthread_mutex_lock(&this->controlImgDEPTH);
		imageDEPTH_aux.create(data.rows, data.cols, CV_8UC3);
		//pthread_mutex_unlock(&this->controlImgDEPTH);
	}

    /*void Model::createEmptyImageRGB() {
		pthread_mutex_lock(&this->controlGui);
		imageRGB.create(w, w, CV_8UC3);
		pthread_mutex_unlock(&this->controlGui);
    }

	void Model::createEmptyImageRGB() {
		pthread_mutex_lock(&this->controlGui2);
		imageDEPTH.create(w, w, CV_8UC3);
		pthread_mutex_unlock(&this->controlGui2);
    }*/

	void Model::updateImageRGB(cv::Mat data){
		pthread_mutex_lock(&this->controlImgRGB);
		/*if (this->isChangeImageAux) {
			this->updateImageRGBAux(this->dataRGB);
		}*/
		data.copyTo(currentImageRGB); // FIXME: Evitar copyTo
		//memcpy((unsigned char *) imageRGB.data ,&(data.data), imageRGB.cols*imageRGB.rows * 3);

		//this->dataRGB = data;
		pthread_mutex_unlock(&this->controlImgRGB);
	}

	void Model::updateImageRGBAux(){
		imageRGB.copyTo(imageRGB_aux);
	}

	void Model::updateImageDEPTH(cv::Mat data){
		//std::cout <<  "-------- Window 3x3 --------" << std::endl;
		pthread_mutex_lock(&this->controlImgRGB);
		//std::cout <<  "-------- Window 3x11111113 --------" << std::endl;
		/*if (this->isChangeImageAux) {
			this->updateImageDEPTHAux(this->dataDEPTH);
			this->isChangeImageAux = false;
		}*/
		data.copyTo(currentImageDEPTH);
		//memcpy((unsigned char *) imageDEPTH.data ,&(data.data), imageDEPTH.cols*imageDEPTH.rows * 3);
		//this->dataDEPTH = data;
		pthread_mutex_unlock(&this->controlImgRGB);
	}

	void Model::updateImageDEPTHAux() {
		imageDEPTH.copyTo(imageDEPTH_aux);
	}

	void Model::updateImages() {

    pthread_mutex_lock(&this->controlImgRGB);
		// update aux (n-1)
    if (_finishedOk) {
  		this->updateImageRGBAux();
  		this->updateImageDEPTHAux();
    }

		// update current (n)
		currentImageRGB.copyTo(imageRGB);
		currentImageDEPTH.copyTo(imageDEPTH);
		pthread_mutex_unlock(&this->controlImgRGB);

		// Get distances and gray DEPTH
		if (this->_firstIteration) {
			cv::Mat colorDepth_aux(imageDEPTH_aux.size(),imageDEPTH_aux.type());
			std::vector<cv::Mat> layers_aux;
			cv::split(imageDEPTH_aux, layers_aux);
			cv::cvtColor(layers_aux[0],colorDepth_aux,CV_GRAY2RGB);
			this->imageDEPTH_aux_gray = colorDepth_aux;
			/*for (int x=0; x< layers_aux[1].cols ; x++) {
				for (int y=0; y<layers_aux[1].rows; y++) {
					this->distance_aux->at<float>(y,x) = ((int)layers_aux[1].at<unsigned char>(y,x)<<8)|(int)layers_aux[2].at<unsigned char>(y,x);
				}
			}*/
		} else {
      if (_finishedOk) {
        this->imageDEPTH_aux_gray = this->imageDEPTH_gray;
      }
			//this->distance_aux = this->distance;
		}
		cv::Mat colorDepth(imageDEPTH.size(),imageDEPTH.type());
		std::vector<cv::Mat> layers;
		cv::split(imageDEPTH, layers);
		cv::cvtColor(layers[0],colorDepth,CV_GRAY2RGB);
		this->imageDEPTH_gray = colorDepth;
		/*for (int x=0; x< layers[1].cols ; x++) {
			for (int y=0; y<layers[1].rows; y++) {
				this->distance->at<float>(y,x) = ((int)layers[1].at<unsigned char>(y,x)<<8)|(int)layers[2].at<unsigned char>(y,x);
			}
		}*/
	}

	void Model::changeImageAux() {
		this->isChangeImageAux = true;
	}

	bool Model::sortByDistance(const cv::DMatch &lhs, const cv::DMatch &rhs) {
		//return ((lhs.matchDistance + lhs.matchAprox) < (rhs.matchDistance + rhs.matchAprox));
		return ((lhs.distance) < (rhs.distance));
	}

	jderobot::RGBPoint Model::getPoints3D(int x, int y, cv::Mat* imgRGB, cv::Mat* imgDepth/*, cv::Mat* distances*/) {

		int width = imgDepth->cols;
		int height = imgDepth->rows;

		//std::cout <<  "TAMAÑOnto x! " << width << height << std::endl;
		//std::cout <<  imgDepth->data  << std::endl;
		//float module;
		//float ux,uy,uz;

		pcl::PointXYZRGBA points;

		//std::cout <<  "lol" << std::endl;
		//std::cout << imgDepth->data[3*x+imgDepth->rows*y+1] << std::endl;
		//std::cout << imgDepth->data[3*x+imgDepth->rows*y+1] << std::endl;
		//std::cout << (3*x+imgDepth->rows*y+2) << std::endl;
		unsigned int realDepthDist = ((0 << 24)|(0 << 16)|(imgDepth->data[3*(y*imgDepth->cols+x)+1]<<8)|(imgDepth->data[3*(y*imgDepth->cols+x)+2]));


		//std::cout <<  "---------------------" << std::endl;
		//std::cout <<  "Puntos a 3D: " << x << ", " << y << std::endl;

		/*std::vector<cv::Mat> layers;
		cv::split(this->imageDEPTH, layers);
		cv::Mat* distance;
		distance = new cv::Mat(cv::Size(width, height),CV_32FC1,cv::Scalar(0,0,0));
		for (int x=0; x< layers[1].cols ; x++) {
				for (int y=0; y<layers[1].rows; y++) {
						distance->at<float>(y,x) = ((int)layers[1].at<unsigned char>(y,x)<<8)|(int)layers[2].at<unsigned char>(y,x);
				}
		}

		double dis=distance->at<float>(y,x);*/
		//double dis=distances->at<float>(y,x);
		//std::cout <<  "Distancia WAPA: " << dis << std::endl;


		/* Defining auxiliar points*/
		//HPoint2D auxPoint2DCam1;
		//HPoint3D auxPoint3DCam1;
		float d = (float)realDepthDist;
		d = d;
		//std::cout <<  "Distancia: " << d << std::endl;

		float xp,yp,zp,camx,camy,camz;
		mypro->mybackproject(x, y, &xp, &yp, &zp, &camx, &camy, &camz,0);
		//vector unitario
		float modulo;
		float c1x, c1y, c1z;
		float fx,fy,fz;
		float fmod;
		float t;
		float Fx,Fy,Fz;
		float ux,uy,uz;

		modulo = sqrt(1/(((camx-xp)*(camx-xp))+((camy-yp)*(camy-yp))+((camz-zp)*(camz-zp))));
		mypro->mygetcamerafoa(&c1x, &c1y, &c1z, 0);

		//std::cout <<  "MODULO " << modulo << std::endl;
		fmod = sqrt(1/(((camx-c1x)*(camx-c1x))+((camy-c1y)*(camy-c1y))+((camz-c1z)*(camz-c1z))));
		fx = (c1x - camx)*fmod;
		fy = (c1y - camy)*fmod;
		fz = (c1z - camz)*fmod;
		ux = (xp-camx)*modulo;
		uy = (yp-camy)*modulo;
		uz = (zp-camz)*modulo;
		Fx= d*fx + camx;
		Fy= d*fy + camy;
		Fz= d*fz + camz;

		// Calculamos el punto real
		t = (-(fx*camx) + (fx*Fx) - (fy*camy) + (fy*Fy) - (fz*camz) + (fz*Fz))/((fx*ux) + (fy*uy) + (fz*uz));
		jderobot::RGBPoint p;

		p.r=(int)imgRGB->data[3*(y*imgRGB->cols+x)];
		p.g=(int)imgRGB->data[3*(y*imgRGB->cols+x)+1];
		p.b=(int)imgRGB->data[3*(y*imgRGB->cols+x)+2];

		p.x=t*ux+camx;
		p.y=t*uy+camy;
		p.z=t*uz+camz;

		//std::cout <<  "coloeres dimensiones! " << p.r << ", " << p.g << ", " << p.b << std::endl;
		//std::cout <<  "punto en todas las dimensiones! " << p.x << ", " << p.y << ", " << p.z << std::endl;

		//for(int i=0; i<(3*width*width); i++) {
		//	int realDepthDist = ((0 << 24)|(0 << 16)|(imgDepth->data[i+1]<<8)|(imgDepth->data[i+2]));
		//	std::cout << realDepthDist << std::endl;
		//}
		//std::terminate();
		/*if (p.x == 0) {
			std::terminate();
		}*/
		//std::cout <<  " ------------------------------------------------- 888888888888888888888888" << std::endl;
		//std::cout <<  "Puntos a 3D: " << x << ", " << y << std::endl;
		//std::cout <<  "punto en todas las dimensiones! " << p.x << ", " << p.y << ", " << p.z << std::endl;
		//float px, py;
		//mypro->myproject(p.x, p.y, p.z, &px, &py, 0);
		//std::cout <<  "Punto en foto " << px << ", " << py << std::endl;
		return p;

	}

	bool Model::isBorderPoint(int x, int y, cv::Mat* imgDepth) {
		//int width = imgDepth->cols;
		int height = imgDepth->rows;

		//std::cout << x <<  "asdf" << y << std::endl;

		int d_aux = 0;
		int distance = 0;

		// 5 pixel window
		std::cout <<  "-------- Window 3x3 --------" << std::endl;
		for(int i=-1; i<2; i++) {
			for(int j=-1; j<2; j++) {
				if (((x+i) >= 0) && ((y+j) >= 0)) {
					int d = ((0 << 24)|(0 << 16)|(imgDepth->data[3*(x+i)+height*(y+j)+1]<<8)|(imgDepth->data[3*(x+i)+height*(y+j)+2]));
					std::cout << d << std::endl;
					if (d != 0) {
						if (d_aux != 0) {
							distance += abs(d - d_aux);
						}
						d_aux = d;
					} else {
						distance += 1000;
					}
				} else {
					//std::cout <<  "borde de la imagen" << std::endl;
				}
			}
		}
		std::cout <<  "Typical deviation ¿?: " << distance << std::endl;
 		std::cout <<  "-------------------------" << std::endl;
		if (distance < 3000) {
			return false;
		} else {
			return true;
		}
	}

	jderobot::RGBPoint Model::findPoint(int x, int y, std::vector<myPoint> points) {
		for (int i=0; i<points.size(); i++) {
			if ((x == points[i].x) && (y == points[i].y)) {
				//std::cout <<  "ENCONTRADO ////////////////////////////////////////////////////////////////" << std::endl;
				return points[i].rgbPoint;
			}
		}
		// Not found -> Default (0,0,0)
		//std::cout <<  "NOOOO ENCONTRADO ////////////////////////////////////////////////////////////////" << std::endl;
		jderobot::RGBPoint p;
		p.x=0;
		p.y=0;
		p.z=0;
		return p;
	}

	bool Model::calculatePoints(cv::String detectionMode, cv::String detectionFilterMode) {
		std::cout <<  "detectionMode: " << detectionMode << std::endl;
		std::cout <<  "detectionFilterMode: " << detectionFilterMode << std::endl;
		if (detectionMode.empty()) {
			return false;
		}
		if (this->_firstIteration) { // Get from image_rgb_aux
			cv::cvtColor(this->imageRGB_aux, this->imageGray_aux, CV_BGR2GRAY);
		} else { // Copy from present
      if (_finishedOk) {
			  this->imageGray.copyTo(this->imageGray_aux);
      }
		}
		cv::cvtColor(this->imageRGB, this->imageGray, CV_BGR2GRAY);

		cv::SiftDescriptorExtractor extractor;
		cv::SiftFeatureDetector detector;
		if (detectionMode.compare("sift") == 0) { // SIFT Detector
		} else if (detectionMode.compare("surf") == 0) { // SURF Detector
			cv::SurfDescriptorExtractor extractor;
		  int minHessian = 400;
		  cv::SurfFeatureDetector detector(minHessian);
		} else {
			return false;
		}

		if (this->_firstIteration) {
			std::vector<cv::KeyPoint> kp_n_aux;
			detector.detect(this->imageGray_aux, kp_n_aux);
			if (detectionFilterMode.compare("borderline") == 0) { // Borderline
				this->keypoints_n_aux.resize(0);
				for (int i=0; i<kp_n_aux.size(); i++) {
					if (!isBorderPoint((int)kp_n_aux[i].pt.x,
														 (int)kp_n_aux[i].pt.y,
														 &this->imageDEPTH_aux)) {
						this->keypoints_n_aux.push_back(kp_n_aux[i]);
					}
				}
			} else {
				this->keypoints_n_aux = kp_n_aux;
			}
			extractor.compute(this->imageGray_aux, this->keypoints_n_aux, this->descriptors_n_aux);
		} else {
      if (_finishedOk) {
  			this->keypoints_n_aux = this->keypoints_n;
  			this->descriptors_n.copyTo(this->descriptors_n_aux);
      }
		}

		std::vector<cv::KeyPoint> kp_n;
		detector.detect(this->imageGray, kp_n);

		if (detectionFilterMode.compare("borderline") == 0) { // Borderline
			this->keypoints_n.resize(0);
			for (int i=0; i<kp_n.size(); i++) {
				if (!isBorderPoint((int)kp_n[i].pt.x,
													 (int)kp_n[i].pt.y,
													 &this->imageDEPTH)) {
					this->keypoints_n.push_back(kp_n[i]);
				}
			}
		} else {
			this->keypoints_n = kp_n;
		}

		extractor.compute(this->imageGray, this->keypoints_n, this->descriptors_n);


		//-- Draw keypointsif (this->finishedOk)
		cv::drawKeypoints(this->imageGray, this->keypoints_n, this->imageRGB_kp, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT );
		cv::drawKeypoints(this->imageGray_aux, this->keypoints_n_aux, this->imageRGB_aux_kp, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT );
		cv::drawKeypoints(this->imageDEPTH_gray, this->keypoints_n, this->imageDEPTH_kp, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT );
		cv::drawKeypoints(this->imageDEPTH_aux_gray, this->keypoints_n_aux, this->imageDEPTH_aux_kp, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT );

		// Save keypoints_prev, if firstTime and news
		if (this->_firstIteration) {
			this->myPrevPoints.resize(0);
			for (int i=0; i<this->keypoints_n_aux.size(); i++) {
				int x = (int)this->keypoints_n_aux[i].pt.x;
				int y = (int)this->keypoints_n_aux[i].pt.y;
				Model::myPoint point_aux;
				point_aux.x = x;
				point_aux.y = y;
				point_aux.rgbPoint = getPoints3D(x, y, &this->imageRGB_aux, &this->imageDEPTH_aux);
				if (point_aux.rgbPoint.z != 0) {
					this->myPrevPoints.push_back(point_aux);
				}
			}
			this->_firstIteration = false;
		} else {
      if (_finishedOk) {
		     this->myPrevPoints = this->myNewPoints;
       }
		}
		std::vector<myPoint> points_aux;
		points_aux.resize(0);
		for (int i=0; i<this->keypoints_n.size(); i++) {
			int x = (int)this->keypoints_n[i].pt.x;
			int y = (int)this->keypoints_n[i].pt.y;
			Model::myPoint point_aux;
			point_aux.x = x;
			point_aux.y = y;
			point_aux.rgbPoint = getPoints3D(x, y, &this->imageRGB, &this->imageDEPTH);
			if (point_aux.rgbPoint.z != 0) {
				points_aux.push_back(point_aux);
			}
		}
		this->myNewPoints = points_aux;

		// Debug
		pc.resize(0);
		for (int i=0; i<this->imageRGB.cols; i+=10) {
			for (int j=0; j<this->imageRGB.rows; j+=10) {
				pc.push_back(getPoints3D(i, j, &this->imageRGB, &this->imageDEPTH));
			}
		}


		return true;
	}

	bool Model::calculateMatching(cv::String matchingMode, cv::String matchingFilterMode, int percentagePoints) {
		std::cout <<  "matchingMode: " << matchingMode << std::endl;
		std::cout <<  "matchingFilterMode: " << matchingFilterMode << std::endl;
		if (matchingMode.empty()) {
			return false;
		}

		std::vector<cv::DMatch> matches;
		matches.resize(0);
		if (matchingMode.compare("bruteforce") == 0) { // brute force matcher
			cv::BruteForceMatcher<cv::L2<float> > matcher;
			if (matchingFilterMode.compare("outstanding") != 0) {
				matcher.match(this->descriptors_n, this->descriptors_n_aux, matches);

			} else { // outstanding filter
				std::vector<std::vector<cv::DMatch> > matches_vector;
				matcher.knnMatch(this->descriptors_n, this->descriptors_n_aux, matches_vector, 2);

				int outNumber = 0;
				for (int i=0; i<matches_vector.size(); i++) {
					//std::cout <<  "-----------------------------------------------------" << std::endl;
					/*
					for (int j=0; j<matches_vector[i].size(); j++) {
						std::cout <<  "--------------------------" << std::endl;
						std::cout <<  "queryIdx" << matches_vector[i][j].queryIdx << std::endl;
						std::cout <<  "trainIdx" << matches_vector[i][j].trainIdx << std::endl;
						std::cout <<  "imgIdx" << matches_vector[i][j].imgIdx << std::endl;
						std::cout <<  "distance" << matches_vector[i][j].distance << std::endl;
					}*/
					outNumber = matches_vector[i][1].distance - matches_vector[i][0].distance;
					if (outNumber >= OUTSTANDING_DISTANCE) {
						matches.push_back(matches_vector[i][0]);
					}
				}
				/*std::cout <<  "CUANTAS:" << matches.size() << std::endl;
				for (int j=0; j<matches.size(); j++) {
					std::cout <<  "queryIdx" << matches[j].queryIdx << std::endl;
					std::cout <<  "trainIdx" << matches[j].trainIdx << std::endl;
					std::cout <<  "imgIdx" << matches[j].imgIdx << std::endl;
					std::cout <<  "distance" << matches[j].distance << std::endl;
				}*/




			}
		} else if (matchingMode.compare("flann") == 0) { // flann matcher
			cv::FlannBasedMatcher matcher;
			matcher.match(this->descriptors_n, this->descriptors_n_aux, matches);
		//} else if (matchingMode.compare("correlation") == 0) { // manual correlation matcher
		}

		// if (matches.size() < 10) {
		// 	return false;
		// }

		//Sort match vector, best first
		std::sort(matches.begin(), matches.end(), sortByDistance);

		/*for(int i=0; i<((int)matches.size()); i++){
			std::cout <<  "matcherrrr: " << matches[i].distance << std::endl;
		}*/

		int matchingPoints_all = matches.size();
		int matchingPoints_best = (int)(((percentagePoints+0.0)/100)*matchingPoints_all+0.49);

		this->v_rgbp.resize(0);
		this->v_rgbp_aux.resize(0);
		std::vector<cv::DMatch> matches_aux;
		matches_aux.resize(0);

		int x_1, y_1, x_2, y_2;
		std::cout <<  "matchingPoints_best-> " << matchingPoints_all << "/" << matchingPoints_best << std::endl;
		for (int i=0; i<matchingPoints_best; i++) {

			x_1 = (int)(this->keypoints_n[matches[i].queryIdx].pt.x);//+0.49f);
			y_1 = (int)(this->keypoints_n[matches[i].queryIdx].pt.y);//+0.49f);
			x_2 = (int)(this->keypoints_n_aux[matches[i].trainIdx].pt.x);//+0.49f);
			y_2 = (int)(this->keypoints_n_aux[matches[i].trainIdx].pt.y);//+0.49f);

			//std::cout <<  "Entramos funcion" << std::endl;
			//std::cout <<  x_1 << ", " << y_1 << ", " << x_2 << ", " << y_2 << std::endl;

			jderobot::RGBPoint p1 = findPoint(x_1, y_1, this->myNewPoints);
			jderobot::RGBPoint p2 = findPoint(x_2, y_2, this->myPrevPoints);
			//jderobot::RGBPoint p1 = getPoints3D(x_1, y_1, &this->imageRGB, &this->imageDEPTH);
			//jderobot::RGBPoint p2 = getPoints3D(x_2, y_2, &this->imageRGB_aux, &this->imageDEPTH_aux);

			//if (!isBorderPoint(x_1, y_1, &this->imageDEPTH) && !isBorderPoint(x_2, y_2, &this->imageDEPTH_aux)) {
			if (p1.z > 0 && p2.z > 0) {
				//std::cout <<  "Points: x_1: " <<  x_1 << ", y_1: " << y_1 << ", x_2: " << x_2 << ", y_2: " << y_2 << std::endl;
				//std::cout <<  "P_1: (" << p1.x << ", " << p1.y << ", " <<  p1.z << ")" << std::endl;
				//std::cout <<  "P_2: (" << p2.x << ", " << p2.y << ", " <<  p2.z << ")" << std::endl;
				this->v_rgbp.push_back(p1);
				this->v_rgbp_aux.push_back(p2);
			}
			matches_aux.push_back(matches[i]);
		}
		std::cout <<  ">>>>>>>>>> maches calculados ---------------------------> " << this->v_rgbp.size() << std::endl;
		cv::drawMatches(this->imageGray, this->keypoints_n, this->imageGray_aux, this->keypoints_n_aux, matches_aux, this->imageRGBMatches);
		cv::drawMatches(this->imageDEPTH_gray, this->keypoints_n, this->imageDEPTH_aux_gray, this->keypoints_n_aux, matches_aux, this->imageDEPTHMatches);

    /* Save data to Gui */
    pthread_mutex_lock(&this->controlVars);
    this->matchingPoints = matchingPoints_best;
    this->totalPoints = matchingPoints_all;
    pthread_mutex_unlock(&this->controlVars);
    /********************/

		return true;
	}

	bool Model::estimateRT(cv::String estimateFilterMode) {
		//pthread_mutex_lock(&this->controlPcConverted);


		// TODO: Comprobar número mínimo de puntos!!!!!!!!!
		// FIXME: Arruba mejor


		int num_points_for_RT = v_rgbp.size();
		std::cout << "The points number for RT calculation is: \n" << num_points_for_RT << std::endl;
    if (num_points_for_RT < 8) {
      _finishedOk = false;
      return false;
    };

		/*Eigen::Vector4f points_ref_2;
		Eigen::Vector4f points_ref_2_ant;
		Eigen::MatrixXf points_ref_2_world(num_points_for_RT, 4);*/
		std::cout << "WEBOO: \n" << "[" << estimateFilterMode << "]" << std::endl;

    // Eigen::MatrixXf points_ref_11(num_points_for_RT, 4);
    // Eigen::MatrixXf points_ref_21(num_points_for_RT, 4);
    // for (int i=0; i<(num_points_for_RT); i++) {
    //   std::cout << (i+1) << " ";
    //   points_ref_11(i,0) = v_rgbp[i].x;
    //   points_ref_11(i,1) = v_rgbp[i].y;
    //   points_ref_11(i,2) = v_rgbp[i].z;
    //   points_ref_11(i,3) = 1;
    //   points_ref_21(i,0) = v_rgbp_aux[i].x;
    //   points_ref_21(i,1) = v_rgbp_aux[i].y;
    //   points_ref_21(i,2) = v_rgbp_aux[i].z;
    //   points_ref_21(i,3) = 1;
    // }
    // std::cout << "." << std::endl;
    // Eigen::Matrix4f RT_estimate111 = points_ref_21.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(points_ref_11).transpose();
    // std::cout << "The estimate RT Matrix 11 is: \n" << "[" << RT_estimate111 << "]" << std::endl;

		// int part = (int)(num_points_for_RT/4);
		// std::cout << "-------------------------------------dd---: \n" << part << std::endl;
		//
		// Eigen::MatrixXf points_ref_1((num_points_for_RT-part-(num_points_for_RT%4)), 4);
		// Eigen::MatrixXf points_ref_2((num_points_for_RT-part-(num_points_for_RT%4)), 4);
		//
		// std::cout << "-------------------------------------dd---: \n" << (num_points_for_RT-part) << std::endl;
		// int count = 0;
		// std::cout << "Points: ";
		// for (int i=0; i<(num_points_for_RT-(num_points_for_RT%4)); i++) {
		// 	if ((i+1) > part) {
		// 		std::cout << (i+1) << " ";
		// 		points_ref_1(count,0) = v_rgbp[i].x;
		// 		points_ref_1(count,1) = v_rgbp[i].y;
		// 		points_ref_1(count,2) = v_rgbp[i].z;
		// 		points_ref_1(count,3) = 1;
		// 		points_ref_2(count,0) = v_rgbp_aux[i].x;
		// 		points_ref_2(count,1) = v_rgbp_aux[i].y;
		// 		points_ref_2(count,2) = v_rgbp_aux[i].z;
		// 		points_ref_2(count,3) = 1;
		// 		count++;
		// 		/*points_ref_2_ant = this->RT_final.inverse()*points_ref_2;
		// 		points_ref_2_world(i,0) = points_ref_2_ant(0);
		// 		points_ref_2_world(i,1) = points_ref_2_ant(1);
		// 		points_ref_2_world(i,2) = points_ref_2_ant(2);
		// 		points_ref_2_world(i,3) = 1;
		// 		std::cout << "v_rgbp_aux[i]" << std::endl;*/
		// 		//std::cout << "--------zz------" << std::endl;
		// 		//std::cout << v_rgbp[i].x << ", " << v_rgbp[i].y << ", " << v_rgbp[i].z << std::endl;
		// 		//std::cout << v_rgbp_aux[i].x << ", " << v_rgbp_aux[i].y << ", " << v_rgbp_aux[i].z << std::endl;
		// 	}
		// }
		// std::cout << "." << std::endl;
		// Eigen::Matrix4f RT_estimate1 = points_ref_2.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(points_ref_1).transpose();
		// std::cout << "The estimate RT Matrix 1 is: \n" << "[" << RT_estimate1 << "]" << std::endl;
		// count = 0;
		// std::cout << "Points: ";
		// for (int i=0; i<(num_points_for_RT-(num_points_for_RT%4)); i++) {
		// 	if (((i+1) > (part*2)) || ((i+1) <= part)) {
		// 		std::cout << (i+1) << " ";
		// 		points_ref_1(count,0) = v_rgbp[i].x;
		// 		points_ref_1(count,1) = v_rgbp[i].y;
		// 		points_ref_1(count,2) = v_rgbp[i].z;
		// 		points_ref_1(count,3) = 1;
		// 		points_ref_2(count,0) = v_rgbp_aux[i].x;
		// 		points_ref_2(count,1) = v_rgbp_aux[i].y;
		// 		points_ref_2(count,2) = v_rgbp_aux[i].z;
		// 		points_ref_2(count,3) = 1;				count++;
		// 		/*points_ref_2_ant = this->RT_final.inverse()*points_ref_2;
		// 		points_ref_2_world(i,0) = points_ref_2_ant(0);
		// 		points_ref_2_world(i,1) = points_ref_2_ant(1);
		// 		points_ref_2_world(i,2) = points_ref_2_ant(2);
		// 		points_ref_2_world(i,3) = 1;
		// 		std::cout << "v_rgbp_aux[i]" << std::endl;*/
		// 		//std::cout << "--------zz------" << std::endl;
		// 		//std::cout << v_rgbp[i].x << ", " << v_rgbp[i].y << ", " << v_rgbp[i].z << std::endl;
		// 		//std::cout << v_rgbp_aux[i].x << ", " << v_rgbp_aux[i].y << ", " << v_rgbp_aux[i].z << std::endl;
		// 	}
		// }
		// std::cout << "." << std::endl;
		// Eigen::Matrix4f RT_estimate2 = points_ref_2.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(points_ref_1).transpose();
		// std::cout << "The estimate RT Matrix 2 is: \n" << "[" << RT_estimate2 << "]" << std::endl;
		// count = 0;
		// std::cout << "Points: ";
		// for (int i=0; i<(num_points_for_RT-(num_points_for_RT%4)); i++) {
		// 	if (((i+1) > (part*3)) || ((i+1) <= (part*2))) {
		// 		std::cout << (i+1) << " ";
		// 		points_ref_1(count,0) = v_rgbp[i].x;
		// 		points_ref_1(count,1) = v_rgbp[i].y;
		// 		points_ref_1(count,2) = v_rgbp[i].z;
		// 		points_ref_1(count,3) = 1;
		// 		points_ref_2(count,0) = v_rgbp_aux[i].x;
		// 		points_ref_2(count,1) = v_rgbp_aux[i].y;
		// 		points_ref_2(count,2) = v_rgbp_aux[i].z;
		// 		points_ref_2(count,3) = 1;				count++;
		// 		/*points_ref_2_ant = this->RT_final.inverse()*points_ref_2;
		// 		points_ref_2_world(i,0) = points_ref_2_ant(0);
		// 		points_ref_2_world(i,1) = points_ref_2_ant(1);
		// 		points_ref_2_world(i,2) = points_ref_2_ant(2);
		// 		points_ref_2_world(i,3) = 1;
		// 		std::cout << "v_rgbp_aux[i]" << std::endl;*/
		// 		//std::cout << "--------zz------" << std::endl;
		// 		//std::cout << v_rgbp[i].x << ", " << v_rgbp[i].y << ", " << v_rgbp[i].z << std::endl;
		// 		//std::cout << v_rgbp_aux[i].x << ", " << v_rgbp_aux[i].y << ", " << v_rgbp_aux[i].z << std::endl;
		// 	}
		// }
		// std::cout << "." << std::endl;
		// Eigen::Matrix4f RT_estimate3 = points_ref_2.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(points_ref_1).transpose();
		// std::cout << "The estimate RT Matrix 3 is: \n" << "[" << RT_estimate3 << "]" << std::endl;
		// count = 0;
		// std::cout << "Points: ";
		// for (int i=0; i<(num_points_for_RT-(num_points_for_RT%4)); i++) {
		// 	if ((i+1) <= (part*3)) {
		// 		std::cout << (i+1) << " ";
		// 		points_ref_1(count,0) = v_rgbp[i].x;
		// 		points_ref_1(count,1) = v_rgbp[i].y;
		// 		points_ref_1(count,2) = v_rgbp[i].z;
		// 		points_ref_1(count,3) = 1;
		// 		points_ref_2(count,0) = v_rgbp_aux[i].x;
		// 		points_ref_2(count,1) = v_rgbp_aux[i].y;
		// 		points_ref_2(count,2) = v_rgbp_aux[i].z;
		// 		points_ref_2(count,3) = 1;				count++;
		// 		/*points_ref_2_ant = this->RT_final.inverse()*points_ref_2;
		// 		points_ref_2_world(i,0) = points_ref_2_ant(0);
		// 		points_ref_2_world(i,1) = points_ref_2_ant(1);
		// 		points_ref_2_world(i,2) = points_ref_2_ant(2);
		// 		points_ref_2_world(i,3) = 1;
		// 		std::cout << "v_rgbp_aux[i]" << std::endl;*/
		// 		//std::cout << "--------zz------" << std::endl;
		// 		//std::cout << v_rgbp[i].x << ", " << v_rgbp[i].y << ", " << v_rgbp[i].z << std::endl;
		// 		//std::cout << v_rgbp_aux[i].x << ", " << v_rgbp_aux[i].y << ", " << v_rgbp_aux[i].z << std::endl;
		// 	}
		// }
		// std::cout << "." << std::endl;
		//
		// 	//std::cout << "buuu: \n" << points_ref_1 << " --------- " << points_ref_2 << std::endl;
		//
		// 	//JacobiSVD<MatrixXf> svd(points_ref_1, Eigen::ComputeThinU | Eigen::ComputeThinV);
		// 	//std::cout << "The estimate RT Matrix is: \n" << svd.solve(points_ref_2) << std::endl;
		//
		//
		// 	Eigen::Matrix4f RT_estimate4 = points_ref_2.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(points_ref_1).transpose();
		//
		// 	std::cout << "The estimate RT Matrix 4 is: \n" << "[" << RT_estimate4 << "]" << std::endl;


      Eigen::Matrix4f RT_estimate_final;

      // Euclidean error
      float euclidean_error = 0;

      if (estimateFilterMode.compare("ransac") == 0) { // RANSAC
        // RANSAC ///////////////////////////////////////////////////////////////
        int ransac_points = (int)(v_rgbp.size() * (1-RANSAC_PERC));
        std::vector<jderobot::RGBPoint> points, points_aux;

        srand (time(NULL));
        int best_error = 1000;

        for (int i=0; i<RANSAC_ITER; i++) {
          points.resize(0);
          points_aux.resize(0);

          for (int i=0; i<v_rgbp.size(); i++) {
            points.push_back(v_rgbp[i]);
            points_aux.push_back(v_rgbp_aux[i]);
          }
        	//std::cout << "Tpoints: " << "[" << points[2].x << "]" << points.size() << std::endl;
          //std::cout << "Tv_rgbp: " << "[" << v_rgbp[2].x << "]" << v_rgbp.size() << std::endl;
          /*for (int i=0; i<points.size(); i++) {
            std::cout << "puntos de antes: " << i << " . " << points[i].x << ", " << points[i].y << ", " << points[i].z << std::endl;
          }*/
          for (int i=0; i<ransac_points; i++) {
            int secret = rand() % points.size();
            //std::cout << "--------> Número aleatorio: " << "[" << secret << "]" << points.size() << std::endl;
            points.erase(points.begin()+secret);
            points_aux.erase(points_aux.begin()+secret);
          }
          /*for (int i=0; i<points.size(); i++) {
            std::cout << "puntos de despues: " << i << " . " << points[i].x << ", " << points[i].y << ", " << points[i].z << std::endl;
          }*/
          Eigen::MatrixXf matrix_points(points.size(), 4);
          Eigen::MatrixXf matrix_points_aux(points.size(), 4);
          for (int i=0; i<points.size(); i++) {
            matrix_points(i,0) = points[i].x;
            matrix_points(i,1) = points[i].y;
            matrix_points(i,2) = points[i].z;
            matrix_points(i,3) = 1;
            matrix_points_aux(i,0) = points_aux[i].x;
            matrix_points_aux(i,1) = points_aux[i].y;
            matrix_points_aux(i,2) = points_aux[i].z;
            matrix_points_aux(i,3) = 1;
          }
          Eigen::Matrix4f RT_estimate_ransac = matrix_points_aux.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(matrix_points).transpose();

          //Eigen::JacobiSVD<Eigen::MatrixXf> svd(matrix_points_aux, Eigen::ComputeThinU | Eigen::ComputeThinV);
          //Eigen::Matrix4f RT_estimate_ransac = svd.solve(matrix_points).transpose();
          std::cout << "The estimate RT Matrix in RANSAC: \n" << "[" << RT_estimate_ransac << "]" << std::endl;
          //std::cout << "The estimate RT Matrix in RANSAC later : \n" << "[" << RT_estimate_ransac2 << "]" << std::endl;

    			Eigen::Vector4f vector_points;
  			  Eigen::Vector4f vector_points_world;

          for (int i=0; i<points.size(); i++) {
            vector_points(0) = points[i].x;
            vector_points(1) = points[i].y;
            vector_points(2) = points[i].z;
            vector_points(3) = 1;

            vector_points_world = RT_estimate_ransac.inverse()*vector_points;

            euclidean_error += abs(vector_points_world(0) - points_aux[i].x);
            euclidean_error += abs(vector_points_world(1) - points_aux[i].y);
            euclidean_error += abs(vector_points_world(2) - points_aux[i].z);
          }
          euclidean_error /= points.size();
          std::cout << "ERROR ESPACIAL EUCLÍDEO RANSAC: " << euclidean_error << std::endl;

          if (euclidean_error < best_error) {
            best_error = euclidean_error;
            RT_estimate_final = RT_estimate_ransac;
          }
        }
  			std::cout << "The FINAL RT Matrix is: \n" << "[" << RT_estimate_final << "]" << std::endl;
        std::cout << "ERROR ESPACIAL EUCLÍDEO FINAL RANSAC: " << best_error << std::endl;
        // RANSAC ///////////////////////////////////////////////////////////////




      } else {
				Eigen::MatrixXf points_ref_1(num_points_for_RT, 4);
				Eigen::MatrixXf points_ref_2(num_points_for_RT, 4);
				for (int i=0; i<(num_points_for_RT); i++) {
					std::cout << (i+1) << " ";
					points_ref_1(i,0) = v_rgbp[i].x;
					points_ref_1(i,1) = v_rgbp[i].y;
					points_ref_1(i,2) = v_rgbp[i].z;
					points_ref_1(i,3) = 1;
					points_ref_2(i,0) = v_rgbp_aux[i].x;
					points_ref_2(i,1) = v_rgbp_aux[i].y;
					points_ref_2(i,2) = v_rgbp_aux[i].z;
					points_ref_2(i,3) = 1;
				}
				std::cout << "." << std::endl;
				RT_estimate_final = points_ref_2.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(points_ref_1).transpose();
				std::cout << "The estimate RT Matrix 11 is: \n" << "[" << RT_estimate_final << "]" << std::endl;

        Eigen::Vector4f points_ref_aux;
        Eigen::Vector4f points_ref_1_world;

        /* Medimos error espacial euclídeo */
        for (int i=0; i<this->v_rgbp.size(); i++) {
          points_ref_aux(0) = this->v_rgbp[i].x;
          points_ref_aux(1) = this->v_rgbp[i].y;
          points_ref_aux(2) = this->v_rgbp[i].z;
          points_ref_aux(3) = 1;

          points_ref_1_world = RT_estimate_final.inverse()*points_ref_aux;

          euclidean_error += abs(points_ref_1_world(0) - this->v_rgbp_aux[i].x);
          euclidean_error += abs(points_ref_1_world(1) - this->v_rgbp_aux[i].y);
          euclidean_error += abs(points_ref_1_world(2) - this->v_rgbp_aux[i].z);
        }
        euclidean_error /= v_rgbp.size();
        std::cout << "ERROR ESPACIAL EUCLÍDEO (no ransac): " << euclidean_error << std::endl;
      }
      //Eigen::Matrix4f RT_final = RT_estimate4;
      if (euclidean_error > 50) {
        _finishedOk = false;
        return false;
      };
      pthread_mutex_lock(&this->controlVars);
      this->euclidean_err = euclidean_error;
      pthread_mutex_unlock(&this->controlVars);

      Eigen::Vector4f pc_aux;
      Eigen::Vector4f pc_world;
      // Nube de puntos con los datos de los puntos de interés ///
			for (int i=0; i<this->myNewPoints.size(); i++) {
				pc_aux(0) = this->myNewPoints[i].rgbPoint.x;
				pc_aux(1) = this->myNewPoints[i].rgbPoint.y;
				pc_aux(2) = this->myNewPoints[i].rgbPoint.z;
				pc_aux(3) = 1;

				pc_world = RT_estimate_final.inverse()*pc_aux;

				this->myNewPoints[i].rgbPoint.x = pc_world(0);
				this->myNewPoints[i].rgbPoint.y = pc_world(1);
				this->myNewPoints[i].rgbPoint.z = pc_world(2);
			}

      // Nube de puntos para pintar en el visor gráfico ///
			this->pc_converted.resize(pc.size());
			for (int i=0; i<this->pc.size(); i++) {
				pc_aux(0) = this->pc[i].x;
				pc_aux(1) = this->pc[i].y;
				pc_aux(2) = this->pc[i].z;
				pc_aux(3) = 1;

				pc_world = RT_estimate_final.inverse()*pc_aux;

				this->pc[i].x = pc_world(0);
				this->pc[i].y = pc_world(1);
				this->pc[i].z = pc_world(2);

				this->pc_converted[i] = this->pc[i];
			}

			/*for(int i=0; i<num_points_for_RT; i++){
				points_ref_aux(0) = points_ref_1(i,0);
				points_ref_aux(1) = points_ref_1(i,1);
				points_ref_aux(2) = points_ref_1(i,2);
				points_ref_aux(3) = 1;

				points_ref_1_world = RT_estimate.inverse()*points_ref_aux;
				//points_ref_1_aux = RT_final.inverse()*points_ref_2_aux;
				//points_ref_1_aux = RT_final*points_ref_2_aux;
				//std::cout << "asdfasdfasdfasdftrix is: \n" << points_ref_2_aux << std::endl;
				this->pc_converted[i].x = points_ref_1_aux(0);
				this->pc_converted[i].y = points_ref_1_aux(1);
				this->pc_converted[i].z = points_ref_1_aux(2);
				//this->pc_converted[i].r = this->pc[i].r;
				//this->pc_converted[i].g = this->pc[i].g;
				//this->pc_converted[i].b = this->pc[i].b;
				std::cout << "ANTES DE CONVERTIR: \n" << points_ref_1_world(i,0) << ", " << points_ref_1_world(i,1) << ", " << points_ref_1_world(i,2) << std::endl;
				std::cout << "FINAL >>>>>>>: \n" << this->pc_converted[i].x << ", " << this->pc_converted[i].y << ", " << this->pc_converted[i].z << std::endl;
				if (this->iterationCloud == 0) {
					this->pc_converted[i].r = 255;
					this->pc_converted[i].g = 0;
					this->pc_converted[i].b = 0;
				}
				if (this->iterationCloud == 1) {
					this->pc_converted[i].r = 0;
					this->pc_converted[i].g = 255;
					this->pc_converted[i].b = 0;
				}
				if (this->iterationCloud == 2) {
					this->pc_converted[i].r = 0;
					this->pc_converted[i].g = 0;
					this->pc_converted[i].b = 255;
				}
				if (this->iterationCloud == 3) {
					this->pc_converted[i].r = 255;
					this->pc_converted[i].g = 255;
					this->pc_converted[i].b = 0;
				}
				if (this->iterationCloud == 4) {
					this->pc_converted[i].r = 0;
					this->pc_converted[i].g = 255;
					this->pc_converted[i].b = 255;
				}
				if (this->iterationCloud == 5) {
					this->pc_converted[i].r = 255;
					this->pc_converted[i].g = 0;
					this->pc_converted[i].b = 255;
				}
				if (this->iterationCloud == 6) {
					this->pc_converted[i].r = 255;
					this->pc_converted[i].g = 255;
					this->pc_converted[i].b = 255;
				}

			}
			this->iterationCloud++;*/

			// Camera camera converted
			moveCamera(RT_estimate_final);
			std::cout << "ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ: \n" << std::endl;
			/*for(int i=0; i<16; i++) {
				this->RT_final(i) = RT_estimate(i);
			}*/
		//}

		/*
		for(int i=0; i<num_points_for_RT; i++){
			this->pointCloud_estimate->points[i+num_points_for_RT].x = this->pointCloud->points[i].x;
			this->pointCloud_estimate->points[i+num_points_for_RT].y = this->pointCloud->points[i].y;
			this->pointCloud_estimate->points[i+num_points_for_RT].z = this->pointCloud->points[i].z;
			this->pointCloud_estimate->points[i].r = this->pointCloud->points[i].r;
			this->pointCloud_estimate->points[i].g = this->pointCloud->points[i].g;
			this->pointCloud_estimate->points[i].b = this->pointCloud->points[i].b;
			this->pointCloud_estimate->points[i+num_points_for_RT].r = (int)0.3*this->pointCloud->points[i].r;
			this->pointCloud_estimate->points[i+num_points_for_RT].g = (int)0.3*this->pointCloud->points[i].g;
			this->pointCloud_estimate->points[i+num_points_for_RT].b = (int)0.3*this->pointCloud->points[i].b;
		}*/

		//this->is_final = true;
		//pthread_mutex_unlock(&this->controlPcConverted);

		// Save image n to n-1 if sucess estimate
		// FIXME: LOCK
		//this->imageRGB.copyTo(this->imageRGB_aux);
		//this->imageDEPTH.copyTo(this->imageDEPTH_aux);

    // Sabe last RT matrix
    pthread_mutex_lock(&this->controlRTMatrix);
    this->RT_final = RT_estimate_final;
    pthread_mutex_unlock(&this->controlRTMatrix);

    _finishedOk = true;
    return true;
	}

	bool Model::isEstimated() {
		return (this->iterationCloud > 0);
	}

	void Model::RotateXAxis() {
		Eigen::Matrix4f RT_estimate;
		RT_estimate << 1, 0, 			 0, 			 0,
									 0, cos(30), -sin(30), 0,
									 0, sin(30), cos(30),  0,
									 0, 0, 			 0, 			 1;
		moveCamera(RT_estimate);
	}

	void Model::RotateYAxis() {
		Eigen::Matrix4f RT_estimate;
		RT_estimate << cos(30),  0, sin(30), 0,
									 0, 			 1, 0, 			 0,
									 -sin(30), 0, cos(30), 0,
									 0, 			 0, 0, 			 1;
		moveCamera(RT_estimate);
	}

	void Model::moveDownRT() {
		Eigen::Matrix4f RT_estimate;
		RT_estimate << 1, 0, 0, -10,
									 0, 1, 0, 0,
									 0, 0, 1, -10,
									 0, 0, 0, 1;
	  moveCamera(RT_estimate);
	}

	void Model::moveUpRT() {
		Eigen::Matrix4f RT_estimate;
		RT_estimate << 1, 0, 0, 10,
									 0, 1, 0, 0,
									 0, 0, 1, 10,
									 0, 0, 0, 1;
		moveCamera(RT_estimate);
	}

	void Model::moveCamera(Eigen::Matrix4f RT_estimate) {
		//std::cout << "The estimate RT Matrix is: \n" << "[" << RT_estimate << "]" << std::endl;

		//this->RT_final = this->RT_final*RT_estimate;

		//std::cout << "The FINAL RT Matrix is: \n" << "[" << RT_final << "]" << std::endl;

    Eigen::Matrix4f R_matrix;
		R_matrix << RT_estimate(0,0), RT_estimate(0,1), RT_estimate(0,2), 0,
									 RT_estimate(1,0), RT_estimate(1,1), RT_estimate(1,2), 0,
									 RT_estimate(2,0), RT_estimate(2,1), RT_estimate(2,2), 0,
									 0, 0, 0, 1;

    Eigen::Matrix4f T_matrix;
    T_matrix << 1, 0, 0, RT_estimate(0,3),
									 0, 1, 0, RT_estimate(1,3),
									 0, 0, 1, RT_estimate(2,3),
									 0, 0, 0, 1;




    Eigen::Vector4f points_ref_1_aux;
		Eigen::Vector4f points_ref_2_aux;




		// Camera camera converted
    pthread_mutex_lock(&this->controlCamera);
		this->pc_camera_converted.resize(pc_camera.size());
		for(int i=0; i<pc_camera.size(); i++) {
			points_ref_2_aux(0) = this->pc_camera[i].x;
			points_ref_2_aux(1) = this->pc_camera[i].y;
			points_ref_2_aux(2) = this->pc_camera[i].z;
			points_ref_2_aux(3) = 1;

			points_ref_2_aux = R_matrix.inverse()*points_ref_2_aux;
      points_ref_1_aux = T_matrix.inverse()*points_ref_2_aux;

			this->pc_camera_converted[i].x = points_ref_1_aux(0);
			this->pc_camera_converted[i].y = points_ref_1_aux(1);
			this->pc_camera_converted[i].z = points_ref_1_aux(2);
		}
    pthread_mutex_unlock(&this->controlCamera);
	}
}
