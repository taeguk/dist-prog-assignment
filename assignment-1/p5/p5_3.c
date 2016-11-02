#include "collapse.h"

void process(int *data, int cnt, int *p_local_result)
{
	int i;

	*p_local_result = 0;
	for (i = 0; i < cnt; ++i)
		(*p_local_result) += data[i];

	*p_local_result = ultimate_collapse(*p_local_result);
}

void merge(int *gathered_results, int cnt, int *p_final_result)
{
	int i;

	// sum of local ultimate collapse
	*p_final_result = 0;
	for (i = 0; i < cnt; ++i)
		(*p_final_result) += gathered_results[i];

	*p_final_result = ultimate_collapse(*p_final_result);
}