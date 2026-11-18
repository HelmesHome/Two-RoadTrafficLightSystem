# Two-RoadTrafficLightSystem
This is a small project using code for XNUCLEO-F103RBBoard

# CO3009 - Course Project: Two-Road Traffic Light System

## 1. Project Description
This repository contains the source code for the Microprocessors - Microcontrollers (CO3009) course project. The system implements a complete Two-Road Traffic Light System running on an STM32F103RB microcontroller, governed by a custom $O(1)$ real-time task scheduler.

### Key Features
*   **Automatic Mode:** Cycles traffic lights (Red, Green, Amber) automatically based on configured durations.
*   **Manual Mode:** Allows manual toggling of traffic light phases via push buttons.
*   **Tuning Mode:** Allows users to adjust the duration of Red, Amber, and Green lights with auto-balancing logic (Red = Green + Amber).
*   **Environment Display:** Reads real-time temperature and humidity from a DHT20 sensor and displays it on a 16x2 I2C LCD along with traffic light statuses.
*   **Delta List $O(1)$ Scheduler:** A robust real-time scheduler managing periodic and one-shot tasks without relying on blocking `HAL_Delay` functions.

## 2. Hardware Setup
*   **Microcontroller Board:** XNUCLEO-F103RB (STM32F103RB)
*   **Traffic Lights:** 2x Traffic Light Modules connected to D3-D4 and D5-D6
*   **Inputs:** 4x Push Buttons connected to D7-D10
*   **Peripherals (I2C):** 16x2 LCD (via PCF8574) and DHT20 Sensor connected to the I2C Grove slot.

## 3. Build and Flashing Instructions

### Prerequisites
*   [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) installed on your computer.
*   The XNUCLEO-F103RB board connected via a Mini-USB cable.

### Build Instructions
1. Clone this repository to your local machine:
   ```bash
   git clone https://github.com/<your-account>/<your-repo>.git
   ```
2. Open **STM32CubeIDE**. Go to `File` -> `Import...` -> `General` -> `Existing Projects into Workspace`.
3. Browse to the cloned repository folder and select it. Click **Finish**.
4. In the Project Explorer, right-click on the project name and select **Build Project** (or click the 🔨 Hammer icon on the toolbar).
5. Ensure the console outputs `0 errors, 0 warnings` (or expected warnings).

### Flashing (Downloading) Instructions
1. Ensure your XNUCLEO-F103RB is plugged into your PC.
2. In STM32CubeIDE, right-click the project folder -> `Run As` -> `1 STM32 Cortex-M C/C++ Application` (or click the ▶️ green Play button).
3. If prompted with the Edit Configuration window:
   * Go to the **Debugger** tab.
   * Ensure the Debug probe is set to **ST-LINK (ST-LINK GDB server)**.
   * Interface must be set to **SWD** (Serial Wire Debug).
4. Click **OK**. The IDE will flash the `.elf` file onto the microcontroller.
5. Once the console says `Download verified successfully`, press the black **RESET** button on the NUCLEO board to start the system.

## 4. Operation Guide
*   **Button 1 (Mode):** Cycles between Auto -> Manual -> Tune Red -> Tune Amber -> Tune Green -> Auto.
*   **Button 2 (Modify):** 
    * In Manual Mode: Transitions to the next light phase.
    * In Tuning Mode: Increases the selected time duration (+1). Hold to auto-increase.
*   **Button 3 (Decrease):** Decreases the selected time duration in Tuning Mode (-1). Hold to auto-decrease.
*   **Button 4 (Confirm/Save):** Saves the modified duration in Tuning Mode and auto-balances the constraint $Red = Green + Amber$.