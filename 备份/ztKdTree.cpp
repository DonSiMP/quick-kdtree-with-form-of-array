#include "ztKdTree.h"
#include <stack>
#include <queue>
#include <vector>

namespace zt
{
	ZtKDTree::ZtKDTree()
		: nDimension(0), treeSize(0), offset(0),
		tree(0), treePtr(0), data(0), dataPtr(0)
	{

	}

	ZtKDTree::ZtKDTree(int dimension, unsigned int sz)
		: nDimension(dimension), treeSize(sz), offset(0),
		tree(0), treePtr(0), data(0), dataPtr(0)
	{
		nDimension = dimension;
		treeSize = sz;

		if (nDimension > 0 && treeSize > 0)
		{
			offset = new double[nDimension];

			tree = new int *[4];
			treePtr = new int[4 * treeSize];
			for (int i = 0; i < 4; i++)
			{
				tree[i] = treePtr + i * treeSize;
			}

			data = new float *[nDimension];
			dataPtr = new float[nDimension * sz];
			for (int i = 0; i < nDimension; i++)
			{
				data[i] = dataPtr + i * treeSize;
			}
		}
	}

	ZtKDTree::~ZtKDTree()
	{
		if (offset)
		{
			delete[] offset;
		}
		if (tree)
		{
			delete[] tree;
		}
		if (treePtr)
		{
			delete[] treePtr;
		}
		if (data)
		{
			delete[] data;
		}
		if (dataPtr)
		{
			delete[] dataPtr;
		}
	}

	int ZtKDTree::setSize(int dimension, unsigned int sz)
	{
		nDimension = dimension;	// ���ݵ�ά��
		treeSize = sz;			// ���ݵ�����

		if (nDimension > 0 && treeSize > 0)
		{
			offset = new double[nDimension];

			tree = new int *[4];
			treePtr = new int[4 * treeSize];
			for (int i = 0; i < 4; i++)
			{
				tree[i] = treePtr + i * treeSize;
			}

			data = new float *[nDimension];
			dataPtr = new float[nDimension * sz];
			for (int i = 0; i < nDimension; i++)
			{
				data[i] = dataPtr + i * treeSize;
			}
		}

		return 0;
	}

	int ZtKDTree::setOffset(double oft[])
	{
		for (int i = 0; i < nDimension; i++)
		{
			offset[i] = oft[i];
		}

		return 0;
	}

	/*
	*	indata��һά�����ʾ��ά����
	*	�����Ų���ʽ��{[x,y,z����], [x,y,z����], ����}
	*	dataPtrҲ��һά�����ʾ��ά����
	*	�����Ų���ʽ��{[x1, x2, x3����], [y1, y2, y3����], [z1, z2, z3����], ����}
	*/
	int ZtKDTree::setData(float *indata)
	{
		for (unsigned int i = 0; i < treeSize; i++)
		{
			for (int j = 0; j < nDimension; j++)
			{
				dataPtr[j * treeSize + i] = indata[i * nDimension + j];
			}
		}

		return 0;
	}

	int ZtKDTree::setData(double *indata)
	{
		for (unsigned int i = 0; i < treeSize; i++)
		{
			for (int j = 0; j < nDimension; j++)
			{
				dataPtr[j * treeSize + i] = float(indata[i * nDimension + j] - offset[j]);
			}
		}

		return 0;
	}

	int ZtKDTree::buildTree()
	{
		std::vector<int> vtr(treeSize);

		for (int i = 0; i < treeSize; i++)
		{
			vtr[i] = i;
		}

		std::random_shuffle(vtr.begin(), vtr.end());

		treeRoot = buildTree(&vtr[0], treeSize, -1);	// ���ڵ�ĸ��ڵ���-1

		return treeRoot;
	}

