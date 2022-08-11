/**
 * @file /src/main_window.cpp
 *
 * @brief Implementation for the qt gui.
 *
 * @date February 2011
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include "../include/simple_gui/main_window.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace simple_gui {

int ros_topic_data;
bool ros_status_flag = 0;

bool ros_status_flag_cmd = 0;

QString q_command_string;

extern int State[8];
//extern int Arm_State[5];
extern int Ready;
extern QImage qt_image;
extern QImage qt_image_gripper;

using namespace Qt;
//using namespace Ui;

/*****************************************************************************
** Implementation [MainWindow]
*****************************************************************************/

MainWindow::MainWindow(int argc, char** argv, QWidget *parent)
        : QMainWindow(parent)
	, qnode(argc,argv)
{

	ui.setupUi(this); // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.
  //ui.Button_nuc1_screenshot->setCheckable(true);

  dialog = new Sc_Dialog; //add
  //dialog->setModal(true); //add
  //dialog->show();//add

    QObject::connect(ui.actionAbout_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt())); // qApp is a global variable for the application

    ReadSettings();
	setWindowIcon(QIcon(":/images/icon.png"));
	ui.tab_manager->setCurrentIndex(0); // ensure the first tab is showing - qt-designer should have this already hardwired, but often loses it (settings?).
    QObject::connect(&qnode, SIGNAL(rosShutdown()), this, SLOT(close()));

	/*********************
	** Logging
	**********************/
  //ui.view_logging->setModel(qnode.loggingModel());
    //QObject::connect(&qnode, SIGNAL(loggingUpdated()), this, SLOT(updateLoggingView()));

    /*********************
    ** Auto Start
    **********************/
    //if ( ui.checkbox_remember_settings->isChecked() ) {
    //    on_button_connect_clicked(true);
    //}

    QObject::connect(&qnode, SIGNAL(statusUpdated()), this, SLOT(updateState()));
    QObject::connect(&qnode, SIGNAL(statusUpdated_sc()), this, SLOT(updateState_sc())); //add
    QObject::connect(&qnode, SIGNAL(statusUpdated()), this, SLOT(getReady()));

    //QObject::connect(ui.Button_getmission, SIGNAL(clicked()), this, SLOT(launch_getmission()));
    //QObject::connect(ui.Button_Web, SIGNAL(clicked()), this, SLOT(Refresh_Web()));
/*
    QObject::connect(ui.Button_AutoDriving, SIGNAL(clicked()), this, SLOT(AutoDriving()));
    QObject::connect(ui.Button_Door, SIGNAL(clicked()), this, SLOT(Door()));
    QObject::connect(ui.Button_Obstacle, SIGNAL(clicked()), this, SLOT(Obstacle()));
    QObject::connect(ui.Button_Parking, SIGNAL(clicked()), this, SLOT(Parking()));
    QObject::connect(ui.Button_Stair, SIGNAL(clicked()), this, SLOT(Stair()));

    QObject::connect(ui.Button_md, SIGNAL(clicked()), this, SLOT(MD()));
    QObject::connect(ui.Button_Joy, SIGNAL(clicked()), this, SLOT(JOY()));

    QObject::connect(ui.Button_Start, SIGNAL(clicked()), this, SLOT(Start()));    
    QObject::connect(ui.Button_All_stop, SIGNAL(clicked()), this, SLOT(All_stop()));

    QObject::connect(ui.Off_AutoDriving, SIGNAL(clicked()), this, SLOT(Off_auto()));
    QObject::connect(ui.Off_Door, SIGNAL(clicked()), this, SLOT(Off_door()));
    QObject::connect(ui.Off_Obstacle, SIGNAL(clicked()), this, SLOT(Off_obstacle()));
    QObject::connect(ui.Off_Parking, SIGNAL(clicked()), this, SLOT(Off_parking()));
    QObject::connect(ui.Off_Stair, SIGNAL(clicked()), this, SLOT(Off_stair()));

    QObject::connect(ui.Off_md, SIGNAL(clicked()), this, SLOT(Off_MD()));
    QObject::connect(ui.Off_Joy, SIGNAL(clicked()), this, SLOT(Off_JOY()));
    */

    QObject::connect(ui.Button_Front_CAM, SIGNAL(clicked()), this, SLOT(Front_CAM()));   //Front CAM
    QObject::connect(ui.Off_Front_CAM, SIGNAL(clicked()), this, SLOT(Off_Front_CAM()));  //Front CAM OFf
