/*************************************************************************************************
                                   °ÍÌØÎÖË¹ÂË²¨Æ÷  v1.0
*************************************************************************************************/
#define BUTTERWORTH_FILTER_C
#include "Butterworth_filter.h"
#include <stdio.h>
#include <math.h>

float un=0;
float yn=0;
// float dly[4][3][2]={{{0,0},{0,0},{0,0}},{{0,0},{0,0},{0,0}},{{0,0},{0,0},{0,0}},{{0,0},{0,0},{0,0}}};

float dlx[4][2]={{0,0},{0,0},{0,0},{0,0}};
float dly[4][2]={{0,0},{0,0},{0,0},{0,0}};
	
const float data_a[3]={1.000000000000000,-1.142980502539901,0.412801598096189};          

const float data_b[3]={0.067455273889072,0.134910547778144,0.067455273889072};
            

float Butterworth_filter(float input,u8 num)
{
// 	u8 ii;
// 	for(ii=0;ii<3;ii++)
// 	{
// 		un = input - data_b[ii][0] * dly[num][ii][0] - data_b[ii][1] * dly[num][ii][1];
// 		yn = data_a[ii][0] * un + data_a[ii][1] * dly[num][ii][0] + data_a[ii][2] * dly[num][ii][1];
// 		dly[num][ii][1] = dly[num][ii][0];
// 		dly[num][ii][0] = un;
// 		input = yn;
// 	}

	yn = data_b[0] * input + data_b[1] * dlx[num][0] + data_b[2] * dlx[num][1]
			 - data_a[1] * dly[num][0] - data_a[2] * dly[num][1];
	dlx[num][1] = dlx[num][0];
	dlx[num][0] = input;
	dly[num][1] = dly[num][0];
	dly[num][0] = yn;
	input = yn;
	return input;
}
