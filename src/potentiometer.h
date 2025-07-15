#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

// Parameters for digital potentiometer
#define POT_STEPS 0x100 // Total number of potentiometer steps

void POT_init();
void POT_set_voltage(uint16_t vmpp);
void POT_set_steps(uint16_t steps);

#endif