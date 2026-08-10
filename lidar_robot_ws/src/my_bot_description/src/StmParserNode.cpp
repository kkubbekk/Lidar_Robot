#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "stm_custom_msgs/msg/stm_hardware_state.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/transform_broadcaster.h"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include <cmath>

using std::placeholders::_1;

class StmParserNode : public rclcpp::Node
{
    public:
        StmParserNode() : Node("stm_parser_node")
        {
            subscription_ = this->create_subscription<stm_custom_msgs::msg::StmHardwareState>(
            "/stm_sensor_values",
            10,
            std::bind(&StmParserNode::hardware_callback,this,_1)
            );

            publisher_ = this->create_publisher<nav_msgs::msg::Odometry>(
                "/odom",
                10
              );
             tf_broadcaster_ =  std::make_unique<tf2_ros::TransformBroadcaster>(*this);

            RCLCPP_INFO(this->get_logger(),"Parser STM ZOSTAL URUCHOMIONY!");
          }
    private:
        rclcpp::Subscription<stm_custom_msgs::msg::StmHardwareState>::SharedPtr subscription_;
        rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr publisher_;
        std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;

        stm_custom_msgs::msg::StmHardwareState::SharedPtr last_value_subscription;
          
        float total_x =0; //wszystko mam w mm wiec to chyba tez wypada miec (double moze wareto zrobic )

        float total_y=0;
        
        
        void hardware_callback(const stm_custom_msgs::msg::StmHardwareState::SharedPtr msg)
          {
            if (!last_value_subscription) {
            last_value_subscription = msg;
            return;
        } 
          update_odometry_tf2(msg);        

            last_value_subscription = msg;
          }

       

          void update_odometry_tf2(const stm_custom_msgs::msg::StmHardwareState::SharedPtr &msg)
          {
            float dt = (msg->time_stamp - last_value_subscription->time_stamp)/1000.0f;

            if(dt<0) return;


            double avg_v_left =  (msg->v_left_mps + last_value_subscription->v_left_mps)/2.0f;
            double avg_v_right = (msg->v_right_mps + last_value_subscription->v_right_mps)/2.0f;


            //narazie czytamy kat z imu pozniej se poczytam o filtrze kalmana i sproboje to dodac z imu oraz enkoder
            // float delta_angle = msg->imu_yaw - last_value_subscription->imu_yaw;
            float length_driven = (avg_v_left + avg_v_right)/2.0f*dt;

            float angular_vel_avg = (msg->motor_angular_speed + last_value_subscription->motor_angular_speed)/2.0f;

            double yaw_rad = msg->imu_yaw * (M_PI / 180.0);

            total_x += length_driven*cos(yaw_rad);
            total_y += length_driven*sin(yaw_rad);

            RCLCPP_INFO(this->get_logger(), "Odometria -> X: %.3f m | Y: %.3f m | Kąt: %.2f rad", total_x, total_y,msg->imu_yaw);

            nav_msgs::msg::Odometry odom_msg;

            odom_msg.header.stamp = this->get_clock()->now();
            odom_msg.header.frame_id = "odom";
            odom_msg.child_frame_id = "base_link";

            odom_msg.pose.pose.position.x = total_x;
            odom_msg.pose.pose.position.y = total_y;
            odom_msg.pose.pose.position.z = 0.0;

            tf2::Quaternion q;

            q.setRPY(0.0,0.0,yaw_rad);

            odom_msg.pose.pose.orientation.x = q.x();
            odom_msg.pose.pose.orientation.y = q.y();
            odom_msg.pose.pose.orientation.z = q.z();
            odom_msg.pose.pose.orientation.w = q.w();

            odom_msg.twist.twist.linear.x = msg->motor_linear_speed;
            odom_msg.twist.twist.angular.z = angular_vel_avg;

            publisher_->publish(odom_msg);

            //transform
            geometry_msgs::msg::TransformStamped t;

            t.header.stamp = this->get_clock()->now();
            t.header.frame_id = "odom";
            t.child_frame_id = "base_link";
            t.transform.translation.x = total_x;
            t.transform.translation.y = total_y;
            t.transform.translation.z = 0.0;

            t.transform.rotation.x = q.x();
            t.transform.rotation.y = q.y();
            t.transform.rotation.z = q.z();
            t.transform.rotation.w = q.w();

            tf_broadcaster_->sendTransform(t);

          }


          //proba kalman ale to jeszcze musze poczytac moze zrobie 2d filter nie wiem 

          // //jakies wartosci z dupy zeby zobaczyc logike
          // float p =0.2; 
          // float q=0.1
          // float angle =0;
          // float r = 0.02;
          // float k_gain;
          
          // void kalman_filter_1D(const stm_custom_msgs::msg::StmHardwareState::SharedPtr msg)
          // {
          //   //angular
          //   angle = angle + (msg->motor_angular_velocity*msg->time_stamp);
          //   p=p+q;
          //   k_gain = p/(p+r)

          //   angle = angle + K * (msg->imu_yaw - angle);
          //   p=(1-K)*P;
            
          //   //pozniej jakies returnowanie i publioshowanie to przed trzbea bedzie zmienic update odom
          
};


int main(int argc, char * argv[])
{

    rclcpp::init(argc, argv);
    
    rclcpp::spin(std::make_shared<StmParserNode>());
    
    rclcpp::shutdown();
    
    return 0;
}