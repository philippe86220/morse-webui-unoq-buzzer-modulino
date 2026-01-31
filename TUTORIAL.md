
# Tutorial – Morse WebUI on Arduino UNO Q

## 1. Introduction

This tutorial explains in detail how the **Morse WebUI project** works internally and how it can be reproduced step by step.  
It is intended for **university students**, educators, and developers who want to understand:

- how the Arduino UNO Q architecture works,
- how a Web interface can control hardware behavior,
- how responsibilities are split between Linux and the MCU,
- how real-time constraints are handled correctly.

The project uses Morse code as a concrete and educational example, but the concepts apply to many other domains.

---

## 2. Arduino UNO Q Architecture Overview

The Arduino UNO Q is composed of two distinct computing domains:

### Linux Core
- Runs a full Linux system
- Executes Python code
- Hosts application-level services (WebUI, HTTP APIs)
- Manages scheduling, threading, and synchronization

### MCU (STM32)
- Executes Arduino sketches
- Handles real-time behavior
- Drives hardware peripherals (buzzer, LEDs, sensors)
- Must remain simple, deterministic, and responsive

These two domains communicate using the **Arduino Bridge**.

---

## 3. Conceptual Architecture of the Project

```
User (Browser)
   |
   | HTTP requests
   v
WebUI (Linux application layer)
   |
   | Python logic + queue + synchronization
   v
Bridge (Linux ↔ MCU)
   |
   | function call with parameters
   v
MCU (STM32)
   |
   | real-time Morse playback
   v
Modulino Buzzer
```

This architecture enforces a clear separation of concerns:
- **Web**: user interaction and visualization
- **Linux/Python**: orchestration and safety
- **MCU**: real-time execution

---

## 4. Data Flow: From Click to Sound

Let us follow the complete path of a Morse message:

1. The user enters text in the Web interface and clicks *Send*.
2. The browser sends an HTTP request:
   ```
   /morse?data=HELLO&speed=17
   ```
3. WebUI routes the request to the Python function `morse()`.
4. Python validates parameters and pushes the request into a queue.
5. The main loop checks if the MCU is idle.
6. Python calls:
   ```python
   bridge.call("morse_play", text, speed)
   ```
7. The MCU receives the parameters.
8. Morse code is generated and played on the buzzer.
9. Python updates the internal status.

---

## 5. Web Layer (HTML / JavaScript)

### 5.1 index.html

The HTML file defines:
- text input
- speed slider
- buttons
- a container for Morse visualization

CSS is used to:
- scale the slider,
- apply consistent colors,
- visually represent dots and dashes.

### 5.2 app.js

The JavaScript code:
- listens to user input events,
- dynamically updates the speed value,
- builds URLs with query parameters,
- sends HTTP requests using `fetch()`,
- renders Morse symbols graphically.

Important design choice:
> The Web layer never talks directly to the MCU.

It only communicates with the Linux-side API.

---

## 6. Python Layer (Linux Side)

### 6.1 Why Python Handles Orchestration

Python is used because:
- it runs comfortably on Linux,
- it supports threading and synchronization,
- it can safely manage queues and shared state.

### 6.2 Request Queue

A queue is used to:
- avoid concurrent MCU access,
- allow multiple browser tabs,
- serialize Morse playback requests.

### 6.3 Thread Safety

A lock protects:
- the queue,
- the global speed value,
- the status structure.

This prevents race conditions.

### 6.4 The Main Loop

The loop:
- checks if the MCU is busy,
- pops a job from the queue,
- calls the MCU via Bridge,
- waits for completion,
- updates status.

This design acts as a **scheduler**.

---

## 7. Bridge Communication

The Arduino Bridge allows:
- calling named functions on the MCU,
- passing parameters,
- receiving acknowledgements.

Example:
```python
bridge.call("morse_play", text, speed)
```

On the MCU side:
```cpp
Bridge.provide("morse_play", morse_play);
```

This creates a clean and explicit interface.

---

## 8. MCU Layer (Arduino / STM32)

### 8.1 Design Philosophy

The MCU:
- does not manage networking,
- does not parse HTTP,
- does not deal with concurrency.

It only:
- receives clean parameters,
- performs real-time work.

### 8.2 Morse Implementation

- A static Morse table maps characters to symbols.
- Speed is converted to time units.
- Dot, dash, and spacing follow standard Morse rules.
- Unsupported characters are ignored safely.

### 8.3 Timing Accuracy

Morse timing:
- dot = 1 unit
- dash = 3 units
- inter-element gap = 1 unit
- inter-letter gap = 3 units
- inter-word gap ≈ 7–8 units

This was validated by ear by a former Morse instructor.

---

## 9. Step-by-step Experiments (UNO Q)

### Step 1 – System startup (Run)

Click `Run` in the Arduino App Lab environment.  

This single action automatically :  

- flashes and restarts the MCU (STM32),
- starts the Linux environment,
- launches the Python application (`main.py`),
- starts the WebUI server,
- initializes the Bridge between Linux and the MCU.

**Expected result :**
  
- the buzzer emits two short beeps at startup,
- the Python monitor shows messages such as:
  
```
Activating python virtual environment
======== App is starting ============================
2026-01-31 16:25:38.506 INFO - [WebUI.execute] WebUI:  The application interface is available here:
- Local URL:   http://localhost:7000
- Network URL: <UNO_Q_IP>:7000
2026-01-31 16:25:38.506 INFO - [MainThread] App:  App started
```
No manual action is required at this stage.  

---


### Step 2 – WebUI availability check

Open a web browser on the same network and navigate to: 
```
http://<UNO_Q_IP>:7000/ping
```
**Expected result** :  
```
{ "ok": true }
```
This confirms that:
- the Linux application is running,
- the WebUI layer is active,
- HTTP communication is operational.

  ---

### Step 3 – API Morse playback test

In the browser, open :  

```
http://<UNO_Q_IP>:7000/morse?data=SOS&speed=15
```
Expected result:
- the request is accepted immediately,
- Morse playback starts on the buzzer,
- the call returns a JSON confirmation.
- This validates the full path :
```
WebUI → Python → Bridge → MCU → Buzzer
```



### Step 4 – Web interface usage

Open the main Web UI:   
```
http://<UNO_Q_IP>:7000/
```

**Expected result** :
- text input field for Morse messages,
- adjustable speed slider,
- real-time visual representation (dots and dashes),
- synchronized Morse audio playback.  
Multiple browser tabs or devices can interact with the WebUI alternately without blocking the system.

---

## 10. Common Issues

- No sound: check buzzer wiring and Modulino initialization.
- 404 error: Python WebUI not running.
- Bridge error: function name mismatch.
- No response: MCU not flashed or busy.

---

## 11. Educational Value

After completing this tutorial, students understand:
- layered system design,
- separation of concerns,
- safe concurrency handling,
- real-time vs non-real-time tasks,
- practical Web-to-hardware integration.

---

## 12. Conclusion

This project is not only a Morse code tool.  
It is a **reference educational example** showing how to design clean, maintainable systems on the Arduino UNO Q.

The same architecture can be reused for:
- LED control,
- displays,
- sensors,
- radio interfaces,
- robotics.




