#include "ros/ros.h"
#include "std_msgs/Float64.h"

#define FIRST_MAX 0.56
#define FIRST_MIN 0.54
#define SECOND_THRESHOLD 0.01
#define OFFSET 0.02
#define V_OFFSET 0.05
#define V_OFFSET2 0.01
#define V_OFFSET3 -0.02
#define P_OFFSET 0.04
#define DISTANCE_X 0.12
#define DISTANCE_Y 0.2
#define DISTANCE_Y2 0.2
#define FB_OFFSET 0.005
#define LEFT 0
#define RIGHT 1
#define STRAIGHT 2
#define PASSED 0
#define NOT_PASSED 1
#define REACHED 2

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
    double vel3;
    double vel4;
    double current_pos;
    double current_pos2;
    double current_pos3;
    double theta1;
    double position_x;
    double position_y;
    short gostop;

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
            if((degree - current_pos) < 0.02 && (degree2 - current_pos2) < 0.02)
            {
                //ros::Duration(0.8).sleep();
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
            if((degree - current_pos) < 0.02 && (degree2 - current_pos2) < 0.02)
            {
                //ros::Duration(0.8).sleep();
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
            nh.setParam("vel", vel);
            nh.setParam("vel2", vel2);
            nh.getParam("current_pos", current_pos);
            nh.getParam("current_pos2", current_pos2);
            nh.setParam("degree", degree);
            nh.setParam("degree2", degree2);
        }

    }
    


    void vertical()
    {
        gostop = 1;
        nh.setParam("gostop", gostop);
        short count = 0;

        while(ros::ok() && count != 1)
        {
            nh.getParam("theta1", theta1);
            nh.getParam("current_pos3", current_pos3);
            
            ROS_INFO("@vertical@\ntheta1 : %f, current_pos3 : %f", theta1, current_pos3);
            if(current_pos3 >  theta1 + OFFSET)
            {
                turn(RIGHT, 0.3, 1.2);
                count = 2;
            }
            else if(current_pos3 <  theta1 - OFFSET)
            {
                turn(LEFT, 0.3, 1.2);
                count = 2;
            }
            else if(current_pos3 <=  theta1 + OFFSET && current_pos3 >=  theta1 - OFFSET && count != 0)
            {
                turn(STRAIGHT, 0, 0);
                count = 1;
            }
        }
        gostop = 0;
        nh.setParam("gostop", gostop);
    }

    void vertical_precise()
    {
        short count = 0;

        while(ros::ok() && count != 1)
        {
            nh.getParam("orientation_x", orientation_x);
            nh.getParam("current_pos3", current_pos3);

            orientation_x = orientation_x * ((3.141592 * 0.5) / 0.64);
            
            ROS_INFO("@vertical_precise@\norientation_x : %f, current_pos3 : %f", orientation_x, current_pos3);
            if(3.141592 * 0.5 + V_OFFSET - V_OFFSET2 > orientation_x + current_pos3)
            {
                turn(LEFT, 0.3, 1);
                count = 2;
            }
            else if(3.141592 * 0.5 + V_OFFSET + V_OFFSET2 < orientation_x + current_pos3)
            {
                turn(RIGHT, 0.3, 1);
                count = 2;
            }
            else if(orientation_x + current_pos3 <= (3.141592 * 0.5 + V_OFFSET + V_OFFSET2) && orientation_x + current_pos3 >= (3.141592 * 0.5 + V_OFFSET - V_OFFSET2) && count != 0)
            {
                turn(STRAIGHT, 0, 0);
                count = 1;
            }
        }
        gostop = 1;
        nh.setParam("gostop", gostop);
    }


    void frontback()
    {
        short count = NOT_PASSED;
        short already_prescise_count = 0;
        gostop = 0;
        nh.setParam("gostop", gostop);

        while(ros::ok() && count != REACHED)
        {
            nh.getParam("position_x", position_x);
            ROS_INFO("@frontback@\nposition_x : %f", position_x);

            if(position_x > DISTANCE_X + FB_OFFSET)
            {
                turn(STRAIGHT, 0, 1);
                count = PASSED;
            }
            else if(position_x < DISTANCE_X - FB_OFFSET)
            {
                turn(STRAIGHT, 0, -1);
                count = PASSED;
            }
            else if(position_x <= DISTANCE_X + FB_OFFSET && position_x >= DISTANCE_X - FB_OFFSET && count != NOT_PASSED)
            {
                turn(STRAIGHT, 0, 0);
                count = REACHED;
            }
            else if(position_x <= DISTANCE_X + FB_OFFSET && position_x >= DISTANCE_X - FB_OFFSET && count == NOT_PASSED)
            {
                already_prescise_count++;
                if(already_prescise_count > 200) count = PASSED;
                ROS_INFO("Here");
            }
        }
        

        gostop = 1;
        nh.setParam("gostop", gostop);
    }



    void parallel()
    {
        short count = 0;
        
        while(ros::ok() && count != 1)
        {
            nh.getParam("theta1", theta1);
            nh.getParam("current_pos3", current_pos3);
            
            ROS_INFO("theta1 : %f, current_pos3 : %f", theta1, current_pos3);
            if(current_pos3 > theta1 + P_OFFSET + OFFSET - (3.141592 *0.5))
            {
                turn(RIGHT, 0.2, 1.2);
                count = 2;
            }
            else if(current_pos3 <  theta1 + P_OFFSET - OFFSET - (3.141592 *0.5))
            {
                turn(LEFT, 0.2, 1.2);
                count = 2;
            }
            else if(current_pos3 <=  theta1 + P_OFFSET + OFFSET - (3.141592 *0.5) && current_pos3 >=  theta1 + P_OFFSET - OFFSET - (3.141592 *0.5) && count != 0)
            {
                turn(STRAIGHT, 0, 0);
                count = 1;
            }
        }

        gostop = 0;
        nh.setParam("gostop", gostop);
    }

    void parallel_precise()
    {
        short count = NOT_PASSED;
        short already_prescise_count = 0;
        while(ros::ok() && count != REACHED)
        {
            nh.getParam("orientation_x", orientation_x);
            nh.getParam("current_pos3", current_pos3);

            orientation_x = orientation_x * ((3.141592 * 0.5) / 0.64);
            
            ROS_INFO("@parallel_precise@\norientation_x : %f, current_pos3 : %f", orientation_x, current_pos3);
            if(V_OFFSET - V_OFFSET2 > orientation_x + current_pos3)
            {
                turn(LEFT, 0.3, 1);
                count = PASSED;
            }
            else if(V_OFFSET + V_OFFSET2 < orientation_x + current_pos3)
            {
                turn(RIGHT, 0.3, 1);
                count = PASSED;
            }
            else if(orientation_x + current_pos3 <= (V_OFFSET + V_OFFSET2) && orientation_x + current_pos3 >= (V_OFFSET - V_OFFSET2) && count != NOT_PASSED)
            {
                turn(STRAIGHT, 0, 0);
                count = REACHED;
            }
            else if(orientation_x + current_pos3 <= (V_OFFSET + V_OFFSET2) && orientation_x + current_pos3 >= (V_OFFSET - V_OFFSET2) && count == NOT_PASSED)
            {
                already_prescise_count++;
                if(already_prescise_count > 200) count = PASSED;
                ROS_INFO("Here");
            }
        }
        gostop = 1;
        nh.setParam("gostop", gostop);
    }

    void frontback2()
    {
        short count = NOT_PASSED;
        short already_prescise_count = 0;
        gostop = 0;
        nh.setParam("gostop", gostop);

        while(ros::ok() && count != REACHED)
        {
            nh.getParam("position_y", position_y);      
            ROS_INFO("@frontback2@\nposition_y : %f", position_y);

            if(position_y > DISTANCE_Y + FB_OFFSET)
            {
                turn(STRAIGHT, 0, 1);
                count = PASSED;
            }
            else if(position_y < DISTANCE_Y - FB_OFFSET)
            {
                turn(STRAIGHT, 0, -1);
                count = PASSED;
            }
            else if(position_y <= DISTANCE_Y + FB_OFFSET && position_y >= DISTANCE_Y - FB_OFFSET && count != NOT_PASSED)
            {
                turn(STRAIGHT, 0, 0);
                count = REACHED;
            }
            else if(position_y <= DISTANCE_Y + FB_OFFSET && position_y >= DISTANCE_Y - FB_OFFSET && count== NOT_PASSED)
            {
                already_prescise_count++;
                if(already_prescise_count > 200) count = PASSED;
                ROS_INFO("Here");
            }
        }
    }



    void gotofreight()
    {
        double gradually;
        gostop = 0;
        nh.setParam("gostop", gostop);

        vel4 = -4;
        nh.setParam("vel4", vel4);
        ros::Duration(9).sleep();
        vel4 = 0;
        nh.setParam("vel4", vel4);

        ros::Duration(1).sleep();

        turn(STRAIGHT, 0, 1);
        ros::Duration(15).sleep();
        turn(STRAIGHT, 0, 0);

        ros::Duration(1).sleep();

        vel4 = -1;
        nh.setParam("vel4", vel4);
        ros::Duration(11).sleep();
        vel4 = 0;
        nh.setParam("vel4", vel4);

        ros::Duration(1).sleep();

        turn(STRAIGHT, 0, -1);
        ros::Duration(18).sleep();
        turn(STRAIGHT, 0, 0);

        vel4 = 4;
        nh.setParam("vel4", vel4);
        ros::Duration(6).sleep();
        vel4 = 0;
        nh.setParam("vel4", vel4);

        degree = atan( 0.26 / ( 0.2 - 0.122 ));
        degree2 = atan( 0.26 / ( 0.2 + 0.122 ));
        vel = -2;
        vel2 = -(-2);
        nh.getParam("current_pos", current_pos);
        nh.getParam("current_pos2", current_pos2);
        nh.setParam("degree", degree);
        nh.setParam("degree2", degree2);
        ros::Duration(2).sleep();
        nh.setParam("vel", vel);
        nh.setParam("vel2", vel2);
        ros::Duration(18).sleep();
        turn(STRAIGHT, 0, -2);
        ros::Duration(3).sleep();
        turn(STRAIGHT, 0, 0);
    }


    void inverse_vertical()
    {
        gostop = 1;
        nh.setParam("gostop", gostop);
        short count = 0;

        while(ros::ok() && count != 1)
        {
            nh.getParam("theta1", theta1);
            nh.getParam("current_pos3", current_pos3);
            
            ROS_INFO("@inverse_vertical@\ntheta1 : %f, current_pos3 : %f", theta1, current_pos3);
            if(-current_pos3 >  -theta1 + OFFSET)
            {
                turn(RIGHT, 0.2, -1.2);
                count = 2;
            }
            else if(-current_pos3 <  -theta1 - OFFSET)
            {
                turn(LEFT, 0.2, -1.2);
                count = 2;
            }
            else if(-current_pos3 <=  -theta1 + OFFSET && -current_pos3 >=  -theta1 - OFFSET && count != 0)
            {
                turn(STRAIGHT, 0, 0);
                count = 1;
            }
        }
        gostop = 0;
        nh.setParam("gostop", gostop);
    }

    void inverse_vertical_precise()
    {
        short count = 0;

        while(ros::ok() && count != 1)
        {
            nh.getParam("orientation_x", orientation_x);
            nh.getParam("current_pos3", current_pos3);

            orientation_x = orientation_x * ((3.141592 * 0.5) / 0.64);
            
            ROS_INFO("@inverse_vertical_precise@\norientation_x : %f, current_pos3 : %f", -orientation_x, -current_pos3);
            if(3.141592 * 0.5 + V_OFFSET - V_OFFSET2 > -orientation_x - current_pos3)
            {
                turn(LEFT, 0.3, -1);
                count = 2;
            }
            else if(3.141592 * 0.5 + V_OFFSET + V_OFFSET2 < -orientation_x - current_pos3)
            {
                turn(RIGHT, 0.3, -1);
                count = 2;
            }
            else if(-orientation_x - current_pos3 <= (3.141592 * 0.5 + V_OFFSET + V_OFFSET2) && -orientation_x - current_pos3 >= (3.141592 * 0.5 + V_OFFSET - V_OFFSET2) && count != 0)
            {
                turn(STRAIGHT, 0, 0);
                count = 1;
            }
        }
    }

    void find_marker()
    {
        double first_orientation_x = orientation_x;
        
        while(ros::ok() && first_orientation_x == orientation_x)
        {
            nh.getParam("orientation_x", orientation_x);
            vel3 = -1 * 0.6;
            nh.setParam("vel3", vel3);
        }
        vel3 = 0;
        nh.setParam("vel3", vel3);
        gostop = 1;
        nh.setParam("gostop", gostop);
    }

    void frontback3()
    {
        short count = NOT_PASSED;
        short already_prescise_count = 0;
        gostop = 0;
        nh.setParam("gostop", gostop);

        while(ros::ok() && count != REACHED)
        {
            nh.getParam("position_x", position_x);
            ROS_INFO("@frontback3@\nposition_x : %f", position_x);

            if(position_x > - DISTANCE_X + FB_OFFSET)
            {
                turn(STRAIGHT, 0, -1);
                count = PASSED;
            }
            else if(position_x < - DISTANCE_X - FB_OFFSET)
            {
                turn(STRAIGHT, 0, 1);
                count = PASSED;
            }
            else if(position_x <= - DISTANCE_X + FB_OFFSET && position_x >= - DISTANCE_X - FB_OFFSET && count != NOT_PASSED)
            {
                turn(STRAIGHT, 0, 0);
                count = REACHED;
            }
            else if(position_x <= - DISTANCE_X + FB_OFFSET && position_x >= - DISTANCE_X - FB_OFFSET && count== NOT_PASSED)
            {
                already_prescise_count++;
                if(already_prescise_count > 200) count = PASSED;
                ROS_INFO("Here");
            }
        }
        

        gostop = 1;
        nh.setParam("gostop", gostop);
    }

    void parallel2()
    {
        short count = 0;
        
        while(ros::ok() && count != 1)
        {
            nh.getParam("theta1", theta1);
            nh.getParam("current_pos3", current_pos3);
            
            ROS_INFO("theta1 : %f, current_pos3 : %f", -theta1, current_pos3);
            if(theta1 <= 0)
            {
                if(current_pos3 > theta1 + P_OFFSET + OFFSET + (3.141592 *0.5))
                {
                    turn(RIGHT, 0.2, 1.2);
                    count = 2;
                }
                else if(current_pos3 <  theta1 + P_OFFSET - OFFSET + (3.141592 *0.5))
                {
                    turn(LEFT, 0.2, 1.2);
                    count = 2;
                }
                else if(current_pos3 <=  theta1 + P_OFFSET + OFFSET + (3.141592 *0.5) && current_pos3 >=  theta1 + P_OFFSET - OFFSET + (3.141592 *0.5) && count != 0)
                {
                    turn(STRAIGHT, 0, 0);
                    count = 1;
                }
            }
            else
            {
                if(current_pos3 > theta1 + P_OFFSET + OFFSET - (3.141592 *0.5))
                {
                    turn(RIGHT, 0.2, 1.2);
                    count = 2;
                }
                else if(current_pos3 <  theta1 + P_OFFSET - OFFSET - (3.141592 *0.5))
                {
                    turn(LEFT, 0.2, 1.2);
                    count = 2;
                }
                else if(current_pos3 <=  theta1 + P_OFFSET + OFFSET - (3.141592 *0.5) && current_pos3 >=  theta1 + P_OFFSET - OFFSET - (3.141592 *0.5) && count != 0)
                {
                    turn(STRAIGHT, 0, 0);
                    count = 1;
                }
            }

        }

        gostop = 0;
        nh.setParam("gostop", gostop);
    }

    void parallel_precise2()
    {
        short count = NOT_PASSED;
        short already_prescise_count = 0;
        while(ros::ok() && count != REACHED)
        {
            nh.getParam("orientation_x", orientation_x);
            nh.getParam("current_pos3", current_pos3);

            orientation_x = orientation_x * ((3.141592 * 0.5) / 0.64);
            
            ROS_INFO("@parallel_precise@\norientation_x : %f, current_pos3 : %f", -orientation_x, -current_pos3);
            if(V_OFFSET3 - V_OFFSET2 > -orientation_x - current_pos3)
            {
                turn(RIGHT, 0.3, 1);
                count = PASSED;
            }
            else if(V_OFFSET3 + V_OFFSET2 < -orientation_x - current_pos3)
            {
                turn(LEFT, 0.3, 1);
                count = PASSED;
            }
            else if(-orientation_x - current_pos3 <= (V_OFFSET3 + V_OFFSET2) && -orientation_x - current_pos3 >= (V_OFFSET3 - V_OFFSET2) && count != NOT_PASSED)
            {
                turn(STRAIGHT, 0, 0);
                count = REACHED;
            }
            else if(-orientation_x - current_pos3 <= (V_OFFSET3 + V_OFFSET2) && -orientation_x - current_pos3 >= (V_OFFSET3 - V_OFFSET2) && count == NOT_PASSED)
            {
                already_prescise_count++;
                if(already_prescise_count > 200) count = PASSED;
                ROS_INFO("Here");
            }
        }
        gostop = 0;
        nh.setParam("gostop", gostop);
    }

    void frontback4()
    {
        short count = NOT_PASSED;
        short already_prescise_count = 0;
        gostop = 0;
        nh.setParam("gostop", gostop);

        while(ros::ok() && count != REACHED)
        {
            nh.getParam("position_y", position_y);      
            ROS_INFO("@frontback4@\nposition_y : %f", position_y);

            if(position_y > DISTANCE_Y2 + FB_OFFSET)
            {
                turn(STRAIGHT, 0, 1);
                count = PASSED;
            }
            else if(position_y < DISTANCE_Y2 - FB_OFFSET)
            {
                turn(STRAIGHT, 0, -1);
                count = PASSED;
            }
            else if(position_y <= DISTANCE_Y2 + FB_OFFSET && position_y >= DISTANCE_Y2 - FB_OFFSET && count != NOT_PASSED)
            {
                turn(STRAIGHT, 0, 0);
                count = REACHED;
            }
            else if(position_y <= DISTANCE_Y2 + FB_OFFSET && position_y >= DISTANCE_Y2 - FB_OFFSET && count== NOT_PASSED)
            {
                already_prescise_count++;
                if(already_prescise_count > 200) count = PASSED;
                ROS_INFO("Here");
            }
        }
    }

    void putthefreight()
    {
        vel4 = 4;
        nh.setParam("vel4", vel4);
        ros::Duration(4).sleep();
        vel4 = 0;
        nh.setParam("vel4", vel4);

        turn(STRAIGHT, 0, -2);
        ros::Duration(6).sleep();
        turn(STRAIGHT, 0, 0);
    }

    void third()
    {
        short count = 0;
        
        while(ros::ok() && count == 0)
        {

            if(orientation_x < 0) orientation_x * -1;

            if(orientation_x < 0.5)
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

    void end()
    {
        turn(STRAIGHT, 0, 0);
        while(ros::ok())
        {
            
        }
    }
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "param");
    
    param letsgo;
    letsgo.orientation_x = 0;

    letsgo.end();
    letsgo.vertical();
    letsgo.vertical_precise();
    ros::Duration(3).sleep();
    letsgo.frontback();
    letsgo.parallel();
    letsgo.parallel_precise();
    letsgo.frontback2();
    letsgo.parallel_precise();
    ros::Duration(2).sleep();
    letsgo.gotofreight();
    letsgo.find_marker();
    ros::Duration(4).sleep();
    letsgo.inverse_vertical();
    letsgo.inverse_vertical_precise();
    letsgo.frontback3();
    letsgo.parallel2();
    letsgo.parallel_precise2();
    letsgo.frontback4();
    letsgo.putthefreight();
    //letsgo.parallel_precise();
    //letsgo.second();
    letsgo.end();
    
    
    return 0;
}
