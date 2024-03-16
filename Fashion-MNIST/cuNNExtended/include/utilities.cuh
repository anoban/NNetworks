#pragma once
#ifndef __UTILITIES_H__
    #define __UTILITIES_H__

// a header for all the utility functions needed in gradient descent.
// all these are GPU kernel functions intended to run on the device.

// activation kernels

__global__ void kSoftmax() { }

__global__ void kReLU() { }

__global__ void kLeakyReLU() { }

// activation reversal kernels

__global__ void kRevSoftmax() { }

__global__ void kRevReLU() { }

__global__ void kRevLeakyReLU() { }

#endif // !__UTILITIES_H__
