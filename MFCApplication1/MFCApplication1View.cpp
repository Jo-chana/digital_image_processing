
// MFCApplication1View.cpp : implementation of the CMFCApplication1View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MFCApplication1.h"
#endif

#include "MFCApplication1Doc.h"
#include "MFCApplication1View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma once
#include "Jpegfile.h"
#define _USE_MATH_DEFINES
#include <cmath>

// CMFCApplication1View

IMPLEMENT_DYNCREATE(CMFCApplication1View, CView)

BEGIN_MESSAGE_MAP(CMFCApplication1View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_IMAGE_BMP, &CMFCApplication1View::OnImageBmp)
	ON_COMMAND(ID_IMAGE_JEPG, &CMFCApplication1View::OnImageJepg)
	ON_COMMAND(ID_RGBTOHSI_CHANGE, &CMFCApplication1View::OnRgbtohsiChange)
	ON_COMMAND(ID_POINTPROCESSING_CONTRASTSTRETCHING, &CMFCApplication1View::OnPointprocessingContraststretching)
	ON_COMMAND(ID_POINTPROCESSING_SATURATIONINJECTING, &CMFCApplication1View::OnPointprocessingSaturationinjecting)
	ON_COMMAND(ID_POINTPROCESSING_HISTOGRAM, &CMFCApplication1View::OnPointprocessingHistogram)
	ON_COMMAND(ID_HISTOGRAM_COLOR, &CMFCApplication1View::OnHistogramColor)
	ON_COMMAND(ID_HISTOGRAM_INTENSITY, &CMFCApplication1View::OnHistogramIntensity)
	ON_COMMAND(ID_RGB2HSI_CHANGE, &CMFCApplication1View::OnRgb2hsiChange)
	ON_COMMAND(ID_RGBTOHSI_HSI2RGB, &CMFCApplication1View::OnRgbtohsiHsi2rgb)
	ON_COMMAND(ID_POINTPROCESSING_FACEDETECTION, &CMFCApplication1View::OnPointprocessingFacedetection)
	ON_COMMAND(ID_HISTOGRAM_EQUALIZATION, &CMFCApplication1View::OnHistogramEqualization)
	ON_COMMAND(ID_BMP_BLACKWHITE, &CMFCApplication1View::OnBmpBlackwhite)
	ON_COMMAND(ID_BMP_COLOROPTION, &CMFCApplication1View::OnBmpColoroption)
	ON_COMMAND(ID_HISTOGRAM_COLOREQUALIZATION, &CMFCApplication1View::OnHistogramColorequalization)
	ON_COMMAND(ID_BASICGRAYLEVEL_NEGATIVE, &CMFCApplication1View::OnBasicgraylevelNegative)
	ON_COMMAND(ID_BASICGRAYLEVEL_LOG, &CMFCApplication1View::OnBasicgraylevelLog)
	ON_COMMAND(ID_BASICGRAYLEVEL_POWER, &CMFCApplication1View::OnBasicgraylevelPower)
	ON_COMMAND(ID_BASICGRAYLEVEL_THRESHOLD, &CMFCApplication1View::OnBasicgraylevelThreshold)
	ON_COMMAND(ID_POWER_GAMMA0, &CMFCApplication1View::OnPowerGamma0)
	ON_COMMAND(ID_POWER_GAMMA1, &CMFCApplication1View::OnPowerGamma1)
	ON_COMMAND(ID_POWER_GAMMA2, &CMFCApplication1View::OnPowerGamma2)
	ON_COMMAND(ID_BASICGRAYLEVEL_SOLARIZING, &CMFCApplication1View::OnBasicgraylevelSolarizing)
	ON_COMMAND(ID_POWER_GAMMA3, &CMFCApplication1View::OnPowerGamma3)
	ON_COMMAND(ID_POWER_GAMMA4, &CMFCApplication1View::OnPowerGamma4)
END_MESSAGE_MAP()



