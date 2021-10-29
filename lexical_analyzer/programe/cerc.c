#include <stdio.h>

int main(){
	float pi = 3.14159265358979323846, r;

	scanf("%f", &r);

	printf("Perimetru: %f\nArie: %f\n", 2 * pi * r, pi * r * r);

	return 0;
}
