#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "stm_custom_msgs/msg/stm_hardware_state.hpp"

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

            RCLCPP_INFO(this->get_logger(),"Parser STM ZOSTAL URUCHOMIONY!");
          }
    private:
        const rclcpp::Subscription<stm_custom_msgs::msg::StmHardwareState>::SharedPtr subscription_;

        stm_custom_msgs::msg::StmHardwareState::SharedPtr last_value_subscription;
          
        float total_x =0; //wszystko mam w mm wiec to chyba tez wypada miec (double moze wareto zrobic )

        float total_y=0;
        
        
        void hardware_callback(const stm_custom_msgs::msg::StmHardwareState::SharedPtr msg)
          {
            if (!last_value_subscription) {
            last_value_subscription = msg;
            return;
        }
            last_value_subscription = msg;
          }

       

          void update_odometry(const stm_custom_msgs::msg::StmHardwareState::SharedPtr &msg)
          {
            float dt = (msg->time_stamp - last_value_subscription->time_stamp)/1000.0f;


            float avg_v_left =  (msg->v_left_mps + laast_value_subscription->v_left_mps)/2.0f;
            float avg_v_right = (msg->v_right_mps + last_value_subscription->v_right_mps)/2.0f;


            //narazie czytamy kat z imu pozniej se poczytam o filtrze kalmana i sproboje to dodac z imu oraz enkoder
            // float delta_angle = msg->imu_yaw - last_value_subscription->imu_yaw;
            float length_drien = (avg_v_left + avg_v_right)/2.0f;

            float angular_vel_avg = (msg->motor_angular_speed -laast_value_subscription->motor_angular_speed)/2.0f;

            total_x = l*sin(msg->imu_yaw);
            total_y = l*cos(msg->imu_yaw);

            RCLCPP_INFO(this->get_logger(), "Odometria -> X: %.3f m | Y: %.3f m | Kąt: %.2f rad", total_x, total_y, theta);

            nav_msgs::msg::Odometry odom_msg;

            odom_msg.header.stamp = this->get_clock()->now();
            odom_msg.header.frame_id = "odom";
            odom_msg.child_frame_id = "base_link";

            odom_msg.pose.pose.position.x = total_x;
            odom_msg.pose.pose.position.y = total_y;
            odom_msg.pose.pose.position.z = 0.0;

            tf2::Quaternion q;

            q.setRPY(0.0,0.0,msg->imu_yaw);

            odom_msg.pose.pose.orientation.x = q.x();
            odom_msg.pose.pose.orientation.y = q.y();
            odom_msg.pose.pose.orientation.z = q.z();
            odom_msg.pose.pose.orientation.w = q.w();

            odom_msg.twist.twist.linear.x = msg->motor_linear_speed;
            odom_msg.twist.twist.angular.z = angular_vel_avg;

          }



};


int main(int argc, char * argv[])
{

    rclcpp::init(argc, argv);
    
    rclcpp::spin(std::make_shared<StmParserNode>());
    
    rclcpp::shutdown();
    
    return 0;
}