// CMFCApplication1View construction/destruction
RGBQUAD** rgbBuffer; //-> 이미지를 저장할 변수
float** rgb_red; // 변환된 이미지 R 수치
float** rgb_green; // 변환된 이미지 G 수치
float** rgb_blue; // 변환된 이미지 B 수치
int imgHeight; //-> 이미지 높이 정보
int imgWidth; //-> 이미지 너비 정보
BITMAPINFOHEADER bmpInfo; //-> BMP 정보 헤더
BITMAPFILEHEADER bmpHeader;// -> BMP 파일 헤더	
						   /*동적 할당하는 포인터 배열은 반드시 '생성자'와 '소멸자' 에
						   null 로 초기화 및 해제를 수행해야 한다.*/

float** hueBuffer; //이미지 색조 정보
float** satuBuffer; //이미지 채도 정보
float** intenBuffer; //이미지 밝기 정보
int viewType = 0; // 화면 출력 유형 정보

int* histogram; // 히스토그램 수치 정보
bool is_color = FALSE; //컬러사진 여부 정보

float** intensity; // 흑백 이미지 편의용 

CMFCApplication1View::CMFCApplication1View()
{
	// TODO: add construction code here

}

CMFCApplication1View::~CMFCApplication1View()
{
}

BOOL CMFCApplication1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMFCApplication1View drawing

void CMFCApplication1View::OnDraw(CDC* pDC)
{
	CMFCApplication1Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);//기존의 것을 그대로 사용
	if (!pDoc)
		return;

	if (rgbBuffer != NULL) { //이미지 들어있는 경우 출력
		for (int i = 0;i < imgHeight;i++) { //이미지 사이즈 만큼 돌면서 한 픽셀씩 출력
			for (int j = 0;j < imgWidth;j++) {//rgbBuffer를 출력했던 2중 for문
				POINT p; //출력할 픽셀의 위치 지정
				p.x = j;
				p.y = i;
				pDC->SetPixel(p, RGB(rgbBuffer[i][j].rgbRed, rgbBuffer[i][j].rgbGreen, rgbBuffer[i][j].rgbBlue));
				//픽셀 위치 p에 RGB 값을 입력
				if (viewType == 2) {
					p.x = j + imgWidth + 10;
					p.y = i;
					pDC->SetPixel(p, RGB(hueBuffer[i][j], hueBuffer[i][j], hueBuffer[i][j]));

					p.x = j + imgWidth + 10;
					p.y = i + imgHeight + 10;
					pDC->SetPixel(p, RGB(satuBuffer[i][j], satuBuffer[i][j], satuBuffer[i][j]));

					p.x = j;
					p.y = i + imgHeight + 10;
					pDC->SetPixel(p, RGB(intenBuffer[i][j], intenBuffer[i][j], intenBuffer[i][j]));
				}

				if (viewType == 3) {
					p.x = j + imgWidth + 10;
					p.y = i;
					pDC->SetPixel(p, RGB(rgb_red[i][j], rgb_green[i][j], rgb_blue[i][j]));

				}
				if (viewType == 4) {
					p.x = j + imgWidth + 10;
					p.y = i;
					pDC->SetPixel(p, RGB(intensity[i][j], intensity[i][j], intensity[i][j]));
				}

			}
		}		
		
	}
}


// CMFCApplication1View printing

BOOL CMFCApplication1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMFCApplication1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMFCApplication1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CMFCApplication1View diagnostics

#ifdef _DEBUG
void CMFCApplication1View::AssertValid() const
{
	CView::AssertValid();
}

void CMFCApplication1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCApplication1Doc* CMFCApplication1View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication1Doc)));
	return (CMFCApplication1Doc*)m_pDocument;
}
#endif //_DEBUG


// CMFCApplication1View message handlers


