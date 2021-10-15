/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date February 2011
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Bool.h>
#include <sstream>
#include "../include/launchgui/qnode.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/


QImage qt_image_screenshot; //add

namespace launchgui {

/*****************************************************************************
** Implementation
*****************************************************************************/

int State[7];
int Arm_State[4];
int Ready;
QImage qt_image;
QImage qt_image_gripper;
//QImage qt_image_screenshot; //add

extern int ros_topic_data;
extern bool ros_status_flag;

std_msgs::UInt16 msg;

std_msgs::UInt16 Autodriving_state;
std_msgs::UInt16 Door_state;
std_msgs::UInt16 Obstacle_state;
std_msgs::UInt16 Parking_state;
std_msgs::UInt16 Stair_state;

QNode::QNode(int argc, char** argv ) :
	init_argc(argc),
	init_argv(argv)
	{}

QNode::~QNode() {
    if(ros::isStarted()) {
      ros::shutdown(); // explicitly needed since we use ros::start();
      ros::waitForShutdown();
    }
	wait();
}

bool QNode::init() {
	ros::init(init_argc,init_argv,"launchgui");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;
	// Add your ros communications here.
        mission_publisher = n.advertise<std_msgs::UInt16>("mission", 1);

        get_Ready_subscriber = n.subscribe("getmission_ready", 1000, &QNode::getready_Callback, this);

        A_state_subscriber = n.subscribe("autodriving_state", 1000,  &QNode::A_state_Callback, this); //mission state
        D_state_subscriber = n.subscribe("door_state", 1000,  &QNode::D_state_Callback, this);
        O_state_subscriber = n.subscribe("obstacle_state", 1000,  &QNode::O_state_Callback, this);
        S_state_subscriber = n.subscribe("stair_state", 1000,  &QNode::S_state_Callback, this);
        P_state_subscriber = n.subscribe("parking_state", 1000,  &QNode::P_state_Callback, this);
        MD_state_subscriber = n.subscribe("md_driver_status", 1000, &QNode::MD_state_Callback, this);
        JOY_state_subscriber = n.subscribe("rosjoy_status", 1000, &QNode::JOY_state_Callback, this);
        Front_Image_subscriber = n.subscribe("/usb_cam/image_raw",1000,&QNode::Front_ImageCb, this);
        Gripper_Image_subscriber = n.subscribe("/realsence_cam/image_raw",1000,&QNode::Gripper_ImageCb, this);
        //Arm
        Arm_status_subscriber = n.subscribe("/arm_status/arm",1000,&QNode::Arm_status_Callback, this);
        Arm_joy_status_subscriber = n.subscribe("/arm_status/joy",1000,&QNode::Arm_joy_status_Callback, this);
        Arm_key_status_subscriber = n.subscribe("/arm_status/key",1000,&QNode::Arm_key_status_Callback, this);
        Arm_service_status_subscriber = n.subscribe("/arm_status/service",1000,&QNode::Arm_service_status_Callback, this);
        screenshot_subscriber = n.subscribe("/screenshot/image_raw",1000,&QNode::screenshot_Callback, this);
        //screenshot
	start();
	return true;
}

bool QNode::init(const std::string &master_url, const std::string &host_url) {
	std::map<std::string,std::string> remappings;
	remappings["__master"] = master_url;
	remappings["__hostname"] = host_url;
	ros::init(remappings,"launchgui");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;
	// Add your ros communications here.
        mission_publisher = n.advertise<std_msgs::UInt16>("mission", 1);

        get_Ready_subscriber = n.subscribe("getmission_ready", 1000, &QNode::getready_Callback, this);

        A_state_subscriber = n.subscribe("autodriving_state", 1000,  &QNode::A_state_Callback, this); //mission state
        D_state_subscriber = n.subscribe("door_state", 1000,  &QNode::D_state_Callback, this);
        O_state_subscriber = n.subscribe("obstacle_state", 1000,  &QNode::O_state_Callback, this);
        S_state_subscriber = n.subscribe("stair_state", 1000,  &QNode::S_state_Callback, this);
        P_state_subscriber = n.subscribe("parking_state", 1000,  &QNode::P_state_Callback, this);
        MD_state_subscriber = n.subscribe("md_driver_status", 1000, &QNode::MD_state_Callback, this);
        JOY_state_subscriber = n.subscribe("rosjoy_status", 1000, &QNode::JOY_state_Callback, this);
        Front_Image_subscriber = n.subscribe("/usb_cam/image_raw",1000,&QNode::Front_ImageCb, this);
        Gripper_Image_subscriber = n.subscribe("/realsence_cam/image_raw",1000,&QNode::Gripper_ImageCb, this);
        //Arm
        Arm_status_subscriber = n.subscribe("/arm_status/arm",1000,&QNode::Arm_status_Callback, this);
        Arm_joy_status_subscriber = n.subscribe("/arm_status/joy",1000,&QNode::Arm_joy_status_Callback, this);
        Arm_key_status_subscriber = n.subscribe("/arm_status/key",1000,&QNode::Arm_key_status_Callback, this);
        Arm_service_status_subscriber = n.subscribe("/arm_status/service",1000,&QNode::Arm_service_status_Callback, this);
        //screenshot
        screenshot_subscriber = n.subscribe("/screenshot/image_raw",1000,&QNode::screenshot_Callback, this);

	start();
	return true;
}

void QNode::run() {
        ros::Rate loop_rate(30);
        ros::NodeHandle n;
        //image_transport::ImageTransport it(n);

        get_Ready_subscriber = n.subscribe("getmission_ready", 1000, &QNode::getready_Callback, this);

        A_state_subscriber = n.subscribe("autodriving_state", 1000,  &QNode::A_state_Callback, this); //mission state
        D_state_subscriber = n.subscribe("door_state", 1000,  &QNode::D_state_Callback, this);
        O_state_subscriber = n.subscribe("obstacle_state", 1000,  &QNode::O_state_Callback, this);
        S_state_subscriber = n.subscribe("stair_state", 1000,  &QNode::S_state_Callback, this);
        P_state_subscriber = n.subscribe("parking_state", 1000,  &QNode::P_state_Callback, this);
        MD_state_subscriber = n.subscribe("md_driver_status", 1000, &QNode::MD_state_Callback, this);
        JOY_state_subscriber = n.subscribe("rosjoy_status", 1000, &QNode::JOY_state_Callback, this);
        Front_Image_subscriber = n.subscribe("/usb_cam/image_raw",1000,&QNode::Front_ImageCb, this);
        Gripper_Image_subscriber = n.subscribe("/realsence_cam/image_raw",1000,&QNode::Gripper_ImageCb, this);
        //Front_Image_subscriber2 = it.subscribe("/image_raw/compressed",1000,&QNode::Front_ImageCb,image_transport::TransportHints("compressed"),this);

        //Arm
        Arm_status_subscriber = n.subscribe("/arm_status/arm",1000,&QNode::Arm_status_Callback, this);
        Arm_joy_status_subscriber = n.subscribe("/arm_status/joy",1000,&QNode::Arm_joy_status_Callback, this);
        Arm_key_status_subscriber = n.subscribe("/arm_status/key",1000,&QNode::Arm_key_status_Callback, this);
        Arm_service_status_subscriber = n.subscribe("/arm_status/service",1000,&QNode::Arm_service_status_Callback, this);
        //screenshot
        screenshot_subscriber = n.subscribe("/screenshot/image_raw",1000,&QNode::screenshot_Callback, this);

  //int count = 0;
        while ( ros::ok() ) {
            if(ros_status_flag == true) {
                msg.data = ros_topic_data;
                mission_publisher.publish(msg);
                ros_status_flag = false;
            }
            blackout(0);

            ros::spinOnce();
            loop_rate.sleep();
            //++count;
        }

        State[0] = 0;

        //ros::spin();
	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
	Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}

void QNode::A_state_Callback(const std_msgs::UInt16& state_msg){
    State[0] = state_msg.data;
        Q_EMIT statusUpdated();
}

void QNode::D_state_Callback(const std_msgs::UInt16& state_msg){
    State[1] = state_msg.data;
        Q_EMIT statusUpdated();
}

void QNode::O_state_Callback(const std_msgs::UInt16& state_msg){
    State[2] = state_msg.data;
        Q_EMIT statusUpdated();
}

void QNode::P_state_Callback(const std_msgs::UInt16& state_msg){
    State[3] = state_msg.data;
        Q_EMIT statusUpdated();
}

void QNode::S_state_Callback(const std_msgs::UInt16& state_msg){
    State[4] = state_msg.data;
        Q_EMIT statusUpdated();
}

void QNode::MD_state_Callback(const std_msgs::UInt16& state_msg){
    State[5] = state_msg.data;
        Q_EMIT statusUpdated();
}

void QNode::JOY_state_Callback(const std_msgs::UInt16& state_msg){
    State[6] = state_msg.data;
        Q_EMIT statusUpdated();
}

void QNode::blackout(int a){

    for(int i=0; i<7; i++) {
        State[i] = a;
    }
    Ready = a;

    Q_EMIT statusUpdated();
}

void QNode::getready_Callback(const std_msgs::UInt16& ready){
    Ready = ready.data;
        Q_EMIT statusUpdated();
}

void QNode::Front_ImageCb(const sensor_msgs::ImageConstPtr& msg){ //ImageConstPtr
  cv_bridge::CvImagePtr cv_ptr;
  try{
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::RGB8);
   }
   catch (cv_bridge::Exception& e){
     ROS_ERROR("cv_bridge exception: %s", e.what());
     return;
   }
   cv::Mat frame2 = cv_ptr->image;
   //cv::Mat frame2;
   //cv::resize(frame,frame2,cv::Size(640, 480),0,0,cv::INTER_CUBIC);
   //cv::Mat frame3 = cv_ptr->image;
   //cv::Mat frame4;
   //cv::cvtColor(frame3, frame4, cv::COLOR_RGB2BGR);
   //cv::cvShowImage("Received Image", &frame);
   //cv::imshow("aaaa",frame);
   int WIDTH = 320*(2);
   int HEIGHT = 180*(2);
   qt_image = QImage((const unsigned char*)(frame2.data),frame2.cols,frame2.rows,QImage::Format_RGB888).scaled(WIDTH,HEIGHT,Qt::KeepAspectRatio, Qt::SmoothTransformation);
  //qt_image = qt_image.scaled(600,500,Qt::KeepAspectRatio, Qt::SmoothTransformation);
   Q_EMIT statusUpdated();

}

