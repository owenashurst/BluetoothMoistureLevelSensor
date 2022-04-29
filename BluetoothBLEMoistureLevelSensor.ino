#include <Mouse.h>

#include <ArduinoBLE.h>

BLEService environmentalService("181A"); // Environmental
BLEUnsignedCharCharacteristic humidityCharacteristic("2A6F", BLERead); // Humidity

int moistureLevelSensor = A0;

const int AirValue = 884;   // Calibrated
const int WaterValue = 422; // Calibrated

void setup() {
  Serial.begin(9600);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed");
    while (1);
  }

  // Set a name that will appear when scanning for bluetooth devices
  BLE.setLocalName("Moisture Sensor");
  BLE.setAdvertisedService(environmentalService);

  environmentalService.addCharacteristic(humidityCharacteristic);

  BLE.addService(environmentalService);

  // Set initial value for the characteristic
  humidityCharacteristic.writeValue(0);

  // Start advertising the service
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // Wait for a BLE central
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // Check the moisture level when a central is connected
    while (central.connected()) {
      int soilMoistureValue = analogRead(moistureLevelSensor);
      int moisureLevelPercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
      
      humidityCharacteristic.writeValue(moisureLevelPercent);
  
      Serial.print("Written moisture level percent to characteristic. Moisture level percent: ");
      Serial.println(moisureLevelPercent);

      delay(1000);
    }
    
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
