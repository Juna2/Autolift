#include "ros/ros.h"
#include <iostream>
#include "geometry_msgs/PoseStamped.h"
#include "std_msgs/Float64.h"
#include <signal.h>


void mySigintHandler(int sig);

class pose2command
{
public:

    ros::NodeHandle nh;

    std_msgs::Float64 degree;
    std_msgs::Float64 degree2;
    std_msgs::Float64 vel;
    std_msgs::Float64 vel2;

    ros::Publisher give_degree;
    ros::Publisher give_degree2;
    ros::Publisher give_vel;
    ros::Publisher give_vel2;

    ros::Subscriber get_pose;

    ros::Timer timer1;

    geometry_msgs::PoseStamped PoseStamped_copy;

    short firstornot = 0;



public:
    void msgCallback(const geometry_msgs::PoseStamped::ConstPtr& PoseStamped)
    {
        PoseStamped_copy.pose.orientation.x = PoseStamped->pose.orientation.x;
        PoseStamped_copy.pose.orientation.y = PoseStamped->pose.orientation.y;
        PoseStamped_copy.pose.orientation.z = PoseStamped->pose.orientation.z;
        ROS_INFO("copying orientation...");
    }   


    pose2command()
    {
        get_pose = nh.subscribe("/aruco_single/pose", 100, &pose2command::msgCallback, this);

        give_degree = nh.advertise<std_msgs::Float64>("/tilt_controller/command", 50);
        give_degree2 = nh.advertise<std_msgs::Float64>("/tilt_controller2/command", 50);
        give_vel = nh.advertise<std_msgs::Float64>("/pan_controller3/command", 50);
        give_vel2 = nh.advertise<std_msgs::Float64>("/pan_controller4/command", 50);
    
        timer1 = nh.createTimer(ros::Duration(0.1), &pose2command::callback_degree, this);
    }


    void callback_degree(const ros::TimerEvent& event)
    {
        if(PoseStamped_copy.pose.orientation.x > 0.01)
        {
            degree.data = 1;
            degree2.data = 1;
            vel.data = 1.0;
            vel2.data = -1.0;
            firstornot = 1;
        }
        else if(PoseStamped_copy.pose.orientation.x <= 0.01 && PoseStamped_copy.pose.orientation.x >= - 0.01)
        {
            ROS_INFO("reached to the parallel state");
            degree.data = 0;
            degree2.data = 0;
            vel.data = 0;
            vel2.data = 0;
            if(firstornot != 0) timer1.stop();
        }
        else if(PoseStamped_copy.pose.orientation.x < - 0.01)
        {
            degree.data = -1;
            degree2.data = -1;
            vel.data = 1.0;
            vel2.data = -1.0;
            firstornot = 1;
        }
        
        give_degree.publish(degree);
        give_degree2.publish(degree2);
        //ros::Duration(1).sleep();
        give_vel.publish(vel);
        give_vel2.publish(vel2);

        
    }


};

pose2command *forklift;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "pose2command");

    forklift = new pose2command();
    
    forklift->timer1.start();

    signal(SIGINT, mySigintHandler);

    ros::spin();

    return 0;

}

void mySigintHandler(int sig)
{
    ROS_INFO("Shutting down");
    forklift->timer1.stop();
    delete forklift;
    ros::shutdown();
}

