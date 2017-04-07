#include "deploy.h"
#include "lib_io.h"
#include "lib_time.h"
#include "stdio.h"
#include <stdlib.h>
int main()
{
	print_time("Begin");
	char *topo[MAX_EDGE_NUM];
	int line_num;

	//char *topo_file = argv[1];
	char *topo_file = ".\\case_example\\The second batch of test cases\\0 primary\\case2.txt";
	line_num = read_file(topo, MAX_EDGE_NUM, topo_file);

	printf("line num is :%d \n", line_num);
	if (line_num == 0)
	{
		printf("Please input valid topo file.\n");
		return -1;
	}

	//char *result_file = argv[2];
	char *result_file = "result.txt";
	deploy_server(topo, line_num, result_file);

	release_buff(topo, line_num);

	print_time("End");
	system("pause");
	return 0;
}

