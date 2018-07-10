#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Vector3.h"
#include <tf/transform_listener.h>
#include <iostream>


int main(int argc, char **argv)
{

  ros::init(argc, argv, "get_tf");

  ros::NodeHandle n;

  tf::TransformListener listener;

  ros::Rate rate(20);

  while (n.ok())
  {
    tf::StampedTransform transform;
    try
    {
        //ROS_INFO("Attempting to read pose...");
        listener.lookupTransform("/marker_frame","/base_link",ros::Time(0), transform);
        n.setParam("position_x", transform.getOrigin().x());
        n.setParam("position_y", transform.getOrigin().y());
        //ROS_INFO("Got a transform! x = %f, y = %f",transform.getOrigin().x(),transform.getOrigin().y());
    }
    catch (tf::TransformException ex)
    {
        //ROS_ERROR("Nope! %s", ex.what());
    } 

    rate.sleep();

  }
  ROS_ERROR("I DIED");
  return 0;
}
