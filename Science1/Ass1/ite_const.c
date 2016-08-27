#include<stdlib.h>
#include<math.h>
#include<stdio.h>
int main()
{
	int w=0,x=0,y=0,z=0,j;
	double count=0,ite,i,n;
	// for random values
	time_t t;
	srand((unsigned) time(&t));

	n=5;


	double a;
	for(a=1,i=1;i<n+1;i++)
		a=(a*(n+i)*1.0)/(4*i);
	
	for(ite=1;ite<=5000;ite++)
	{
		count=0;
		for(j=0;j<ite;j++)
		{
			w=0;
			z=0;
			for(i=0;i<n;i++)
			{
				x=rand();
				if(x%2 == 0)
					w++;
				else
					w--;
				y=rand();
				if(y%2 == 0)
					z++;
				else
					z--;
			}
			if(w==z)
				count++;
		}
		double expvalue = count/ite;
		//printf("%lf %lf\n",expvalue,ite);
		printf("%lf %lf\n",a,ite);
	}
	return 0;
}
