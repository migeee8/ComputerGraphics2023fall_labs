// CGPainterView.cpp : implementation of the CCGPainterView class
//

#include "stdafx.h"
#include "CGPainter.h"
#include "GlobalVariables.h"
#include "math.h"

#include "CGPainterDoc.h"
#include "CGPainterView.h"

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
	ON_COMMAND(ID_BUTTON_CLEARWINDOW, OnButtonClearwindow)
	ON_COMMAND(ID_BUTTON_POLYGON, OnButtonPolygon)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_POLYGON, OnUpdateButtonPolygon)
	ON_COMMAND(ID_BUTTON_POLYGON_FILL, OnButtonPolygonFill)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_POLYGON_FILL, OnUpdateButtonPolygonFill)
	ON_WM_RBUTTONUP()
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
	G_cLineColor = RGB(0,1,0);
	numPolygons = 0; //初始化 多边形数量为0

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
// CCGPainterView button methods
void CCGPainterView::OnButtonClearwindow()
{
	//清屏.
	CBrush brush;
	brush.CreateStockObject(WHITE_BRUSH);
	CRect rect(-1, -1, m_nMaxX, m_nMaxY);	//原图像并不清除.
	m_pMemDC->FillRect(rect, &brush);

	Invalidate(false);

	G_iDrawState = DRAW_NOTHING;

}

void CCGPainterView::OnButtonPolygon()
{
	// 切换为绘制多边形状态
	if (G_iDrawState != DRAW_POLYGON)
	{
		G_iDrawState = DRAW_POLYGON;
		ThePolygons[numPolygons].m_VerticeNumber = 0;

	}
	else
		G_iDrawState = DRAW_NOTHING;

}

void CCGPainterView::OnUpdateButtonPolygon(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(G_iDrawState == DRAW_POLYGON);

}

void CCGPainterView::OnButtonPolygonFill()
{
	// 切换为填充多边形状态
	if (G_iDrawState != DRAW_POLYGON_FILL)
	{
		G_iDrawState = DRAW_POLYGON_FILL;
		ThePolygons[numPolygons].m_VerticeNumber = 0;
	}
	else
		G_iDrawState = DRAW_NOTHING;

}

void CCGPainterView::OnUpdateButtonPolygonFill(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(G_iDrawState == DRAW_POLYGON_FILL);

}

int CCGPainterView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDC* pDC = GetDC();
	m_pMemDC->CreateCompatibleDC(pDC);
	m_pBitmap->CreateCompatibleBitmap(pDC, m_nMaxX, m_nMaxY);
	m_pMemDC->SelectObject(m_pBitmap);

	CBrush brush;
	brush.CreateStockObject(WHITE_BRUSH);
	CRect rect(-1, -1, m_nMaxX, m_nMaxY);
	m_pMemDC->FillRect(rect, &brush);

	ReleaseDC(pDC);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CCGPainterView message handlers

// 当鼠标左键按下
void CCGPainterView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	G_iMouseState = MOUSE_LEFT_BUTTON; //设置按下的状态
	m_ptStart=point;
	m_ptOld=point;
}

