// PIDController.h
#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <vector>
#include <string>

class PIDController {
private:
    double Kp;        // Proportional gain
    double Ki;        // Integral gain
    double Kd;        // Derivative gain

    double setpoint;  // Desired value
    double error_sum; // Accumulated error for integral term
    double last_error;// Previous error for derivative term
    double output;    // Controller output
    double output_min;// Output minimum limit
    double output_max;// Output maximum limit

public:
    PIDController(double kp, double ki, double kd, double target);

    void setGains(double kp, double ki, double kd);
    void setSetpoint(double target);
    void setOutputLimits(double min, double max);
    void reset();
    double compute(double current_value, double dt);
};

class DCMotorModel {
private:
    // Electrical parameters
    double resistance;    // Armature resistance (ohms)
    double inductance;    // Armature inductance (henries)
    double kTorque;       // Torque constant (N·m/A)
    double kEMF;          // Back-EMF constant (V·s/rad)

    // Mechanical parameters
    double inertia;       // Rotor inertia (kg·m²)
    double friction;      // Viscous friction coefficient (N·m·s/rad)
    double loadTorque;    // External load torque (N·m)

    // State variables
    double current;       // Armature current (A)
    double voltage;       // Input voltage (V)
    double speed;         // Angular velocity (rad/s)
    double position;      // Angular position (rad)

public:
    DCMotorModel(double r = 1.0, double l = 0.5, double kt = 0.01, double ke = 0.01,
        double j = 0.01, double f = 0.001, double lt = 0.0);

    void setVoltage(double v);
    void setLoadTorque(double lt);
    void update(double dt);

    double getCurrent() const;
    double getSpeed() const;
    double getPosition() const;

    void reset(double initial_position = 0.0, double initial_speed = 0.0, double initial_current = 0.0);

    // Method to set all motor parameters at once
    void setMotorParameters(double r, double l, double kt, double ke, double j, double f);
};

// Function to run simulation and generate data for graphing
std::vector<std::vector<double>> runMotorSimulation(double Kp, double Ki, double Kd,
    double setpoint, double simulation_time,
    double dt, double r, double l, double kt,
    double ke, double j, double f,
    bool position_control = true,
    bool add_disturbance = true);

// Function to save data to CSV for easy graphing in external tools
void saveToCSV(const std::vector<std::vector<double>>& data, const std::string& filename);

#endif // PID_CONTROLLER_H