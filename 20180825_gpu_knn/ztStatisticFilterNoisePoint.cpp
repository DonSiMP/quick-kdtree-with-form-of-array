#include "ztStatisticFilterNoisePoint.h"
#include <string.h>
#include <omp.h>
#include <math.h>
#include "ztKdTree.h"


ZtStatisticFilterNoisePoint::ZtStatisticFilterNoisePoint()
	:mk(25), mt(1.0)
{

}

ZtStatisticFilterNoisePoint::~ZtStatisticFilterNoisePoint()
{

}

int ZtStatisticFilterNoisePoint::setParameter(int mean_k, float std_mul)
{
	mk = mean_k;
	mt = std_mul;

	return 0;
}

int ZtStatisticFilterNoisePoint::applyFilter(int n, float *pts, bool *res)
{
	// ��һ��������kdtree

	zt::ZtKDTree ztkdt;
	ztkdt.setSize(3, n);
	ztkdt.setData(pts);
	ztkdt.buildTree();


	// �ڶ����������ٽ��㣬��������ֵ

	memset(res, 1, n * sizeof(bool));	// ��resȫ����ʼ��Ϊtrue

	int *knn = new int[mk];			// k�ٽ�����
	float *knd = new float[mk];		// k�ٽ�����
	float *distances = new float[n];	// �洢ÿһ�����ƽ������

#pragma omp parallel for num_threads(omp_get_num_procs())
	for (int i = 0; i < n; i++)
	{
		ztkdt.findKNearestsNTP(pts + i * 3, mk, knn, knd);

		float sum = 0;
		for (int j = 0; j < mk; j++)
		{
			sum += knd[j];
		}

		distances[i] = sum / mk;
	}

	double sum = 0, sq_sum = 0;
	for (int i = 0; i < n; ++i)
	{
		sum += distances[i];
		sq_sum += distances[i] * distances[i];
	}

	double mean = sum / n;
	double variance = (sq_sum - sum * sum / n) / (n - 1);
	double stddev = sqrt(variance);

	float distance_threshold = mean + mt * stddev;

	// �����������ݾ�����ֵ���˵���

	for (int i = 0; i < n; i++)
	{
		if (distances[i] > distance_threshold)
		{
			res[i] = false;
		}
	}


	delete[] knn;
	delete[] knd;
	delete[] distances;

	return 0;
}