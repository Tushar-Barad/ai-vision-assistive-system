
## 📌 Overview
An AI-powered assistive system designed to help visually impaired individuals understand their surroundings through real-time audio feedback.

This system integrates computer vision with embedded hardware to detect objects and environmental changes, converting them into speech output.

---

##  Key Features
-  Real-time image capture using ESP32-CAM  
-  AI-based scene understanding using OpenAI API  
-  Voice feedback using web-based text-to-speech  
-  Depth detection using ultrasonic sensor  
-  Button-triggered intelligent response system  

---

## 🧠 System Architecture
1. Image captured via ESP32-CAM  
2. Image encoded to Base64  
3. Sent to AI API for analysis  
4. Text response generated  
5. Sent to Flask server  
6. Converted to speech output  

Depth Detection:
- Ultrasonic sensor detects sudden distance increase  
- Used to identify stairs / drops  

---

## 🛠 Tech Stack
- Embedded System: ESP32-CAM  
- Backend: Python (Flask)  
- AI: OpenAI API  
- Communication: HTTP / JSON  
- Sensors: Ultrasonic Sensor  

---

## ⚙️ How It Works
- User presses button  
- Device captures image  
- AI analyzes scene  
- System provides audio description  

---

## 🎯 Use Case
Assistive technology for visually impaired individuals to navigate safely and independently.

---

## 🔮 Future Improvements
- Real-time continuous detection  
- Offline AI processing  
- Compact hardware integration  
- Battery optimization  

---
## 📷 Demo

<img width="3072" height="4096" alt="image" src="https://github.com/user-attachments/assets/7da142ec-d288-480a-a706-c8837c1b3481" />

video demo -  https://drive.google.com/file/d/146jbzx7vG6mVJuqVmpyGwqjQxipi_6GZ/view?usp=drivesdk





*thiss is just thing that i need to possible later i update so much good thing that is far batter than this
also this project focus on cost cutting that is possible by intigrate AI and IOT 

enjoy and use it and learn it 