void CMFCApplication1View::OnImageBmp()
{
	
	//1. 파일 다이얼로그로부터 BMP 파일 입력
	CFileDialog dlg(TRUE, ".bmp", NULL, NULL, "Bitmap File(*.bmp)|*.bmp||");
	if (IDOK != dlg.DoModal())
		return;
	CString filename = dlg.GetPathName();
	if(rgbBuffer != NULL){//이미 할당된 경우 메모리 헤제
		for (int i = 0;i < imgHeight;i++)
			delete [] rgbBuffer[i];
		delete [] rgbBuffer;
	}

	//2. 파일을 오픈하여 영상 정보 획득
	CFile file;
	file.Open(filename, CFile::modeRead);
	file.Read(&bmpHeader, sizeof(BITMAPFILEHEADER));
	file.Read(&bmpInfo, sizeof(BITMAPINFOHEADER));
	imgWidth = bmpInfo.biWidth;
	imgHeight = bmpInfo.biHeight;

	//3. 이미지를 저장할 버퍼 할당(2차원 배열) [이미지 높이*이미지 너비 만큼 할당]
	rgbBuffer = new RGBQUAD*[imgHeight];
	for (int i = 0;i < imgHeight;i++)
		rgbBuffer[i] = new RGBQUAD[imgWidth];

	//4. 이미지의 너비가 4의 배수인지 체크
	// BMP 조건 가로는 4byte 이어야 한다.
	// 한 픽셀이 3바이트(R,G,B)이므로, 가로(m_width)*3 이 4의 배수인지 아닌지 알아야 함
	// b4byte : 4byte 의 배수인지 아닌지 안다.
	// upbyte : 4byte 에 모자라는 바이트다.
	bool b4byte = false;
	int upbyte = 0;
	if ((imgWidth * 3) % 4 == 0) {
		// 4의 배수로 떨어지는 경우
		b4byte = true;
		upbyte = 0;
	}
	else {
		// 4의 배수로 떨어지지 않는 경우
		b4byte = false;
		upbyte = 4 - (imgWidth * 3) % 4;
	}

	//5. 픽셀 데이터를 파일로부터 읽어옴
	BYTE data[3];
	for (int i = 0; i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			file.Read(&data, 3);
			//이미지 거꾸로 저장되어 있으므로 거꾸로 읽어옴
			rgbBuffer[imgHeight - i - 1][j].rgbBlue = data[0];
			rgbBuffer[imgHeight - i - 1][j].rgbGreen = data[1];
			rgbBuffer[imgHeight - i - 1][j].rgbRed = data[2];

		}
		if (b4byte == false)
		{
			//가로가 4byte 배수가 아니면 쓰레기 값을 읽는다
			file.Read(&data, upbyte);
		}
	}
	file.Close(); //파일 닫기
	viewType = 0;
	Invalidate(TRUE); // 화면 갱신

}

void CMFCApplication1View::OnBmpColoroption()
{
	is_color = TRUE;
	OnImageBmp();
}

void CMFCApplication1View::OnBmpBlackwhite()
{ 
	is_color = FALSE;
	//1. 파일 다이얼로그로부터 BMP 파일 입력
	CFileDialog dlg(TRUE, ".bmp", NULL, NULL, "Bitmap File(*.bmp)|*.bmp||");
	if (IDOK != dlg.DoModal())
		return;
	CString filename = dlg.GetPathName();
	if (rgbBuffer != NULL) {//이미 할당된 경우 메모리 헤제
		for (int i = 0;i < imgHeight;i++)
			delete[] rgbBuffer[i];
		delete[] rgbBuffer;
	}

	//2. 파일을 오픈하여 영상 정보 획득
	CFile file;
	file.Open(filename, CFile::modeRead);
	file.Read(&bmpHeader, sizeof(BITMAPFILEHEADER));
	file.Read(&bmpInfo, sizeof(BITMAPINFOHEADER));
	imgWidth = bmpInfo.biWidth;
	imgHeight = bmpInfo.biHeight;

	//3. 이미지를 저장할 버퍼 할당(2차원 배열) [이미지 높이*이미지 너비 만큼 할당]
	rgbBuffer = new RGBQUAD*[imgHeight];
	for (int i = 0;i < imgHeight;i++)
		rgbBuffer[i] = new RGBQUAD[imgWidth];

	//4. 이미지의 너비가 4의 배수인지 체크
	// BMP 조건 가로는 4byte 이어야 한다.
	// 한 픽셀이 1바이트이므로, 가로(m_width) 이 4의 배수인지 아닌지 알아야 함
	// b4byte : 4byte 의 배수인지 아닌지 안다.
	// upbyte : 4byte 에 모자라는 바이트다.
	bool b4byte = false;
	int upbyte = 0;
	if (imgWidth % 4 == 0) {
		// 4의 배수로 떨어지는 경우
		b4byte = true;
		upbyte = 0;
	}
	else {
		// 4의 배수로 떨어지지 않는 경우
		b4byte = false;
		upbyte = 4 - (imgWidth % 4);
	}

	//5. 픽셀 데이터를 파일로부터 읽어옴
	BYTE data[1];
	for (int i = 0; i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			file.Read(&data, 1);
			//이미지 거꾸로 저장되어 있으므로 거꾸로 읽어옴
			rgbBuffer[imgHeight - i - 1][j].rgbBlue = data[0];
			rgbBuffer[imgHeight - i - 1][j].rgbGreen = data[0];
			rgbBuffer[imgHeight - i - 1][j].rgbRed = data[0];

		}
		if (b4byte == false)
		{
			//가로가 4byte 배수가 아니면 쓰레기 값을 읽는다
			file.Read(&data, upbyte);
		}
	}
	file.Close(); //파일 닫기
	viewType = 0;
	Invalidate(TRUE); // 화면 갱신
}

