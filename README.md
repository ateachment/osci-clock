Arduino-based analog clock drawn on an oscilloscope in X-Y mode
   
<img src="https://github.com/ateachment/osci-clock/blob/main/GouldWithARDnanoV4.jpg?raw=true" alt="Gould Osci with MC-Board ARD nano V4" width="400"/>

Generates analog signals via a low-pass filter of PWM output.<br>

Clock can be advanced by a push button. <br>

DELAY may need to be tuned for your scope (I used a Gould 50 MHz Oscilloskope 350).<br>

# Oscilloscope Setup & XY Configuration
To display the vector clock correctly on your oscilloscope, connect the hardware and adjust the instrument settings as follows:

## 1. Hardware Connections (BNC)
**Channel 1 (X-Axis):** Connect to the Arduino Pin 5 (via your RC low-pass filter).

**Channel 2 (Y-Axis):** Connect to the Arduino Pin 6 (via your RC low-pass filter).

**Ground:** Ensure a common ground between the Arduino and the oscilloscope.

## 2. Oscilloscope Instrument Settings
**Display Mode:** Switch the oscilloscope from YT (Time-Base) to XY Mode.

**Channel Coupling:** Set both CH1 and CH2 to DC Coupling.

**Volt/Div:** Adjust the volts per division on both channels until the clock dial fills the screen without clipping.

**Position:** Use the horizontal and vertical position knobs to center the clock on the screen graticule.

## 3. Optimization for Digital Storage Oscilloscopes (DSO)
If you are using a DSO (like the Gould storage scope) rather than a pure analog instrument, apply these settings to achieve a solid, flicker-free vector image:

**Persistence:**  Locate the display menu and set Persistence to Infinite (or a fixed decay of 1–2 seconds). This forces the scope to retain the sequentially drawn vector points, connecting them into smooth, solid lines.

**Acquisition/Sampling:** Ensure the scope is capturing data points continuously to prevent any stuttering or gaps in the clock hands.


<img src="https://github.com/ateachment/osci-clock/blob/main/schematic.png?raw=true" alt="Wiring diagram" width="400"/>




(Inspired by https://github.com/daveyburke/Oscilloscope-Clock)