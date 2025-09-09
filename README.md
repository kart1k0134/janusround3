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


##task 2
# Live 3D GPS Data Plotter via Web Serial API

This project provides a simple and effective way to read GPS data from a serial device (like an Arduino with a GPS module) directly in your web browser and visualize it in real-time on interactive 2D and 3D plots. It utilizes the modern Web Serial API for device communication and Plotly.js for powerful data visualization.

## Features

-   **Driverless Connectivity**: Uses the Web Serial API, so no special drivers or software are needed on the host computer. [2]
-   **Live Data Streaming**: Reads NMEA sentences from your device and updates the plots as new data arrives.
-   **Dual-Plot Visualization**:
    -   A **3D scatter plot** visualizes the device's path in terms of Longitude, Latitude, and Altitude.
    -   A **2D line graph** shows the real-time changes in Altitude.
-   **Simple and Self-Contained**: The entire application is built with basic HTML and JavaScript, making it easy to understand and modify.

## How It Works

1.  **Connect to Device**: Clicking the "Connect to Device" button prompts you to select a serial port to connect to. This initiates a connection with your Arduino or other serial device. [3]
2.  **Data Reception**: The browser listens for incoming data from the serial port. It is configured to read NMEA sentences, which are a standard format for GPS data.
3.  **Data Parsing**: The JavaScript code specifically looks for `$GNGGA` sentences, which contain essential fix data. [4, 8, 9]
4.  **Coordinate Conversion**: The latitude and longitude, which are in a Degrees Minutes format in the NMEA sentence, are converted to decimal degrees for plotting.
5.  **Live Plotting**: The extracted latitude, longitude, and altitude are then plotted on the 3D graph, and the altitude is simultaneously plotted on the 2D graph. Plotly.js handles the dynamic updating of these plots.

## Setup and Usage

### Prerequisites

-   A modern web browser that supports the Web Serial API (e.g., Google Chrome, Microsoft Edge).
-   An Arduino or similar microcontroller with a GPS module connected.
-   The Arduino must be programmed to output GPS data in the NMEA format (specifically `$GNGGA` sentences) over its serial connection.






