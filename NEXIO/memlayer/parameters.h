/**
 * @file parameters.h
 * @date 10/10/2023
 * @author zz.wang
 * @brief Header for global parameter initialization and management.
 *
 * This header contains declarations and initialization functions for global parameters used across 
 * the NEXIO project. Centralizing parameter management ensures consistent and type-safe 
 * handling of configuration values and runtime parameters.
 *
 * @note Ensure to initialize parameters before using them in the application.
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

// Global variable declarations
extern int global_var1;
extern double global_var2;
extern char global_var3[];

// Initialization functions for global parameters
void initialize_global_parameters();
void set_global_var1(int value);
double compute_and_set_global_var2(double param1, double param2);

int initialize_system(void);

#endif // PARAMETERS_H
