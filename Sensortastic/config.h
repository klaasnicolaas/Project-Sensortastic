///////////////////////////////////////////////////////////////////////////
//  CONFIGURATION
///////////////////////////////////////////////////////////////////////////

// Basic Information
#define LOCATION        ""

// Wi-Fi credentials
#define WIFI_SSID       ""
#define WIFI_PASSWORD   ""

// InfluxDB
#define INFLUXDB_URL          "http://:8086"
#define INFLUXDB_MEASUREMENT  ""
#define INFLUXDB_DB_NAME      ""
#define INFLUXDB_USER         ""
#define INFLUXDB_PASSWORD     ""

// Settings
const bool uploadLedPulse = false;
//const float waitTime = 60 * 1000;       // 1 minute
const float waitTime = 60 * 60 * 1000;  // 1 hour
//const float waitTime = (60 * 60) * 24 * 1000; // 1 day
