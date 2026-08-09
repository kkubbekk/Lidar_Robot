#include "rclcpp/rclcpp.hpp"
#include "stm_custom_msgs/msg/stm_hardware_state.hpp"
#include <chrono>
#include <cmath>

using namespace std::chrono_literals;

class MockStmNode : public rclcpp::Node
{
public:
    MockStmNode() : Node("mock_stm_node"), yaw_(0.0)
    {
        publisher_ = this->create_publisher<stm_custom_msgs::msg::StmHardwareState>("/stm_sensor_values", 10);
        
        start_time_ = this->now();

        timer_ = this->create_wall_timer(
            50ms, std::bind(&MockStmNode::timer_callback, this));

        RCLCPP_INFO(this->get_logger(), "Mock Stm uruchomiony");
    }

private:
    void timer_callback()
    {
        auto msg = stm_custom_msgs::msg::StmHardwareState();
        
        auto current_time = this->now();
        auto duration = current_time - start_time_;
        msg.time_stamp = duration.nanoseconds() / 1000000;

        msg.v_left_mps = 500.0;
        msg.v_right_mps = 550.0;

        yaw_ += 0.01;
        if (yaw_ > M_PI) {
            yaw_ -= 2.0 * M_PI;
        }
        msg.imu_yaw = yaw_;
         
        publisher_->publish(msg);
    }

    rclcpp::Publisher<stm_custom_msgs::msg::StmHardwareState>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Time start_time_;
    double yaw_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MockStmNode>());
    rclcpp::shutdown();
    return 0;
}