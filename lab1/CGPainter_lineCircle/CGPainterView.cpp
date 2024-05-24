// CGPainterView.cpp : implementation of the CCGPainterView class
//

#include "stdafx.h"
#include "CGPainter.h"
#include "GlobalVariables.h"
#include "math.h"

#include "CGPainterDoc.h"
#include "CGPainterView.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView

IMPLEMENT_DYNCREATE(CCGPainterView, CView)

BEGIN_MESSAGE_MAP(CCGPainterView, CView)
	//{{AFX_MSG_MAP(CCGPainterView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView construction/destruction

CCGPainterView::CCGPainterView()
{
	m_pMemDC=new CDC;
	m_pBitmap=new CBitmap;
	m_nMaxX=GetSystemMetrics(SM_CXSCREEN);
	m_nMaxY=GetSystemMetrics(SM_CYSCREEN);

	G_iDrawState = DRAW_NOTHING;
	G_cLineColor = RGB(0,0,0);

	G_iMouseState = NO_BUTTON;
}

CCGPainterView::~CCGPainterView()
{
	delete m_pMemDC;
	delete m_pBitmap;
}

BOOL CCGPainterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView drawing

void CCGPainterView::OnDraw(CDC* pDC)
{
	CCGPainterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/////////////////////////////////////////
	CBitmap* pOldBitmap = m_pMemDC->SelectObject(m_pBitmap);
	pDC->BitBlt(0,0,m_nMaxX,m_nMaxY,m_pMemDC,0,0,SRCCOPY);
	m_pMemDC->SelectObject(pOldBitmap);
}

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView printing

BOOL CCGPainterView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCGPainterView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CCGPainterView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView diagnostics

#ifdef _DEBUG
void CCGPainterView::AssertValid() const
{
	CView::AssertValid();
}

void CCGPainterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGPainterDoc* CCGPainterView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGPainterDoc)));
	return (CCGPainterDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView message handlers

void CCGPainterView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	G_iMouseState = MOUSE_LEFT_BUTTON;
	m_ptStart=point;
	m_ptOld=point;
}

void CCGPainterView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	G_iMouseState = NO_BUTTON;

	if(G_iDrawState == DRAW_NOTHING)
		return;
	
	//set the drawing context
	CDC* pDC=GetDC();
	CBitmap* pOldBitmap=m_pMemDC->SelectObject(m_pBitmap);
	
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, G_cLineColor);
	CPen* pOldPen=m_pMemDC->SelectObject(&pen);
	m_pMemDC->SelectObject(&pen);

	CBrush* pOldBrush=(CBrush*)m_pMemDC->SelectStockObject(NULL_BRUSH);
	m_pMemDC->SelectStockObject(NULL_BRUSH);

	//draw graph
	if(G_iDrawState == DRAW_LINE)
		DrawLine(m_pMemDC, m_ptStart, point, G_cLineColor);
	else if(G_iDrawState == DRAW_CIRCLE)
	{
		CPoint dist = point - m_ptStart;
		int radius = int(sqrt(float(dist.x*dist.x + dist.y*dist.y)) + 0.5);
		DrawCircle(m_pMemDC, m_ptStart, radius, G_cLineColor);
	}	

	//retrieve the old contex
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	m_pMemDC->SelectObject(pOldBitmap);
	m_pMemDC->SelectObject(pOldPen);
	m_pMemDC->SelectObject(pOldBrush);
	ReleaseDC(pDC);
}

