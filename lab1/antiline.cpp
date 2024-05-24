void CCGPainterView::DrawAntiLine(CDC *pDC, CPoint ptStartPoint, CPoint ptEndPoint, COLORREF cLineColor)
{
	//pDC->MoveTo(ptStartPoint);
	//pDC->LineTo(ptEndPoint);

    float accumulate;
	int i;	int dx, dy;
	int x_acc, y_acc;
	int xx, yy;
	int interchange;
	float slope;
	int x0 = ptStartPoint.x;
	int y0 = ptStartPoint.y;
	int x1 = ptEndPoint.x;
	int y1 = ptEndPoint.y;
	
	if(x0==x1)
	{
		if(y1>=y0)
			for(i=y0; i<=y1; i++)
				pDC->SetPixelV(x0, i, cLineColor);
		else
			for(i=y0; i>=y1; i--)
				pDC->SetPixelV(x0, i, cLineColor);
		
		return;
	}
	
	if(y0==y1)
	{
		if(x1>=x0)
			for(i=x0; i<=x1; i++)
				pDC->SetPixelV(i, y0, cLineColor);
		else
			for(i=x0; i>=x1; i--)
				pDC->SetPixelV(i, y0, cLineColor);
		
		return;
	}
//////////////////////////////////
	if(y1>y0)
	{
		dy=y1-y0;
		y_acc=1;
	}
	else
	{
		dy=y0-y1;
		y_acc=-1;
	}

	if(x1>x0)
	{
		dx=x1-x0;
		x_acc=1;
	}
	else
	{
		dx=x0-x1;
		x_acc=-1;
	}
//////////////////////////////////////
	if(dx==dy)
	{
		xx=x0; yy=y0;
		for(i=0; i<=dx; i++)
		{
			pDC->SetPixelV(xx, yy, cLineColor);
			xx+=x_acc;   yy+=y_acc;
		}
	}
//////////////////////////////////////
	accumulate=0;
	xx=x0; yy=y0;

	if(dx>=dy)
	{
		interchange=0;
		slope=dy/(float)dx;
	}
	else
	{
		interchange=1;
		slope=dx/(float)dy;
		int temp=dx;
		dx=dy;
		dy=temp;
	}
/////////COLORREF's order is A,B,G,R.
	COLORREF color;
	float red, green, blue;
	red=(float)(cLineColor&0x000000ff);
	green=(float)((cLineColor>>8)&0x000000ff);
	blue=(float)(cLineColor>>16);
	pDC->SetPixelV(xx, yy, cLineColor);
	BYTE red1, green1, blue1;
	//background's color;
	COLORREF bkcolor;
	BYTE bkred, bkgreen, bkblue;

	for(i=1; i<=dx; i++)
	{
		
		if(interchange==0)
		{
			xx+=x_acc;
			accumulate+=slope;
			if(accumulate>=1)
			{
				yy+=y_acc;
				accumulate-=1;
			}
			//get the bk's color.
			bkcolor = pDC->GetPixel(xx, yy);
			bkred=(BYTE)(bkcolor&0x000000ff);
			bkgreen=(BYTE)((bkcolor>>8)&0x000000ff);
			bkblue=(BYTE)(bkcolor>>16);

			color=0;
			red1=(BYTE)(red*(1-accumulate)+bkred*accumulate);
			green1=(BYTE)(green*(1-accumulate)+bkgreen*accumulate);
			blue1=(BYTE)(blue*(1-accumulate)+bkblue*accumulate);
			color=RGB(red1, green1, blue1);
		//	color=color|(int)(red*(1-accumulate)+255*accumulate);
		//	color=(color<<8)|(int)(green*(1-accumulate)+255*accumulate);
		//	color=(color<<8)|(int)(blue*(1-accumulate)+255*accumulate);
			pDC->SetPixelV(xx, yy, color);

			//get the bk's color.
			bkcolor = pDC->GetPixel(xx, yy+y_acc);
			bkred=(BYTE)(bkcolor&0x000000ff);
			bkgreen=(BYTE)((bkcolor>>8)&0x000000ff);
			bkblue=(BYTE)(bkcolor>>16);

			color=0;
			red1=(BYTE)(red*accumulate+bkred*(1-accumulate));
			green1=(BYTE)(green*accumulate+bkgreen*(1-accumulate));
			blue1=(BYTE)(blue*accumulate+bkblue*(1-accumulate));
			color=RGB(red1, green1, blue1);
	//		color=color|(int)(red*accumulate+255*(1-accumulate));
	//		color=(color<<8)|(int)(green*accumulate+255*(1-accumulate));
	//		color=(color<<8)|(int)(blue*accumulate+255*(1-accumulate));
			pDC->SetPixelV(xx, yy+y_acc, color);
		}
		else
		{
			yy+=y_acc;
			accumulate+=slope;
			if(accumulate>=1)
			{
				xx+=x_acc;
				accumulate-=1;
			}
			
			//get the bk's color.
			bkcolor = pDC->GetPixel(xx, yy);
			bkred=(BYTE)(bkcolor&0x000000ff);
			bkgreen=(BYTE)((bkcolor>>8)&0x000000ff);
			bkblue=(BYTE)(bkcolor>>16);
			
			color=0;
//			color=color|(int)(red*(1-accumulate));
//			color=(color<<8)|(int)(green*(1-accumulate));
//			color=(color<<8)|(int)(blue*(1-accumulate));

			color=color|(int)(red*(1-accumulate)+bkred*accumulate);
			color=color|((int)(green*(1-accumulate)+bkgreen*accumulate)<<8);
			color=color|((int)(blue*(1-accumulate)+bkblue*accumulate)<<16);
			pDC->SetPixelV(xx, yy, color);
		
			//get the bk's color.
			bkcolor = pDC->GetPixel(xx+x_acc, yy);
			bkred=(BYTE)(bkcolor&0x000000ff);
			bkgreen=(BYTE)((bkcolor>>8)&0x000000ff);
			bkblue=(BYTE)(bkcolor>>16);

			color=0;
//			color=color|(int)(red*accumulate);
//			color=(color<<8)|(int)(green*accumulate);
//			color=(color<<8)|(int)(blue*accumulate);

			color=color|(int)(red*accumulate+bkred*(1-accumulate));
			color=color|((int)(green*accumulate+bkgreen*(1-accumulate))<<8);
			color=color|((int)(blue*accumulate+bkblue*(1-accumulate))<<16);
			pDC->SetPixelV(xx+x_acc, yy, color);
		}
	}

}