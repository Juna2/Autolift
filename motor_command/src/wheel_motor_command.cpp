#include "ros/ros.h"
#include <iostream>
#include "std_msgs/Float64.h"
#include <math.h>

#define GOAL 0.0
#define OFFSET 0.003
#define VEL 0.45
#define AVER_NUM 3
#define LEFT 0
#define RIGHT 1
#define STRAIGHT 2

class camera_motor_command
{
public:
    ros::NodeHandle nh;

    int gostop;
    int wheel_direction;
    double position;
    double speed;
    double vel;
    double vel2;
    double vel3;
    double degree;
    double degree2;
    double current_pos;
    double current_pos2;

    camera_motor_command()
    {
        gostop = 0;
        wheel_direction = STRAIGHT;
        position = 0.0;
        speed = 0;
        nh.setParam("gostop", gostop);
        nh.setParam("wheel_direction", wheel_direction);
        nh.setParam("position", position);
        nh.setParam("speed", speed);
    }

    void go()
    {
        ROS_INFO("Let's go!!");

        while(ros::ok() && gostop == 1)
        {
            nh.getParam("gostop", gostop);
            nh.getParam("wheel_direction", wheel_direction);
            nh.getParam("position", position);
            nh.getParam("speed", speed);
            ROS_INFO("wheel_direction : %d, position : %f, speed : %f", wheel_direction, position, speed);
            turn(wheel_direction, position, speed);
        }
    }

    void stop()
    {
        ROS_INFO("Hold on");
        while(ros::ok() && gostop == 0)
        {
            nh.getParam("gostop", gostop);
            wheel_direction = STRAIGHT;
            position = 0;
            speed = 0;
            turn(wheel_direction, position, speed);
        }
    }

    void turn(int lor, double position, double velocity)
    {
        if(lor == LEFT)
        {
            degree = -atan( 0.26 / ( position + 0.122 ));
            degree2 = -atan( 0.26 / ( position - 0.122 ));
            vel = velocity;
            vel2 = -velocity;
            nh.getParam("current_pos", current_pos);
            nh.getParam("current_pos2", current_pos2);
            nh.setParam("degree", degree);
            nh.setParam("degree2", degree2);
    
            /*To rotate the wheels after tilting them*/
            if((degree - current_pos) > -0.02 && (degree2 - current_pos2) < 0.02)
            {
                ros::Duration(0.8).sleep();
                nh.setParam("vel", vel);
                nh.setParam("vel2", vel2);
            }
        }
        else if(lor == RIGHT)
        {
            degree = atan( 0.26 / ( position - 0.122 ));
            degree2 = atan( 0.26 / ( position + 0.122 ));
            vel = velocity;
            vel2 = -velocity;
            nh.getParam("current_pos", current_pos);
            nh.getParam("current_pos2", current_pos2);
            nh.setParam("degree", degree);
            nh.setParam("degree2", degree2);
    
            /*To rotate the wheels after tilting them*/
            if((degree - current_pos) > -0.02 && (degree2 - current_pos2) < 0.02)
            {
                ros::Duration(0.8).sleep();
                nh.setParam("vel", vel);
                nh.setParam("vel2", vel2);
            }
        }
        else if(lor == STRAIGHT)
        {
            degree = 0;
            degree2 = 0;
            vel = velocity;
            vel2 = -velocity;
            nh.getParam("current_pos", current_pos);
            nh.getParam("current_pos2", current_pos2);
            nh.setParam("degree", degree);
            nh.setParam("degree2", degree2);
            nh.setParam("vel", vel);
            nh.setParam("vel2", vel2);
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