void QNode::Gripper_ImageCb(const sensor_msgs::ImageConstPtr& msg){ //ImageConstPtr
  cv_bridge::CvImagePtr cv_ptr;
  try{
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::RGB8);
   }
   catch (cv_bridge::Exception& e){
     ROS_ERROR("cv_bridge exception: %s", e.what());
     return;
   }
   cv::Mat frame2 = cv_ptr->image;
   //cv::Mat frame2;
   //cv::resize(frame,frame2,cv::Size(640, 480),0,0,cv::INTER_CUBIC);
   //cv::Mat frame3 = cv_ptr->image;
   //cv::Mat frame4;
   //cv::cvtColor(frame3, frame4, cv::COLOR_RGB2BGR);
   //cv::cvShowImage("Received Image", &frame);
   //cv::imshow("aaaa",frame);
   int WIDTH = 320*(1.8);
   int HEIGHT = 180*(1.8);
   qt_image_gripper = QImage((const unsigned char*)(frame2.data),frame2.cols,frame2.rows,QImage::Format_RGB888).scaled(WIDTH,HEIGHT,Qt::KeepAspectRatio, Qt::SmoothTransformation);
  //qt_image = qt_image.scaled(600,500,Qt::KeepAspectRatio, Qt::SmoothTransformation);
   Q_EMIT statusUpdated_sc();

}

