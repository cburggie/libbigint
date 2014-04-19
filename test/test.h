#ifndef TEST_H
#define TEST_H

#define FATAL(x,y) printf("%s failed critical test of %s.\n", x, y ); return 1

int doTest(int test, char * methodName)
{
	if (test)
	{
		printf("failure during test of %s\n", methodName);
	}
	
	else
	{
		printf("%s passed tests\n", methodName);
	}
	
	return test;
}

#endif
