// Stair and Depth Alert System for Blind Persons
// Uses HC-SR04 Ultrasonic Sensor tilted at ~30 degrees

// Define pins
const int trigPin = 9;
const int echoPin = 10;
const int buzzerPin = 8;
const int vibrationPin = 7; // Connect a vibration motor here

// Parameters
const float sensorHeight = 15.0; // Sensor height from ground in cm
const float sensorAngle = 30.0; // Sensor tilt angle in degrees (approx 30°)
const int sampleSize = 5; // Number of samples for averaging
const int maxValidDistance = 200; // Maximum valid distance in cm
const int minValidDistance = 2; // Minimum valid distance in cm

// Alert thresholds
const int warningDistance = 50; // cm - starts gentle alert
const int criticalDistance = 30; // cm - stronger alert
const int immediateDangerDistance = 15; // cm - urgent alert

// Variables
float previousValidDistance = sensorHeight; // Initialize with expected ground distance

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(vibrationPin, OUTPUT);
  
  Serial.begin(9600); // For debugging
  
  // Initial calibration beep
  tone(buzzerPin, 1000, 100);
  delay(200);
  tone(buzzerPin, 1500, 100);
  delay(200);
  tone(buzzerPin, 2000, 100);
}

float getFilteredDistance() {
  float distances[sampleSize];
  int validSamples = 0;
  
  // Take multiple samples
  for (int i = 0; i < sampleSize; i++) {
    // Trigger the sensor
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Read echo time
    long duration = pulseIn(echoPin, HIGH);
    float distance = duration * 0.034 / 2; // Convert to cm
    
    // Filter out invalid readings
    if (distance >= minValidDistance && distance <= maxValidDistance) {
      distances[validSamples] = distance;
      validSamples++;
    }
    delay(10); // Short delay between samples
  }
  
  // If no valid samples, return previous valid distance
  if (validSamples == 0) {
    return previousValidDistance;
  }
  
  // Calculate median of valid samples (better than average for filtering outliers)
  sortArray(distances, validSamples);
  float medianDistance = distances[validSamples/2];
  
  // Apply angle compensation (simple trigonometry)
  // Actual vertical distance = measured distance * cos(angle)
  float angleRad = sensorAngle * PI / 180;
  float verticalDistance = medianDistance * cos(angleRad);
  
  // Calculate expected ground distance based on sensor height and angle
  float expectedGroundDistance = sensorHeight / cos(angleRad);
  
  // If we're detecting the ground, return the sensor height
  if (abs(verticalDistance - sensorHeight) < 5) { // 5cm tolerance for ground detection
    previousValidDistance = sensorHeight;
    return sensorHeight;
  }
  
  // If we detect a significant drop (stairs or hole)
  if (verticalDistance > sensorHeight + 10) { // 10cm threshold for drop detection
    previousValidDistance = verticalDistance;
    return verticalDistance;
  }
  
  // If we detect an obstacle (distance less than expected ground)
  if (verticalDistance < sensorHeight - 5) { // 5cm threshold for obstacle
    previousValidDistance = verticalDistance;
    return verticalDistance;
  }
  
  // Otherwise return previous valid distance
  return previousValidDistance;
}

// Simple bubble sort for the distance array
void sortArray(float *array, int size) {
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (array[j] > array[j+1]) {
        float temp = array[j];
        array[j] = array[j+1];
        array[j+1] = temp;
      }
    }
  }
}

void alertUser(float distance) {
  // Calculate vertical distance (compensate for angle)
  float angleRad = sensorAngle * PI / 180;
  float verticalDistance = distance * cos(angleRad);
  
  // Determine if we're detecting stairs/drop (distance > expected ground)
  float expectedGroundDistance = sensorHeight / cos(angleRad);
  bool isDrop = (verticalDistance > sensorHeight + 10);
  
  // Different alert patterns for drops vs obstacles
  if (isDrop) {
    // Stair/drop detection
    float dropDepth = verticalDistance - sensorHeight;
    
    if (dropDepth > 20) { // Significant drop
      // Loud, urgent alert with vibration
      tone(buzzerPin, 2000, 300);
      digitalWrite(vibrationPin, HIGH);
      delay(300);
      noTone(buzzerPin);
      digitalWrite(vibrationPin, LOW);
      delay(map(constrain(dropDepth, 20, 100), 20, 100, 500, 100));
    } else if (dropDepth > 10) { // Moderate drop
      // Pulsing alert
      tone(buzzerPin, 1500, 100);
      digitalWrite(vibrationPin, HIGH);
      delay(100);
      noTone(buzzerPin);
      digitalWrite(vibrationPin, LOW);
      delay(map(constrain(dropDepth, 10, 20), 10, 20, 300, 150));
    }
  } else {
    // Obstacle detection
    if (distance < immediateDangerDistance) {
      // Continuous alert for immediate danger
      tone(buzzerPin, 2500);
      digitalWrite(vibrationPin, HIGH);
    } else if (distance < criticalDistance) {
      // Fast beeping for critical distance
      tone(buzzerPin, 2000, 100);
      digitalWrite(vibrationPin, HIGH);
      delay(100);
      noTone(buzzerPin);
      digitalWrite(vibrationPin, LOW);
      delay(100);
    } else if (distance < warningDistance) {
      // Slow beeping for warning distance
      tone(buzzerPin, 1500, 100);
      digitalWrite(vibrationPin, HIGH);
      delay(100);
      noTone(buzzerPin);
      digitalWrite(vibrationPin, LOW);
      delay(map(constrain(distance, criticalDistance, warningDistance), 
                criticalDistance, warningDistance, 200, 500));
    } else {
      // No alert
      noTone(buzzerPin);
      digitalWrite(vibrationPin, LOW);
    }
  }
}

void loop() {
  float distance = getFilteredDistance();
  
  // Debug output
  Serial.print("Measured: ");
  Serial.print(distance);
  Serial.print(" cm | Vertical: ");
  Serial.print(distance * cos(sensorAngle * PI / 180));
  Serial.println(" cm");
  
  // Alert user based on distance
  alertUser(distance);
  
  // Short delay to prevent sensor interference
  delay(50);
}