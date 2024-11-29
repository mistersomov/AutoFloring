
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
- ESP32 Development Board ![ESP32](https://www.google.com/url?sa=i&url=https%3A%2F%2Faliexpress.ru%2Fitem%2F1005005104863437.html&psig=AOvVaw0b4j7lPHmfJGxaukokwS_h&ust=1732995368764000&source=images&cd=vfe&opi=89978449&ved=0CBQQjRxqFwoTCOi6wv2kgooDFQAAAAAdAAAAABAJ)
- Soil Moisture Sensor ![Soil Sensor](https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.flipkart.com%2F2a-digital-soil-moisture-sensor-module-electronic-components-hobby-kit%2Fp%2Fitmf46a71b5245c9%3Fpid%3DEHKGD396KRKJQVY3%26cmpid%3Dproduct.share.pp%26_refId%3DPP.e5cdbc3a-381f-4b9a-b22f-295692e93ae0.EHKGD396KRKJQVY3%26lid%3DLSTEHKGD396KRKJQVY3GOGLSH&psig=AOvVaw141x29eix2ZqImPyXBPoSd&ust=1732995406010000&source=images&cd=vfe&opi=89978449&ved=0CBQQjRxqFwoTCODCo5ClgooDFQAAAAAdAAAAABAE)
- Water Level Sensor ![Water sensor image](https://www.google.com/url?sa=i&url=https%3A%2F%2Faliexpress.ru%2Fpopular%2Farduino-%25D0%25BF%25D1%2580%25D0%25BE%25D1%2582%25D0%25B5%25D1%2587%25D0%25BA%25D0%25B0-water-sensor.html&psig=AOvVaw1kFkFTEkQ47iwjnyuuQhMk&ust=1732995233301000&source=images&cd=vfe&opi=89978449&ved=0CBQQjRxqFwoTCPDus8mkgooDFQAAAAAdAAAAABAE)
- 16x2 LCD with I2C Interface (e.g. 1602a) ![LCD Display](https://www.google.com/url?sa=i&url=https%3A%2F%2Farduinokit.com.ua%2Fp1433688907-displej-lcd-1602.html&psig=AOvVaw1ofqEMLNfaHwSuq8ARXGLv&ust=1732995299656000&source=images&cd=vfe&opi=89978449&ved=0CBQQjRxqFwoTCNjGitykgooDFQAAAAAdAAAAABAS)
- Water Pump 5V ![Water pump](https://www.google.com/url?sa=i&url=https%3A%2F%2Faliexpress.ru%2Fitem%2F1005002610901356.html&psig=AOvVaw1SwXnqHI93Z4YlexPeRrGt&ust=1732995426080000&source=images&cd=vfe&opi=89978449&ved=0CBQQjRxqFwoTCMin7ZelgooDFQAAAAAdAAAAABAE)
- Power Supply (e.g., battery or solar panel)

## Future Enhancements
- Integration with cloud platforms for remote monitoring.
- Advanced scheduling based on weather data.
- Customizable thresholds via a web interface.

## Demo
![Demo](/assets/auto_irrigation.jpg)
