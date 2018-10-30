/*******************************************************************
 *	
 *	ZhengTu(Beijing) Laser Technology Co., Ltd
 *	(http://www.ztlidar.com/)
 *
 *	����:	Sun Zhenxing
 *	��������:	20180825
 *
 *	˵������Ҫʵ���˶�άkdtree�Ľ��������ڽ����������������룩��
 *	      ʹ��������ʽ�洢������û�б�дɾ��������Ĺ��ܣ�����
 *		  Ч������pcl
 *
 ******************************************************************/

#ifndef ZTKDTREE_H
#define ZTKDTREE_H

#include <vector>

namespace zt
{
#define SAMPLE_MEAN 1024

	class ZtKDTree
	{
	public:
		ZtKDTree();
		ZtKDTree(int dimension, unsigned int sz);
		~ZtKDTree();

		int setSize(int dimension, unsigned int sz);
		int setOffset(double oft[]);
		int setData(float *indata);	 // �����Ѿ���ȥƫ����
		int setData(double *indata); // ����δ��ȥƫ����
		int buildTree();

		// ���������
		int findNearest(float *p);
		int findNearest(double *p);

		// ���������k����
		// ʹ��stl��ջ
		int findKNearestsSTL(float *p, int k, int *res);
		// ʹ���Ա�д��ջ none third party, ��stl��5������
		int findKNearestsNTP(float *p, int k, int *res, float *dit);
		int findKNearests(double *p, int k, int *res);

		// ���Ҹ�����Χ�ڵĵ�
		int findNearestRange(float *p, float range, std::vector<int> &res);
		int findNearestRange(double *p, float range, std::vector<int> &res);

		// gpu�豸�Ƿ����
		bool isGpuEnable;

		//// gpu���������k����
		//int gpuInit(int nn);
		//int gpuFindKNearests(float *p, int k, int *res);
		//int gpuFindKNearests(double *p, int k, int *res);

		int outKdTree(const char *outPath);

	private:
		int nDimension;
		unsigned int treeSize;
		double *offset;

		int treeRoot;	// �����ڵ�
		int **tree;		// 4 * n :�ָ�ά�ȡ����ڵ㡢��������������
		int *treePtr;	// ʹ��һά���ݱ�ʾ��ά���飬�洢������kdtree����

		/*
		*	�������ݴ洢��һά����dataPtr�data�ֱ���x/y/z�����ݵ���ʼ��ַ
		*	��ˣ�������knnֻ�贫�����ݵ�������ż���
		*/
		float **data;
		float *dataPtr;

		// ����kdtree
		int buildTree(int *indices, int count, int parent);
		
		/*
		*	�������ܺ���
		*/
		int chooseSplitDimension(int *ids, int sz, float &key);
		int chooseMiddleNode(int *ids, int sz, int dim, float key);
		float computeDistance(float *p, int n);
	};
}

#endif