void QNode::Arm_status_Callback(const std_msgs::UInt16& state_msg){
    Arm_State[0] = state_msg.data;
        Q_EMIT statusUpdated();
}
void QNode::Arm_joy_status_Callback(const std_msgs::UInt16& state_msg){
    Arm_State[1] = state_msg.data;
        Q_EMIT statusUpdated();
}
void QNode::Arm_key_status_Callback(const std_msgs::UInt16& state_msg){
    Arm_State[2] = state_msg.data;
        Q_EMIT statusUpdated();
}
void QNode::Arm_service_status_Callback(const std_msgs::UInt16& state_msg){
    Arm_State[3] = state_msg.data;
        Q_EMIT statusUpdated();
}

void QNode::screenshot_Callback(const sensor_msgs::Image& msg){
  cv_bridge::CvImagePtr cv_ptr;
  try{
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::RGB8);
   }
   catch (cv_bridge::Exception& e){
     ROS_ERROR("cv_bridge exception: %s", e.what());
     return;
   }
   cv::Mat frame2 = cv_ptr->image;
   //cv::Mat frame2;
   //cv::resize(frame,frame2,cv::Size(640, 480),0,0,cv::INTER_CUBIC);
   //cv::Mat frame3 = cv_ptr->image;
   //cv::Mat frame4;
   //cv::cvtColor(frame3, frame4, cv::COLOR_RGB2BGR);
   //cv::cvShowImage("Received Image", &frame);
   //cv::imshow("aaaa",frame);
   int WIDTH = 320*(3);
   int HEIGHT = 180*(3);
   qt_image_screenshot = QImage((const unsigned char*)(frame2.data),frame2.cols,frame2.rows,QImage::Format_RGB888).scaled(WIDTH,HEIGHT,Qt::KeepAspectRatio, Qt::SmoothTransformation);
  //qt_image = qt_image.scaled(600,500,Qt::KeepAspectRatio, Qt::SmoothTransformation);
   Q_EMIT statusUpdated_sc();
}



void QNode::log( const LogLevel &level, const std::string &msg) {
	logging_model.insertRows(logging_model.rowCount(),1);
	std::stringstream logging_model_msg;
	switch ( level ) {
		case(Debug) : {
				ROS_DEBUG_STREAM(msg);
				logging_model_msg << "[DEBUG] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Info) : {
				ROS_INFO_STREAM(msg);
				logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Warn) : {
				ROS_WARN_STREAM(msg);
				logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Error) : {
				ROS_ERROR_STREAM(msg);
				logging_model_msg << "[ERROR] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Fatal) : {
				ROS_FATAL_STREAM(msg);
				logging_model_msg << "[FATAL] [" << ros::Time::now() << "]: " << msg;
				break;
		}
	}
	QVariant new_row(QString(logging_model_msg.str().c_str()));
	logging_model.setData(logging_model.index(logging_model.rowCount()-1),new_row);
	Q_EMIT loggingUpdated(); // used to readjust the scrollbar
}

}  // namespace launchgui


//md_driver_status int number 1
