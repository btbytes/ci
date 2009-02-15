#include <math.h>

#include "vector.h"

float
variance(vector<float> a, float b)
{
    float sum=0.0;
    for (int i=0;i<a.len();i++)
    {
        float tem=a[i]-b;
        tem=tem*tem;
        sum+=tem;
    }
    sum=sum/a.len();
    sum=sqrt(sum);
    return(sum);
}