	int ZtKDTree::buildTree(int *indices, int count, int parent)
	{
		if (count == 1)
		{
			int rd = indices[0];
			tree[0][rd] = 0;
			tree[1][rd] = parent;
			tree[2][rd] = -1;
			tree[3][rd] = -1;

			return rd;
		}
		else
		{
			float key = 0;
			int split = chooseSplitDimension(indices, count, key);
			int idx = chooseMiddleNode(indices, count, split, key);

			// rd ��ʵ�ʵ���±꣬ idx�ǵ������������±�
			int rd = indices[idx];

			tree[0][rd] = split;	// �ָ�ά��
			tree[1][rd] = parent;

			if (idx > 0)
			{
				tree[2][rd] = buildTree(indices, idx, rd);
			}
			else
			{
				tree[2][rd] = -1;
			}

			if (idx + 1 < count)
			{
				tree[3][rd] = buildTree(indices + idx + 1, count - idx - 1, rd);
			}
			else
			{
				tree[3][rd] = -1;
			}

			return rd;
		}
	}

	int ZtKDTree::findNearest(float *p)
	{
		int nearestNode;
		float minDist = 9999999, distance = 0;

		std::stack<int> paths;

		int node = treeRoot;
		while (node > -1)
		{
			paths.emplace(node);
			node = p[tree[0][node]] <= data[tree[0][node]][node] ? tree[2][node] : tree[3][node];
		}

		// ����·��
		while (!paths.empty())
		{
			node = paths.top();
			paths.pop();

			distance = computeDistance(p, node);

			if (distance < minDist)
			{
				minDist = distance;
				nearestNode = node;
			}

			// ���ҽڵ㶼Ϊ�գ���ΪҶ�ӽڵ�
			if (tree[2][node] + tree[3][node] == -2)
			{
				continue;
			}
			else
			{
				//$$$$$$$$$$$$$$$$$$   ��������  $$$$$$$$$$$$$$$$$$$$$//
				//													  //
				//  ��ǰ����·���Ľڵ㶼������������ͬһ�ռ��еĽڵ�  //
				//	����·��ʱ��������ڵ��뵱ǰ��������뾶�ཻ��    //
				//  ������ֵܿռ䲢����·��						  //
				//													  //
				//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$//

				int dim = tree[0][node];

				if (fabs(p[dim] - data[dim][node]) < minDist)
				{
					if (p[dim] < data[dim][node] && tree[3][node] != -1)
					{
						/*paths.push(tree[3][node]);*/
						int reNode = tree[3][node];
						while (reNode > -1)
						{
							paths.emplace(reNode);

							reNode = p[tree[0][reNode]] <= data[tree[0][reNode]][reNode] ? tree[2][reNode] : tree[3][reNode];
						}
					}
					else if (p[dim] > data[dim][node] && tree[2][node] != -1)
					{
						/*paths.push(tree[2][node]);*/
						int reNode = tree[2][node];
						while (reNode > -1)
						{
							paths.emplace(reNode);

							reNode = p[tree[0][reNode]] <= data[tree[0][reNode]][reNode] ? tree[2][reNode] : tree[3][reNode];
						}
					}
				}
			}
		}

		return nearestNode;
	}

	int ZtKDTree::findNearest(double *p)
	{
		float *pf = new float[nDimension];

		for (int i = 0; i < nDimension; i++)
		{
			pf[i] = p[i] - offset[i];
		}

		return findNearest(pf);
	}

	struct NearestNode
	{
		int node;
		float distance;
		NearestNode()
		{
			node = 0;
			distance = 0.0f;
		}
		NearestNode(int n, float d)
		{
			node = n;
			distance = d;
		}
	};

	struct cmp	// ������Ԫ�ط��ڶ���
	{
		bool operator()(NearestNode a, NearestNode b)
		{
			return a.distance < b.distance;
		}
	};

