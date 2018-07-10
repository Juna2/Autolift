#include "ros/ros.h"
#include <iostream>
#include "geometry_msgs/PoseStamped.h"
#include "std_msgs/Float64.h"
#include "dynamixel_msgs/JointState.h"

#define OFFSET -0.052333
#define AVER_NUM 10

std_msgs::Float64 *degree = new std_msgs::Float64;
std_msgs::Float64 *degree2 = new std_msgs::Float64;
std_msgs::Float64 *vel = new std_msgs::Float64;
std_msgs::Float64 *vel2 = new std_msgs::Float64;
std_msgs::Float64 *vel3 = new std_msgs::Float64;
std_msgs::Float64 *vel4 = new std_msgs::Float64;

double orientation_x = 1;

double current_pos = 1;
double current_pos2 = 1;
double current_pos3 = 0;
double current_pos3_wait = 0;
double average_num;
int current_pos3_count = 0;

void PoseCallback(const geometry_msgs::PoseStamped::ConstPtr& PoseStamped);
void PositionCallback_1(const dynamixel_msgs::JointState::ConstPtr& Joint);    
void PositionCallback_2(const dynamixel_msgs::JointState::ConstPtr& Joint);    
void PositionCallback_3(const dynamixel_msgs::JointState::ConstPtr& Joint);    


int main(int argc, char **argv)
{
    ros::init(argc, argv, "pose2command");
    ros::NodeHandle nh;
    
    ros::Subscriber get_pose = nh.subscribe("/aruco_single/pose", 100, PoseCallback);
    ros::Subscriber get_position = nh.subscribe("/tilt_controller/state", 100, PositionCallback_1);
    ros::Subscriber get_position2 = nh.subscribe("/tilt_controller2/state", 100, PositionCallback_2);
    ros::Subscriber get_position3 = nh.subscribe("/pan_controller5/state", 100, PositionCallback_3);

    ros::Publisher give_degree = nh.advertise<std_msgs::Float64>("/tilt_controller/command", 50);
    ros::Publisher give_degree2 = nh.advertise<std_msgs::Float64>("/tilt_controller2/command", 50);
    ros::Publisher give_vel = nh.advertise<std_msgs::Float64>("/pan_controller3/command", 50);
    ros::Publisher give_vel2 = nh.advertise<std_msgs::Float64>("/pan_controller4/command", 50);
    ros::Publisher give_vel3 = nh.advertise<std_msgs::Float64>("/pan_controller5/command", 50);
    ros::Publisher give_vel4 = nh.advertise<std_msgs::Float64>("/pan_controller6/command", 50);

    degree->data = 0.0;
    degree2->data = 0.0;
    vel->data = 0.0;
    vel2->data = 0.0;
    vel3->data = 0.0;
    vel4->data = 0.0;

    ros::Rate loop_rate(50);
    
    while(ros::ok())
    {
        //nh.getParam("average_num", average_num);
        //degree->data = (degree->data) + OFFSET;
        //degree2->data = (degree2->data) + OFFSET;
        nh.getParam("degree", degree->data);
        nh.getParam("degree2", degree2->data);
        nh.getParam("vel", vel->data);
        nh.getParam("vel2", vel2->data);
        nh.getParam("vel3", vel3->data);
        nh.getParam("vel4", vel4->data);

        give_degree.publish(*degree);
        give_degree2.publish(*degree2);
        give_vel.publish(*vel);
        give_vel2.publish(*vel2);
        give_vel3.publish(*vel3);
        give_vel4.publish(*vel4);

        nh.setParam("orientation_x", orientation_x);

        nh.setParam("current_pos", current_pos);
        nh.setParam("current_pos2", current_pos2);
        nh.setParam("current_pos3", current_pos3);

        //ROS_INFO("degree : %f", degree.data);
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}

void PoseCallback(const geometry_msgs::PoseStamped::ConstPtr& PoseStamped)
{
    orientation_x = PoseStamped->pose.orientation.x;
    //ROS_INFO("%f", orientation_x);
}

void PositionCallback_1(const dynamixel_msgs::JointState::ConstPtr& Joint)
{
    current_pos = Joint->current_pos;
}
void PositionCallback_2(const dynamixel_msgs::JointState::ConstPtr& Joint)
{
    current_pos2 = Joint->current_pos;
}
void PositionCallback_3(const dynamixel_msgs::JointState::ConstPtr& Joint)
{
    /*current_pos3_wait += Joint->current_pos;
    current_pos3_count++;
    if(current_pos3_count == AVER_NUM)
    {
        //ROS_INFO("current_pos3 : %f, current_pos3_wait : %f, average_num : %f", current_pos3);
        current_pos3 = current_pos3_wait / AVER_NUM - 2.6;
        current_pos3_wait = 0;
        current_pos3_count = 0;
    }
    ROS_INFO("current_pos3 : %f", current_pos3);*/
    current_pos3 = Joint->current_pos - 2.6;
}