/*
    QObject::connect(ui.Button_Back_CAM, SIGNAL(clicked()), this, SLOT(Back_CAM()));   //Back CAM
    QObject::connect(ui.Off_Back_CAM, SIGNAL(clicked()), this, SLOT(Off_Back_CAM()));  //Back CAM Off

    QObject::connect(ui.Button_Front_and_Back_CAM, SIGNAL(clicked()), this, SLOT(Front_and_Back_CAM()));   //Back CAM
    QObject::connect(ui.Off_Front_and_Back_CAM, SIGNAL(clicked()), this, SLOT(Off_Front_and_Back_CAM()));  //Back CAM Off
*/
    //Sub_Pub_again(nuc2_connect)
  //  QObject::connect(ui.Button_NUC2_Connect, SIGNAL(clicked()), this, SLOT(nuc2_conncet()));
    //ARM
    /*
    QObject::connect(ui.Button_Arm, SIGNAL(clicked()), this, SLOT(Arm()));
    QObject::connect(ui.Button_Arm_joy, SIGNAL(clicked()), this, SLOT(Arm_joy()));
    QObject::connect(ui.Button_Arm_key, SIGNAL(clicked()), this, SLOT(Arm_key()));
    QObject::connect(ui.Button_Arm_service, SIGNAL(clicked()), this, SLOT(Arm_service()));

    QObject::connect(ui.Button_teleop_keyboard_pub, SIGNAL(clicked()), this, SLOT(Arm_teleopkey_topicpub()));

    QObject::connect(ui.Off_Arm, SIGNAL(clicked()), this, SLOT(Off_Arm()));
    QObject::connect(ui.OFf_Arm_joy, SIGNAL(clicked()), this, SLOT(Off_Arm_joy()));
    QObject::connect(ui.Off_Arm_key, SIGNAL(clicked()), this, SLOT(Off_Arm_key()));
    QObject::connect(ui.Off_Arm_service, SIGNAL(clicked()), this, SLOT(Off_Arm_service()));
    */

    //COMM
    QObject::connect(ui.Button_html, SIGNAL(clicked()), this, SLOT(Html()));
    QObject::connect(ui.Button_controll_pc_websocket, SIGNAL(clicked()), this, SLOT(controll_pc_websocket()));
    QObject::connect(ui.Button_Edit_html, SIGNAL(clicked()), this, SLOT(Edit_html()));

    //Teleoperation
    QObject::connect(ui.rtsp_client_button, SIGNAL(clicked()), this, SLOT(RTSP()));
    QObject::connect(ui.joy_node_button, SIGNAL(clicked()), this, SLOT(Joy_node()));
    QObject::connect(ui.rtsp_off_button, SIGNAL(clicked()), this, SLOT(RTSP_off()));
    QObject::connect(ui.joy_off_button, SIGNAL(clicked()), this, SLOT(JOY_off()));


    //Screenshot
    /*
    QObject::connect(ui.Button_nuc1_screenshot, SIGNAL(clicked(bool)), this, SLOT(NUC1_screenshot_clicked(bool)));
    QObject::connect(ui.Button_nuc2_screenshot, SIGNAL(clicked(bool)), this, SLOT(NUC2_screenshot_clicked(bool)));
*/

    //command_linedeit
   // QObject::connect(ui.command_lineEdit, SIGNAL(testChanged(QString)), this, SLOT((testChanged(QString))));
   // QObject::connect(ui.Button_Cmd_go, SIGNAL(clicked(bool)), this, SLOT(Button_Cmd_go_clicked()));




    /*********************
    ** Label
    **********************/

   m_lightimg[0].load(":/images/led-off.png");
   m_lightimg[1].load(":/images/led-on.png");

   m_readyimg[0].load(":/images/switch2.jpg");
   m_readyimg[1].load(":/images/switch1.jpg");

   KUDOS_img.load(":/images/KUDOS2.png");


   cat1_img.load(":/images/cat1.jpg");
   cat2_img.load(":/images/cat2.jpg");

   ui.label_7->setPixmap(cat1_img);
   ui.label_9->setPixmap(cat2_img);
   //ui.label_11->setPixmap(KUDOS_img);

   qnode.init(); //ola


}


MainWindow::~MainWindow() {}

/*****************************************************************************
** Implementation [Slots]
*****************************************************************************/

void MainWindow::showNoMasterMessage() {
	QMessageBox msgBox;
	msgBox.setText("Couldn't find the ros master.");
	msgBox.exec();
    close();
}

/*
 * These triggers whenever the button is clicked, regardless of whether it
 * is already checked or not.
 */

