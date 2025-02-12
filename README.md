# Smart-Trash-Bin

The Smart Trash Bin is an automated system that opens its lid when a person approaches and monitors its fill level. It uses an ESP32 microcontroller, ultrasonic sensors, servo motor, LEDs, LCD screen, and WiFi connectivity to collect and display data.

Features

    Automatic Lid Opening: Uses ultrasonic sensors to detect a person nearby and opens the lid.
    Bin Fill Level Detection: Monitors trash level and alerts when it's full.
    Visual Indicators:
        Green LED: Bin is ready to use.
        Red LED: Bin is full.
    LCD Display: Shows system status.
    Cloud Data Logging: Sends bin status to ThingSpeak for remote monitoring.
    SD Card Logging: Stores event logs locally.

Components Used

    ESP32 (WiFi-enabled microcontroller)
    Ultrasonic Sensors (Detect objects and measure bin level)
    Servo Motor (Opens and closes the bin lid)
    LEDs (Red and Green indicators)
    LCD Display (Shows messages like "Bin is Full")
    SD Card Module (Stores logs)
    WiFi Connection (Sends data to the cloud)

How It Works

    Idle Mode: The bin waits for someone to approach.
    Person Detected: If someone is nearby and the bin isn’t full, the lid opens.
    Trash Detection: The system checks if the bin is reaching its capacity.
    Full Bin Alert: If the bin is 90% full, the red LED lights up and the bin won’t open.
    Data Logging: The system sends status updates to ThingSpeak and stores logs on an SD card.

You can check out the live demo here:
👉 https://wokwi.com/projects/417931704777582593
