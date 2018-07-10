#include "ros/ros.h"
#include <iostream>
#include "geometry_msgs/PoseStamped.h"
#include "std_msgs/Float64.h"
#include <signal.h>

geometry_msgs::PoseStamped *PoseStamped_copy = new geometry_msgs::PoseStamped;

void PoseCallback(const geometry_msgs::PoseStamped::ConstPtr& PoseStamped)
{
    PoseStamped_copy->pose.orientation.x = PoseStamped->pose.orientation.x;
    PoseStamped_copy->pose.orientation.y = PoseStamped->pose.orientation.y;
    PoseStamped_copy->pose.orientation.z = PoseStamped->pose.orientation.z;
    ROS_INFO("%f", PoseStamped_copy->pose.orientation.x);
}  

int main(int argc, char **argv)
{
    ros::init(argc, argv, "pose2command_turn_ready");
    ros::NodeHandle nh;
    
    ros::Subscriber get_pose = nh.subscribe("/aruco_single/pose", 100, PoseCallback);

    ros::spin();

    return 0;
}