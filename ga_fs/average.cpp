#include "vector.h"

float
average(vector<float> a)
{
    float aver=0.0;
    for (int i=0;i<a.len();i++)
        aver +=a[i];
    aver =aver/a.len();
    return(aver);
}
