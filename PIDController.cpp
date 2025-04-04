// Ali Haghour 
// 04-04-2025
// PID controller  implementation file
// PIDController.cpp
#include "PIDController.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm> // For std::clamp

PIDController::PIDController(double kp, double ki, double kd, double target)
    : Kp(kp), Ki(ki), Kd(kd), setpoint(target), error_sum(0.0), last_error(0.0), output(0.0),
    output_min(-12.0), output_max(12.0) {
}

void PIDController::setGains(double kp, double ki, double kd) {
    Kp = kp;
    Ki = ki;
    Kd = kd;
}

void PIDController::setSetpoint(double target) {
    setpoint = target;
}

void PIDController::setOutputLimits(double min, double max) {
    output_min = min;
    output_max = max;
}

void PIDController::reset() {
    error_sum = 0.0;
    last_error = 0.0;
    output = 0.0;
}

double PIDController::compute(double current_value, double dt) {
    // Calculate error
    double error = setpoint - current_value;

    // Proportional term
    double P_term = Kp * error;

    // Integral term
    error_sum += error * dt;
    double I_term = Ki * error_sum;

    // Derivative term (on measurement, not error)
    double derivative = (error - last_error) / dt;
    double D_term = Kd * derivative;

    // Calculate total output
    output = P_term + I_term + D_term;

    // Apply output limits (anti-windup)
    if (output > output_max) {
        output = output_max;
        // Anti-windup: Prevent integral term from growing when saturated
        error_sum -= error * dt;
    }
    else if (output < output_min) {
        output = output_min;
        // Anti-windup: Prevent integral term from growing when saturated
        error_sum -= error * dt;
    }

    // Store error for next iteration
    last_error = error;

    return output;
}

DCMotorModel::DCMotorModel(double r, double l, double kt, double ke, double j, double f, double lt)
    : resistance(r), inductance(l), kTorque(kt), kEMF(ke), inertia(j), friction(f),
    loadTorque(lt), current(0.0), voltage(0.0), speed(0.0), position(0.0) {
}

void DCMotorModel::setVoltage(double v) {
    voltage = v;
}

void DCMotorModel::setLoadTorque(double lt) {
    loadTorque = lt;
}

void DCMotorModel::update(double dt) {
    // Calculate back-EMF
    double backEMF = kEMF * speed;

    // Update current based on di/dt = (V - Ri - kEMF*ω) / L
    double di_dt = (voltage - resistance * current - backEMF) / inductance;
    current += di_dt * dt;

    // Calculate motor torque
    double motorTorque = kTorque * current;

    // Calculate net torque
    double netTorque = motorTorque - loadTorque - friction * speed;

    // Update angular acceleration based on torque
    double angular_acceleration = netTorque / inertia;

    // Update speed
    speed += angular_acceleration * dt;

    // Update position
    position += speed * dt;
}

double DCMotorModel::getCurrent() const {
    return current;
}

double DCMotorModel::getSpeed() const {
    return speed;
}

double DCMotorModel::getPosition() const {
    return position;
}

void DCMotorModel::reset(double initial_position, double initial_speed, double initial_current) {
    position = initial_position;
    speed = initial_speed;
    current = initial_current;
    voltage = 0.0;
}

void DCMotorModel::setMotorParameters(double r, double l, double kt, double ke, double j, double f) {
    resistance = r;
    inductance = l;
    kTorque = kt;
    kEMF = ke;
    inertia = j;
    friction = f;
}

std::vector<std::vector<double>> runMotorSimulation(double Kp, double Ki, double Kd,
    double setpoint, double simulation_time,
    double dt, double r, double l, double kt,
    double ke, double j, double f,
    bool position_control,
    bool add_disturbance) {
    // Initialize controller and DC motor
    PIDController pid(Kp, Ki, Kd, setpoint);
    DCMotorModel motor(r, l, kt, ke, j, f);

    // Set appropriate output limits (voltage limits)
    pid.setOutputLimits(-12.0, 12.0);  // Typical supply voltage for DC motors

    // Data for graphing:
    // [time, setpoint, actual_value(position or speed), voltage, current, speed, position, p_term, i_term, d_term]
    std::vector<std::vector<double>> data;

    // Run simulation
    double time = 0.0;
    double p_term = 0.0;
    double i_term = 0.0;
    double d_term = 0.0;

    while (time <= simulation_time) {
        // Get current motor state
        double actual_value = position_control ? motor.getPosition() : motor.getSpeed();

        // Calculate control output
        double error = setpoint - actual_value;
        p_term = Kp * error;

        // Compute PID output (voltage)
        double control_voltage = pid.compute(actual_value, dt);

        // Approximate the I and D terms based on total output and P term
        double last_error = (position_control ? motor.getPosition() : motor.getSpeed()) - setpoint;
        d_term = Kd * (error - last_error) / dt;
        i_term = control_voltage - p_term - d_term;

        // Add disturbance at 50% of simulation time (additional load torque)
        if (add_disturbance && time >= simulation_time * 0.5 && time <= simulation_time * 0.6) {
            motor.setLoadTorque(0.05);  // Small load torque disturbance
        }
        else {
            motor.setLoadTorque(0.0);
        }

        // Apply control voltage to motor
        motor.setVoltage(control_voltage);

        // Update motor state
        motor.update(dt);

        // Store data
        data.push_back({
            time,
            setpoint,
            actual_value,
            control_voltage,
            motor.getCurrent(),
            motor.getSpeed(),
            motor.getPosition(),
            p_term,
            i_term,
            d_term
            });

        // Increment time
        time += dt;
    }

    return data;
}

void saveToCSV(const std::vector<std::vector<double>>& data, const std::string& filename) {
    std::ofstream file(filename);

    // Write header
    file << "Time,Setpoint,ActualValue,Voltage,Current,Speed,Position,P_Term,I_Term,D_Term\n";

    // Write data
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << std::fixed << std::setprecision(6) << row[i];
            if (i < row.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file.close();
}