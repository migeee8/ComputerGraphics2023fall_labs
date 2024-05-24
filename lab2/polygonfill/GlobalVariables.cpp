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

	for (int i = 0; i < m_VerticeNumber; i++)  // ����ƽ�� 
	{
		m_TempVertex[i].x = m_Vertex[i].x - thePoint.x;
		m_TempVertex[i].y = m_Vertex[i].y - thePoint.y;
	}
	m_TempVertex[m_VerticeNumber].x = m_Vertex[0].x - thePoint.x;
	m_TempVertex[m_VerticeNumber].y = m_Vertex[0].y - thePoint.y;

	// ��������
	int f, t1, t2, sum = 0;
	t1 = m_TempVertex[0].x >= 0 ? (m_TempVertex[0].y >= 0 ? 0 : 3)
		: (m_TempVertex[0].y >= 0 ? 1 : 2);
	for (int i = 1; i <= m_VerticeNumber; i++)
	{
		// �����Ϊ����ζ���
		if (!m_TempVertex[i].x && !m_TempVertex[i].y) return false;

		f = m_TempVertex[i].y * m_TempVertex[i - 1].x
			- m_TempVertex[i].x * m_TempVertex[i - 1].y;
		if (!f && m_TempVertex[i - 1].x * m_TempVertex[i].x <= 0
			&& m_TempVertex[i - 1].y * m_TempVertex[i].y <= 0) return false;  // ���ڱ���
		t2 = m_TempVertex[i].x >= 0 ? (m_TempVertex[i].y >= 0 ? 0 : 3)
			: (m_TempVertex[i].y >= 0 ? 1 : 2);						// ��������

		if (t2 == (t1 + 1) % 4)			sum += 1;              // ���1
		else if (t2 == (t1 + 3) % 4)	sum -= 1;              // ���2
		else if (t2 == (t1 + 2) % 4)                           // ���3
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
		// �Ա��б߽��и���
		while (current != nullptr) {
			if (current->ymax == y) {
				//ymax = y �ı߽���ɾ��
				if (previous != nullptr) {
					//���ɾ���Ĳ��ǵ�һ���ڵ�
					previous->next = current->next;
					current = previous->next;
				}
				else {
					//���ɾ�����ǵ�һ���ڵ�
					current = current->next;
					activeEdgeTable = current; // ���»�߱��ͷ��
				}
			}
			else {
				//x��Ϊx+dx
				current->x += current->dx;
				previous = current;
				current = current->next;
			}
		}
		// ��ʱcurrentΪnull��previousΪ���������һ��
		// ���߱���ڻ�߱��ĩβ
		if (previous != nullptr) previous->next = edgeTable[y];
		else activeEdgeTable = edgeTable[y]; //���previousΪ�գ���ô˵���߱�ȫ��ɾ���ˣ���Ϊ��
	}
	else {
		//����߱�Ϊ�գ�ֱ�ӽ��߱�ֵ����߱�
		activeEdgeTable = edgeTable[y];
	}

	// �Ի�߱��� x ����� dx ��������ȷ���䰴�� x ����ĵ���˳�����С�
	if (activeEdgeTable != nullptr) {
		Edge* edge = activeEdgeTable;
		Edge* sortedList = nullptr;

		while (edge != nullptr) {
			Edge* nextEdge = edge->next; // �洢��һ������ӱ�

			
			if (sortedList == nullptr || edge->x < sortedList->x || (edge->x == sortedList->x && edge->dx < sortedList->dx)) {
				//�������ӱߵ�x�������������еĵ�һ���ߵ�xС����������������Ϊ�գ�ֱ����ӵ�ͷ��
				edge->next = sortedList;
				sortedList = edge;
			}
			else {
				Edge* currentSorted = sortedList; // ���������ָ��

				// ȷ������λ��
				while (currentSorted->next != nullptr && (edge->x > currentSorted->next->x || (edge->x == currentSorted->next->x && edge->dx > currentSorted->next->dx))) {
					currentSorted = currentSorted->next;
				}

				edge->next = currentSorted->next;
				currentSorted->next = edge;
			}

			edge = nextEdge; //����
		}

		activeEdgeTable = sortedList;// ���»�߱�Ϊ����������
	}
}

void MyPolygon::BuildEdgeTable() {
	// �߱�����
	for (int i = 0; i < MAX_Y_NUM; ++i) {
		edgeTable[i] = nullptr;
	}
	// �����߹����߱�
	for (int i = 0; i < m_VerticeNumber; ++i) {
		int nextIndex = (i + 1) % m_VerticeNumber; // ��һ�����������

		// ��ȡ��ǰ�ߵĶ�������
		CPoint currentVertex = m_Vertex[i];
		CPoint nextVertex = m_Vertex[nextIndex];

		// �ų�ˮƽ��
		if (currentVertex.y != nextVertex.y) {
			// ȷ���ߵ��϶˵���¶˵�
			CPoint upperVertex = (currentVertex.y < nextVertex.y) ? currentVertex : nextVertex;
			CPoint lowerVertex = (currentVertex.y < nextVertex.y) ? nextVertex : currentVertex;

			// ���ñߵı���
			Edge* newEdge = new Edge;
			newEdge->x = upperVertex.x;
			float t1 = lowerVertex.x - upperVertex.x;
			float t2 = lowerVertex.y - upperVertex.y;
			float t3 = t1 / t2;
			newEdge->dx = t3;
			newEdge->ymax = lowerVertex.y;
			newEdge->next = nullptr;

			// ������ӵ��߱���
			int y = upperVertex.y;
			if (edgeTable[y] != nullptr)
			{
				//����Ϊ��
				Edge* current = edgeTable[y];
				while (current->next != nullptr) {
					current = current->next;
				}
				current->next = newEdge;
			}
			else {
				//����Ϊ��
				edgeTable[y] = newEdge;
			}
		}
	}
}