BYTE* LoadJpegFromOpenFile(FILE *fp, BITMAPINFOHEADER *pbh, UINT *pWidth, UINT *pHeight) {
	//파일로부터 JPG 정보 획득
	if (pWidth == NULL || pHeight == NULL) return NULL;
	BYTE *tmp = JpegFile::OpenFileToRGB(fp , pWidth, pHeight);
	if (!tmp) return NULL;
	JpegFile::BGRFromRGB(tmp, *pWidth, *pHeight);
	UINT dw;
	BYTE *pbuf = JpegFile::MakeDwordAlignedBuf(tmp, *pWidth, *pHeight, &dw);
	delete[] tmp;
	if (!pbuf) return NULL;
	JpegFile::VertFlipBuf(pbuf, dw, *pHeight);
	pbh->biSize = sizeof(BITMAPINFOHEADER);
	pbh->biWidth = *pWidth;
	pbh->biHeight = *pHeight;
	pbh->biPlanes = 1;
	pbh->biBitCount = 24;
	pbh->biCompression = BI_RGB;
	pbh->biSizeImage = 0;
	pbh->biXPelsPerMeter = 0;
	pbh->biYPelsPerMeter = 0;
	pbh->biClrUsed = 0;
	pbh->biClrImportant = 0;
	return pbuf;
}

void CMFCApplication1View::OnImageJepg()
{
	is_color = TRUE;
	CFileDialog dlg(TRUE, ".jpg", NULL, NULL, "Jpeg File(*.jpg)|*jpg||");
	if (IDOK != dlg.DoModal()) return;
	CString filename = dlg.GetPathName();

	//BMP 로드와 마찬가지로 rgbBuffer가 NULL이 아닌 경우 메모리 헤제 코드 추가하기
	if (rgbBuffer != NULL) {//이미 할당된 경우 메모리 헤제
		for (int i = 0;i < imgHeight;i++)
			delete[] rgbBuffer[i];
		delete[] rgbBuffer;
	}

	FILE *fp = fopen(filename, "rb");
	BITMAPINFOHEADER pbh;
	UINT w, h;
	BYTE *pbuf = LoadJpegFromOpenFile(fp, &pbh, &w, &h);
	imgWidth = (int)w;
	imgHeight = (int)h;
	//BMP 로드와 마찬가지로 rgbBuffer를 이미지 사이즈 만큼 메모리 할당 코드 추가
	rgbBuffer = new RGBQUAD*[imgHeight];
	for (int i = 0;i < imgHeight;i++)
		rgbBuffer[i] = new RGBQUAD[imgWidth];

	int dw = WIDTHBYTES(imgWidth * 24);
	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			rgbBuffer[imgHeight - i - 1][j].rgbBlue = pbuf[dw * i + j * 3 + 0];
			rgbBuffer[imgHeight - i - 1][j].rgbGreen = pbuf[dw * i + j * 3 + 1];
			rgbBuffer[imgHeight - i - 1][j].rgbRed = pbuf[dw * i + j * 3 + 2];
					}
	}
	delete[] pbuf;
	fclose(fp);
	viewType = 0;
	Invalidate(TRUE);
}


