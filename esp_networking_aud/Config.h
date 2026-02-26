const bool WIFI_START_MODE_CLIENT = true;
const bool WIFI_START_MODE_AP = false;  // access point

String SSID_AP = "esp8266";
String PASSWORD_AP = "12345678";

int led = LED_BUILTIN;

String SSID_CLI = "Юлия";
String SSID_PASSWORD = "yulya123";

char* mqtt_broker = "broker.emqx.io";
const int mqtt_port = 1883;