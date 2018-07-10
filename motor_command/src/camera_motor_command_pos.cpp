#include "ros/ros.h"
#include "std_msgs/Float64.h"
#include <math.h>

#define GOAL 0.1
#define OFFSET 0.01
#define VEL 0.8
#define AVER_NUM 10
#define CHANGE 2.5
 
class camera_motor_command
{
public:
    ros::NodeHandle nh;

    double position_x;
    double position_y;
    double orientation_x;
    double current_pos3;
    double goal_pos;
    double theta1;  
    double theta2;
    double theta3;
    double fixing_number;
    int theta3_count;

    void go()
    {
        theta3 = 0;
        theta3_count = 0;
        ROS_INFO("Here");

        while(ros::ok())
        {
            nh.getParam("position_x", position_x);
            nh.getParam("position_y", position_y);
            nh.getParam("orientation_x", orientation_x);
            nh.getParam("current_pos3", current_pos3);
            nh.getParam("goal_pos", goal_pos);

            theta1 = atan(position_y / position_x);
            nh.setParam("theta1", theta1);
            theta2 = orientation_x * ((3.141592 * 0.5) / 0.55);
            theta3 += theta1 - ((3.141592 * 0.5) - theta2);

            ++theta3_count;

            if(theta3_count == AVER_NUM)
            {
                theta3 = theta3 / AVER_NUM;
                //fixing_number = current_pos3 - 1;
                //theta3 = theta3 ;
                theta3 = theta3 * (1 + (goal_pos - 0.3)) * CHANGE;
                ROS_INFO("theta1 : %f, theta2 : %f, theta3 : %f",theta1, theta2, theta3);

                nh.setParam("degree3", theta3);

                theta3_count = 0;
                theta3 = 0;
            }
        }  
    }
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "camera_motor_command");
    
    camera_motor_command letsgo;

    letsgo.go();

    return 0;
}
