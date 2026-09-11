# Task Communication Matrix

## Communication overview

| Message | Producer | Consumer | Type | Frequency / Trigger | Data | Purpose |
|---|---|---|---|---|---|---|
| `ImuData` | IMU Task | Control Task | Periodic | TBD | IMU measurements | Provide sensor data to the control loop |
| `ImuStatus` | IMU Task | System Task | Periodic | TBD | IMU health/status | Report IMU availability and faults |
| `RcData` | RC Task | Control Task | Periodic | TBD | RC inputs | Provide pilot commands |
| `RcStatus` | RC Task | System Task | Periodic | TBD | RC health/status | Report RC availability and faults |
| `SystemState` | System Task | Motor Task, LED Task, Log Task | Periodic | TBD | Current system state | Distribute system-level state |
| `MotorCommand` | Control Task | Motor Task | Periodic | TBD | Motor outputs | Command the motors |
| `LogEvent` | Any task / component | Log Task | Event | TBD | Diagnostic event data | Record system events and faults |



### Task Diagram

<img src="img/TaskDiagram.png" alt="Task Diagram" width="400px" />


## Control-loop

The flight control system is organized as a pipeline of independent components:

- AttitudeEstimator: estimates the current attitude from the IMU.
- AttitudeController: converts the desired attitude into desired angular rates.
- RateController: compares desired and measured angular rates and generates the final control output.

All flight control algorithms implement the same interface, `IFlightController`. This allows different control algorithms to be used without modifying ControlTask.
The default implementation is FlightController, but an experimental controller can be provided when creating the task.

`ControlTask` does not contain control algorithm logic. Its responsibilities are:

1. Receive IMU data.
2. Receive new RC data and keep the latest valid command.
3. Run the selected flight controller when a new IMU sample is available.
4. Pass the resulting ControlOutput to the motor mixer.
5. Send the resulting MotorCommands to the motor task.

The controller is optional when constructing the task. If none is provided, the standard FlightController is used.


