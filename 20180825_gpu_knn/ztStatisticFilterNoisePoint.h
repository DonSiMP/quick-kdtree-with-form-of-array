/*******************************************************************
*
*	ZhengTu(Beijing) Laser Technology Co., Ltd
*	(http://www.ztlidar.com/)
*
*	����:	Sun Zhenxing
*	��������:	20180923
*
*	˵����ʵ�ֵ��Ƶ�ͳ���˲�ȥ�빦�ܣ��������˲��ӿڣ���һ����ֱ�ӽ�
*	      ���Ƽӽ�ȥ�����ڶ����ǽ����ڲ��ֿ���ٽ����˲�����Ŀ
*		  ���Ƿ�ֹ���Ʒֲ���Ϊ������ʱ�������˲�����
*
******************************************************************/

#ifndef ZTSTATISTICFILTERNOISEPOINT_H
#define ZTSTATISTICFILTERNOISEPOINT_H


class ZtStatisticFilterNoisePoint
{
public:
	ZtStatisticFilterNoisePoint();
	~ZtStatisticFilterNoisePoint();

	int setParameter(int mean_k, float std_mul);
	int applyFilter(int n, float *pts, bool *res);

	// Ԥ����ֿ�����˲�
	int applyFilter_2(int n, float *pts, bool *res);

private:
	int mk;		// �ٽ�����
	float mt;	// sigma����
};

#endif