void CMFCApplication1View::OnRgbtohsiChange() // 함수 재사용 목적으로 작성
{   
	//2. rgbBuffer 에 이미지가 들어있는 지 여부 확인
	if (rgbBuffer == NULL)
		OnImageBmp(); //rgbBuffer 에 데이터가 없는 경우, 로드 함수를 호출하여 이미지 획득

	hueBuffer = new float*[imgHeight];
	satuBuffer = new float*[imgHeight];
	intenBuffer = new float*[imgHeight];

	for (int i = 0;i < imgHeight;i++) {
		hueBuffer[i] = new float[imgWidth];
		satuBuffer[i] = new float[imgWidth];
		intenBuffer[i] = new float[imgWidth];
	}

	//3. RGB to HSI 값 변환
	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			float r = rgbBuffer[i][j].rgbRed;
			float g = rgbBuffer[i][j].rgbGreen;
			float b = rgbBuffer[i][j].rgbBlue;
			intenBuffer[i][j] = (r + g + b) / (float)(3 * 255);//intensity
			float total = r + g + b;
			r = r / total; g = g / total; b = b / total;
			satuBuffer[i][j] = 1 - 3 * (r > g ? (g > b ? b : g) : (r > b ? b : r));
			if (r == g&&g == b) {
				hueBuffer[i][j] = 0;satuBuffer[i][j] = 0;
			}
			else {
				total = (0.5*(r - g + r - b) / sqrt((r - g)*(r - g) + (r - b)*(g - b)));
				hueBuffer[i][j] = acos((double)total);
				if (b > g) {
					hueBuffer[i][j] = 2*M_PI - hueBuffer[i][j];
				}
			} // 함수 재사용을 위해 정규화 와 출력은 함수적으로 분리하였음.
		}
	}
	
}

void CMFCApplication1View::OnRgb2hsiChange()
{
	OnRgbtohsiChange();

	//4.출력 값 범위 정규화: 출력 시, 값의 범위를 [0,255]로 맞춤
	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			hueBuffer[i][j] = hueBuffer[i][j] * 255 / (3.14 * 2);
			satuBuffer[i][j] = satuBuffer[i][j] * 255;
			intenBuffer[i][j] = intenBuffer[i][j] * 255;
		}
	}

	//5.출력
	viewType = 2;
	Invalidate(FALSE);

}

void CMFCApplication1View::OnRgbtohsiHsi2rgb() // 함수 재사용 목적으로 작성
{
	rgb_red = new float*[imgHeight];
	rgb_green = new float*[imgHeight];
	rgb_blue = new float*[imgHeight];
	for (int i = 0;i < imgHeight;i++) {
		rgb_red[i] = new float[imgWidth];
		rgb_green[i] = new float[imgWidth];
		rgb_blue[i] = new float[imgWidth];
	}

	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			float R = 0;
			float G = 0;
			float B = 0;
			float r = 0;
			float g = 0;
			float b = 0;
			float S = satuBuffer[i][j];
			float H = hueBuffer[i][j];
			float I = intenBuffer[i][j];

			if (I == 0) {
				r = 0;
				g = 0;
				b = 0;
			}
			else if (S == 0) {
				r = 1. / 3.;
				g = 1. / 3.;
				b = 1. / 3.;
			}
			else {

				if (H < 2. * M_PI / 3.) { // 0 <= H <= 2/3Pi
					b = (1 - S) / 3.;
					r = (1 + (S*cos((double)H) / cos((double)M_PI / 3 - H))) / 3.;
					g = 1 - (r + b);

				}
				else if (H < 4 * M_PI / 3) { // 2/3Pi < H <= 4/3Pi
					H = H - 2. * M_PI / 3.;
					r = (1 - S) / 3.;
					g = (1 + (S*cos((double)H) / cos((double)M_PI / 3 - H))) / 3.;
					b = 1 - (r + g);

				}
				else { // 4/3Pi < H =< 2Pi
					H = H - 4. * M_PI / 3.;
					g = (1 - S) / 3.;
					b = (1 + (S*cos((double)H) / cos((double)M_PI / 3 - H))) / 3.;
					r = 1 - (g + b);

				}
			}

			R = min(255, 255 * 3 * r * I); //255 * 3 * r*I;
			G = min(255, 255 * 3 * g * I); //255 * 3 * g*I;
			B = min(255, 255 * 3 * b * I); //255 * 3 * b*I;

			//원본 이미지 데이터를 보존하기 위해 새로운 변수에 저장
			rgb_red[i][j] = R;
			rgb_green[i][j] = G;
			rgb_blue[i][j] = B;
		}
	}
}

