#include "math.h"
#include "sigmoid.h"

#define Beta    1

double
sigmoid(double x)
{	
	x=-1*Beta*x;
	x=exp(x);
	x+=1.0;
	x=1/x;
	return(x);
}

	
