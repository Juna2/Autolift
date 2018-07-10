#include "ros/ros.h"
#include <iostream>
#include "geometry_msgs/PoseStamped.h"
#include "std_msgs/Float64.h"

std_msgs::Float64 vel1;
std_msgs::Float64 vel2;

ros::Publisher give_vel;
ros::Publisher give_vel2;

void msgCallback(const geometry_msgs::PoseStamped::ConstPtr& PoseStamped)
{
    //ROS_INFO("Point x = %f", PoseStamped->pose.position.x);

    if(PoseStamped->pose.position.z > 0.5)
    {
        vel1.data = 3.0;
        vel2.data = -3.0;
    }
    else if(PoseStamped->pose.position.z <= 0.5 && PoseStamped->pose.position.z >= 0.3)
    {
        vel1.data = 0.0;
        vel2.data = 0.0;
    }
    else if(PoseStamped->pose.position.z < 0.3)
    {
        vel1.data = -3.0;
        vel2.data = 3.0;
    }
    

    give_vel.publish(vel1);
    give_vel2.publish(vel2);

}

int main(int argc, char **argv)
{
    
    ros::init(argc, argv, "pose2command");

    ros::NodeHandle nh;

    ros::Subscriber get_pose = nh.subscribe("/aruco_single/pose", 100, msgCallback);
    give_vel = nh.advertise<std_msgs::Float64>("/pan_controller/command", 50);
    give_vel2 = nh.advertise<std_msgs::Float64>("/pan_controller2/command", 50);
    
    ros::spin();

    return 0;

}