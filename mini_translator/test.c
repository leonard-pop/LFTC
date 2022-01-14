#include <stdio.h>

int main() {
	int a;
	int b;

	scanf("%d", &a);
	scanf("%d", &b);

	a = a * 2 + 4 / b;

	printf("%d\n", a);
}