	int ZtKDTree::findKNearests(float *p, int k, int *res)
	{
		std::priority_queue<NearestNode, std::vector<NearestNode>, cmp> kNeighbors;
		std::stack<int> paths;
		
		// ��¼����·��
		int node = treeRoot;
		while (node > -1)
		{
			paths.emplace(node);

			node = p[tree[0][node]] <= data[tree[0][node]][node] ? tree[2][node] : tree[3][node];
		}

		// Ԥ�ȼ���һ������ڵ�
		kNeighbors.emplace(-1, 9999999);

		// ����·��
		float distance = 0;
		while (!paths.empty()) 
		{
			node = paths.top();
			paths.pop();

			distance = computeDistance(p, node);
			if (kNeighbors.size() < k)
			{
				kNeighbors.emplace(node, distance);
			}
			else
			{
				if (distance < kNeighbors.top().distance)
				{
					kNeighbors.pop();
					kNeighbors.emplace(node, distance);
				}
			}
				
			if (tree[2][node] + tree[3][node] > -2)
			{
				int dim = tree[0][node];
				if (p[dim] > data[dim][node])
				{
					if (p[dim] - data[dim][node] < kNeighbors.top().distance && tree[2][node] > -1)
					{
						int reNode = tree[2][node];
						while (reNode > -1)
						{
							paths.emplace(reNode);

							reNode = p[tree[0][reNode]] <= data[tree[0][reNode]][reNode] ? tree[2][reNode] : tree[3][reNode];
						}
					}
				}
				else
				{
					if (data[dim][node] - p[dim] < kNeighbors.top().distance && tree[3][node] > -1)
					{
						int reNode = tree[3][node];
						while (reNode > -1)
						{
							paths.emplace(reNode);

							reNode = p[tree[0][reNode]] <= data[tree[0][reNode]][reNode] ? tree[2][reNode] : tree[3][reNode];
						}
					}
				}
			}
		}

		if (!res)
		{ 
			res = new int[k];
		}

		int i = kNeighbors.size();
		while (!kNeighbors.empty()) 
		{
			res[--i] = kNeighbors.top().node;
			kNeighbors.pop();
		}

		return 0;
	}

	int ZtKDTree::findKNearests2(float *p, int k, int *res)
	{
		// ������ʽ������
		int *kNeighbors = new int[k];
		float *kNDistance = new float[k];
		int _currentNNode = 0;

		// ������ʽ��·����ջ
		int paths[64];
		int _currentPath = 0;

		// ��¼����·��
		int node = treeRoot;
		while (node > -1)
		{
			paths[_currentPath++] = node;

			node = p[tree[0][node]] <= data[tree[0][node]][node] ? tree[2][node] : tree[3][node];
		}

		kNeighbors[_currentNNode] = -1;
		kNDistance[_currentNNode++] = 9999999;

		// ����·��
		float distance = 0;
		while (_currentPath > 0)
		{
			node = paths[_currentPath-- - 1];

			distance = computeDistance(p, node);
			if (_currentNNode < k)
			{
				kNeighbors[_currentNNode] = node;
				kNDistance[_currentNNode++] = distance;

				// ���ﵽk���ڵ�󣬽�����������
				if (_currentNNode == k)
				{
					for (int i = _currentNNode / 2 - 1; i >= 0; i--)
					{
						int parent = i;
						
						for (int son = i * 2 + 1; son <= _currentNNode; son = son * 2 + 1)
						{
							if (son + 1 < _currentNNode && kNDistance[son] < kNDistance[son + 1])
								son++;

							if (kNDistance[parent] < kNDistance[son])  // ������ڵ�С���ӽڵ㣬�򽻻�
							{
								float tempD = kNDistance[parent];
								int tempI = kNeighbors[parent];
								kNDistance[parent] = kNDistance[son];
								kNeighbors[parent] = kNeighbors[son];
								kNDistance[son] = tempD;
								kNeighbors[son] = tempI;
							}

							parent = son;
						}
					}
				}
			}
			else
			{
				if (distance < kNDistance[0])
				{
					// pop
					kNeighbors[0] = kNeighbors[_currentNNode - 1];
					kNDistance[0] = kNDistance[_currentNNode - 1];

					// ɾ���Ѷ���Ҫ�ع�����
					int parent = 0;
					int son = parent * 2 + 1;
					for (; son < _currentNNode - 1; son = son * 2 + 1)
					{
						if (son + 1 < _currentNNode - 1 && kNDistance[son] < kNDistance[son + 1])
							son++;

						if (kNDistance[parent] < kNDistance[son])  // ������ڵ�С���ӽڵ㣬�򽻻�
						{
							float tempD = kNDistance[parent];
							int tempI = kNeighbors[parent];
							kNDistance[parent] = kNDistance[son];
							kNeighbors[parent] = kNeighbors[son];
							kNDistance[son] = tempD;
							kNeighbors[son] = tempI;
						}

						parent = son;
					}

					// push
					son = _currentNNode - 1;
					parent = (son - 1) / 2;
					while (son != 0 && distance > kNDistance[parent])
					{
						kNeighbors[son] = kNeighbors[parent];
						kNDistance[son] = kNDistance[parent];
						son = parent;
						parent = (son - 1) / 2;
					}

					kNDistance[son] = distance;
					kNeighbors[son] = node;
				}
			}

			if (tree[2][node] + tree[3][node] > -2)
			{
				int dim = tree[0][node];
				if (p[dim] > data[dim][node])
				{
					if (p[dim] - data[dim][node] < kNDistance[0] && tree[2][node] > -1)
					{
						int reNode = tree[2][node];
						while (reNode > -1)
						{
							paths[_currentPath++] = reNode;

							reNode = p[tree[0][reNode]] <= data[tree[0][reNode]][reNode] ? tree[2][reNode] : tree[3][reNode];
						}
					}
				}
				else
				{
					if (data[dim][node] - p[dim] < kNDistance[0] && tree[3][node] > -1)
					{
						int reNode = tree[3][node];
						while (reNode > -1)
						{
							paths[_currentPath++] = reNode;

							reNode = p[tree[0][reNode]] <= data[tree[0][reNode]][reNode] ? tree[2][reNode] : tree[3][reNode];
						}
					}
				}
			}
		}

		// ���ж�����
		for (int i = _currentNNode - 1; i > 0; i--)
		{
			int tempI = kNeighbors[0];
			float tempD = kNDistance[0];
			kNeighbors[0] = kNeighbors[i];
			kNDistance[0] = kNDistance[i];
			kNeighbors[i] = tempI;
			kNDistance[i] = tempD;

			int parent = 0;
			int son = parent * 2 + 1;
			for (; son < i; son = parent * 2 + 1)
			{
				if (son + 1 < i && kNDistance[son] < kNDistance[son + 1])
					son++;

				if (kNDistance[parent] < kNDistance[son])
				{
					tempD = kNDistance[parent];
					tempI = kNeighbors[parent];
					kNDistance[parent] = kNDistance[son];
					kNeighbors[parent] = kNeighbors[son];
					kNDistance[son] = tempD;
					kNeighbors[son] = tempI;
				}

				parent = son;
			}
		}

		if (!res)
		{
			res = new int[k];
		}

		int i = k;
		while (i != 0)
		{	
			i--;
			res[i] = kNeighbors[i];
		}

		delete[] kNeighbors;
		delete[] kNDistance;

		return 0;
	}

