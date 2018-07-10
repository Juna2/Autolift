#include "ros/ros.h"
#include "std_msgs/Float64.h"



class param
{
public:
    ros::NodeHandle nh;

    double orientation_x;
    double orientation_y;
    double orientation_z;
    double degree;
    double degree2;
    double vel;
    double vel2;
    double current_pos;
    double current_pos2;

    void first()
    {
        short count = 0;

        while(ros::ok() && count == 0)
        {
            nh.getParam("orientation_x", orientation_x);

            if(orientation_x > 0.01)
            {
                degree = 1;
                degree2 = 1;
                vel = 1.0;
                vel2 = -1.0;
                nh.getParam("current_pos", current_pos);
                nh.getParam("current_pos2", current_pos2);
                nh.setParam("degree", degree);
                nh.setParam("degree2", degree2);

                /*To rotate the wheels after tilting them*/
                if((degree - current_pos) > -0.02 && (degree2 - current_pos2) < 0.02)
                {
                    nh.setParam("vel", vel);
                    nh.setParam("vel2", vel2);
                }
            }
            else if(orientation_x < -0.01)
            {
                degree = -1;
                degree2 = -1;
                vel = 1.0;
                vel2 = -1.0;
                nh.getParam("current_pos", current_pos);
                nh.getParam("current_pos2", current_pos2);
                nh.setParam("degree", degree);
                nh.setParam("degree2", degree2);

                /*To rotate the wheels after tilting them*/
                if((degree - current_pos) > -0.02 && (degree2 - current_pos2) < 0.02)
                {
                    nh.setParam("vel", vel);
                    nh.setParam("vel2", vel2);
                }
            }
            else
            {
                degree = 0;
                degree2 = 0;
                vel = 0;
                vel2 = 0;
                nh.setParam("degree", degree);
                nh.setParam("degree2", degree2);
                nh.setParam("vel", vel);
                nh.setParam("vel2", vel2);
                count = 1;
            }
        }  
    }
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "param_last2");
    
    param letsgo;
    letsgo.orientation_x = 0;

    letsgo.first();
    //letsgo.second();
    
    
    return 0;
}