void CMFCApplication1View::OnPointprocessingContraststretching()
{
	OnRgbtohsiChange();

	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0; j < imgWidth;j++) {
			intenBuffer[i][j] = intenBuffer[i][j] * 1.5; // 밝기 1.5배
		}
	}


	//3. 변환된 HSI 컬러를 RGB 컬러스페이스로 변환
	OnRgbtohsiHsi2rgb(); // 해당 함수 주석 참조
	
	
	//4. 출력
	viewType = 3;
	Invalidate(FALSE);
}


void CMFCApplication1View::OnPointprocessingSaturationinjecting()
{
	OnRgbtohsiChange();
	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			satuBuffer[i][j] = min(1, satuBuffer[i][j] * 1.5); // 채도 1.5배 개선
		}
	}

	OnRgbtohsiHsi2rgb();
	//4. 출력
	viewType = 3;
	Invalidate(FALSE);
}


void CMFCApplication1View::OnPointprocessingHistogram()
{
	
}


void CMFCApplication1View::OnHistogramColor()
{
	OnRgbtohsiChange(); // HSI 정보 획득

	if (histogram==NULL)
		histogram = new int[360]; // Hue 360도 스케일
	for (int i = 0;i < 360;i++) {
		histogram[i] = 0;
	}

	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			int color = (int)(hueBuffer[i][j] * 180 / M_PI);
			histogram[color]++;
		}
	}
	int max = 0;
	for (int i = 0; i < 360;i++) {
		if( histogram[i] > max)
			max = histogram[i];
	}
	for (int i = 0; i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			int color = (int)(hueBuffer[i][j] * 180 / M_PI);
			hueBuffer[i][j] = (float)histogram[color] * 255 / max;
		}
	}
	viewType = 2;
	Invalidate(FALSE);
}


void CMFCApplication1View::OnHistogramIntensity()
{
	OnRgbtohsiChange(); // HSI 정보 획득

	if(histogram==NULL)
		histogram = new int[255]; 
	for (int i = 0;i < 256;i++) {
		histogram[i] = 0;
	}

	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			int inten = (int)(intenBuffer[i][j] * 255);
			histogram[inten]++;
		}
	}
	int max = 0;
	for (int i = 0; i < 256;i++) {
		if (histogram[i] > max)
			max = histogram[i];
	}
	for (int i = 0; i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			int inten = (int)(intenBuffer[i][j] * 255);
			hueBuffer[i][j] = (float)histogram[inten] * 255 / max;
		}
	}
	viewType = 2;
	Invalidate(FALSE);
}








void CMFCApplication1View::OnPointprocessingFacedetection()
{
	/*
	피부색 조건
	Saturation >= 0.2
	Hue < 50도
	Intensity/Saturation 0.5 ~ 3.0 -> deleted
	*/

	OnRgbtohsiChange();
	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			float S = satuBuffer[i][j];
			float H = hueBuffer[i][j];
			float I = intenBuffer[i][j];

			if (S < 0.1 || S > 0.6)
				intenBuffer[i][j] = 0;
			
			if (H > 0.88 || H < 0.1)
				intenBuffer[i][j] = 0;

			//if (I / S < 0.5 || I / S > 3.0)
				//intenBuffer[i][j] = 0;
		}
	}
	OnRgbtohsiHsi2rgb();
	viewType = 3;
	Invalidate(FALSE);

}


