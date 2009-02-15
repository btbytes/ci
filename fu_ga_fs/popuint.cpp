#include "popuint.h"

int search(int);	                /* search the second Kid index */
void reorder(int *,int, int);       /* reorder the remain individual indexs */

//constructors
PopulationInt::PopulationInt(int a,int b):
length(a),width(b),c_rate(0),m_rate(0)
{
    ptr=new IndividualInt[length];
    assert(ptr !=0);
}

PopulationInt::PopulationInt(int a,int b,float c=0,float d=0):
length(a),width(b),c_rate(c),m_rate(d)
{
    ptr=new IndividualInt[length];
    assert(ptr !=0);
}

PopulationInt::PopulationInt(const PopulationInt& a):
length(a.length),width(a.width),c_rate(a.c_rate),m_rate(a.m_rate)
{
	 delete []ptr;
	 ptr= new IndividualInt[length];
    for (int i=0;i<length;i++)
        ptr[i]=a.ptr[i];
}

PopulationInt&
PopulationInt::change_length(const int& a)
{
	 assert(a>=0);
	 length = a;
    delete []ptr;
	 ptr= new IndividualInt[length];
	 assert(ptr !=0);
    for (int i=0;i<length;i++)
		  ptr[i].change_mrate(m_rate);
	 return *this;
}

PopulationInt&
PopulationInt::change_mrate(const float& a)
{
	 assert(a>=0&&a<=1);
	 m_rate=a;
	 for (int i=0;i<length;i++)
		  ptr[i].change_mrate(a);
	 return *this;
}

PopulationInt&
PopulationInt::initialize_range(const IndividualInt& a)
{
    time_t  t;
    srand((unsigned) time(&t));
    if (((width-1)%(a.get_length()-1))!=0)
    {
        cerr<<"dimendions doesn't match in initialize_range of popuint.cpp"<<endl;
        exit(1);
    }
    IndividualInt tmp(width,m_rate);
    for (int i=0;i<length;i++)
        ptr[i]=tmp.initialize_range(a);
    return *this;
}

PopulationInt&
PopulationInt::initialize_range_RM(const IndividualInt& a)
{
    time_t  t;
    srand((unsigned) time(&t));
	
    IndividualInt tmp(width,m_rate);
    for (int i=0;i<length;i++)
        ptr[i]=tmp.initialize_range_RM(a);
    return *this;
}

PopulationInt&
PopulationInt::initialize_range_RMT(const IndividualInt& a)
{
    time_t  t;
    srand((unsigned) time(&t));

    IndividualInt tmp(width,m_rate);
    for (int i=0;i<length;i++)
        ptr[i]=tmp.initialize_range_RMT(a);
    return *this;
}

PopulationInt&
PopulationInt::mutate_one(const IndividualInt& a,const int& b)
{
    time_t  t;
    srand((unsigned) time(&t));

    for (int i=0;i<length;i++)
    {
        if (i !=b)
	 ptr[i].mutate_one(a);
    }
    return *this;
}

PopulationInt&
PopulationInt::mutate_one_RM(const IndividualInt& a,const int& b)
{
    time_t  t;
    srand((unsigned) time(&t));

    for (int i=0;i<length;i++)
    {
        if (i !=b)
	 ptr[i].mutate_one_RM(a);
    }
    return *this;
}

PopulationInt&
PopulationInt::mutate_one_RMT(const IndividualInt& a,const int& b)
{
    time_t  t;
    srand((unsigned) time(&t));

    for (int i=0;i<length;i++)
    {
        if (i !=b)
	 ptr[i].mutate_one_RMT(a);
    }
    return *this;
}

PopulationInt&
PopulationInt::crossover(const int& a,const int& b)
{
    time_t  t;
    srand((unsigned) time(&t));

    int *sg;     /* sg[popsize];	 to store the flag of beibg chosen or not */
    sg=new int[length];
	/* initialization */
		/* at the begining, all have not been selected */
		/* sg[i] store i, and rno=popsize */
    for (int i=0;i<length;i++)
        sg[i]=i;
    int rno=length;  //rno: no. of remaining unchosen individuals
    int k1,k2; //k1: first chosen kid  k2: second chosen kid

    /* begin crossover among population */
    for (i=0;i<(length/2+1);i++)
    {
        if (rno>=2) 	/* at least two indivuals remain unchosen */
        {
	 //chosen two individuals to be crossovered
	 int j=search(rno);	/* find first kid */
	 k1=sg[j];
	 rno=rno-1;
	 reorder(sg,rno,j);	/* reorder the sign vector */
	 j=search(rno); 		/* find second kid */
	 k2=sg[j];
	 rno=rno-1;
	 reorder(sg,rno,j);	/* reorder the sign vector */
            float prob=(1.0*random(1000))/1000;
            if (prob<=c_rate) /* probability for crossover */
	 {
                if ((k1 !=b)&&(k2 !=b))
	     {
		switch(a)
		{
		    case 0:
		        crossoverUniform(ptr[k1],ptr[k2]);
		        break;
		    case 1:
		        crossoverOP(ptr[k1],ptr[k2]);
		        break;
		    default:
		        crossoverTP(ptr[k1],ptr[k2]);
		}
	     }
	 }
        }
    }
    delete[]sg;
    return *this;
}

