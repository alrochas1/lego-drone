# HARDWARE DESCRIPTION

This is a description of the hardware, decisions, prices, ... 

## Frame

The LEGO Technic frame is one of the defining constraints of the project. I had multiple boxes of LEGO Technic from my childhood, so I decided to use them as the basis for the drone frame rather than using a conventional drone frame.

Multiple frame iterations were built and tested before reaching the current design. Some of the discarded concepts are shown below.

<img src="img/frames/frame1.png" alt="Frame Assembly" width="600px" />

<img src="img/frames/frame2.png" alt="Frame Assembly" width="600px" />


The final design prioritizes rigidity, compactness, and component integration. The selected dimensions also constrain the propulsion system to small DC brushed motors, which are cheaper and simpler to integrate than brushless motors since they do not require an ESC.

<img src="img/frames/frame_final_.png" alt="Frame Assembly" width="600px" />


## Electronics

The Raspberry Pi Pico is used as the main controller. The first unit was already available when the project started, and an additional unit was later purchased for the final PCB.

The motor drivers are custom-designed using MOSFETs, diodes, and resistors. A capacitor is used to reduce the effect of current transients from the motors.

The battery was selected primarily based on the available space within the frame and the requirements of the propulsion system.

Radio control is based on ExpressLRS (ELRS), chosen as a low-cost and widely supported RC system. Even thought, the transmitter is still one of the most expensive individual components of the project.

### Estimated Price

| Component | Weight (g) | Price (€) |
|---|---|---|
| Motor 8520 Brushed x4 | 20 | 4.38 |
| Helices (with motor) x4 | 0 | 0.00 |
| Raspberry Pi Pico | 5 | 3.49 |
| IMU (10-axis: L3GD20, LSM303D) | 1 | 7.39 |
| 10k Resistor SMD x4 (x470 pack) | 0 | 1.05 |
| MOSFET SI2300 SMD x4 (x100 pack) | 0.2 | 2.18 |
| Diode 1N4148W SMD x4 (x100 pack) | 0 | 1.26 |
| PCB x2 Driver | TBD | TBD |
| ExpressLRS Transmitter Module | 0 | TBD |
| ExpressLRS Receiver Module | 0 | TBD |
| Battery 1S 30C 400 mAh (x5 + charger) | 11 | 18.99 |
| Battery Connector Molex 51005 | 0 | 1.99 |
| Capacitor 470µF | 0 | 0.00 |
| Capacitor 100nF x4 (x100 pack) | 0 | 1.60 |
| Wiring (miscellaneous) | 1 | 0.00 |
| Frame (LEGO Technic) | 20 | 0.00 |
| PCB Main | TBD | TBD |
| **Total** | | **41.33 €** |

## PCBs

The final electronics are distributed across three PCBs:
- Two identical motor-driver boards, mounted on each pair of arms.
- The main PCB uses LEGO Technic-compatible mounting holes so that it can be integrated into the frame as a structural component.

### PCB Driver Motor

<img src="img/pcb/pcb_driver_circuit.png" alt="Circuit Diagram" width="600px" />

<img src="img/pcb/pcb_driver_layout.png" alt="PCB Layout" width="600px" />

### PCB Raspberry Pi Pico

<img src="img/pcb/pcb_raspberry_circuit.png" alt="Circuit Diagram" width="600px" />

<img src="img/pcb/pcb_raspberry_layout.png" alt="PCB Layout" width="600px" />