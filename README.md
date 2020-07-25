## Sensortastic! ✨

With this project it's possible to collect temperature and humidity data using an esp8266 and save it to an InfluxDB database. To make a nice dashboard overview in Grafana afterwards. It's also possible to access a web interface via the IP address, so that you can always view the values per sensor in a simple way (as seen in the screenshot below).

<p align="center">
    <img src="./assets/img/webinterface.png" alt="drawing" width="350"/>
</p>

If you have suggestions or questions, open an [issue][issues] or contribute via a [pull request][pr]!

### 🧰 What do you need?

- ESP8266
- DHT22 sensor (you can also use other types)
- USB power :smile:
- Docker :whale:

#### Schematic

| DHT22 | NodeMCU / ESP8266 |
| --- | --- |
| DAT | D1 (GPIO5) |
| VCC | VCC (3V3) |
| GND | GND |

If interested, I plan to design a circuit board for this project in the future.

### How to setup

- You need Arduino IDE to configure some files, and use the serial monitor (other IDE's are okey).
- You need the `ESP8266WebServer`,`Scheduler` and `InfluxDbClient` libraries.

Further down this page you will find all information about which adjustments you have to make.

### Credentials

#### Software

Before we can run Grafana and InfluxDB, there are a few values that we have to add in the yaml files for docker swarm / docker-compose.

##### Docker-compose

If you use the docker-compose option, adjust the `.env` file:

```bash
INFLUXDB_NAME=sensortastic
INFLUXDB_USERNAME=thebigboss
INFLUXDB_PASSWORD=sexypassword

GRAFANA_USERNAME=admin
GRAFANA_PASSWORD=admin
```

After this you can run it with `docker-compose up -d`

##### Docker stack (swarm)

If you use the docker stack deploy (swarm) option, adjust the `sensortastic.yml` file:

```yaml
# InfluxDB
environment:
  INFLUXDB_DB: sensortastic
  INFLUXDB_HTTP_AUTH_ENABLED: 'true'
  INFLUXDB_ADMIN_USER: thebigboss
  INFLUXDB_ADMIN_PASSWORD: sexypassword

# Grafana
environment:
  GF_SECURITY_ADMIN_USER: admin
  GF_SECURITY_ADMIN_PASSWORD: admin
```

You can deploy it with `docker stack deploy -c sensortastic.yml sensortastic`

##### Grafana

For grafana we only need to supplement the data source with the database name and admin account credentials. You can also skip this step and add your data source via the UI later.

```yaml
datasources:
  - name: InfluxDB
    type: influxdb
    access: proxy
    database:
    user: 
    password:
    url: http://influxdb:8086
    isDefault: true
    editable: true
```

---

#### Hardware

Within the arduino project there is a `config.h`, which contains all the necessary variables that the maincode needs to run properly.

```C++
// Basic Information
#define LOCATION        "Bathroom"

// Wi-Fi credentials
#define WIFI_SSID       "wifi6"
#define WIFI_PASSWORD   "ddoswillwork"

// InfluxDB
#define INFLUXDB_URL          "https://IP_TO_SERVER:8086"
#define INFLUXDB_MEASUREMENT  "example"
#define INFLUXDB_DB_NAME      "sensortastic"
#define INFLUXDB_USER         "thebigboss"
#define INFLUXDB_PASSWORD     "sexypassword"

// Settings
const bool uploadLedPulse = true;
const float waitTime = 60 * 60 * 1000;       // 1 hour
```

If you set the **uploadLedPulse** to `true`, the ESP8266 will flash its LED every time the data upload to InfluxDB was successful, if you prefer that the LED does not pulse then set the value to `false`. **waitTime** determines how long to wait until new data is saved again.

## Troubleshooting

If you get an error, create then an [issue][issues] so that I can sort things out for you.

[issues]: https://github.com/klaasnicolaas/Project-Sensortastic/issues
[pr]: https://github.com/klaasnicolaas/Project-Sensortastic/pulls