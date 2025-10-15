#include <ptz_camera_driver/ptz_camera_driver.hpp>

#include <cpr/cpr.h>

//modifica per prova commit edoardo
int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;
  // modifica
  printf("hello world ptz_camera_driver package \n");
  PtzCameraDriver();
  return 0;
}

//costruttore del nodo
PtzCameraDriver::PtzCameraDriver()
: Node("ptz_camera_driver")
{
  RCLCPP_INFO(this->get_logger(), "Inizializzazione del nodo PtzCameraDriver...");
  //dichiaro parametri
  this->declare_parameter<bool>("autostart", true);
  this->declare_parameter<std::string>("camera.ip", "192.168.0.90");
  this->declare_parameter<std::string>("camera.username", "root");
  this->declare_parameter<std::string>("camera.password", "TorVergata2");
  this->declare_parameter<double>("camera.pan0", 0.0);
  this->declare_parameter<int>("camera.sampling_period", 100); // Valore in millisecondi
  // Ora che i parametri sono stati dichiarati, li leggi e salvi i loro valori
  // nei campi corrispondenti della tua struct 'params_'. In questo modo,
  // saranno facilmente accessibili in tutto il resto del codice del tuo nodo.

  RCLCPP_INFO(this->get_logger(), "Lettura dei valori dei parametri...");
  params_.autostart = this->get_parameter("autostart").as_bool();
  params_.ip = this->get_parameter("camera.ip").as_string();
  params_.username = this->get_parameter("camera.username").as_string();
  params_.password = this->get_parameter("camera.password").as_string();
  params_.pan0 = this->get_parameter("camera.pan0").as_double();
  params_.sampling_period_ms = this->get_parameter("camera.sampling_period").as_int();
  //LOG DI VERIFICA
  RCLCPP_INFO(this->get_logger(), "--- Configurazione Camera Caricata ---");
  RCLCPP_INFO(this->get_logger(), "IP Address: %s", params_.ip.c_str());
  RCLCPP_INFO(this->get_logger(), "Username: %s", params_.username.c_str());
  // Nota: Evita di stampare la password in un log in un'applicazione reale per motivi di sicurezza.
  // Lo facciamo qui solo a scopo didattico.
  RCLCPP_INFO(this->get_logger(), "Password: %s", params_.password.c_str());
  RCLCPP_INFO(this->get_logger(), "Pan Offset (pan0): %.2f gradi", params_.pan0);
  RCLCPP_INFO(this->get_logger(), "Sampling Period: %d ms", params_.sampling_period_ms);
  RCLCPP_INFO(this->get_logger(), "Autostart: %s",params_.autostart ? "Abilitato" : "Disabilitato");
  RCLCPP_INFO(this->get_logger(), "------------------------------------");
}