void CCGPainterView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(G_iDrawState == DRAW_NOTHING)
		return;
	
	if(G_iMouseState != MOUSE_LEFT_BUTTON)
		return;

	//set drawing context
	CDC* pDC=GetDC();
	CBitmap* pOldBitmap = m_pMemDC->SelectObject(m_pBitmap);
	
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, G_cLineColor);
	CPen* pOldPen=m_pMemDC->SelectObject(&pen);
	pDC->SelectObject(&pen);
	
	CBrush* pOldBrush=(CBrush*)m_pMemDC->SelectStockObject(NULL_BRUSH);
	pDC->SelectStockObject(NULL_BRUSH);

	//wipe the screen. This is used to realize the "elastic band" effect.
	CRect wipeRect(m_ptStart, m_ptOld);
	int radius;
	if(G_iDrawState == DRAW_CIRCLE)		//if drawing circle, the wipeRect is larger
	{
		CPoint dist = m_ptOld - m_ptStart;
		radius = int(sqrt(float(dist.x*dist.x + dist.y*dist.y)) + 0.5);

		CPoint ptLeftTop(m_ptStart.x - radius, m_ptStart.y - radius);
		CPoint ptRightBottom(m_ptStart.x + radius, m_ptStart.y + radius);
		wipeRect = CRect(ptLeftTop, ptRightBottom);
	}
	wipeRect.NormalizeRect();
	wipeRect.InflateRect(2, 2);
	pDC->BitBlt(wipeRect.left, wipeRect.top,
				wipeRect.Width(), wipeRect.Height(),
				m_pMemDC,
				wipeRect.left,wipeRect.top,
				SRCCOPY);

	//draw graph
	if(G_iDrawState == DRAW_LINE)
		DrawLine(pDC, m_ptStart, point, G_cLineColor);
	else if(G_iDrawState == DRAW_CIRCLE)
	{
		CPoint dist = point - m_ptStart;
		int radius = int(sqrt(float(dist.x*dist.x + dist.y*dist.y)) + 0.5);
		DrawCircle(pDC, m_ptStart, radius, G_cLineColor);
	}
	
	//retrieve the old contex
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	m_pMemDC->SelectObject(pOldBitmap);
	m_pMemDC->SelectObject(pOldPen);
	m_pMemDC->SelectObject(pOldBrush);
	ReleaseDC(pDC);
	m_ptOld=point;
}


void BresenhamDrawLine(CDC* pDC, CPoint ptStartPoint, CPoint ptEndPoint, COLORREF cLineColor) {
	//获取直线起点和终点
	int x1 = ptStartPoint.x;
	int y1 = ptStartPoint.y;
	int x2 = ptEndPoint.x;
	int y2 = ptEndPoint.y;

	//这里保留符号，不使用绝对值来确定象限
	int dx = x2 - x1;
	int dy = y2 - y1;

	//步长，默认值为1
	int xStep = 1;
	int yStep = 1;

	//如果是从右向左
	if (dx < 0)
	{
		dx = -dx;
		xStep = -xStep;
	}

	//如果是从下到上（屏幕坐标系的y轴正方向为向下）
	if (dy < 0)
	{
		dy = -dy;
		yStep = -yStep;
	}

	bool steep = dy > dx;//是否斜率绝对值大于1

	int increE = 0;
	int increNE = 0;
	int d = 0; //增值的初始化，具体赋值放在下面的条件分支中

	int x = x1, y = y1;
	pDC->SetPixelV(x, y, cLineColor);

	if (steep) {
		//沿y轴
		increE = 2 * dx;
		increNE = 2 * (dx - dy);
		d = 2 * dx - dy;
		while (y != y2) {
			y += yStep;
			if (d < 0) {
				d += increE; // d是下一次不等式中的增值
			}
			else {
				d += increNE;
				x += xStep;
			}
			pDC->SetPixelV(x, y, cLineColor);
		}
	}
	else {
		//沿x轴
		increE = 2 * dy;
		increNE = 2 * (dy - dx);
		d = 2 * dy - dx;
		while (x != x2) {
			x += xStep;
			if (d < 0) {
				d += increE; // d是下一次不等式中的增值
			}
			else {
				d += increNE;
				y += yStep;
			}
			pDC->SetPixelV(x, y, cLineColor);
		}
	}
}