// 当鼠标左键抬起
// 获取 直线的终端点，圆的半径， 多边形的顶点
void CCGPainterView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	G_iMouseState = NO_BUTTON;

	if(G_iDrawState == DRAW_NOTHING)
		return;

	//set the drawing context
	CDC* pDC=GetDC();
 	CBitmap* pOldBitmap=m_pMemDC->SelectObject(m_pBitmap);
	
	CPen pen;
	pen.CreatePen(PS_SOLID, 2, G_cLineColor);

	CPen* pOldPen=m_pMemDC->SelectObject(&pen);
	m_pMemDC->SelectObject(&pen);
	pDC->SelectObject(&pen);

	CBrush* pOldBrush=(CBrush*)m_pMemDC->SelectStockObject(NULL_BRUSH);
	m_pMemDC->SelectStockObject(NULL_BRUSH);
	pDC->SelectStockObject(NULL_BRUSH);
 
	//draw graph
	if(G_iDrawState == DRAW_LINE)//直线
		DrawLine(m_pMemDC, m_ptStart, point, G_cLineColor);
	else if(G_iDrawState == DRAW_CIRCLE)//圆
	{
		CPoint dist = point - m_ptStart;
		int radius = int(sqrt(float(dist.x*dist.x + dist.y*dist.y)) + 0.5);
		DrawCircle(m_pMemDC, m_ptStart, radius, G_cLineColor);
	}
	else if(G_iDrawState == DRAW_POLYGON || G_iDrawState == DRAW_POLYGON_FILL)//多边形
	{
		//如果有顶点，连接上一个顶点和鼠标点击的点
		if(ThePolygons[numPolygons].m_VerticeNumber>0)
			DrawLine(m_pMemDC, ThePolygons[numPolygons].m_Vertex[ThePolygons[numPolygons].m_VerticeNumber-1], point, G_cLineColor);
		
		ThePolygons[numPolygons].InsertVertex(point);//如果没有顶点，将鼠标点击的点加入第一个顶点
	}


	//retrieve the old contex
 	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	m_pMemDC->SelectObject(pOldBitmap);
	m_pMemDC->SelectObject(pOldPen);
	m_pMemDC->SelectObject(pOldBrush);
 	ReleaseDC(pDC);

}

// 鼠标移动
// 实现绘制过程中的交互效果
void CCGPainterView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(G_iDrawState == DRAW_NOTHING)
		return;
	
	//如果鼠标左键没有按下（对应画直线和圆）或者不在画多边形状态下 或者在画多边形状态下但是还没有画第一个顶点
	if((G_iMouseState != MOUSE_LEFT_BUTTON) 
		&& (G_iDrawState!=DRAW_POLYGON || ThePolygons[numPolygons].m_VerticeNumber==0)
		&& (G_iDrawState != DRAW_POLYGON_FILL || ThePolygons[numPolygons].m_VerticeNumber == 0))
		return; //返回，不做任何动作

	//set drawing context
	CDC* pDC=GetDC();
 	CBitmap* pOldBitmap = m_pMemDC->SelectObject(m_pBitmap);
	
	CPen pen;
	pen.CreatePen(PS_SOLID, 2, G_cLineColor);

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
	if(G_iDrawState == DRAW_LINE || G_iDrawState == DRAW_POLYGON || G_iDrawState == DRAW_POLYGON_FILL)
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

