#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Joy.h"
#include "std_msgs/Float64MultiArray.h"

ros::Publisher pub;
float pre_joint_l = 0.0;
float pre_joint_r = 0.0;
float pre_velocity = 0.0;
float pre_angular = 0.0;

float send_joint_l = 0.0;
float send_joint_r = 0.0;
float send_velocity = 0.0;
float send_angular = 0.0;

bool stop_flag = false;

void joyCallback(const sensor_msgs::Joy::ConstPtr& msg)
{
  if(msg->buttons[4])
    stop_flag = true;
  
  pre_velocity = msg->axes[1];
  pre_angular = -msg->axes[0];
  
  if(msg->buttons[6]){
    pre_joint_l = msg->axes[3];
  }
  
  if(msg->buttons[7]){
    pre_joint_r = msg->axes[3];
  }
  
}

int main(int argc, char **argv)
{

  ros::init(argc, argv, "joystick_listener");

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("joy", 10, joyCallback);
  
  pub = n.advertise<std_msgs::Float64MultiArray>("Wheel", 10);
  
  ros::Rate loop_rate(10);
  
  while (ros::ok())
  {
    std_msgs::Float64MultiArray wheel;
    send_velocity = pre_velocity;
    send_angular = pre_angular;
    if(send_velocity >= 1)
      send_velocity = 1;
    else if(send_velocity <= -1)
      send_velocity = -1;
  
    if(send_angular >= 1)
      send_angular = 1;
    else if(send_angular <= -1)
      send_angular = -1;
    
    send_joint_l += 0.02*pre_joint_l;
    send_joint_r += 0.02*pre_joint_r;
    if(send_joint_l >= 0.9)
      send_joint_l = 0.9;
    else if(send_joint_l <= 0.3)
      send_joint_l = 0.3;
  
    if(send_joint_r >= 0.9)
      send_joint_r = 0.9;
    else if(send_joint_r <= 0.3)
      send_joint_r = 0.3;
    
    if(stop_flag){
      send_velocity = 0;
      send_angular = 0;
      send_joint_l = 0;
      send_joint_r = 0;
      stop_flag = false;
    }
    wheel.data.push_back(send_velocity);
    wheel.data.push_back(send_angular);
    wheel.data.push_back(send_joint_l);
    wheel.data.push_back(send_joint_r);
    
    pub.publish(wheel);

    ros::spinOnce();

    loop_rate.sleep();
  }

  return 0;
}