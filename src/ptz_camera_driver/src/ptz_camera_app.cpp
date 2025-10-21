#include <ptz_camera_driver/ptz_camera_driver.hpp>
#include <cpr/cpr.h>
#include "rclcpp/rclcpp.hpp"

//modifica per prova commit edoardo
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions options;

  auto node = std::make_shared<PtzCameraDriver>();
  printf("nodo creato");

  //chiamo la funzione setup del nodo per la costuzione mancante
  node->setup();
  rclcpp::spin(node);
  return 0;
}