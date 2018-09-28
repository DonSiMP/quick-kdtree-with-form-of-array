#ifndef ZTKDTREE_H
#define ZTKDTREE_H

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
		int findKNearests(float *p, int k, int *res);	// ʹ��stl��ջ
		int findKNearests2(float *p, int k, int *res);	// ʹ���Ա�д��ջ
		int findKNearests(double *p, int k, int *res);

		// ���Ҹ�����Χ�ڵĵ�
		int findNearestRange(float *p, float range, int *res);
		int findNearestRange(double *p, float range, int *res);

		int outKdTree(const char *outPath);

	public:
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