//void MainWindow::on_button_connect_clicked(bool check ) {
//	if ( ui.checkbox_use_environment->isChecked() ) {
//		if ( !qnode.init() ) {
//			showNoMasterMessage();
//		} else {
//			ui.button_connect->setEnabled(false);
//		}
//  }
//  //else {
//    //if ( ! qnode.init(ui.line_edit_master->text().toStdString(),
//    //		   ui.line_edit_host->text().toStdString()) ) {
//    //	showNoMasterMessage();
//    //} else {
//      //ui.button_connect->setEnabled(false);
//      //ui.line_edit_master->setReadOnly(true);
//      //ui.line_edit_host->setReadOnly(true);
//      //ui.line_edit_topic->setReadOnly(true);
//    //}
//  //}
//}
//

//void MainWindow::on_checkbox_use_environment_stateChanged(int state) {
//	bool enabled;
//	if ( state == 0 ) {
//		enabled = true;
//	} else {
//		enabled = false;
//	}
//  //ui.line_edit_master->setEnabled(enabled);
//  //ui.line_edit_host->setEnabled(enabled);
//	//ui.line_edit_topic->setEnabled(enabled);
//}

/*****************************************************************************
** Implemenation [Slots][manually connected]
*****************************************************************************/

/**
 * This function is signalled by the underlying model. When the model changes,
 * this will drop the cursor down to the last line in the QListview to ensure
 * the user can always see the latest log message.
 */
//void MainWindow::updateLoggingView() {
//        ui.view_logging->scrollToBottom();
//}

void MainWindow::updateState() {
  /*
    if(State[0] == 1){
        ui.state_label_1->setPixmap(m_lightimg[1]);
    }
    else{
        ui.state_label_1->setPixmap(m_lightimg[0]);
    }

    if(State[1] == 1){
        ui.state_label_2->setPixmap(m_lightimg[1]);
    }
    else{
        ui.state_label_2->setPixmap(m_lightimg[0]);
    }

    if(State[2] == 1){
        ui.state_label_3->setPixmap(m_lightimg[1]);
    }
    else{
        ui.state_label_3->setPixmap(m_lightimg[0]);
    }

    if(State[3] == 1){
        ui.state_label_4->setPixmap(m_lightimg[1]);
    }
    else{
        ui.state_label_4->setPixmap(m_lightimg[0]);
    }

    if(State[4] == 1){
        ui.state_label_5->setPixmap(m_lightimg[1]);
    }
    else{
        ui.state_label_5->setPixmap(m_lightimg[0]);
    }

    if(State[5] == 1){
        ui.state_label_6->setPixmap(m_lightimg[1]);
    }
    else{
        ui.state_label_6->setPixmap(m_lightimg[0]);
    }

    if(State[6] == 1){
        ui.state_label_7->setPixmap(m_lightimg[1]);
    }
    else{
        ui.state_label_7->setPixmap(m_lightimg[0]);
    }
    if(State[7] == 1){
      ui.state_label_8->setPixmap(m_lightimg[0]);
      ui.state_label_9->setPixmap(m_lightimg[1]);
    }
    else{
      ui.state_label_8->setPixmap(m_lightimg[1]);
      ui.state_label_9->setPixmap(m_lightimg[0]);
    }
    */
    //Arm
    /*
    if(Arm_State[0] == 1){
        ui.state_label_arm->setPixmap(m_lightimg[1]);
    }
    else{
        ui.state_label_arm->setPixmap(m_lightimg[0]);
    }

    if(Arm_State[1] == 1){
        ui.status_label_arm_joy->setPixmap(m_lightimg[1]);
    }
    else{
        ui.status_label_arm_joy->setPixmap(m_lightimg[0]);
    }

    if(Arm_State[2] == 1){
        ui.status_label_arm_key->setPixmap(m_lightimg[1]);
    }
    else{
        ui.status_label_arm_key->setPixmap(m_lightimg[0]);
    }

    if(Arm_State[3] == 1){
        ui.status_label_arm_service->setPixmap(m_lightimg[1]);
    }
    else{
        ui.status_label_arm_service->setPixmap(m_lightimg[0]);
    }

    if(Arm_State[4] == 1){
        ui.state_label_nuc2->setPixmap(m_lightimg[1]);
    }
    else{
        ui.state_label_nuc2->setPixmap(m_lightimg[0]);
    }

*/

    ui.label_7->setPixmap(QPixmap::fromImage(qt_image));
    ui.label_7->resize(ui.label_7->pixmap()->size());

    ui.label_9->setPixmap(QPixmap::fromImage(qt_image_gripper));
    ui.label_9->resize(ui.label_9->pixmap()->size());

}

