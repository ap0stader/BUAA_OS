#include <stdio.h>
int main() {
	int n;
	scanf("%d", &n);
	
	int loop_n = n;
	int reverse_n = 0;
	
	while (loop_n != 0) {
        	reverse_n *= 10;
        	reverse_n += loop_n % 10;
        	loop_n /= 10;
    	}

	if (reverse_n == n) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
