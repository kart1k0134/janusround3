# janusround3
missed my dinner ;( [induct kar lo plis]

# janusround3

Submission repository for JANUS Inductions Round 3.  
This repo contains solutions to multiple tasks.  

***5 mins were left so got my readme edited by chatgpt pls forgive me <3

##  Task 1 — Flight Software (GNSS Module & State Machine)

Question Recap
Write Arduino code to set up a **Quectel L89HA GNSS Module** for a CanSat to receive and parse GPS data.  
The requirements were:
1. Print **time, latitude, longitude, and MSL altitude** to the serial monitor in a clean format.  
2. Configure the module to only enable **GNRMC** and **GNGGA** NMEA sentences.  
3. Use the altitude data to implement a **state machine** with the following states:  
   - IDLE: CanSat stationary on the ground  
   - ASCENT: CanSat ascending  
   - APOGEE: Peak altitude reached  
   - DESCENT: Descending without payload deployed  
   - PAYLOAD DEPLOYED: Descending after payload deployed at 75% of peak height  
   - LANDED: Landed at ground level  

**Implementation Highlights**
- Uses SoftwareSerial to communicate with the GPS module.  
- Integrates the TinyGPS++ library for parsing GNSS data.  
- Implements a clean class-based design (CanSat class) to handle:
  - GPS parsing  
  - State machine logic  
  - Logging of flight status  
- State transitions are based on thresholds for ascent, apogee detection, payload deployment, and landing.  
- Outputs live telemetry (time, lat, lon, alt, state) to the serial monitor.  


## Task 2 — (to be filled in)
 Add description of Task 2 here once finalized:
- Problem statement
- Approach/implementation details
- Any special libraries or hardware used


## How to Run

1. Open the code in Arduino IDE.  
2. Install the required library:  
   - [TinyGPS++](https://github.com/mikalhart/TinyGPSPlus)  
3. Connect the **Quectel L89HA GNSS module**:  
   - RX → Pin 4  
   - TX → Pin 5  
   - Baud rate = 9600  
4. Upload the code to your Arduino board.  
5. Open Serial Monitor at 115200 baud to see live output.


##  Tech Stack
- Arduino C++
- TinyGPS++ library
- SoftwareSerial






