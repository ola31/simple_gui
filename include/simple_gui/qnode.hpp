/**
 * @file /include/simple_gui/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date February 2011
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef simple_gui_QNODE_HPP_
#define simple_gui_QNODE_HPP_

/*****************************************************************************
** Includes
*****************************************************************************/

// To workaround boost/qt4 problems that won't be bugfixed. Refer to
//    https://bugreports.qt.io/browse/QTBUG-22829
#ifndef Q_MOC_RUN
#include <ros/ros.h>

#include <string>
#include <QThread>
#include <QStringListModel>
#include <std_msgs/UInt16.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Bool.h>
//#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
//#include <image_transport/image_transport.h>
#endif


/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace simple_gui {

/*****************************************************************************
** Class
*****************************************************************************/

class QNode : public QThread {
    Q_OBJECT
public:
	QNode(int argc, char** argv );
	virtual ~QNode();
	bool init();
	bool init(const std::string &master_url, const std::string &host_url);
	void run();
/*
        void A_state_Callback(const std_msgs::UInt16& state_msg);
        void D_state_Callback(const std_msgs::UInt16& state_msg);
        void O_state_Callback(const std_msgs::UInt16& state_msg);
        void P_state_Callback(const std_msgs::UInt16& state_msg);
        void S_state_Callback(const std_msgs::UInt16& state_msg);
        void MD_state_Callback(const std_msgs::UInt16& state_msg);
        void JOY_state_Callback(const std_msgs::UInt16& state_msg);
        */
        void blackout(int a);
        void getready_Callback(const std_msgs::UInt16& ready);

        void Front_ImageCb(const sensor_msgs::ImageConstPtr& msg);
        void Gripper_ImageCb(const sensor_msgs::ImageConstPtr& msg);

        //Arm
        /*
        void nuc_status_Callback(const std_msgs::Bool& state_msg);
        void Arm_status_Callback(const std_msgs::Bool& state_msg);
        void Arm_joy_status_Callback(const std_msgs::Bool& state_msg);
        void Arm_key_status_Callback(const std_msgs::Bool& state_msg);
        void Arm_service_status_Callback(const std_msgs::Bool& state_msg);
        void screenshot_Callback(const sensor_msgs::Image& msg);
        */

        void teleop_onoff_Callback(const std_msgs::Int8& msg);


	/*********************
	** Logging
	**********************/
	enum LogLevel {
	         Debug,
	         Info,
	         Warn,
	         Error,
	         Fatal
	 };

	QStringListModel* loggingModel() { return &logging_model; }
	void log( const LogLevel &level, const std::string &msg);

Q_SIGNALS:
	void loggingUpdated();
    void rosShutdown();

    void statusUpdated();
    void statusUpdated_sc(); //add

private:
	int init_argc;
	char** init_argv;
        ros::Publisher mission_publisher;
        ros::Publisher command_publisher;

        ros::Subscriber get_Ready_subscriber;
/*
        ros::Subscriber A_state_subscriber;
        ros::Subscriber D_state_subscriber;
        ros::Subscriber O_state_subscriber;
        ros::Subscriber S_state_subscriber;
        ros::Subscriber P_state_subscriber;
        ros::Subscriber MD_state_subscriber;
        ros::Subscriber JOY_state_subscriber;
        */
        ros::Subscriber Front_Image_subscriber;
        ros::Subscriber Gripper_Image_subscriber;
        //image_transport::Subscriber Front_Image_subscriber2;

      //  ros::Subscriber nuc2_status_subscriber;
/*
        ros::Subscriber Arm_status_subscriber;
        ros::Subscriber Arm_joy_status_subscriber;
        ros::Subscriber Arm_key_status_subscriber;
        ros::Subscriber Arm_service_status_subscriber;
        ros::Subscriber screenshot_subscriber;
*/
       // ros::Subscriber teleop_onoff_subscriber;


        QStringListModel logging_model;
};

}  // namespace simple_gui

#endif /* simple_gui_QNODE_HPP_ */
