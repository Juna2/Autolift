#include "ros/ros.h"
#include <iostream>
#include "geometry_msgs/PoseStamped.h"
#include "std_msgs/Float64.h"

std_msgs::Float64 degree;
std_msgs::Float64 degree2;
std_msgs::Float64 vel;
std_msgs::Float64 vel2;

ros::Publisher give_degree;
ros::Publisher give_degree2;
ros::Publisher give_vel;
ros::Publisher give_vel2;

void msgCallback(const geometry_msgs::PoseStamped::ConstPtr& PoseStamped)
{
    //ROS_INFO("Point x = %f", PoseStamped->pose.position.x);

    if(PoseStamped->pose.orientation.x > 0.01)
    {
        degree.data = 1;
        degree2.data = 1;
        vel.data = 1.0;
        vel2.data = -1.0;
    }
    else if(PoseStamped->pose.orientation.x <= 0.08 && PoseStamped->pose.orientation.x >= - 0.01)
    {
        vel.data = 0.0;
        vel2.data = 0.0;
    }
    else if(PoseStamped->pose.orientation.x < - 0.01)
    {
        degree.data = -1;
        degree2.data = -1;
        vel.data = 1.0;
        vel2.data = -1.0;
    }
    
    give_degree.publish(degree);
    give_degree2.publish(degree2);
    //ros::Duration(1).sleep();
    give_vel.publish(vel);
    give_vel2.publish(vel2);

}

int main(int argc, char **argv)
{
    
    ros::init(argc, argv, "pose2command");

    ros::NodeHandle nh;

    ros::Subscriber get_pose = nh.subscribe("/aruco_single/pose", 100, msgCallback);
    give_degree = nh.advertise<std_msgs::Float64>("/tilt_controller/command", 50);
    give_degree2 = nh.advertise<std_msgs::Float64>("/tilt_controller2/command", 50);
    give_vel = nh.advertise<std_msgs::Float64>("/pan_controller3/command", 50);
    give_vel2 = nh.advertise<std_msgs::Float64>("/pan_controller4/command", 50);
    
    ros::spin();

    return 0;

}