// --------------------- Global variables ---------------------
#include <vector>
#ifndef __GLOBAL_VARIABLES_H__
#define __GLOBAL_VARIABLES_H__


//drawing state
#define DRAW_NOTHING	0
#define DRAW_LINE		1
#define DRAW_CIRCLE		2
#define DRAW_POLYGON    3
#define DRAW_POLYGON_FILL 4

#define NO_BUTTON                0
#define MOUSE_LEFT_BUTTON        1
#define MOUSE_RIGHT_BUTTON       2

#define MAX_Y_NUM	1024
/////////////////////////////////////////////

extern int G_iDrawState;
extern int G_iMouseState;
extern COLORREF G_cLineColor;

struct Edge {
	float x;          // 当前 x 坐标
	float dx;         // 斜率的倒数
	float ymax;       // 线段结束的 y 坐标
	Edge* next;     // 指向下一个边的指针（链表）
};

class  MyPolygon
{
public:
	int m_VerticeNumber;
	CPoint m_Vertex[50]; 
	COLORREF m_LineColor;
private:
	bool isFilled;
	COLORREF m_FillColor;
	CPoint m_Seed;

public:
	Edge* edgeTable[MAX_Y_NUM]; //ET
	Edge* activeEdgeTable; //AET

public:
	MyPolygon()
	{
		isFilled = false;
		m_VerticeNumber = 0;
		m_LineColor = 	RGB(0,1,0);
		m_FillColor =   RGB(0,0,1);

		for (int i = 0; i < MAX_Y_NUM; ++i) {
			edgeTable[i] = nullptr;
		}

		activeEdgeTable = nullptr;
	};

	void SetTheSeed(CPoint point1);
	void SetLineColor(COLORREF color1);
	void SetFillColor(COLORREF color1);
	COLORREF GetLineColor();
	COLORREF GetFillColor();
	CPoint GetTheSeed();
	void FillThePolygon();
	void InsertVertex(CPoint point1);
	bool InsidePolygon(CPoint point1);

	int GetMinY();
	int GetMaxY();
	void BuildEdgeTable();
	void SetAET(int y);
};


#endif