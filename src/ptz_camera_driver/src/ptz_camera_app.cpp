#include <ptz_camera_driver/ptz_camera_driver.hpp>
#include <cpr/cpr.h>
#include "rclcpp/rclcpp.hpp"

//Main function
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions options;

  auto node = std::make_shared<PtzCameraDriver>(rclcpp::NodeOptions());
  printf("nodo creato");

  //chiamo la funzione setup del nodo per la costuzione mancante
  node->setup();
  rclcpp::spin(node);
  return 0;
}