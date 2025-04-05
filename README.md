# PID Controller Simulation

This project provides a C++ implementation of a PID (Proportional-Integral-Derivative) controller.
The simulation outputs data to CSV files for visualization and analysis (I prefer to use GNU plot).

## Dependencies

GNUPlot - Required for graphing (WILL NOT WORK WITHOUT)

http://www.gnuplot.info/download.html

## Instructions

### 1. Open the .exe

### 2. Enter the PID values and then click on Run Simulation

### 3. Click 'OK' on both dialog boxes

### 4. Graph should display (see examples below).

### 5. To restart simulation. Close the graph and the CMD terminal.  Then repeat from step 2.


## Some examples

### All coefficients Kp, Ki, Kd set to 0

![All coefficients 0](https://github.com/alihaghour/Plotting/blob/master/WithoutPIDResponse.PNG?raw=true)

(No correction).

<br/><br/> 

### Kp coefficient set to 8.0

![Kp set to 8.0](https://github.com/alihaghour/Plotting/blob/master/PIDKp8.PNG?raw=true)

(You can see the oscillations starting to show. As Kp increases they tend to increase).

<br/><br/> 

### Kp coefficient set to 50.0

![Kp set to 50.0](https://github.com/alihaghour/Plotting/blob/master/PIDkp50.PNG?raw=true)

(Even more violent and frequent oscillations).

<br/><br/> 

### Kp coefficient set to 50.0; Ki coefficient set to 20.0; Kd coefficient set to 2;

![Kp set to 50.0](https://github.com/alihaghour/Plotting/blob/master/PIDkp50Ki20Kd2.PNG?raw=true)

(Almost instant correction with no oscillations).

<br/><br/> 

## NOTE
After each run, the simulation results are saved as a .csv in the directory where main.cpp is located. Please remember to remove the files since I did not implement a method to remove the data after each simulation run.