// 当鼠标右键抬起
// 完成多边形最后一条边的绘制和填充
void CCGPainterView::OnRButtonUp(UINT nFlags, CPoint point)
{
	int i = 0;
	if (G_iDrawState == DRAW_POLYGON || G_iDrawState == DRAW_POLYGON_FILL)
	{
		//set the drawing context
		CDC* pDC = GetDC();
		CBitmap* pOldBitmap = m_pMemDC->SelectObject(m_pBitmap);

		CPen pen;
		if (G_iDrawState == DRAW_POLYGON || G_iDrawState == DRAW_POLYGON_FILL)
			pen.CreatePen(PS_SOLID, 3, ThePolygons[numPolygons].GetLineColor());
		else
			pen.CreatePen(PS_SOLID, 2, G_cLineColor);
		CPen* pOldPen = m_pMemDC->SelectObject(&pen);
		pDC->SelectObject(&pen);

		CBrush* pOldBrush = (CBrush*)m_pMemDC->SelectStockObject(NULL_BRUSH);
		m_pMemDC->SelectStockObject(NULL_BRUSH);

		//最后一条线段，连接最后一个顶点和第一个顶点
		if (ThePolygons[numPolygons].m_VerticeNumber > 2)
		{
			DrawLine(pDC, ThePolygons[numPolygons].m_Vertex[ThePolygons[numPolygons].m_VerticeNumber - 1], ThePolygons[numPolygons].m_Vertex[0], G_cLineColor);
			DrawLine(m_pMemDC, ThePolygons[numPolygons].m_Vertex[ThePolygons[numPolygons].m_VerticeNumber - 1], ThePolygons[numPolygons].m_Vertex[0], G_cLineColor);
		}

		//如果选择的是填充多边形状态，就填充所绘制的多边形
		if (G_iDrawState == DRAW_POLYGON_FILL)
		{
			ThePolygons[numPolygons].BuildEdgeTable();
			ScanlineFill(pDC, ThePolygons[numPolygons], G_cLineColor);
			ThePolygons[numPolygons].BuildEdgeTable();//上面的边表被改动，重新建立边表
			ScanlineFill(m_pMemDC, ThePolygons[numPolygons], G_cLineColor);
		}
		G_iDrawState = DRAW_NOTHING;

		//多边形的数量+1
		numPolygons++;


		//retrieve the old contex
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		m_pMemDC->SelectObject(pOldBitmap);
		m_pMemDC->SelectObject(pOldPen);
		m_pMemDC->SelectObject(pOldBrush);
		ReleaseDC(pDC);
		Invalidate(false);

	}

	CView::OnRButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
//CCGPainterView Algorithm 

void CCGPainterView::DrawLine(CDC *pDC, CPoint ptStartPoint, CPoint ptEndPoint, COLORREF cLineColor)
{
//	pDC->MoveTo(ptStartPoint);
//	pDC->LineTo(ptEndPoint);

// DDA 算法画直线 
	//use function:
	//pDC->SetPixelV(point, cLineColor); to draw a pixel

	//求出步数
	CPoint CurrentPoint;

	float deltax = fabs(float(ptEndPoint.x - ptStartPoint.x));
	float deltay = fabs(float(ptEndPoint.y - ptStartPoint.y));


	// caculate the length(step number)
	int Length=0;

	if(deltax>=deltay) Length = deltax;
	else			   Length = deltay;

    if(Length == 0) 		return;

	int myXsign =  -1;
	int myYsign = -1;
	if(ptEndPoint.x-ptStartPoint.x >= 0)
		myXsign = 1;
 
	if(ptEndPoint.y-ptStartPoint.y >= 0)
		myYsign = 1;

	//求步长
	float stepX= deltax /  Length;
	float stepY= deltay /  Length;

	// 初始点坐标
	float x, y;
	x = 0.5  +  ptStartPoint.x  ;
    y = 0.5  +  ptStartPoint.y  ;

	//begin main loop
	
	for(int i=1;i<=Length;i++)
	{
	//坐标值取整
		CurrentPoint.x = (int)x;
		CurrentPoint.y = (int)y;
	//画点		
		pDC->SetPixelV(CurrentPoint, cLineColor);
	//迭加更新点坐标值		
		x =   x + stepX * myXsign;
		y =   y + stepY * myYsign;	
	}

	//write the Bresenham' line algorithm for drawing the line
	//use function:
	//pDC->SetPixelV(point, cLineColor); to drawing a pixel
/*	
//my realization

	int deltax = abs(ptEndPoint.x - ptStartPoint.x);
	int deltay = abs(ptEndPoint.y - ptStartPoint.y);
	
	int s1, s2;
	if(ptEndPoint.x-ptStartPoint.x >= 0)
		s1 = 1;
	else
		s1 = -1;
	if(ptEndPoint.y-ptStartPoint.y >= 0)
		s2=1;
	else
		s2 = -1;
	
	int temp, interchange;
	if(deltay > deltax)
	{
		temp = deltax;
		deltax = deltay;
		deltay = temp;
		interchange = 1;
	}
	else
		interchange = 0;
	
	int x = ptStartPoint.x;
	int y = ptStartPoint.y;
	int f = 2*deltay - deltax;
	CPoint point;
	for(int i=1; i<=deltax; i++)
	{
		point.x = x;	point.y = y;
		pDC->SetPixelV(point, cLineColor);
		if (f >= 0)
		{
			if(interchange == 1)
				x = x + s1;
			else
				y = y + s2;
			f = f - 2*deltax;
		}

		if(interchange==1)
			y = y + s2;
		else
			x = x + s1;
		f = f + 2*deltay;
	}
*/
}

void CCGPainterView::DrawCircle(CDC *pDC, CPoint ptOrigin, int iRadius, COLORREF cLineColor)
{
/* 	CPoint ptLeftTop(ptOrigin.x - iRadius, ptOrigin.y - iRadius);
 	CPoint ptRightBottom(ptOrigin.x + iRadius, ptOrigin.y + iRadius);
 	CRect circleRect(ptLeftTop, ptRightBottom);
 	pDC->Ellipse(circleRect);
*/
	//write the Midpoint circle algorithm for drawing the circle
	//use function:
	//pDC->SetPixelV(point, cLineColor); to drawing a pixel

	//the iterate equation is:
	//D0 = 5/4 - R;
	//D(i+1) = D(i) + 2*xi + 3, if D(i) < 0;  (or D(i+1) = D(i) + 2*x(i+1) + 1)
	//D(i+1) = D(i) + 2*xi -2*yi + 5, if D(i) >= 0;  (or D(i+1) = D(i) + 2*x(i+1) -2*y(i+1) + 1)
//my realization
 
	int x = 0;
	int y = iRadius;
	float D = 5/4.0f - iRadius;
	CPoint point;
	while(x<y)
	{
		//once get one point, eight points will be filled
		point.x = ptOrigin.x + x;
		point.y = ptOrigin.y + y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - x;
		point.y = ptOrigin.y + y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x + x;
		point.y = ptOrigin.y - y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - x;
		point.y = ptOrigin.y - y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x + y;
		point.y = ptOrigin.y + x;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - y;
		point.y = ptOrigin.y + x;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x + y;
		point.y = ptOrigin.y - x;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - y;
		point.y = ptOrigin.y - x;
		pDC->SetPixelV(point, cLineColor);

		//compute the next step value (D and x, y)
		if(D<0)
			D = D + 2*x + 3;
		else
		{
			D = D + 2*x - 2*y + 5; 
			y = y-1;
		}
		x = x + 1;
    }

    if(x == y)
	{
		point.x = ptOrigin.x + x;
		point.y = ptOrigin.y + y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - x;
		point.y = ptOrigin.y + y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x + x;
		point.y = ptOrigin.y - y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - x;
		point.y = ptOrigin.y - y;
		pDC->SetPixelV(point, cLineColor);
	}
 
}

void CCGPainterView::ScanlineFill(CDC* pDC, MyPolygon polygon, COLORREF fillCol) {
	int minY = polygon.GetMinY();
	int maxY = polygon.GetMaxY();

	//重置活化边表
	polygon.activeEdgeTable = nullptr;

	// 从最小 Y 坐标到最大 Y 坐标，按扫描线的顺序进行扫描
	for (int y = minY; y <= maxY; ++y) {
		// 构建当前扫描线的 AET
		polygon.SetAET(y);

		// 对当前扫描线进行填充
		Edge* currentEdge = polygon.activeEdgeTable;
		while (currentEdge != nullptr && currentEdge->next != nullptr) {
			// 遍历 AET 中的边，填充每一对边之间的像素点
			float xStart = currentEdge->x;
			float xEnd = currentEdge->next->x;

			// 从左端点开始到右端点，给每个像素点设置颜色
			for (int x = xStart; x <= xEnd; ++x) {
				// 使用 SetPixelV 设置像素点颜色
				pDC->SetPixelV(x, y, fillCol);
			}

			currentEdge = currentEdge->next->next; // 跳过两条边
		}
	}
}
