#ifndef ZTSTATISTICFILTERNOISEPOINT_H
#define ZTSTATISTICFILTERNOISEPOINT_H


class ZtStatisticFilterNoisePoint
{
public:
	ZtStatisticFilterNoisePoint();
	~ZtStatisticFilterNoisePoint();

	int setParameter(int mean_k, float std_mul);
	int applyFilter(int n, float *pts, bool *res);

private:
	int mk;		// �ٽ�����
	float mt;	// sigma����
};

#endif