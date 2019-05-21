#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget vel_cmd;
    vel_cmd.request.linear_x = lin_x;
    vel_cmd.request.angular_z = ang_z;
    
    if (!client.call(vel_cmd)){
        ROS_ERROR("Failed to execute command");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    int pos_sum = 0;
    int tot_white_pixels = 0;
    float mean_ball_position;
    float x, z;
   // ROS_INFO("image width %d height %d step_size %d",img.width,img.height,img.step);
    for(int i=0; i<img.height; ++i){
        for (int j=0; j<img.width; ++j){
               if (img.data[(i*img.step)+(j*3)] == white_pixel && img.data[(i*img.step)+(j*3)+1] == white_pixel && img.data[(i*img.step)+(j*3)+2] == white_pixel){
                ++tot_white_pixels;
                pos_sum = pos_sum + j;
                mean_ball_position = pos_sum/tot_white_pixels;
            }
        }
    }
   // ROS_INFO("%d white pixels found. mean_ball_position is %1.2f ", tot_white_pixels,mean_ball_position);
    if (tot_white_pixels == 0) { 
       drive_robot(0,0);
    }  
    else {
    if (mean_ball_position < img.width / 3)
    {
      drive_robot(0.2, 0.5);
    }
    else if (mean_ball_position > img.width * 2 / 3)
    {
      drive_robot(0.2, -0.5);
    }
    else
    {
      drive_robot(0.2, 0.0);
    }
     
    }
    
   
    

}


int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