void CMFCApplication1View::OnHistogramEqualization()
{
	// Intensity 값을 기준으로 histogram equalize
	OnRgbtohsiChange();

	// 히스토그램 초기화
	if (histogram == NULL)
		histogram = new int[255];
	for (int i = 0;i < 256;i++) {
		histogram[i] = 0;
	}

	float max_pixel = 0;
	float total_pixel = 0;

	// 밝기 히스토그램 
	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			int inten = (int)(intenBuffer[i][j] * 255.);
			histogram[inten]++;
			total_pixel++;
			if (inten > max_pixel)
				max_pixel = inten;
		}
	}

	// 히스토그램 정규화
	float sum = 0.;
	float* sum_hist = new float[255];
	for (int i = 0;i < 256;i++) {
		sum += histogram[i];
		sum_hist[i] = sum*max_pixel/total_pixel + 0.5;
	}

	//컬러사진일때
	if (is_color) {
		//변환
		for (int i = 0;i < imgHeight;i++) {
			for (int j = 0;j < imgWidth;j++) {
				intenBuffer[i][j] = (float)sum_hist[(int)(intenBuffer[i][j] * 255)] / 255.;
			}
		}

		OnRgbtohsiHsi2rgb();

		viewType = 3;
		Invalidate(FALSE);
	}
	
	//흑백사진일때
	else {

		intensity = new float*[imgHeight];
		for (int i = 0;i < imgHeight;i++) {
			intensity[i] = new float[imgWidth];
		}
		// 변환
		for (int i = 0;i < imgHeight;i++) {
			for (int j = 0;j < imgWidth;j++) {
				intensity[i][j] = sum_hist[(int)(intenBuffer[i][j] * 255.)];
			}
		}

		// 출력
		viewType = 4;
		Invalidate(FALSE);
	}

}


void CMFCApplication1View::OnHistogramColorequalization()
{
	if (rgbBuffer == NULL)
		OnImageBmp();

	int* hred = new int[255];
	int* hgreen = new int[255];
	int* hblue = new int[255];

	for (int i = 0;i < 256;i++) {
		hred[i] = 0;hgreen[i] = 0;hblue[i] = 0;
	}

	float total_pixel = 0;
	float max_red = 0;
	float max_green = 0;
	float max_blue = 0;

	// RGB 히스토그램 
	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			total_pixel++;
			hred[rgbBuffer[i][j].rgbRed]++;
			hgreen[rgbBuffer[i][j].rgbGreen]++;
			hblue[rgbBuffer[i][j].rgbBlue]++;
			max_red = (rgbBuffer[i][j].rgbRed > max_red ? rgbBuffer[i][j].rgbRed : max_red);
			max_green = (rgbBuffer[i][j].rgbGreen > max_green ? rgbBuffer[i][j].rgbGreen : max_green);
			max_blue = (rgbBuffer[i][j].rgbBlue > max_blue ? rgbBuffer[i][j].rgbBlue : max_blue);
		}
	}

	// 히스토그램 정규화
	int sumred = 0;
	int sumgreen = 0;
	int sumblue = 0;

	float* sum_red = new float[255];
	float* sum_green = new float[255];
	float* sum_blue = new float[255];

	for (int i = 0;i < 256;i++) {
		sumred += hred[i];
		sumgreen += hgreen[i];
		sumblue += hblue[i];
		sum_red[i] = (float)sumred*max_red / total_pixel + 0.5;
		sum_green[i] = (float)sumgreen*max_green / total_pixel + 0.5;
		sum_blue[i] = (float)sumblue*max_blue / total_pixel + 0.5;
	}

	rgb_red = new float*[imgHeight];
	rgb_green = new float*[imgHeight];
	rgb_blue = new float*[imgHeight];
	for (int i = 0;i < imgHeight;i++) {
		rgb_red[i] = new float[imgWidth];
		rgb_green[i] = new float[imgWidth];
		rgb_blue[i] = new float[imgWidth];
	}
	

	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			rgb_red[i][j] = sum_red[rgbBuffer[i][j].rgbRed];
			rgb_green[i][j] =sum_green[rgbBuffer[i][j].rgbGreen];
			rgb_blue[i][j] = sum_blue[rgbBuffer[i][j].rgbBlue];
		}
	}

	viewType = 3;
	Invalidate(FALSE);
}






