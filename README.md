
# Auto-Irrigation System for ESP32 🌱💧
This project is an automated irrigation system powered by an ESP32 
microcontroller. It uses sensors to monitor soil moisture and water levels, controls a pump for irrigation, and logs information on an LCD display. The system ensures efficient water usage while minimizing human intervention.

## Features
- **Soil & Water Monitoring**: Reads data from moisture and water level sensors.
- **Automated Irrigation**: Activates the pump when moisture levels drop below a threshold.
- **LCD Display**: Displays real-time sensor readings and warnings.
- **Wi-Fi Connectivity**: Uses NTP for time synchronization and potential remote monitoring.
- **Energy Efficiency**: Enters deep sleep mode between irrigation cycles.
- **Error Handling**: Provides warnings for low water levels and device initialization failures.

## Hardware Requirements
- ESP32 Development Board
![ESP32](/assets/esp.avif =100x)
- Soil Moisture Sensor
![Soil Sensor](/assets/soil_sensor.webp)
- Water Level Sensor
![Water sensor](/assets/water_sensor.jpg)
- 16x2 LCD with I2C Interface (e.g. 1602a)
![LCD Display](/assets/lcd.webp)
- Water Pump 5V
![Water pump](/assets/pump.avif =100x)
- Power Supply (e.g., battery or solar panel)

## Future Enhancements
- Integration with cloud platforms for remote monitoring.
- Advanced scheduling based on weather data.
- Customizable thresholds via a web interface.

## Screenshots
![Screenshot](/assets/auto_irrigation.jpg)