void MainWindow::updateState_sc(){
    dialog->setWindowTitle("NUC Screen");
    dialog->show();//add
    // dialog->show_screenshot();//add
}

void MainWindow::getReady() {
    if(Ready == 1){
        ui.get_ready->setPixmap(m_readyimg[1]);
    }
    else{
        ui.get_ready->setPixmap(m_readyimg[0]);
    }
}

/*
void MainWindow::AutoDriving() {
    ros_topic_data = 1;
    ros_status_flag = true;

    ui.mode_ready -> setText("Auto Driving");
}

void MainWindow::Door() {
    ros_topic_data = 2;
    ros_status_flag = true;

    ui.mode_ready -> setText("Open Door");
}

void MainWindow::Obstacle() {
    ros_topic_data = 3;
    ros_status_flag = true;

    ui.mode_ready -> setText("Obstacle");
}

void MainWindow::Parking() {
    ros_topic_data = 4;
    ros_status_flag = true;

    ui.mode_ready -> setText("Parking");
}

void MainWindow::Stair() {
    ros_topic_data = 5;
    ros_status_flag = true;

    ui.mode_ready -> setText("Stair");
}

void MainWindow::MD() {
    ros_topic_data = 60;
    ros_status_flag = true;
}

void MainWindow::JOY() {
    ros_topic_data = 70;
    ros_status_flag = true;
}

void MainWindow::Start() {
    ros_topic_data = 0;
    ros_status_flag = true;
}

void MainWindow::All_stop() {
    ros_topic_data = 9;
    ros_status_flag = true;
}
*/

void MainWindow::Front_CAM()
{
  ros_topic_data = 20;
  ros_status_flag = true;
}
/*
void MainWindow::Back_CAM(){
    ros_topic_data = 30;
    ros_status_flag = true;
}

void MainWindow::Front_and_Back_CAM()
{
  ros_topic_data = 40;
  ros_status_flag = true;
}
*/

//Sub_Pub again(nuc2 conncet)

/*void MainWindow::nuc2_conncet()
{
  ros_topic_data = 99;
  ros_status_flag = true;
}
*/
/*
void MainWindow::Arm()
{
  ros_topic_data = 100;
  ros_status_flag = true;
}

void MainWindow::Arm_joy()
{
  ros_topic_data = 110;
  ros_status_flag = true;
}

void MainWindow::Arm_key()
{
  ros_topic_data = 120;
  ros_status_flag = true;
}

void MainWindow::Arm_service()
{
  ros_topic_data = 130;
  ros_status_flag = true;
}

void MainWindow::Arm_teleopkey_topicpub()
{
  std::string command_topicpub = "gnome-terminal -- rosrun manipulator_key_topicpub manipulator_key_topicpub_node";
  const char *t_html = command_topicpub.c_str();
  system(t_html);
}
*/

//COMM
void MainWindow::Html()
{
  //ROS_INFO("Html");
  std::string command_html = "gnome-terminal -- firefox ~/catkin_ws_ola/src/roslibjs/examples/simple_gui.html";
  const char *c_html = command_html.c_str();
  system(c_html);
}
void MainWindow::controll_pc_websocket()
{
  std::string command_web = "gnome-terminal -- roslaunch rosbridge_server rosbridge_websocket.launch";
  const char *c_web = command_web.c_str();
  system(c_web);
}

void MainWindow::Edit_html()
{
  std::string command_edit = "gedit ~/catkin_ws_ola/src/roslibjs/examples/simple_gui.html";
  const char *c_edit = command_edit.c_str();
  system(c_edit);
}

void MainWindow::Joy_node()
{
  std::string command_edit = "gnome-terminal -- rosrun joy joy_node";
  const char *c_edit = command_edit.c_str();
  system(c_edit);
}

void MainWindow::RTSP()
{
  std::string command_edit = "gnome-terminal -- roslaunch rtsp_to_ros rtsp_to_ros.launch";
  const char *c_edit = command_edit.c_str();
  system(c_edit);
}

void MainWindow::RTSP_off()
{
  std::string command_edit = "rosnode kill /rtsp_to_ros_node";
  const char *c_edit = command_edit.c_str();
  system(c_edit);
}