void DDADrawLine(CDC* pDC, CPoint ptStartPoint, CPoint ptEndPoint, COLORREF cLineColor)
{
	//获取直线起点和终点
	int x1 = ptStartPoint.x;
	int y1 = ptStartPoint.y;
	int x2 = ptEndPoint.x;
	int y2 = ptEndPoint.y;

	int dx = x2 - x1;
	int dy = y2 - y1;

	int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
	float xIncrement = (float)dx / (float)steps;
	float yIncrement = (float)dy / (float)steps;

	float x = x1;
	float y = y1;

	for (int i = 0; i <= steps; i++) {
		pDC->SetPixelV((int)x, (int)y, cLineColor);
		x += xIncrement;
		y += yIncrement;
	}
}

void CCGPainterView::DrawLine(CDC *pDC, CPoint ptStartPoint, CPoint ptEndPoint, COLORREF cLineColor)
{
/*************************************************************
 write the Bresenham' line algorithm for drawing the line
 use function: pDC->SetPixelV(point, cLineColor); to drawing a pixel
编码直线生成算法，调用函数pDC->SetPixelV(point, cLineColor)画像素。
*************************************************************/

	BresenhamDrawLine(pDC, ptStartPoint, ptEndPoint,cLineColor);
	//DDADrawLine(pDC, ptStartPoint, ptEndPoint, cLineColor);
}



int CCGPainterView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CDC* pDC=GetDC();
	m_pMemDC->CreateCompatibleDC(pDC);
	m_pBitmap->CreateCompatibleBitmap(pDC,m_nMaxX,m_nMaxY); 
	CBitmap* pOldBitmap=m_pMemDC->SelectObject(m_pBitmap);
	CBrush brush;
	brush.CreateStockObject(WHITE_BRUSH);
	CRect rect(-1,-1,m_nMaxX,m_nMaxY);
	m_pMemDC->FillRect(rect,&brush);
	
	m_pMemDC->SelectObject(pOldBitmap);
	ReleaseDC(pDC);
	
	return 0;
}

void paint8Pixel(CDC* pDC, CPoint ptOrigin, int x, int y, COLORREF cLineColor)
{
	pDC->SetPixelV(ptOrigin.x + x, ptOrigin.y - y, cLineColor);
	pDC->SetPixelV(ptOrigin.x + y, ptOrigin.y - x, cLineColor);
	pDC->SetPixelV(ptOrigin.x + x, ptOrigin.y + y, cLineColor);
	pDC->SetPixelV(ptOrigin.x + y, ptOrigin.y + x, cLineColor);
	pDC->SetPixelV(ptOrigin.x - x, ptOrigin.y + y, cLineColor);
	pDC->SetPixelV(ptOrigin.x - y, ptOrigin.y + x, cLineColor);
	pDC->SetPixelV(ptOrigin.x - x, ptOrigin.y - y, cLineColor);
	pDC->SetPixelV(ptOrigin.x - y, ptOrigin.y - x, cLineColor);
}

void BresenhamDrawCircle(CDC* pDC, CPoint ptOrigin, int iRadius, COLORREF cLineColor)
{
	int x = 0;
	int y = iRadius;
	paint8Pixel(pDC, ptOrigin, x, y, cLineColor);

	int d = 1 - iRadius;

	while (++x && x <= y)
	{
		if (d < 0)
		{
			d += 2 * x + 1;
		}
		else
		{
			y--;
			d += 2 * (x - y) + 1;
		}
		paint8Pixel(pDC, ptOrigin, x, y, cLineColor);
	}
}

void CCGPainterView::DrawCircle(CDC *pDC, CPoint ptOrigin, int iRadius, COLORREF cLineColor)
{
//	CPoint ptLeftTop(ptOrigin.x - iRadius, ptOrigin.y - iRadius);
//	CPoint ptRightBottom(ptOrigin.x + iRadius, ptOrigin.y + iRadius);
//	CRect circleRect(ptLeftTop, ptRightBottom);
//	pDC->Ellipse(circleRect);

/*************************************************************
write the circle algorithm for drawing the circle
use function: pDC->SetPixelV(point, cLineColor); to drawing a pixel
编码圆弧生成算法，调用函数pDC->SetPixelV(point, cLineColor)画像素。
*************************************************************/
	BresenhamDrawCircle(pDC, ptOrigin, iRadius, cLineColor);
}