	int ZtKDTree::findKNearests(double *p, int k, int *res)
	{
		float *pf = new float[nDimension];

		for (int i = 0; i < nDimension; i++)
		{
			pf[i] = p[i] - offset[i];
		}

		findKNearests(pf, k, res);

		return 0;
	}

	int ZtKDTree::findNearestRange(float *p, float range, int *res)
	{
		std::queue<NearestNode> kNeighbors;
		std::stack<int> paths;

		// ��¼����·��
		int node = treeRoot;
		while (node > -1)
		{
			paths.emplace(node);

			node = p[tree[0][node]] <= data[tree[0][node]][node] ? tree[2][node] : tree[3][node];
		}

		// ����·��
		float distance = 0;
		while (!paths.empty())
		{
			node = paths.top();
			paths.pop();

			distance = computeDistance(p, node);
			if (distance < range)
			{
				kNeighbors.emplace(node, distance);
			}

			if (tree[2][node] + tree[3][node] > -2)
			{
				int dim = tree[0][node];
				if (p[dim] > data[dim][node])
				{
					if (p[dim] - data[dim][node] < range && tree[2][node] > -1)
					{
						int reNode = tree[2][node];
						while (reNode > -1)
						{
							paths.emplace(reNode);

							reNode = p[tree[0][reNode]] <= data[tree[0][reNode]][reNode] ? tree[2][reNode] : tree[3][reNode];
						}
					}
				}
				else
				{
					if (data[dim][node] - p[dim] < range && tree[3][node] > -1)
					{
						int reNode = tree[3][node];
						while (reNode > -1)
						{
							paths.emplace(reNode);

							reNode = p[tree[0][reNode]] <= data[tree[0][reNode]][reNode] ? tree[2][reNode] : tree[3][reNode];
						}
					}
				}
			}
		}

		if (!res)
		{
			res = new int[kNeighbors.size()];
		}

		int i = kNeighbors.size();
		while (!kNeighbors.empty())
		{
			res[--i] = kNeighbors.front().node;
			kNeighbors.pop();
		}

		return 0;
	}