PopulationInt&
PopulationInt::selection(const vector<float>& a,const int& b,const int& c)
		//a: fitness vector  b: best indi index  c: shift flag (1: yes, 0: no)
{
    time_t  t;
    srand((unsigned) time(&t));

    int i,j,k;
    float *Prob;	//probability vector of indi to be selected
    Prob=new float[length];

    int* flag;	//information for selected times
    flag=new int[length];

    int no;
    float mini,remain;

    /* shift the fitness */
    	/* find the minimial fitness */
    if (c==1)
    {
        mini=a.minimum();
        if (mini<0.1)
	 remain=mini;
        else
	 remain=0.1;

        for (i=0;i<length;i++)
	 a[i]=a[i]-mini+remain;
    }
    /* calculation the sum of the fitness */
    float Sum=0.0;
    for (i=0;i<length;i++)
        Sum+=a[i];
    if (Sum==0)
        return *this;

    /* calculate the probability vector of individuals */
    float SumProb=0.0;
    for (i=0;i<length;i++)
    {
        Prob[i]=a[i]/Sum+SumProb;
        SumProb+=a[i]/Sum;
    }

    /* generate new population */

    /* set all flags to be zero, means no one has been selected */
    for (i=0;i<length;i++)
        flag[i]=0;

    flag[b]=1;  /* keep the best */
    /* set the flags for all individuals */
    for (i=0;i<(length-1);i++)
    {
        /* turn wheel to see which individual is selected */
        float prob=(1.0*random(1000))/1000;
        k=0;
        for (j=0;j<length;j++)
        {
	 if (prob>=Prob[j])
	     k=j+1;
        }
        flag[k]+=1;
    }
    /* form the new population */
    for (i=0;i<length;i++)
    {
         if (flag[i]==0)
         {
	  no=0;
	  for (j=0;j<length;j++)
	  {
	      if (flag[j]>1)
	      {
		 k=j;
		 no=no+1;
		 break;
	      }
	  }
	  if (no==0)
	  {
	      printf("something wrong in selection \n");
	      exit(1);
	  }
	  flag[k]=flag[k]-1;
	  //copy the selected individual to new individual
	  ptr[i]=ptr[k];

	  //get the flag for the new individual
	  flag[i] +=1;
        }
    }

    /* check the selection */
    for (i=0;i<length;i++)
    {
        if (flag[i]!=1)
        {
	 printf("something wrong with selection \n");
	 exit(1);
        }
    }
    delete []Prob;
    delete []flag;
    return *this;
}

vector<float>
PopulationInt::fitness(const FuzzyRule& a,const array& b,const vector<int>& cn,const int& c,const int& d, const int& e) const
{
    vector<float> tmpvec(length);
    for (int i=0;i<length;i++)
        tmpvec[i]=ptr[i].fitness(a,b,cn,c,d,e);
    return tmpvec;
}

vector<float>
PopulationInt::fitness_RM(const FuzzyRule& a,const array& b,const vector<int>& cn,const int& c,const int& d, const int& e,const IndividualInt& f) const
{
    vector<float> tmpvec(length);
    for (int i=0;i<length;i++)
        tmpvec[i]=ptr[i].fitness_RM(a,b,cn,c,d,e,f);
    return tmpvec;
}

vector<float>
PopulationInt::fitness_RMT(const FuzzyRule& a,const array& b,const vector<int>& cn,const int& c,const int& d, const int& e,const IndividualInt& f) const
{
    vector<float> tmpvec(length);
    for (int i=0;i<length;i++)
        tmpvec[i]=ptr[i].fitness_RMT(a,b,cn,c,d,e,f);
    return tmpvec;
}

//I/O operators
ostream& operator<<(ostream& os,const PopulationInt& a)
{
    os<<a.length<<"\t"<<a.width<<"\t"<<a.c_rate<<"\t"<<a.m_rate<<endl;
    for (int i=0;i<a.length;i++)
        os<<a.ptr[i];
    return os;
}

istream& operator>>(istream& is,PopulationInt& a)
{
    is>>a.length>>a.width>>a.c_rate>>a.m_rate;
    if (a.ptr !=0)
        delete []a.ptr;

    a.ptr =new IndividualInt[a.length];
    assert(a.ptr !=0);
    IndividualInt tmp(a.width,a.m_rate);
    for (int i=0;i<a.length;i++)
    {
        a.ptr[i]=tmp;
        is>>a.ptr[i];
    }
    return is;
}

int
search(int si)	/* number of unchosen individuals */
{
    int re;
    re=random(si);
    return(re);
}

void
reorder(int *vec,int si,int ind)
/* int	*vec;		 pointer to the sign vector */
/* int 	si;		 number of unchosen individuals */
/* int	ind;		 selected and need to be removed */
{
    int i;
    if (ind<si)
    {
        for (i=ind;i<si;i++)
	 *(vec+i)=*(vec+i+1);
    }
}