void CMFCApplication1View::OnBasicgraylevelNegative()
{
	if (rgbBuffer == NULL)
		OnBmpBlackwhite();
	
	rgb_red = new float*[imgHeight];
	rgb_green = new float*[imgHeight];
	rgb_blue = new float*[imgHeight];
	for (int i = 0;i < imgHeight;i++){
		rgb_red[i] = new float[imgWidth];
		rgb_green[i] = new float[imgWidth];
		rgb_blue[i] = new float[imgWidth];
	}

	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			rgb_red[i][j] = 255 - rgbBuffer[i][j].rgbRed;
			rgb_green[i][j] = 255 - rgbBuffer[i][j].rgbGreen;
			rgb_blue[i][j] = 255 - rgbBuffer[i][j].rgbBlue;
		}
	}
	viewType = 3;
	Invalidate(FALSE);

}


void CMFCApplication1View::OnBasicgraylevelLog()
{
	
}

float gamma;

void CMFCApplication1View::OnBasicgraylevelPower()
{
	if (is_color)
		return;
	if (rgbBuffer == NULL)
		OnBmpBlackwhite();

	intensity = new float*[imgHeight];
	for (int i = 0;i < imgHeight;i++) {
		intensity[i] = new float[imgWidth];
	}

	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			float inten = rgbBuffer[i][j].rgbRed;
			inten = inten / 255.;
			inten = pow(inten, (float)gamma);
			inten = inten * 255.;
			intensity[i][j] = inten;
		}
	}
	viewType = 4;
	Invalidate(FALSE);

}


void CMFCApplication1View::OnBasicgraylevelThreshold()
{
	OnHistogramEqualization();

	if (is_color)
		intensity = intenBuffer;

	//평균값 계산
	float sum = 0; // 총 합
	float total = 0; // 픽셀 수
	float med = 0; // 평균
	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			sum += intensity[i][j];
			total++;
		}
	}
	med = sum / total;

	// 이진화
	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			intensity[i][j] = intensity[i][j] > med ? 255 : 0;
		}
	}

	viewType = 4;
	Invalidate(FALSE);
}


void CMFCApplication1View::OnPowerGamma0()
{
	gamma = 0.3;
	OnBasicgraylevelPower();
}


void CMFCApplication1View::OnPowerGamma1()
{
	gamma = 0.5;
	OnBasicgraylevelPower();
}


void CMFCApplication1View::OnPowerGamma2()
{
	gamma = 0.7;
	OnBasicgraylevelPower();
}

void CMFCApplication1View::OnPowerGamma3()
{
	gamma = 1.3;
	OnBasicgraylevelPower();
}


void CMFCApplication1View::OnPowerGamma4()
{
	gamma = 1.5;
	OnBasicgraylevelPower();
}

void CMFCApplication1View::OnBasicgraylevelSolarizing()
{
	OnHistogramEqualization();

	if (is_color)
		intensity = intenBuffer;

	// 데이터 정규화
	for (int i = 0;i < imgHeight;i++) {
		for (int j = 0;j < imgWidth;j++) {
			intensity[i][j] = 3.14*intensity[i][j] / 255.;
			intensity[i][j] = sin(intensity[i][j])*255.;
		}
	}
	if (is_color) {
		intenBuffer = intensity;
		OnRgbtohsiHsi2rgb();
		viewType = 3;
		Invalidate(FALSE);
	}
	else {
		viewType = 4;
		Invalidate(FALSE);
	}
}



