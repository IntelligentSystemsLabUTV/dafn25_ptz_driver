#ifndef PTZ_CAMERA_DRIVER_HPP_
#define PTZ_CAMERA_DRIVER_HPP_

#include <cstdio>
#include <rclcpp/rclcpp.hpp>
#include "std_srvs/srv/set_bool.hpp"
#include "axis_camera_interfaces/msg/ptzf.hpp"
#include "image_transport/image_transport.hpp"
#include "opencv2/opencv.hpp"
#include "cv_bridge/cv_bridge.hpp"

#include <string>
#include <thread>   // Per il thread video
#include <atomic>   // Per la variabile di stato del thread
// Altre inclusioni...

struct CameraParams {
    bool autostart;
    std::string ip;
    std::string username;
    std::string password;
    double pan0;
    int sampling_period_ms;
};

class PtzCameraDriver : public rclcpp::Node
{
public:
    PtzCameraDriver();
    ~PtzCameraDriver(); // Distruttore per pulire le risorse (es. il thread)

private:
    // Qui andranno le variabili e le funzioni membro
    void command_callback(const axis_camera_interfaces::msg::PTZF::SharedPtr msg);
    void enable_disable_callback(const std_srvs::srv::SetBool::Request::SharedPtr request,
                                 std_srvs::srv::SetBool::Response::SharedPtr response);
    void video_publishing_loop(); // La funzione che girerà nel thread separato

    CameraParams params_; // La struct con tutti i parametri

    // Publisher, Subscriber e Service
    image_transport::Publisher image_pub_;
    rclcpp::Subscription<axis_camera_interfaces::msg::PTZF>::SharedPtr command_sub_;
    rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr enable_service_;

    // Variabili per il thread video
    std::thread video_thread_;
    std::atomic<bool> is_active_{false}; // Variabile per attivare/disattivare il loop
};

#endif // PTZ_CAMERA_DRIVER_HPP_
