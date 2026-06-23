# Dual Stepper Motor Control System with dsPIC33F

This project was made for Academic purposes.

**Key Features Demonstrated:**
* Hardware Timers & Interrupt Service Routines (ISRs)
* Analog-to-Digital Conversion (ADC) with hardware timer triggering
* External Interrupts for user input
* Phase-Locked Loop (PLL) clock scaling

## 📌 Project Overview

The system independently controls two stepper motors :

1. **System Clock (PLL):** Configured to achieve a high-performance system frequency of **40 MIPS** using the internal Fast RC (FRC) oscillator.
2. **Motor 1 (Potentiometer Controlled):** Speed is dynamically regulated by reading an analog voltage via an ADC channel (AN5). The raw 12-bit digital value is inversely mapped to hardware Timer 2 periods.
3. **Motor 2 (Button Controlled):** Speed is changed incrementally via an external push-button (S2) connected to the External Interrupt 0 (`INT0`). Pressing the button cycles through predefined speed steps.

## 📂 Project Structure

* **`main.c`** – Contains the core application logic, peripheral initialization functions (`initPLL()`, `initStepper()`, timers, ADC, `INT0`), and the hardware Interrupt Service Routines (ISRs).
* **`header.h`** – Defines the device configurations, timing constants, global volatile variables, and physical pin mappings for the motors, LED, and buttons.
