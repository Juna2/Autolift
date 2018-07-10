#include "ros/ros.h"
#include <iostream>
#include "std_msgs/Float64.h"
#include <math.h>

#define GOAL 0.0
#define OFFSET 0.003
#define VEL 0.6
#define AVER_NUM 3

class camera_motor_command
{
public:
    ros::NodeHandle nh;

    double position_x;
    double position_y;
    double orientation_x;
    double vel3;
    double theta1;
    double theta2;
    double theta3;
    int theta3_count;
    int  gostop;

    camera_motor_command()
    {
        gostop = 1;
        nh.setParam("gostop", gostop);
    }

    void go()
    {
        theta3 = 0;
        theta3_count = 0;
        ROS_INFO("Here");

        while(ros::ok())
        {
            nh.getParam("gostop", gostop);
            if(gostop == 0) break;
            nh.getParam("position_x", position_x);
            nh.getParam("position_y", position_y);
            nh.getParam("orientation_x", orientation_x);

            theta1 = atan(position_y / position_x);
            nh.setParam("theta1", theta1);
            theta2 = orientation_x * ((3.141592 * 0.5) / 0.62);
            theta3 += theta2 - ((3.141592 * 0.5) - theta1);

            ++theta3_count;

            if(theta3_count == AVER_NUM)
            {
                theta3 = theta3 / AVER_NUM;
                //ROS_INFO("theta1 : %f, theta2 : %f, theta3 : %f",theta1, theta2, theta3);

                if(theta3 > GOAL + OFFSET)
                {
                    vel3 = VEL;
                    nh.setParam("vel3", vel3);
                }
                else if(theta3 < GOAL - OFFSET)
                {
                    vel3 = -1 * VEL;
                    nh.setParam("vel3", vel3);
                }
                else if(theta3 >= GOAL - OFFSET && theta3 <= GOAL + OFFSET)
                {
                    vel3 = 0.0;
                    nh.setParam("vel3", vel3);
                }
                theta3_count = 0;
                theta3 = 0;
            }
        }
    }

    void stop()
    {
        while(ros::ok())
        {
            nh.getParam("gostop", gostop);
            if(gostop == 1) break;
            vel3 = 0;
            nh.setParam("vel3", vel3);
        }
    }
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "camera_motor_command");
    
    camera_motor_command letsgo;

    while(ros::ok())
    {
        letsgo.go();
        letsgo.stop();
    }

    return 0;
}
