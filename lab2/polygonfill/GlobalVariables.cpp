#define _MY_GLOBAL_VARIABLE_DEFINITION_FILE_

#include "stdafx.h"
#include "GlobalVariables.h"

// --------------------- Global variables ---------------------

int G_iDrawState;
int G_iMouseState;
COLORREF G_cLineColor;

void MyPolygon::SetTheSeed(CPoint point1)
{
	m_Seed = point1;
}

void MyPolygon::SetLineColor(COLORREF color1)
{
	m_LineColor = color1;
}

void MyPolygon::SetFillColor(COLORREF color1)
{
	m_FillColor = color1;
}
COLORREF MyPolygon::GetLineColor()
{
	return m_LineColor;
}

COLORREF MyPolygon::GetFillColor()
{
	return m_FillColor;
}

CPoint MyPolygon::GetTheSeed()
{
	return m_Seed;
}
void MyPolygon::FillThePolygon()
{ // my algorithm
	if (m_VerticeNumber < 3) return;
}

void MyPolygon::InsertVertex(CPoint point1)
{
	m_Vertex[m_VerticeNumber] = point1;
	m_VerticeNumber++;
}

bool MyPolygon::InsidePolygon(CPoint point1)
{
	CPoint thePoint = point1;
	CPoint m_TempVertex[50];

	for (int i = 0; i < m_VerticeNumber; i++)  // 坐标平移 
	{
		m_TempVertex[i].x = m_Vertex[i].x - thePoint.x;
		m_TempVertex[i].y = m_Vertex[i].y - thePoint.y;
	}
	m_TempVertex[m_VerticeNumber].x = m_Vertex[0].x - thePoint.x;
	m_TempVertex[m_VerticeNumber].y = m_Vertex[0].y - thePoint.y;

	// 计算象限
	int f, t1, t2, sum = 0;
	t1 = m_TempVertex[0].x >= 0 ? (m_TempVertex[0].y >= 0 ? 0 : 3)
		: (m_TempVertex[0].y >= 0 ? 1 : 2);
	for (int i = 1; i <= m_VerticeNumber; i++)
	{
		// 被测点为多边形顶点
		if (!m_TempVertex[i].x && !m_TempVertex[i].y) return false;

		f = m_TempVertex[i].y * m_TempVertex[i - 1].x
			- m_TempVertex[i].x * m_TempVertex[i - 1].y;
		if (!f && m_TempVertex[i - 1].x * m_TempVertex[i].x <= 0
			&& m_TempVertex[i - 1].y * m_TempVertex[i].y <= 0) return false;  // 点在边上
		t2 = m_TempVertex[i].x >= 0 ? (m_TempVertex[i].y >= 0 ? 0 : 3)
			: (m_TempVertex[i].y >= 0 ? 1 : 2);						// 计算象限

		if (t2 == (t1 + 1) % 4)			sum += 1;              // 情况1
		else if (t2 == (t1 + 3) % 4)	sum -= 1;              // 情况2
		else if (t2 == (t1 + 2) % 4)                           // 情况3
		{
			if (f > 0)	sum += 2;
			else			sum -= 2;
		}
		t1 = t2;
	}

	if (sum == -4) return  true;
	else		  return  false;
}

int MyPolygon::GetMinY() {
	int minY = m_Vertex[0].y;
	for (int i = 1; i < m_VerticeNumber; ++i) {
		if (m_Vertex[i].y < minY) {
			minY = m_Vertex[i].y;
		}
	}
	return minY;
}

int MyPolygon::GetMaxY() {
	int maxY = m_Vertex[0].y;
	for (int i = 1; i < m_VerticeNumber; ++i) {
		if (m_Vertex[i].y > maxY) {
			maxY = m_Vertex[i].y;
		}
	}
	return maxY;
}

void MyPolygon::SetAET(int y) {

	if (activeEdgeTable != nullptr) {
		Edge* previous = nullptr;
		Edge* current = activeEdgeTable;
		// 对表中边进行更新
		while (current != nullptr) {
			if (current->ymax == y) {
				//ymax = y 的边进行删除
				if (previous != nullptr) {
					//如果删除的不是第一个节点
					previous->next = current->next;
					current = previous->next;
				}
				else {
					//如果删除的是第一个节点
					current = current->next;
					activeEdgeTable = current; // 更新活化边表的头部
				}
			}
			else {
				//x变为x+dx
				current->x += current->dx;
				previous = current;
				current = current->next;
			}
		}
		// 此时current为null，previous为链表中最后一个
		// 将边表接在活化边表的末尾
		if (previous != nullptr) previous->next = edgeTable[y];
		else activeEdgeTable = edgeTable[y]; //如果previous为空，那么说明边被全部删除了，表为空
	}
	else {
		//若活化边表为空，直接将边表赋值给活化边表
		activeEdgeTable = edgeTable[y];
	}

	// 对活化边表按照 x 坐标和 dx 进行排序，确保其按照 x 坐标的递增顺序排列。
	if (activeEdgeTable != nullptr) {
		Edge* edge = activeEdgeTable;
		Edge* sortedList = nullptr;

		while (edge != nullptr) {
			Edge* nextEdge = edge->next; // 存储下一个待添加边

			
			if (sortedList == nullptr || edge->x < sortedList->x || (edge->x == sortedList->x && edge->dx < sortedList->dx)) {
				//如果待添加边的x比已排序链表中的第一个边的x小，或者已排序链表为空，直接添加到头部
				edge->next = sortedList;
				sortedList = edge;
			}
			else {
				Edge* currentSorted = sortedList; // 遍历链表的指针

				// 确定插入位置
				while (currentSorted->next != nullptr && (edge->x > currentSorted->next->x || (edge->x == currentSorted->next->x && edge->dx > currentSorted->next->dx))) {
					currentSorted = currentSorted->next;
				}

				edge->next = currentSorted->next;
				currentSorted->next = edge;
			}

			edge = nextEdge; //迭代
		}

		activeEdgeTable = sortedList;// 更新活化边表为排序后的链表
	}
}

void MyPolygon::BuildEdgeTable() {
	// 边表重置
	for (int i = 0; i < MAX_Y_NUM; ++i) {
		edgeTable[i] = nullptr;
	}
	// 逐条边构建边表
	for (int i = 0; i < m_VerticeNumber; ++i) {
		int nextIndex = (i + 1) % m_VerticeNumber; // 下一个顶点的索引

		// 获取当前边的顶点坐标
		CPoint currentVertex = m_Vertex[i];
		CPoint nextVertex = m_Vertex[nextIndex];

		// 排除水平边
		if (currentVertex.y != nextVertex.y) {
			// 确定边的上端点和下端点
			CPoint upperVertex = (currentVertex.y < nextVertex.y) ? currentVertex : nextVertex;
			CPoint lowerVertex = (currentVertex.y < nextVertex.y) ? nextVertex : currentVertex;

			// 设置边的变量
			Edge* newEdge = new Edge;
			newEdge->x = upperVertex.x;
			float t1 = lowerVertex.x - upperVertex.x;
			float t2 = lowerVertex.y - upperVertex.y;
			float t3 = t1 / t2;
			newEdge->dx = t3;
			newEdge->ymax = lowerVertex.y;
			newEdge->next = nullptr;

			// 将边添加到边表中
			int y = upperVertex.y;
			if (edgeTable[y] != nullptr)
			{
				//链表不为空
				Edge* current = edgeTable[y];
				while (current->next != nullptr) {
					current = current->next;
				}
				current->next = newEdge;
			}
			else {
				//链表为空
				edgeTable[y] = newEdge;
			}
		}
	}
}

