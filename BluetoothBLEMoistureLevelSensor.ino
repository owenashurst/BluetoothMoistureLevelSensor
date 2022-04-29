#include <Mouse.h>

#include <ArduinoBLE.h>

BLEService environmentalService("181A"); // creating the service
BLEUnsignedCharCharacteristic humidityCharacteristic("2A6F", BLERead); // creating the Analog Value characteristic

int moistureLevelSensor = A0;

const int AirValue = 884;   //replace the value with value when placed in air using calibration code 
const int WaterValue = 422; //replace the value with value when placed in water using calibration code 

void setup() {
  Serial.begin(9600);    // initialize serial communication

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed");
    while (1);
  }

  BLE.setLocalName("Moisture Sensor"); //Setting a name that will appear when scanning for bluetooth devices
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

  if (central) {  // if a central is connected to the peripheral
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
