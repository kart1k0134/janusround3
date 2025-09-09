#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Configuration Constants 
// Pin definitions
const int GPS_RX_PIN = 4;
const int GPS_TX_PIN = 5;

// Baud rates
const long SERIAL_MONITOR_BAUD = 115200;
const long GPS_BAUD = 9600;

// State machine thresholds
const float ASCENT_THRESHOLD_M = 10.0;       // Altitude gain to trigger ascent state
const float APOGEE_DETECTION_DROP_M = 5.0;   // Altitude drop from peak to confirm apogee
const float PAYLOAD_DEPLOY_FRACTION = 0.75;  // 75% of peak altitude
const float LANDING_THRESHOLD_M = 5.0;       // distance from initial altitude to trigger landed

//State Machine Definition
enum class FlightState {
  IDLE,
  ASCENT,
  APOGEE,
  DESCENT,
  PAYLOAD_DEPLOYED,
  LANDED
};

// CanSat Logic Class
class CanSat {
public:
  // Constructor: Takes the GPS serial stream and a logging stream as dependencies
  CanSat(Stream& gpsStream, Stream& logStream) : 
    gpsPort(gpsStream), 
    logPort(logStream), 
    currentState(FlightState::IDLE),
    peakAltitude(0.0),
    initialAltitude(0.0),
    isInitialized(false)
  {}

  // Call once in setup() to configure the module and initialize state
  void begin() {
    logPort.println("Initializing CanSat...");
    configureGpsModule();
    logPort.println("GPS module configured. Waiting for fix...");
  }

  // Call repeatedly in loop() to process GPS data and update state
  void update() {
    // Feed characters from the GPS stream to the TinyGPS++ 
    while (gpsPort.available() > 0) {
      gps.encode(gpsPort.read());
    }

    // Process a new sentence
    if (gps.location.isUpdated() && gps.altitude.isUpdated() && gps.time.isUpdated()) {
      float currentAltitude = gps.altitude.meters();

      // Set the initial ground level altitude on the first valid fix
      if (!isInitialized && currentAltitude > 0) {
        initialAltitude = currentAltitude;
        peakAltitude = currentAltitude;
        isInitialized = true;
        logPort.println("Initial altitude locked.");
      }

      if (isInitialized) {
        updateStateMachine(currentAltitude);
        printStatus();
      }
    }
  }

private:
  // NMEA command helper function
  void sendGpsCommand(const char* command) {
    byte checksum = 0;
    // Start after '$' and go up to  '*'
    for (int i = 1; command[i] != '\0' && command[i] != '*'; i++) {
      checksum ^= command[i];
    }
    gpsPort.print(command);
    gpsPort.print('*');
    if (checksum < 16) gpsPort.print('0');
    gpsPort.println(checksum, HEX);
  }
  
  // Sends the specific commands to enable GNGGA and GNRMC sentences
  void configureGpsModule() {
    // Using PMTK commands which are common for many chipsets.
    // 314 is the packet type for setting NMEA output.
    // The six fields are RMC, VTG, GGA, GSA, GSV, GLL. 1=on, 0=off.
    // enable RMC and GGA .
    sendGpsCommand("$PMTK314,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");
  }

  // The core state machine logic
  void updateStateMachine(float currentAltitude) {
    switch (currentState) {
      case FlightState::IDLE:
        if (currentAltitude > initialAltitude + ASCENT_THRESHOLD_M) {
          currentState = FlightState::ASCENT;
        }
        break;

      case FlightState::ASCENT:
        if (currentAltitude > peakAltitude) {
          peakAltitude = currentAltitude;
        }
        // If altitude drops from its peak, we've hit apogee
        if (currentAltitude < peakAltitude - APOGEE_DETECTION_DROP_M) {
          currentState = FlightState::APOGEE;
        }
        break;

      case FlightState::APOGEE:
        // Apogee is a transient state; immediately move to descent
        currentState = FlightState::DESCENT;
        break;

      case FlightState::DESCENT:
        if (currentAltitude <= (initialAltitude + (peakAltitude - initialAltitude) * PAYLOAD_DEPLOY_FRACTION)) {
          currentState = FlightState::PAYLOAD_DEPLOYED;
        }
        break;

      case FlightState::PAYLOAD_DEPLOYED:
        if (currentAltitude <= initialAltitude + LANDING_THRESHOLD_M) {
          currentState = FlightState::LANDED;
        }
        break;

      case FlightState::LANDED:
        // Stay in the landed state
        break;
    }
  }

  // Prints the current data and state in a clean format
  void printStatus() {
    logPort.print("Time: ");
    logPort.print(gps.time.hour()); logPort.print(":");
    if(gps.time.minute() < 10) logPort.print('0');
    logPort.print(gps.time.minute()); logPort.print(":");
    if(gps.time.second() < 10) logPort.print('0');
    logPort.print(gps.time.second());
    
    logPort.print(" | Lat: ");
    logPort.print(gps.location.lat(), 6);
    
    logPort.print(" | Lon: ");
    logPort.print(gps.location.lng(), 6);
    
    logPort.print(" | Alt: ");
    logPort.print(gps.altitude.meters());
    logPort.print("m");
    
    logPort.print(" | State: ");
    logPort.println(stateToString(currentState));
  }

  // convert enum to a printable string
  const char* stateToString(FlightState state) {
    switch (state) {
      case FlightState::IDLE:             return "IDLE";
      case FlightState::ASCENT:           return "ASCENT";
      case FlightState::APOGEE:           return "APOGEE";
      case FlightState::DESCENT:          return "DESCENT";
      case FlightState::PAYLOAD_DEPLOYED: return "PAYLOAD DEPLOYED";
      case FlightState::LANDED:           return "LANDED";
      default:                          return "UNKNOWN";
    }
  }

  //Member Variables
  TinyGPSPlus gps;
  Stream& gpsPort;
  Stream& logPort;
  
  FlightState currentState;
  float peakAltitude;
  float initialAltitude;
  bool isInitialized;
};

//Arduino specific Implementation
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);
CanSat myCanSat(gpsSerial, Serial);

void setup() {
  Serial.begin(SERIAL_MONITOR_BAUD);
  gpsSerial.begin(GPS_BAUD);
  
  // Wait for serial to be ready 
  while (!Serial); 
  
  myCanSat.begin();
}

void loop() {
  myCanSat.update();
}