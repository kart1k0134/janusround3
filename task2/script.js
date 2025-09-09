// waiting for html file to load 
document.addEventListener('DOMContentLoaded', () => {

    const connectButton = document.getElementById('connectButton');
    //Get references to both plot containers
    const plot3dDiv = document.getElementById('plot3d-container');
    const plot2dDiv = document.getElementById('plot2d-container');

    //  Plotting Function

    // Function to initialize both plots
    function initializePlots() {
        //  Initialize the 3D plot
        Plotly.newPlot(plot3dDiv, [{
            x: [], y: [], z: [], // Initialize coordinates for plot
            mode: 'markers',
            type: 'scatter3d',
            name: 'GPS Path',
            marker: {
                size: 5,
                color: '#007bff',
                symbol: 'circle'
            }
        }], {
            title: 'Live 3D GPS Plot',
            scene: {
                xaxis: { title: 'Longitude' },
                yaxis: { title: 'Latitude' },
                zaxis: { title: 'Altitude (m)' }
            },
            margin: { l: 0, r: 0, b: 0, t: 40 }
        });

        //  Initialize the 2D plot
        Plotly.newPlot(plot2dDiv, [{
            y: [], // Altitude data
            mode: 'lines',
            type: 'scatter',
            name: 'Altitude',
            line: {
                color: '#17a2b8'
            }
        }], {
            title: 'Live Altitude Data',
            xaxis: { title: 'Data Point' },
            yaxis: { title: 'Altitude (m)' }
        });
    }

    // function for 3d plot
    function update3dPlot(lon, lat, alt) {
        Plotly.extendTraces(plot3dDiv, {
            x: [[lon]],
            y: [[lat]],
            z: [[alt]]
        }, [0]);
    }

    // function for 2d plot
    function update2dPlot(alt) {
        Plotly.extendTraces(plot2dDiv, {
            y: [[alt]]
        }, [0]);
    }

    //  NMEA GPS Sentence Parsing
    function convertNMEAToDecimal(coord, hemisphere) {
        const degrees = Math.floor(coord / 100);
        const minutes = coord % 100;
        let decimal = degrees + (minutes / 60);

        if (hemisphere === 'S' || hemisphere === 'W') {
            decimal = -decimal;
        }
        return decimal;
    }

    //Web Serial API Connection Logic
    async function connectToSerialPort() {
        if (!('serial' in navigator)) {
            alert("Error: Web Serial API is not supported by this browser. Please use a modern browser like Chrome or Edge.");
            return;
        }

        try {
            connectButton.disabled = true;
            connectButton.textContent = 'Connecting...';
            const port = await navigator.serial.requestPort();
            await port.open({ baudRate: 9600 });
            console.log("Serial port opened successfully.");
            connectButton.textContent = 'Receiving Data...';

            const textDecoder = new TextDecoderStream();
            const readableStreamClosed = port.readable.pipeTo(textDecoder.writable);
            const reader = textDecoder.readable.getReader();

            while (true) {
                const { value, done } = await reader.read();
                if (done) {
                    reader.releaseLock();
                    break;
                }

                const lines = value.trim().split('\n');
                for (const line of lines) {
                    if (line.startsWith('$GNGGA')) {
                        const parts = line.split(',');
                        if (parts.length > 9) {
                            const rawLat = parseFloat(parts[2]);
                            const latHemisphere = parts[3];
                            const rawLon = parseFloat(parts[4]);
                            const lonHemisphere = parts[5];
                            const altitude = parseFloat(parts[9]);

                            if (!isNaN(rawLat) && !isNaN(rawLon) && !isNaN(altitude) && latHemisphere && lonHemisphere) {
                                const latitude = convertNMEAToDecimal(rawLat, latHemisphere);
                                const longitude = convertNMEAToDecimal(rawLon, lonHemisphere);

                                console.log(`Parsed Data -> Lat: ${latitude.toFixed(6)}, Lon: ${longitude.toFixed(6)}, Alt: ${altitude}m`);

                                // NEW: Call both update functions with the parsed data
                                update3dPlot(longitude, latitude, altitude);
                                update2dPlot(altitude);
                            }
                        }
                    }
                }
            }

        } catch (error) {
            console.error("Web Serial Error: ", error);
            alert(`Error: ${error.message}`);
        } finally {
            connectButton.disabled = false;
            connectButton.textContent = 'Connect to Device';
        }
    }


    // calling the function that creates both plots
    initializePlots();

    connectButton.addEventListener('click', connectToSerialPort);
});