void MainWindow::JOY_off()
{
  std::string command_edit = "rosnode kill /joy_node";
  const char *c_edit = command_edit.c_str();
  system(c_edit);
}
//Screenshot
/*
void MainWindow::NUC1_screenshot_clicked(bool checked)
{
  if(checked == true){
    ros_topic_data = 1000;
    ros_status_flag = true;
    //dialog->show();//add
  }
  else{
    dialog->close();//add
  }

}
*/
/*
void MainWindow::NUC2_screenshot_clicked(bool checked)
{
  if(checked == true){
    ros_topic_data = 2000;
    ros_status_flag = true;
    //dialog->show();//add
  }
  else{
    dialog->close();//add

  }
}
*/
/*
void MainWindow::Button_Cmd_go_clicked()
{
  q_command_string = ui.command_lineEdit->text();
  ros_status_flag_cmd = true;
}
*/

/*****************************************************************************
** Stop the selected thing
*****************************************************************************/
/*
void MainWindow::Off_auto() {
    ros_topic_data = 11;
    ros_status_flag = true;
}

void MainWindow::Off_door() {
    ros_topic_data = 12;
    ros_status_flag = true;
}

void MainWindow::Off_obstacle() {
    ros_topic_data = 13;
    ros_status_flag = true;
}

void MainWindow::Off_parking() {
    ros_topic_data = 14;
    ros_status_flag = true;
}

void MainWindow::Off_stair() {
    ros_topic_data = 15;
    ros_status_flag = true;
}

void MainWindow::Off_MD() {     //md_driver
    ros_topic_data = 61;
    ros_status_flag = true;
}

void MainWindow::Off_JOY() {     //md_driver
    ros_topic_data = 71;
    ros_status_flag = true;
}
*/
void MainWindow::Off_Front_CAM()
{
  ros_topic_data = 21;
  ros_status_flag = true;
}
/*
void MainWindow::Off_Back_CAM(){
    ros_topic_data = 31;
    ros_status_flag = true;
}

void MainWindow::Off_Front_and_Back_CAM()
{
  ros_topic_data = 41;
  ros_status_flag = true;
}
/*
void MainWindow::Off_Arm()
{
  ros_topic_data = 101;
  ros_status_flag = true;
}

void MainWindow::Off_Arm_joy()
{
  ros_topic_data = 111;
  ros_status_flag = true;
}

void MainWindow::Off_Arm_key()
{
  ros_topic_data = 121;
  ros_status_flag = true;
}

void MainWindow::Off_Arm_service()
{
  ros_topic_data = 131;
  ros_status_flag = true;
}
*/


/*****************************************************************************
** Implementation [Menu]
*****************************************************************************/

void MainWindow::on_actionAbout_triggered() {
    QMessageBox::about(this, tr("About ..."),tr("<h2>PACKAGE_NAME Test Program 0.10</h2><p>Copyright Yujin Robot</p><p>This package needs an about description.</p>"));
}

/*****************************************************************************
** Implementation [Configuration]
*****************************************************************************/

void MainWindow::ReadSettings() {
    QSettings settings("Qt-Ros Package", "simple_gui");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    //QString master_url = settings.value("master_url",QString("http://10.30.94.232:11311/")).toString();
    QString master_url = QString("http://localhost:11311/");
    //QString host_url = settings.value("host_url", QString("10.30.93.206")).toString();
    QString host_url = QString("localhost");
    //QString topic_name = settings.value("topic_name", QString("/chatter")).toString();
    //ui.line_edit_master->setText(master_url);
    //ui.line_edit_host->setText(host_url);
    //ui.line_edit_topic->setText(topic_name);
    //bool remember = settings.value("remember_settings", false).toBool();
   // ui.checkbox_remember_settings->setChecked(remember);
    //bool checked = settings.value("use_environment_variables", false).toBool();
    //ui.checkbox_use_environment->setChecked(checked);
    //if ( checked ) {
      //ui.line_edit_master->setEnabled(false);
      //ui.line_edit_host->setEnabled(false);
    	//ui.line_edit_topic->setEnabled(false);
    //}
}

void MainWindow::WriteSettings() {
    QSettings settings("Qt-Ros Package", "simple_gui");
    //settings.setValue("master_url",ui.line_edit_master->text());
    ///settings.setValue("host_url",ui.line_edit_host->text());
    settings.setValue("master_url","http://localhost:11311/");
    settings.setValue("host_url","localhost");

    //settings.setValue("topic_name",ui.line_edit_topic->text());
    //settings.setValue("use_environment_variables",QVariant(ui.checkbox_use_environment->isChecked()));
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    //settings.setValue("remember_settings",QVariant(ui.checkbox_remember_settings->isChecked()));

}

void MainWindow::closeEvent(QCloseEvent *event)
{
	WriteSettings();
	QMainWindow::closeEvent(event);
}

}  // namespace launchgui

