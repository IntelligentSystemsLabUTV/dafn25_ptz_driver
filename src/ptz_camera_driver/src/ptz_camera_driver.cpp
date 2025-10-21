#include <ptz_camera_driver/ptz_camera_driver.hpp>

#include <cpr/cpr.h>

//modifica per prova commit edoardo
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PtzCameraDriver>();
  printf("nodo creato");
  //chiamo la funzione setup del nodo per la costuzione mancante
  node->setup();
  rclcpp::spin(node);
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
  RCLCPP_INFO(this->get_logger(), "Autostart: %s",
    params_.autostart ? "Abilitato" : "Disabilitato");
  RCLCPP_INFO(this->get_logger(), "------------------------------------");


  RCLCPP_INFO(this->get_logger(), "Nodo PtzCameraDriver inizializzato con successo.");
}

void PtzCameraDriver::setup()
{
  //  pubblicazione delle immagini
  RCLCPP_INFO(this->get_logger(), "funzione di Setup del nodo...");
  image_transport::ImageTransport it(shared_from_this());
  image_pub_ = it.advertise("image_raw", 1);   // Pubblica su /image_raw

  // Crea la sottoscrizione al topic dei comandi
  command_sub_ = this->create_subscription<axis_camera_interfaces::msg::PTZF>(
        "ptz_command", // Nome del topic
        10,            // Quality of Service
        std::bind(&PtzCameraDriver::command_callback, this, std::placeholders::_1)
  );

  //CREAZIONE DEL SERVIZIO (richiesto dalla tua traccia)
  enable_service_ = this->create_service<std_srvs::srv::SetBool>(
        "enable_disable_stream",
        std::bind(&PtzCameraDriver::enable_disable_callback, this, std::placeholders::_1,
    std::placeholders::_2)
  );

  //abilitazione servizio di enable
  is_active_ = params_.autostart;
  if(is_active_) {
    RCLCPP_INFO(this->get_logger(), "Autostart abilitato. Avvio del flusso video...");
  } else {
    RCLCPP_INFO(this->get_logger(),
      "Autostart disabilitato. Il flusso video è in attesa del servizio di attivazione.");
  }
  //inizializzazione thread pubblicazione video

  video_thread_ = std::thread(&PtzCameraDriver::video_publishing_loop, this);

  RCLCPP_INFO(this->get_logger(), "fine del processo di setup");
}

//distruttore del nodo
PtzCameraDriver::~PtzCameraDriver()
{
    RCLCPP_INFO(this->get_logger(), "Chiusura del nodo...");
    is_active_ = false; // Ferma il loop nel thread
    if (video_thread_.joinable()) {
        video_thread_.join(); // Attende che il thread termini
    }
    RCLCPP_INFO(this->get_logger(), "Nodo chiuso correttamente.");
}

//AGGIUNTA DELLO SCHELETRO DELLE FUNZIONI MANCANTE SOLO PER PROVARE AD ESEGUIRE IL CODICE.

void PtzCameraDriver::command_callback(const axis_camera_interfaces::msg::PTZF::SharedPtr msg)
{
    RCLCPP_INFO(this->get_logger(), "Ricevuto nuovo comando PTZF.");
    //contenitore di parametri da spedire
    cpr::Parameters params;
    //gestione del pan, vedo se devo applicare la compensazione pan0
     if (msg->pan_global) {
        // Comando Pan Assoluto (pan) - Applica la compensazione pan0
        double target_pan = msg->pan + params_.pan0;
        params.Add({"pan", std::to_string(target_pan)});
        RCLCPP_DEBUG(this->get_logger(), "Comando pan assoluto: %.2f", target_pan);
    } else {
        // Comando Pan Relativo (rpan) - (L'offset pan0 è gestito in lettura, non in scrittura relativa)
        params.Add({"rpan", std::to_string(msg->pan)});
        RCLCPP_DEBUG(this->get_logger(), "Comando pan relativo: %.2f", msg->pan);
    }
    //gestione del tilt
     if (msg->tilt_global) {
        // Comando Tilt Assoluto (tilt)
        params.Add({"tilt", std::to_string(msg->tilt)});
        RCLCPP_DEBUG(this->get_logger(), "Comando tilt assoluto: %.2f", msg->tilt);
    } else {
        // Comando Tilt Relativo (rtilt)
        params.Add({"rtilt", std::to_string(msg->tilt)});
        RCLCPP_DEBUG(this->get_logger(), "Comando tilt relativo: %.2f", msg->tilt);
    }
    //gestione zoom
    if (msg->zoom_global) {
        // Comando Zoom Assoluto (zoom)
        params.Add({"zoom", std::to_string(msg->zoom)});
        RCLCPP_DEBUG(this->get_logger(), "Comando zoom assoluto: %.2f", msg->zoom);
    } else {
        // Comando Zoom Relativo (rzoom)
        params.Add({"rzoom", std::to_string(msg->zoom)});
        RCLCPP_DEBUG(this->get_logger(), "Comando zoom relativo: %.2f", msg->zoom);
    }
    //gestione focus
     if (std::abs(msg->focus) > 0.5) { // verifico se tenere o meno l'autofocus
        params.Add({"autofocus", "off"});
        if (msg->focus_global) {
            // Comando Focus Assoluto (focus)
            params.Add({"focus", std::to_string(msg->focus)});
            RCLCPP_DEBUG(this->get_logger(), "Comando focus assoluto: %.2f", msg->focus);
        } else {
            // Comando Focus Relativo (rfocus)
            params.Add({"rfocus", std::to_string(msg->focus)});
            RCLCPP_DEBUG(this->get_logger(), "Comando focus relativo: %.2f", msg->focus);
        }
    } else {
        params.Add({"autofocus", "on"});
        RCLCPP_DEBUG(this->get_logger(), "Comando autofocus on.");
    }
    //aggiunta dei parametri standard
    params.Add({"camera", "1"});
    params.Add({"html", "no"});
    params.Add({"timestamp", std::to_string(std::time(nullptr))}); //per identficare temporalmente la richiesta inviata
    //creazione della richiesta http

    std::string command_url = "http://" + params_.ip + "//axis-cgi/com/ptz.cgi";
    // Invio della richiesta usando l'autenticazione base
    cpr::Response r = cpr::Get(
        cpr::Url{command_url},
        cpr::Parameters{params},
        cpr::Authentication{params_.username, params_.password, cpr::AuthMode::BASIC},
        cpr::Timeout{500} // Timeout di 500ms
    );
    //gestione della risposta, verifica dei codici di risposta
    if (r.status_code == cpr::status::HTTP_OK) {
        RCLCPP_INFO(this->get_logger(), "Comando PTZ inviato. Risposta: %s", r.text.c_str());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Errore invio comando PTZ. Stato: %ld, Errore: %s",
            r.status_code, r.error.message.c_str());
    }
}

// Implementazione della callback per il servizio di attivazione/disattivazione
void PtzCameraDriver::enable_disable_callback(
    const std_srvs::srv::SetBool::Request::SharedPtr request,
    std_srvs::srv::SetBool::Response::SharedPtr response)
{
    //corpo vuoto per ora
    (void)request;
    (void)response;
}

// Implementazione del loop per la pubblicazione video
void PtzCameraDriver::video_publishing_loop()
{
  //corpo vuoto per ora
}
