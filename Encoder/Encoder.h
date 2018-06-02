#ifndef ENCODER_H
#define ENCODER_H
#define max_encoders 2
typedef struct Encoder
{
    int pin_a;
    int pin_b;
    volatile long long value;
    volatile int lastEncoded;
}Encoder;

//Pre-allocate encoder objects on the stack so we don't have to 
//worry about freeing them
extern Encoder encoders[max_encoders];

/*
  Should be run for every rotary encoder you want to control
  Returns a pointer to the new rotary encoder structer
  The pointer will be NULL is the function failed for any reason
*/
Encoder *setupencoder(int pin_a, int pin_b); 
#endif
