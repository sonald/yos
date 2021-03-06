#include <io.h>
#include <stdio.h>

void test1()
{
	char buf[] = "hello, yos";
	char buf2[] = "YOS";
	
	memset( buf, '#', 4 );
	printf("buf: %s\n", buf);
	memset( buf+6, '@', 3 );
	printf("buf: %s\n", buf);
	memcpy(buf+3, buf2, sizeof(buf2)-1);
	printf("buf: %s\n", buf);
	
	int i = 1420423;
	char *cbuf = early_itos( i, 10 );
	printf("%d: %s\n", i, cbuf);

	char abuf[BUF_SIZE] = "this is a buf for testing";
	early_strncpy(abuf, buf, 8);
	printf("abuf: %s\n", abuf);

	early_strncpy(abuf+5, buf, 7);
	printf("abuf: %s\n", abuf);
	
	int prted = early_kprint(0, "E: %d, %c, %%, %s", 5, 'C', "hello, kprint");
	printf("kprint: %s(%d)\n", __early_print_buf, prted);

	prted = early_kprint(0, "early_kprint: %s, %d, %%, %s, %c, %s, %d, 0x%x, %d",
						 "further demo", 1203, "for yos only", 'C', "fd",
						 233, 0x12ac3d4f, 0x1234);
	
	printf("%s(%d)\n", __early_print_buf, prted);

	uint32 eax, ebx, ecx, edx, edi, esi;

	esi = 0, edi = -3;
	prted = early_kprint( PL_ERROR, "ESI = %x EDI= %x \n", esi, edi );
	printf("%s(%d)\n", __early_print_buf, prted);

	edx = 0;
	prted = early_kprint( PL_ERROR, "EAX = %x EBX = %x ECX = %x EDX = %x \n", eax, ebx, ecx, edx );
	printf("%s(%d)\n", __early_print_buf, prted);
}

void test3()
{
	int i = 0xe023f49c;
	char *cbuf = early_itos( i, 10 );
	printf("base 10: %d: %s\n", i, cbuf);

	cbuf = early_itos( i, 16 );
	printf("base 16: %d: %s\n", i, cbuf);
	
	int prted = early_kprint(0, "test early_itos: i = 0x%x\n", i);
	printf("kprint: %s(%d)\n", __early_print_buf, prted);
}

void test2()
{
	if ( strcmp("123", "123") != 0 )
		printf( "strcmp 1 error\n" );

	if ( strcmp("123", "1234") >= 0 )
		printf( "strcmp 2 error\n" );

	if ( strcmp("1234", "123") <= 0 )
		printf( "strcmp 3 error\n" );
	
	if ( strcmp("abeffe", "abfeefb") >= 0 )
		printf( "strcmp 4 error\n" );
	
}

int main(int argc, char* argv[])
{
	test3();
	return 0;
}