	int ZtKDTree::findNearestRange(double *p, float range, int *res)
	{
		float *pf = new float[nDimension];

		for (int i = 0; i < nDimension; i++)
		{
			pf[i] = p[i] - offset[i];
		}

		findNearestRange(pf, range, res);

		return 0;
	}

	/*	
	*	�������ܺ���
	*/
	int ZtKDTree::chooseSplitDimension(int *ids, int sz, float &key)
	{
		int split = 0;

		float *var = new float[nDimension];
		float *mean = new float[nDimension];

		int cnt = std::min((int)SAMPLE_MEAN, sz);/* cnt = sz;*/
		double rt = 1.0 / cnt;

		for (int i = 0; i < nDimension; i++)
		{
			double sum1 = 0, sum2 = 0;
			for (int j = 0; j < cnt; j++)
			{
				sum1 += rt * data[i][ids[j]] * data[i][ids[j]];
				sum2 += rt * data[i][ids[j]];
			}
			var[i] = sum1 - sum2 * sum2;
			mean[i] = sum2;
		}

		double max = 0;

		for (int i = 0; i < nDimension; i++)
		{
			if (var[i] > max)
			{
				key = mean[i];
				max = var[i];
				split = i;
			}
		}

		delete[] var;
		delete[] mean;

		return split;
	}

	int ZtKDTree::chooseMiddleNode(int *ids, int sz, int dim, float key)
	{
		int left = 0;
		int right = sz - 1;

		while (1)
		{
			while (left <= right && data[dim][ids[left]] <= key)	//����ұ�key���ֵ
				++left;

			while (left <= right && data[dim][ids[right]] >= key)	//�ұ��ұ�keyС��ֵ
				--right; 

			if (left > right)
				break;

			std::swap(ids[left], ids[right]);
			++left; 
			--right;
		}

// 		int lim1 = left;
// 
// 		right = sz - 1;
// 		while (1)
// 		{
// 			while (left <= right && data[dim][ids[left]] <= key)	//����ұ�key���ֵ
// 				++left;
// 
// 			while (left <= right && data[dim][ids[right]] > key)	//�ұ��ұ�keyС��ֵ
// 				--right;
// 
// 			if (left > right)
// 				break;
// 
// 			std::swap(ids[left], ids[right]);
// 			left++;
// 			right--;
// 		}
//
//		int index = 0, lim2 = left;
//
// 		if (lim1 > sz / 2) 
// 			index = lim1;
// 		else if (lim2 < sz / 2) 
// 			index = lim2;
// 		else 
// 			index = sz / 2;
// 
// 		if ((lim1 == sz) || (lim2 == 0)) 
// 			index = sz / 2;
//
//		index = left;

		// �ҳ������������ֵ��Ϊ���ڵ�
		float max = -9999999;
		int maxIndex = 0;
		for (int i = 0; i < left; i++)
		{
			if (data[dim][ids[i]] > max)
			{
				max = data[dim][ids[i]];
				maxIndex = i;
			}
		}

		if (maxIndex != left - 1)
		{
			std::swap(ids[maxIndex], ids[left - 1]);
		}

		return left - 1;
	}

	float ZtKDTree::computeDistance(float *p, int n)
	{
		float sum = 0;

		for (int i = 0; i < nDimension; i++)
		{
			sum += (p[i] - data[i][n]) * (p[i] - data[i][n]);
		}

		return sqrt(sum);
	}

	int ZtKDTree::outKdTree(const char *outPath)
	{
		FILE *fp;
		fopen_s(&fp, outPath, "w");
		if (!fp)
		{
			printf("Failed to create: %s", outPath);
			return 1;
		}

		for (int i = 0; i < treeSize; i++)
		{
			fprintf_s(fp, "%d\t%7.3f\t%7.3f\t%7.3f\t%d\t%6d\t%6d\n",
				i, data[0][i], data[1][i], data[2][i],
				tree[0][i], tree[2][i], tree[3][i]);
		}

		fclose(fp);

		return 0;
	}
}