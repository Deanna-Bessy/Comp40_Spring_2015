/*
 * calcfunctions.h
 * Deanna Bessy and Josh Berl 2-26-15
 * HW4 - arith
 * 
 * Implementation of functions defined in calcfunctions.h
 */


#include "calcfunctions.h"



/*Takes a float value and rounds it down into a int */
extern int round_float(float value)
{
        if (value < 0.5 ) {
                return 0; 
        }
        int rounding_value = (int)(value);
        if (rounding_value == 0) return 1; 
        value += 0.5;
        int update_value = (int)(value);
        int round_amount = (update_value % rounding_value);
        rounding_value += round_amount; 
        return rounding_value;
}

/*Scales from floats between -0.3 and 0.3 to ints between -15 and
 * 15. Does part of the arithmatic to average four pixels (divides by 4).
 */
extern int bcd_calc(float value)
{
        float min = -0.3;
        float max = 0.3;
        float avg = value / 4;
        if (avg < min) {
                avg = min;
        } else if (avg > max) {
                avg = max;
        }
        avg *= 50.0;
        return round_float(avg);
}

/* Scales from an int to a float between -15 and 15. */
extern float bcd_scale(int value)
{
        float nvalue = (float)value / 50.0; 

        if (nvalue < -15) 
                nvalue = -15; 
        else if (nvalue > 15)
                nvalue = 15; 
        return nvalue; 
}