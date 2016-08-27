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

	// scanning values
	printf("Give number of steps\n");
	scanf("%lf",&n);
	printf("Give number of iterations\n");
	scanf("%lf",&ite);

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
	printf("Experimental value: %lf\n",expvalue);
	double a;
	for(a=1,i=1;i<n+1;i++)
		a=(a*(n+i)*1.0)/(4*i);
	printf("Theoritical value : %lf\n",a);
	return 0;
}
