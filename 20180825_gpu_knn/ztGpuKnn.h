/*******************************************************************
*
*	ZhengTu(Beijing) Laser Technology Co., Ltd
*	(http://www.ztlidar.com/)
*
*	����:	Sun Zhenxing
*	��������:	20180912
*
*	˵����ʵ�ֻ���gpu��kdtree����������˼·��ͨ��ͬʱ��������n����
*	      ����������Ч�ʡ�ʵ�ʲ��ԣ�Ч�ʷǳ��ͣ���cpu����10��������
*		  �����ڹ���ͬһ��kdtree���Լ����������ж�Ե�ʣ�����gpu����
*		  �ܼ�������
*
******************************************************************/


#ifndef ZTGPUKNN_H
#define ZTGPUKNN_H

#define CUDA_BLOCK	64
#define CUDA_THREAD 32
#define ALLTHREADS	(CUDA_BLOCK * CUDA_THREAD)

/*
**	�������ܣ�Ԥ��Ϊgpu��������ڴ棬����kdtree����copy��gpu��
**	�������壺	n-������ dim-ά�ȣ� nn-�ٽ����������� index-kdtree����
**				treeData-��������
*/
int initCudaForKdtree(int n, int dim, int nn, int *index, float *treeData);

/*
**	�������ܣ�gpu���������ٽ���
**	�������壺	root-������ ndim-ά�ȣ� size-�������� nn-�ٽ���������
**				points-Ҫ�����ĵ㣬 res-�����Ľ��
*/
int gpuSearchKnnKdtree(int root, int ndim, int size, int nn, float *points, int *res);

/*
**	�������ܣ��ͷ�cuda�ڴ�
*/
int gpuFreeCuda();

/*
**	�������ܣ���ȡ����cuda�豸����
*/
int getCudaDeviceCount();

/*
**	�������ܣ���ȡ��i���豸������
**	�������壺i-�豸���У�name-�豸����
*/
int getCudaDeviceNames(int i, char name[]);

/*
**	�������ܣ��趨Ҫʹ�õ��豸
**	�������壺ndevice-�豸������
*/
int setCudaStatus(int ndevice);

#endif