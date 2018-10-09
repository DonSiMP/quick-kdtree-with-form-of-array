#include "ztStatisticFilterNoisePoint.h"
#include <string.h>
#include <omp.h>
#include <math.h>
#include <stdio.h>
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
	if (!pts || !res || n < 1)
	{
		printf("Can't process null pointer!\n");
		return 1;
	}

	// ��һ��������kdtree

	zt::ZtKDTree ztkdt;

	ztkdt.setSize(3, n);
	ztkdt.setData(pts);
	ztkdt.buildTree();


	// �ڶ����������ٽ��㣬��������ֵ

	memset(res, 1, n * sizeof(bool));	// ��resȫ����ʼ��Ϊtrue

	float *distances = new float[n];	// �洢ÿһ�����ƽ������

#pragma omp parallel for num_threads(omp_get_num_procs())
	for (int i = 0; i < n; i++)
	{
		int *knn = new int[mk];			// k�ٽ�����
		float *knd = new float[mk];		// k�ٽ�����

		ztkdt.findKNearestsNTP(pts + i * 3, mk, knn, knd);

		float sum = 0;
		for (int j = 0; j < mk; j++)
		{
			sum += knd[j];
		}

		distances[i] = sum / mk;

		delete[] knn;
		delete[] knd;
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

	delete[] distances;

	return 0;
}

int ZtStatisticFilterNoisePoint::applyFilter_2(int n, float *pts, bool *res)
{
	// һ����Ԥ������е��Ʒֿ�

	// 1) ���ҵ��ư�Χ��
	double lx = 9999999, ly = 9999999, rx = -9999999, ry = -9999999;

	for (int i = 0; i < n; i++)
	{
		lx = pts[i * 3 + 0] < lx ? pts[i * 3 + 0] : lx;
		ly = pts[i * 3 + 1] < ly ? pts[i * 3 + 1] : ly;

		rx = pts[i * 3 + 0] > rx ? pts[i * 3 + 0] : rx;
		ry = pts[i * 3 + 1] > ry ? pts[i * 3 + 1] : ry;
	}

	// 2) ���ݰ�Χ�еĴ�С���зֿ�
	double ft = (rx - lx) / (ry - ly);

	// ˮƽ����ֱ�ֿ�
	int nhb = 0, nvb = 0;

	// Ĭ������8�л�8��
	nhb = int(8 * ft / (ft + 1) + 0.5);
	nvb = int(8 * 1  / (ft + 1) + 0.5);

	// ��ֹ�ٽ��Խ�� + 1
	double hstep = (rx - lx + 1) / nhb;
	double vstep = (ry - ly + 1) / nvb;

// 	printf("%.3lf\t%.3lf\n%d\t%d\n%.3lf\t%.3lf\n",
// 		rx - lx, ry - ly, nhb, nvb, hstep, vstep);

	// 3) �Ե��ƽ��зֿ鴦��
	
	// ����ͳ��ÿһ��ĵ���
	int *count = new int[nhb * nvb];
	for (int i = 0; i < nhb * nvb; i++)
	{
		count[i] = 0;
	}

	int c = 0, r = 0;
	for (int i = 0; i < n; i++)
	{
		c = int((pts[i * 3 + 0] - lx) / hstep) + int((pts[i * 3 + 1] - ly) / vstep) * nhb;
		count[c]++;
	}

	// ���������ڴ�
	int **indices = new int *[nhb * nvb];
	for (int i = 0; i < nhb * nvb; i++)
	{
		indices[i] = new int[count[i]];
	}

	// ���е��Ʒֿ�
	for (int i = 0; i < nhb * nvb; i++)
	{
		count[i] = 0;
	}

	for (int i = 0; i < n; i++)
	{
		c = int((pts[i * 3 + 0] - lx) / hstep) + int((pts[i * 3 + 1] - ly) / vstep) * nhb;
		indices[c][count[c]++] = i;
	}

	// ������鴦�����е����˲�
	for (int i = 0; i < nhb * nvb; i++)
	{
		if (count[i] < 1)
		{
			continue;
		}

		float *points = new float[count[i] * 3];
		for (int j = 0; j < count[i]; j++)
		{
			points[j * 3 + 0] = pts[indices[i][j] * 3 + 0];
			points[j * 3 + 1] = pts[indices[i][j] * 3 + 1];
			points[j * 3 + 2] = pts[indices[i][j] * 3 + 2];
		}

		bool *result = new bool[count[i]];

		if (applyFilter(count[i], points, result))
		{
			continue;
		}

		for (int j = 0; j < count[i]; j++)
		{
			res[indices[i][j]] = result[j];
		}

		delete[] points;
		delete[] result;
	}

	delete[] count;
	for (int i = nhb * nvb - 1; i >= 0; i--)
	{
		delete[] indices[i];
	}
	delete[] indices;

	return 0;
}