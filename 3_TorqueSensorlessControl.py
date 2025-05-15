# System parameters
Jm = 895e-7  # Motor inertia
bm = 0.0001  # Motor damping coefficient
ktrans = 100
n = 9        # Gear ratio

# Control gains (replace with actual values)
kd_L = 0.4
bd_L = 0.04
kd_R = 0.4
bd_R = 0.04
CICGain_L = 0.1
CICGain_R = 0.1

# Input variables (replace with actual sensor data or reference values)
qhr_L = 0.0       # Desired joint angle (left)
qm_L = 0.0        # Motor angle (left)
dqhr_L = 0.0      # Desired joint velocity (left)
ddqhr_L = 0.0     # Desired joint acceleration (left)

qhr_R = 0.0       # Desired joint angle (right)
qm_R = 0.0        # Motor angle (right)
dqhr_R = 0.0      # Desired joint velocity (right)
ddqhr_R = 0.0     # Desired joint acceleration (right)

# Compute contact impedance gains
k_col_imp_L = (1 / n) * (ktrans / (ktrans - kd_L))
k_col_imp_R = (1 / n) * (ktrans / (ktrans - kd_R))

# Compute torque commands
T_command_L = (kd_L * (qhr_L - qm_L / n) + bd_L * dqhr_L) * k_col_imp_L + Jm * ddqhr_L + bm * dqhr_L
T_command_R = (kd_R * (qhr_R - qm_R / n) + bd_R * dqhr_R) * k_col_imp_R + Jm * ddqhr_R + bm * dqhr_R

# Final impedance torques
impedanceTorque_L = CICGain_L * T_command_L
impedanceTorque_R = CICGain_R * T_command_R