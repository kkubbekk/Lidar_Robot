#define axis_length 0.35
// #define wheel_radius


#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"

struct motor_data_t
{
    float v_left;
    float v_right;
    //kiedys sie moze rozbuduje?
};


class VelParserNode : public rclcpp::Node
{
public:
    VelParserNode() : Node("vel_parser_node")
    {
        subscription_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel",
            10,
            std::bind(&VelParserNode::cmd_vel_callback,this,std::placeholders::_1)
        );
        
        publisher_ = this->create_publisher<std_msgs::msg::Float32MultiArray>(
            "/motor_values",
            10
        );
       

    };
    private:
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscription_;



    rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr publisher_;

    void cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg)
    {
        motor_data_t data;

        float angular_velocity = msg->angular.z;
        float linear_velocity = msg->linear.x;


        float v_left =  linear_velocity - (angular_velocity*(axis_length/2.0));
        float V_right = linear_velocity + (angular_velocity*(axis_length/2.0));

        data.v_left = v_left;
        data.v_right = V_right;

        auto message = std_msgs::msg::Float32MultiArray();
        message.data = {data.v_left, data.v_right};
        publisher_->publish(message);






    }
    
} ;

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<VelParserNode>());
    rclcpp::shutdown();
    return 0;
}