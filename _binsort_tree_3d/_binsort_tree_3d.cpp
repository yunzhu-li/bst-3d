// **
//

#include "stdafx.h"
#include "_binsort_tree_3d.h"

//Windows Handles
HINSTANCE _hInst;
HWND _hMainWnd;
int _glutWindow;

//BST structs and variables 
struct _nodePoint{
	float _x;
	float _y;
	bstNode *_node;
};
_nodePoint *_nodePoints;
bstNode *_pT;
unsigned int _curNumOfNodes;
//BST structs end

// Other global variables
char *_textMsg;
GLuint _glBFListBegin;

//Input
BOOL _bInput;
//Kind of message to input
unsigned char _cInputFlag;	
//Show prompt text
unsigned char _bPromptTextOn; 
//Set _bPromptTextOn to 0 after XX frames rendered
unsigned int _iPromptRemainingFrames; 
char *_lpszInputBoxText;
char *_lpszPromptText[5];


//FPS Calculation
double _framesPerSec;
unsigned int _iFramesRendered;
unsigned long _lastTickCount;

//GL Variables
GLuint _texIDs[100];

//Camera Variables
GLfloat _camX, _camY, _camZ;
UINT8 _camGo_Forward, _camGo_Backward, _camGo_Leftward, _camGo_Rightward, _camGo_Upward, _camGo_Downward;
GLfloat _normX, _normY, _normZ;
GLfloat _normVerticalDeg, _normHorizontalDeg;
UINT8 _normIncDegUp, _normIncDegDown, _normIncDegLeft, _normIncDegRight;

//Mouse Variables
BOOL _lockMouse;
POINT _curPos;
GLuint _lastX, _lastY;
GLuint _centerX, _centerY;






//Mathematical Funcs
double _m_sin_deg(double _deg);
double _m_cos_deg(double _deg);

//Callback Functions
void __rFunc();
void __idleFunc();
void __timerFunc(int value);
void _specFunc(int key, int x, int y);
void _specUpFunc(int key, int x, int y);
void _keyFunc(unsigned char key, int x, int y);
void _keyUpFunc(unsigned char key, int x, int y);
void _motionFunc(int x, int y);
void _passMotionFunc(int x, int y);
void _mouseFunc(int button, int state, int x, int y);

//GL Functions
void __initScene();
void __loadTexture(wchar_t * _fileName, unsigned char _texId, unsigned int _size, unsigned char _alpha);

//Font Functions
void _fat2DStrInit(char *_fontName, char _size);
void _fat2DString(GLfloat _x, GLfloat _y, char * _str);
void _fat3DString(GLfloat _x, GLfloat _y, GLfloat _z, char * _str);

//Shape Functions
void _draw2DObjs();
void _fatHouse(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _size);
void _fat2DBox(GLfloat _x, GLfloat _y, GLfloat _width, GLfloat _height);
void _fatCuboid(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _length, GLfloat _width, GLfloat _height);

//BST Functions
void _bInitBST(char *_seq);
void _bTraverse_CallBack(bstNode *_node);
void _bGenBSTC_CallBack(bstNode *_node, unsigned int _curNum, void *_param);
void _bGenBSTCoords(bstNode *T, float _widthCuboid, float _widthSpace, float _heightSpace, _nodePoint *_points);




int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//Fake Arguments
	int a = 0;
	char *argv = new char[10];
	sprintf_s(argv, 10, "a");

	//MSG Variable
	_textMsg = new char[512];
	memset(_textMsg, 0, 512);

	//Get HINSTANCE
	_hInst = hInstance;

	//Init BST
	_bInitBST("100,50,200");
	//_bInitBST("111230");
	//_testBST(_pT);
	//DeleteBST(_pT, 100);
	//_bGenBSTCoords(_pT, 1.0f, 0.5f, 2.0f, _nodePoints);
	//__rFunc();

	//Initialize Graphics
	glutInit(&a, &argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(200, 90);
	glutInitWindowSize(960, 540); _centerX = 200 + 960 / 2; _centerY = 90 + 540 / 2; _lockMouse = 0;
	_glutWindow = glutCreateWindow("Binary Sort Tree 3D - 3C7A91B7F");
	_hMainWnd = FindWindowA(NULL, "Binary Sort Tree 3D - 3C7A91B7F");
	if(_hMainWnd){
		LONG _wStyle = GetWindowLong(_hMainWnd, GWL_STYLE);
		SetWindowLong(_hMainWnd, GWL_STYLE, _wStyle & ~ (WS_SIZEBOX | WS_MAXIMIZEBOX));
	}

	//Callback Functions
	glutDisplayFunc(&__rFunc);
	glutIdleFunc(&__idleFunc);
	glutTimerFunc(20, &__timerFunc, 1);
	glutKeyboardFunc(&_keyFunc);
	glutKeyboardUpFunc(&_keyUpFunc);
	glutSpecialFunc(&_specFunc);
	glutSpecialUpFunc(&_specUpFunc);
	glutMouseFunc(&_mouseFunc);
	glutMotionFunc(&_motionFunc);
	glutPassiveMotionFunc(&_passMotionFunc);
	
	//Init GL Scene & Variables
	__initScene();
	
	//GLUT Main Loop
	glutMainLoop();

	return 0;
}


void __loadTexture(wchar_t * _fileName, unsigned char _texId, unsigned int _size, unsigned char _alpha)
{
	unsigned char *_bmpTex = new unsigned char[_size * _size * 3];
	FILE *_filebmpTex;	

	//Bind
	glBindTexture(GL_TEXTURE_2D, _texIDs[_texId]);
	//Set Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 

	//Load Data
		//_bmpTex = (unsigned char *)LoadResource(_hInst, FindResource(NULL, _T("IDB_T_SKY"), _T("BITMAP")));
		_wfopen_s(&_filebmpTex, _fileName, _T("rb"));
		if (!_filebmpTex){
			MessageBox(NULL, _T("Open Texture File Failed"), NULL, NULL);
			return;
		}
		fseek(_filebmpTex, 54, SEEK_SET);
		fread(_bmpTex, _size * _size * 3, 1, _filebmpTex);
		fclose(_filebmpTex);

	
	//Add alpha channel
	UINT32 _ix2 = 0;
	unsigned char *_bmpTexA = new unsigned char[_size * _size * 4];
	for (UINT32 _ix = 0; _ix < (_size * _size * 3); _ix += 3)
	{
		_bmpTexA[_ix2 + 0] = _bmpTex[_ix + 0];
		_bmpTexA[_ix2 + 1] = _bmpTex[_ix + 1];
		_bmpTexA[_ix2 + 2] = _bmpTex[_ix + 2];
		_bmpTexA[_ix2 + 3] = _alpha;
		_ix2 += 4;
	}


 

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _size, _size, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, _bmpTexA);
	free(_bmpTex);
	free(_bmpTexA);
	//Bind back
	glBindTexture(GL_TEXTURE_2D, 0);
}









void __initScene()
{


	//*** Load Textures ***
	{
		glEnable(GL_TEXTURE_2D);

		//Generate List
		glGenTextures(50, _texIDs);

		//Load
		
		__loadTexture(_T("_textures\\_blank.bmp"), 2, 64, 0xFF); //blank
		__loadTexture(_T("_textures\\_blank.bmp"), 3, 64, 0xD0); //blank (transparent)

		__loadTexture(_T("_textures\\_bluesky.bmp"), 5, 64, 0xFF);
		__loadTexture(_T("_textures\\_grass.bmp"), 6, 128, 0xFF);

		__loadTexture(_T("_textures\\_OpenGL.bmp"), 7, 256, 0xFF);
		__loadTexture(_T("_textures\\_weibo.bmp"), 8, 256, 0xFF);
		__loadTexture(_T("_textures\\_maple.bmp"), 9, 256, 0xFF);
		__loadTexture(_T("_textures\\_houseface.bmp"), 11, 128, 0xFF);
		__loadTexture(_T("_textures\\_houseroof.bmp"), 12, 64, 0xFF);
		__loadTexture(_T("_textures\\_housewall.bmp"), 13, 128, 0xFF);

		__loadTexture(_T("_textures\\_btn_input.bmp"), 21, 256, 0xA0);
		__loadTexture(_T("_textures\\_btn_insert.bmp"), 22, 256, 0xA0);
		__loadTexture(_T("_textures\\_btn_delete.bmp"), 23, 256, 0xA0);
		__loadTexture(_T("_textures\\_btn_traverse.bmp"), 24, 256, 0xA0);

		__loadTexture(_T("_textures\\_btn_input.bmp"), 26, 256, 0xFF);
		__loadTexture(_T("_textures\\_btn_insert.bmp"), 27, 256, 0xFF);
		__loadTexture(_T("_textures\\_btn_delete.bmp"), 28, 256, 0xFF);
		__loadTexture(_T("_textures\\_btn_traverse.bmp"), 29, 256, 0xFF);

		__loadTexture(_T("_textures\\_bar_input_l.bmp"), 31, 256, 0xC0);
		__loadTexture(_T("_textures\\_bar_input_m.bmp"), 32, 256, 0xC0);
		__loadTexture(_T("_textures\\_bar_input_r.bmp"), 33, 256, 0xC0);

	}


	//Enable Depth Test / Blend
	
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Vert sync
	//wglSwapIntervalEXT(1);

	//Input/Output
	_bInput = 0;
	_bPromptTextOn = 1;
	_iPromptRemainingFrames = 350;

	_fat2DStrInit("Verdana", 22);
	_lpszInputBoxText = new char[128];
	memset(_lpszInputBoxText, 0, 128);
	sprintf_s(_lpszInputBoxText, 128, "|");
	for(unsigned char _i = 0; _i < 5; _i++){
		_lpszPromptText[_i] = new char[512];
		memset(_lpszPromptText[_i], 0, 512);
	}
	sprintf_s(_lpszPromptText[0], 512, "Hit Space Key to enter/exit inspector mode. Input '111230' for demo tree.");

	//Init Variables
	
	_iFramesRendered = 0;
	_lastTickCount = 0;
	_camX = 3; _camY = 20; _camZ = 2;
	_camGo_Forward = 0; _camGo_Backward = 0; _camGo_Leftward = 0; _camGo_Rightward = 0; _camGo_Upward = 0; _camGo_Downward = 0;
	_normHorizontalDeg = 0; _normVerticalDeg = 0;
	_normIncDegUp = 0; _normIncDegDown = 0; _normIncDegLeft = 0; _normIncDegRight = 0;
	

}


/// **************** Idle Func *****************

void __idleFunc()
{

	Sleep(12);
	_iFramesRendered ++;
	
	// ** Mouse Operations
	{
		if(_lockMouse)
		{
			//Get current pos
			GetCursorPos(&_curPos);
			//Move
			_normVerticalDeg -= (float)(long)(_curPos.y - _centerY) / 3.5f;
			_normHorizontalDeg += (float)(long)(_curPos.x - _centerX) / 3.5f;
			//Limit degrees
			if(_normVerticalDeg >= 90){
				_normVerticalDeg = 89;
			}
			if(_normVerticalDeg <= -90){
				_normVerticalDeg = -89;
			}
			//Set pos to center
			SetCursorPos(_centerX, _centerY);

		}
	}
	// ** Mouse Operations


	// ** Walking
	{
		//Pre-calculation
		GLfloat _sinHDeg = _m_sin_deg(_normHorizontalDeg); GLfloat _cosHDeg = _m_cos_deg(_normHorizontalDeg);
		GLfloat _sinVDeg = _m_sin_deg(_normVerticalDeg);   GLfloat _cosVDeg = _m_cos_deg(_normVerticalDeg);
		GLfloat _camXInc = 0, _camYInc = 0, _camZInc = 0;

		//X (Left or Right is unrelated with Vertical Degree)
		_camXInc  = - 0.2f * _camGo_Forward   * _sinHDeg;
		_camXInc -= - 0.2f * _camGo_Backward  * _sinHDeg;
		_camXInc *= _cosVDeg;
		_camXInc +=   0.2f * _camGo_Leftward  * _cosHDeg;
		_camXInc -=   0.2f * _camGo_Rightward * _cosHDeg;

		//Y (Left or Right is unrelated with Vertical Degree)
		_camYInc  = - 0.2f * _camGo_Forward   * _cosHDeg;
		_camYInc -= - 0.2f * _camGo_Backward  * _cosHDeg;
		_camYInc *= _cosVDeg;
		_camYInc -=   0.2f * _camGo_Leftward  * _sinHDeg;
		_camYInc +=   0.2f * _camGo_Rightward * _sinHDeg;

		//Z
		_camZInc  =  0.2f * _camGo_Forward   * _sinVDeg;
		_camZInc -=  0.2f * _camGo_Backward  * _sinVDeg;

		_camX += _camXInc; _camY += _camYInc; _camZ += _camZInc;

		_camZ += 0.2 * _camGo_Upward; _camZ -= 0.2 * _camGo_Downward;
	}
	// ** Walking


	// ** Camera Rotation
	{
		_normVerticalDeg += (2.0f * _normIncDegUp);
		if(_normVerticalDeg >= 90){
			_normVerticalDeg = 89;
		}

		_normVerticalDeg -= (2.0f * _normIncDegDown);
		if(_normVerticalDeg <= -90){
			_normVerticalDeg = -89;
		}
		_normHorizontalDeg -= (2.0f * _normIncDegLeft);
		_normHorizontalDeg += (2.0f * _normIncDegRight);
	}
	// ** Camera Rotation

	//Post refresh message
	glutPostRedisplay();

}


// ****************** rFunc *****************


void __rFunc()
{
	//Clear buffers
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//Init matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Set perspective range
	gluPerspective(45, (16.0 / 9.0), 0.1f, 20000);


	//Pre-calculation
	GLfloat _sinHDeg = _m_sin_deg(_normHorizontalDeg); GLfloat _cosHDeg = _m_cos_deg(_normHorizontalDeg);
	GLfloat _sinVDeg = _m_sin_deg(_normVerticalDeg);   GLfloat _cosVDeg = _m_cos_deg(_normVerticalDeg);
	//Calculate normal vectors
	//X
	_normX = - _sinHDeg;
	_normX *= _cosVDeg;
	_normX += _camX;
	//Y
	_normY = - _cosHDeg;
	_normY *= _cosVDeg;
	_normY += _camY;
	//Z
	_normZ = _camZ + _sinVDeg;

	//Set camera
	gluLookAt(
		_camX, _camY, _camZ, 
		_normX, _normY, _normZ, 
		0, 0, 1
		);


	//***** Objects *****
	{

		//Sky
		glBindTexture(GL_TEXTURE_2D, _texIDs[5]);	
		_fatCuboid(0.0f, 0.0f, 0.0f, 500.0f, 500.0f, 700.0f);

		//Grass
		glBindTexture(GL_TEXTURE_2D, _texIDs[6]);
		_fatCuboid(0.0f, 0.0f, -1.1f, 320.0f, 160.0f, 2.0f);

		//Box_daren
		glBindTexture(GL_TEXTURE_2D, _texIDs[8]);
		_fatCuboid(55.0f, -10.0f, 1.1f, 2.2f, 2.2f, 2.2f);

		//House 1
		_fatHouse(40.0f, -10.0f, 3.5f, 0.7f);
		
		// *** Draw BST ***
		unsigned int _i = 0;
		unsigned int _iParent = 0;

		//Global-Total number of nodes
		_curNumOfNodes = 0;
		while(_nodePoints[_i]._y)
		{
			if(_nodePoints[_i]._node){
				//Draw Cuboid;
				glBindTexture(GL_TEXTURE_2D, _texIDs[9]);
				_fatCuboid(- 1.0f *_nodePoints[_i]._x, 0.0f, _nodePoints[_i]._y, 1.0f, 1.0f, 1.0f);

				//Draw Lines
				if (_nodePoints[_i]._node->Parent)
				{
					//Find Parent
					while(_nodePoints[_iParent]._node != _nodePoints[_i]._node->Parent){
						_iParent++;
						if(_iParent > 4096){break;}
					}
					//Lines
					if(_nodePoints[_iParent]._x && _nodePoints[_iParent]._y){
						glLineWidth(3.0f);
						glBegin(GL_LINES);
							glVertex3f(- 1.0f *_nodePoints[_i]._x, 0.0f, _nodePoints[_i]._y);
							glVertex3f(- 1.0f *_nodePoints[_iParent]._x, 0.0f, _nodePoints[_iParent]._y);
						glEnd();
					}
				}
				
				//Print Text
				glBindTexture(GL_TEXTURE_2D, 0);
				sprintf_s(_textMsg, 512, "%d", _nodePoints[_i]._node->nData.iKey);
				_fat3DString((- 1.0f *_nodePoints[_i]._x), 0.6f,  _nodePoints[_i]._y + 0.6f, _textMsg);

				//Global-Total number of nodes ++
				_curNumOfNodes ++;

			}
			_i++;
		}
		
		// *** Draw 2D Objects ***
		_draw2DObjs();

		//Reset Texture
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	//***** Objects End *****




	//Swap Buffers
	glFlush();
	glutSwapBuffers();

}


// *** Objects Functions ***

void _draw2DObjs()
{

	// *** 2D Objects ***
	// *** Menu ***
	if(!_lockMouse){

		//Input / Create
		if(_lastX > 811 && _lastX < 930 && _lastY > 50 && _lastY < 100){
			glBindTexture(GL_TEXTURE_2D, _texIDs[26]);
		}else{
			glBindTexture(GL_TEXTURE_2D, _texIDs[21]);
		}
		_fat2DBox(-0.6f, 0.3f, 0.18f, 0.08f);

		//Insert
		if(_lastX > 811 && _lastX < 930 && _lastY > 115 && _lastY < 165){
			glBindTexture(GL_TEXTURE_2D, _texIDs[27]);
		}else{
			glBindTexture(GL_TEXTURE_2D, _texIDs[22]);
		}
		_fat2DBox(-0.6f, 0.2f, 0.18f, 0.08f);

		//Delete
		if(_lastX > 811 && _lastX < 930 && _lastY > 180 && _lastY < 230){
			glBindTexture(GL_TEXTURE_2D, _texIDs[28]);
		}else{
			glBindTexture(GL_TEXTURE_2D, _texIDs[23]);
		}
		_fat2DBox(-0.6f, 0.1f, 0.18f, 0.08f);

		//Traverse
		if(_lastX > 811 && _lastX < 930 && _lastY > 245 && _lastY < 295){
			glBindTexture(GL_TEXTURE_2D, _texIDs[29]);
		}else{
			glBindTexture(GL_TEXTURE_2D, _texIDs[24]);
		}
		_fat2DBox(-0.6f, 0.0f, 0.18f, 0.08f);
	}

	// ** Input Box
	if(_bInput)
	{
		//Print text into textBox
		glBindTexture(GL_TEXTURE_2D, _texIDs[3]);
		_fat2DString(0.215f, -0.215f, _lpszInputBoxText);

		glBindTexture(GL_TEXTURE_2D, _texIDs[31]);
			_fat2DBox(  0.30f, -0.23f, 0.16f, 0.12f);
		glBindTexture(GL_TEXTURE_2D, _texIDs[32]);
			_fat2DBox(  0.00f, -0.23f, 0.44f, 0.12f);
		glBindTexture(GL_TEXTURE_2D, _texIDs[33]);
			_fat2DBox(- 0.30f, -0.23f, 0.16f, 0.12f);
	}

	// ** Prompt Text
	if(_bPromptTextOn == 1){
		if(--_iPromptRemainingFrames){
			glBindTexture(GL_TEXTURE_2D, _texIDs[3]);
			_fat2DString(0.45f, -0.12f, _lpszPromptText[0]);
		}else{
			_bPromptTextOn = 0;
		}
	}
	else if(_bPromptTextOn == 2){
		if(--_iPromptRemainingFrames){
			glBindTexture(GL_TEXTURE_2D, _texIDs[3]);
			_fat2DString(0.58f, -0.10f, _lpszPromptText[1]);
			_fat2DString(0.58f, -0.13f, _lpszPromptText[2]);
			_fat2DString(0.58f, -0.16f, _lpszPromptText[3]);
			_fat2DString(0.58f, -0.19f, _lpszPromptText[4]);
		}else{
			_bPromptTextOn = 0;
		}
	}


}


void _fat2DBox(GLfloat _x, GLfloat _y, GLfloat _width, GLfloat _height)
{
	GLfloat _wCoef = _x + (_width / 2);
	GLfloat _hCoef = _y + (_height / 2);
	GLfloat _wCoefOp = _x - (_width / 2);
	GLfloat _hCoefOp = _y - (_height / 2);

	//Pre-calculation
	GLfloat _sinHDeg = _m_sin_deg(_normHorizontalDeg); GLfloat _cosHDeg = _m_cos_deg(_normHorizontalDeg);
	GLfloat _sinVDeg = _m_sin_deg(_normVerticalDeg);   GLfloat _cosVDeg = _m_cos_deg(_normVerticalDeg);

	glBegin(GL_QUADS);
	//Left-top
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(
			_normX + (_wCoef * _cosHDeg + _hCoef * _sinVDeg * _sinHDeg),
			_normY - (_wCoef * _sinHDeg - _hCoef * _sinVDeg * _cosHDeg),
			_normZ + (_hCoef * _cosVDeg)
			);
	//Right-top
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(
			_normX + (_wCoefOp * _cosHDeg + _hCoef * _sinVDeg * _sinHDeg),
			_normY - (_wCoefOp * _sinHDeg - _hCoef * _sinVDeg * _cosHDeg),
			_normZ + (_hCoef * _cosVDeg)
			);
	//Right-bottom
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(
			_normX + (_wCoefOp * _cosHDeg + _hCoefOp * _sinVDeg * _sinHDeg),
			_normY - (_wCoefOp * _sinHDeg - _hCoefOp * _sinVDeg * _cosHDeg),
			_normZ + (_hCoefOp * _cosVDeg)
			);
	//Left-bottom
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(
			_normX + (_wCoef * _cosHDeg + _hCoefOp * _sinVDeg * _sinHDeg),
			_normY - (_wCoef * _sinHDeg - _hCoefOp * _sinVDeg * _cosHDeg),
			_normZ + (_hCoefOp * _cosVDeg)
			);
	glEnd();


}

void _fat2DStrInit(char *_fontName, char _size)
{

    HFONT hFont = CreateFontA(_size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _fontName);
    HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
    DeleteObject(hOldFont);

	_glBFListBegin = glGenLists(128);
	wglUseFontBitmaps(wglGetCurrentDC(), 0, 128, _glBFListBegin);
}


void _fat2DString(GLfloat _x, GLfloat _y, char * _str)
{
	GLfloat _rX, _rY, _rZ;

	//Pre-calculation
	GLfloat _sinHDeg = _m_sin_deg(_normHorizontalDeg); GLfloat _cosHDeg = _m_cos_deg(_normHorizontalDeg);
	GLfloat _sinVDeg = _m_sin_deg(_normVerticalDeg);   GLfloat _cosVDeg = _m_cos_deg(_normVerticalDeg);

	_rX = _normX + (_x * _cosHDeg + _y * _sinVDeg * _sinHDeg + 0.1f * _sinHDeg * _cosVDeg);
	_rY = _normY - (_x * _sinHDeg - _y * _sinVDeg * _cosHDeg - 0.1f * _cosHDeg * _cosVDeg);
	_rZ = _normZ + (_y * _cosVDeg - 0.1f * _sinVDeg);

	_fat3DString(_rX, _rY, _rZ, _str);

}

void _fat3DString(GLfloat _x, GLfloat _y, GLfloat _z, char * _str)
{
	glRasterPos3f(_x, _y, _z);

	unsigned int _chrPos = 0;
	while(_str[_chrPos]){
		glCallList(_glBFListBegin + _str[_chrPos]);
		_chrPos++;
	}
}




// 3D Built-in shapes
void _fatCuboid(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _length, GLfloat _width, GLfloat _height)
{
	
	GLfloat _di_length = _length / 2;
	GLfloat _di_width  = _width / 2;
	GLfloat _di_height = _height / 2;
	
	glBegin(GL_QUADS); //Top
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x + _di_length, _y - _di_width, _z + _di_height); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x - _di_length, _y - _di_width, _z + _di_height); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x - _di_length, _y + _di_width, _z + _di_height); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x + _di_length, _y + _di_width, _z + _di_height); 
	glEnd();

	glBegin(GL_QUADS); //Bottom
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x + _di_length, _y - _di_width, _z - _di_height); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x - _di_length, _y - _di_width, _z - _di_height); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x - _di_length, _y + _di_width, _z - _di_height); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x + _di_length, _y + _di_width, _z - _di_height); 
	glEnd();

	glBegin(GL_QUADS); //Front
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x + _di_length, _y + _di_width, _z + _di_height); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x - _di_length, _y + _di_width, _z + _di_height); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x - _di_length, _y + _di_width, _z - _di_height); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x + _di_length, _y + _di_width, _z - _di_height); 
	glEnd();

	glBegin(GL_QUADS); //Back
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x - _di_length, _y - _di_width, _z + _di_height); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x + _di_length, _y - _di_width, _z + _di_height); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x + _di_length, _y - _di_width, _z - _di_height); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x - _di_length, _y - _di_width, _z - _di_height); 
	glEnd();

	glBegin(GL_QUADS); //Left
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x + _di_length, _y - _di_width, _z + _di_height); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x + _di_length, _y + _di_width, _z + _di_height); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x + _di_length, _y + _di_width, _z - _di_height); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x + _di_length, _y - _di_width, _z - _di_height); 
	glEnd();

	glBegin(GL_QUADS); //Right
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x - _di_length, _y + _di_width, _z + _di_height); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x - _di_length, _y - _di_width, _z + _di_height); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x - _di_length, _y - _di_width, _z - _di_height); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x - _di_length, _y + _di_width, _z - _di_height); 
	glEnd();

}

void _fatHouse(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _size)
{

	GLfloat _sizeMult = 6.0f * _size;
	GLfloat _sizeMult2 = 5.0f * _size;
	GLfloat _sizeMult3 = 3.0f * _size;
	//GLfloat _sizeMult4 = 4.0f * _size;

	//TODO

	//Face Texture
	glBindTexture(GL_TEXTURE_2D, _texIDs[11]);
	glBegin(GL_QUADS); //Front
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x + _sizeMult, _y + _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x - _sizeMult, _y + _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x - _sizeMult, _y + _sizeMult, _z - _sizeMult2); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x + _sizeMult, _y + _sizeMult, _z - _sizeMult2); 
	glEnd();

	//Roof Texture
	glBindTexture(GL_TEXTURE_2D, _texIDs[12]);

	glBegin(GL_QUADS); //Left slope
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x, _y - _sizeMult, _z + _sizeMult2 + _sizeMult3); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x, _y + _sizeMult, _z + _sizeMult2 + _sizeMult3); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x + _sizeMult, _y + _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x + _sizeMult, _y - _sizeMult, _z + _sizeMult2); 
	glEnd();

	glBegin(GL_QUADS); //Right slope
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x, _y + _sizeMult, _z + _sizeMult2 + _sizeMult3); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x, _y - _sizeMult, _z + _sizeMult2 + _sizeMult3); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x - _sizeMult, _y - _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x - _sizeMult, _y + _sizeMult, _z + _sizeMult2); 
	glEnd();


	//Wall Texture
	glBindTexture(GL_TEXTURE_2D, _texIDs[13]);

	glBegin(GL_TRIANGLES); //Front Triangle
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x + _sizeMult, _y + _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x - _sizeMult, _y + _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(0.5f, 0.0f); glVertex3f(_x , _y + _sizeMult, _z + _sizeMult2 + _sizeMult3); 
	glEnd();

	glBegin(GL_TRIANGLES); //Back Triangle
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x + _sizeMult, _y - _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x - _sizeMult, _y - _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(0.5f, 0.0f); glVertex3f(_x , _y - _sizeMult, _z + _sizeMult2 + _sizeMult3); 
	glEnd();

	glBegin(GL_QUADS); //Bottom
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x + _sizeMult, _y - _sizeMult, _z - _sizeMult2); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x - _sizeMult, _y - _sizeMult, _z - _sizeMult2); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x - _sizeMult, _y + _sizeMult, _z - _sizeMult2); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x + _sizeMult, _y + _sizeMult, _z - _sizeMult2); 
	glEnd();

	glBegin(GL_QUADS); //Back
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x - _sizeMult, _y - _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x + _sizeMult, _y - _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x + _sizeMult, _y - _sizeMult, _z - _sizeMult2); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x - _sizeMult, _y - _sizeMult, _z - _sizeMult2); 
	glEnd();

	glBegin(GL_QUADS); //Left
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x + _sizeMult, _y - _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x + _sizeMult, _y + _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x + _sizeMult, _y + _sizeMult, _z - _sizeMult2); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x + _sizeMult, _y - _sizeMult, _z - _sizeMult2); 
	glEnd();

	glBegin(GL_QUADS); //Right
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(_x - _sizeMult, _y + _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(_x - _sizeMult, _y - _sizeMult, _z + _sizeMult2); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(_x - _sizeMult, _y - _sizeMult, _z - _sizeMult2); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(_x - _sizeMult, _y + _sizeMult, _z - _sizeMult2); 
	glEnd();

}


// 

void __timerFunc(int value)
{
	unsigned long _passedTicks;

	glutTimerFunc(580, &__timerFunc, 0);

	if (!_lastTickCount){
		_lastTickCount = GetTickCount();
		return;
	}

	_passedTicks = GetTickCount() - _lastTickCount;
	_framesPerSec = _iFramesRendered / ((double)_passedTicks / (double)1000.0f);
	_iFramesRendered = 0;
	_lastTickCount = GetTickCount();

	char *_lpszTitle = new char[128];
	sprintf_s(_lpszTitle, 128, "Data-structure Course Design - Binary Sort Tree 3-D [fps: %.2f]", _framesPerSec);
	glutSetWindowTitle(_lpszTitle);

}




// ********************* Control Functions *************************

void _keyFunc(unsigned char key, int x, int y)
{
	//w
	if(key == 0x77){
		_camGo_Forward = 1;
	}
	//s
	if(key == 0x73){
		_camGo_Backward = 1;
	}
	//a
	if(key == 0x61){
		_camGo_Leftward = 1;
	}
	//d
	if(key == 0x64){
		_camGo_Rightward = 1;
	}
	//esc
	if(key == 0x1B){
		PostMessage(_hMainWnd, WM_CLOSE, NULL, NULL);
		//glutDestroyWindow(_glutWindow);
	}

	//0-9 and ','
	if(key >= 0x30 && key <= 0x39 || key == 0x2C)
	{
		unsigned char _cLen = (unsigned char)strlen(_lpszInputBoxText);
		if(_cLen < 61){
			_lpszInputBoxText[_cLen - 1] = key;
			_lpszInputBoxText[_cLen    ] = 0x7C;
			_lpszInputBoxText[_cLen + 1] = 0;
		}
	}
	//Backspace
	if(key == 0x08)
	{
		unsigned char _cLen = (unsigned char)strlen(_lpszInputBoxText);
		if(_cLen > 1){
			_lpszInputBoxText[_cLen - 2] = 0x7C;
			_lpszInputBoxText[_cLen - 1] = 0;
		}
	}


	//Enter
	if(key == 0x0D){
		if(!_bInput){return;}
		switch(_cInputFlag)
		{

		//Input
		case 1:
			//Remove '|'
			_lpszInputBoxText[strlen(_lpszInputBoxText) - 1] = 0;
			//Init BST
			if(strlen(_lpszInputBoxText)){
				_bInitBST(_lpszInputBoxText);
			}
			sprintf_s(_lpszPromptText[0], 128, "Tree created.");
			//Prompt result
			_iPromptRemainingFrames = 250;_bPromptTextOn = 1;
			//Close InputBox
			_bInput = !_bInput;
			break;

		//Insert
		case 2:
			//Remove '|'
			_lpszInputBoxText[strlen(_lpszInputBoxText) - 1] = 0;
			//Create temp nodedata & set value
			bstNodeData _nTmpNodeData;
			_nTmpNodeData.iKey = atoi(_lpszInputBoxText);


			if(_nTmpNodeData.iKey){
				if(InsertBST(_pT, _nTmpNodeData)){
					_bGenBSTCoords(_pT, 1.0f, 0.5f, 2.0f, _nodePoints);
					sprintf_s(_lpszPromptText[0], 512, "Node inserted.");
				}else{
					sprintf_s(_lpszPromptText[0],512, "Insertion failed, key already exists.");
				}
			}else{
				sprintf_s(_lpszPromptText[0],512, "Key can't be zero.");
			}
			//Prompt result
			_iPromptRemainingFrames = 250;_bPromptTextOn = 1;

			//Clear TextBox
			_lpszInputBoxText[0] = 0x7C;
			_lpszInputBoxText[1] = 0;
			break;

		//Delete
		case 3:
			//Remove '|'
			_lpszInputBoxText[strlen(_lpszInputBoxText) - 1] = 0;
			//Create temp key & set value
			int _iTmpKey;
			_iTmpKey = atoi(_lpszInputBoxText);

			//Prompt result
			_bPromptTextOn = 1;_iPromptRemainingFrames = 250;
			if(_curNumOfNodes == 1){
				sprintf_s(_lpszPromptText[0],512, "Can't delete the last node.");
			}else{
				if(_iTmpKey){
					DeleteBST(_pT, _iTmpKey);
					_bGenBSTCoords(_pT, 1.0f, 0.5f, 2.0f, _nodePoints);
					sprintf_s(_lpszPromptText[0],512, "Node deleted.");
				}else{
					sprintf_s(_lpszPromptText[0],512, "Key can't be zero.");
				}
			}
			//Clear TextBox
			_lpszInputBoxText[0] = 0x7C;
			_lpszInputBoxText[1] = 0;
			
			break;
		}
	}

	//Space
	if(key == 0x20){
		if(!_bInput){
			SetCursorPos(_centerX, _centerY);
			ShowCursor(_lockMouse);
			_lockMouse = !_lockMouse;
		}
	}
}



void _keyUpFunc(unsigned char key, int x, int y)
{
	//w
	if(key == 0x77){
		_camGo_Forward = 0;
	}
	//s
	if(key == 0x73){
		_camGo_Backward = 0;
	}
	//a
	if(key == 0x61){
		_camGo_Leftward = 0;
	}
	//d
	if(key == 0x64){
		_camGo_Rightward = 0;
	}

}

void _specFunc(int key, int x, int y)
{
	//101 103 100 102

	//Up
	if(key == GLUT_KEY_UP){
		_normIncDegUp = 1;
	}

	//Down
	if(key == GLUT_KEY_DOWN){
		_normIncDegDown = 1;
	}

	//Left
	if(key == GLUT_KEY_LEFT){
		_normIncDegLeft = 1;
	}

	//Right
	if(key == GLUT_KEY_RIGHT){
		_normIncDegRight = 1;
	}

	//PgUp
	if(key == GLUT_KEY_PAGE_UP){
		_camGo_Upward = 1;
	}
	//PgDn
	if(key == GLUT_KEY_PAGE_DOWN){
		_camGo_Downward = 1;
	}
}



void _specUpFunc(int key, int x, int y)
{

	//Up
	if(key == GLUT_KEY_UP){
		_normIncDegUp = 0;
	}

	//Down
	if(key == GLUT_KEY_DOWN){
		_normIncDegDown = 0;
	}

	//Left
	if(key == GLUT_KEY_LEFT){
		_normIncDegLeft = 0;
	}

	//Right
	if(key == GLUT_KEY_RIGHT){
		_normIncDegRight = 0;
	}
	//i
	if(key == GLUT_KEY_PAGE_UP){
		_camGo_Upward = 0;
	}
	//k
	if(key == GLUT_KEY_PAGE_DOWN){
		_camGo_Downward = 0;
	}

}

void _mouseFunc(int button, int state, int x, int y)
{

	if(!_lockMouse && state == GLUT_UP){

		//Input / Create
		if(x > 811 && x < 930 && y > 50 && y < 100){
			if(_bInput){
				if(_cInputFlag != 1)
				{}else{_bInput = 0;}
			}else{_bInput = 1;}

			if(_bInput){
				_cInputFlag = 1;
				_bPromptTextOn = 1;_iPromptRemainingFrames = 350;
				sprintf_s(_lpszPromptText[0],512, "Type integer node keys in level order. ',' to separate 2 numbers.");
				sprintf_s(_lpszInputBoxText, 64, "|");
			}else{
				_bPromptTextOn = 1;_iPromptRemainingFrames = 350;
				sprintf_s(_lpszPromptText[0],512, "Hit Space Key to enter/exit inspector mode. Input '111230' for demo tree.");
			}
		}//Input / Create

		//Insert
		if(x > 811 && x < 930 && y > 115 && y < 165){
			if(_bInput){
				if(_cInputFlag != 2)
				{}else{_bInput = 0;}
			}else{_bInput = 1;}

			if(_bInput){
				_cInputFlag = 2;
				_bPromptTextOn = 1;_iPromptRemainingFrames = 350;
				sprintf_s(_lpszInputBoxText, 64, "|");
				sprintf_s(_lpszPromptText[0],512, "Type an integer key of new node:");
			}else{		
				_bPromptTextOn = 1;_iPromptRemainingFrames = 350;
				sprintf_s(_lpszPromptText[0],512, "Hit Space Key to enter/exit inspector mode. Input '111230' for demo tree.");
			}
		}//Insert

		//Delete
		if(x > 811 && x < 930 && y > 180 && y < 230){
			if(_bInput){
				if(_cInputFlag != 3)
				{}else{_bInput = 0;}
			}else{_bInput = 1;}

			if(_bInput){
				_cInputFlag = 3;
				_bPromptTextOn = 1;_iPromptRemainingFrames = 350;
				sprintf_s(_lpszInputBoxText, 64, "|");
				sprintf_s(_lpszPromptText[0],512, "Type an integer key for node to be deleted:");
			}else{		
				_bPromptTextOn = 1;_iPromptRemainingFrames = 350;
				sprintf_s(_lpszPromptText[0],512, "Hit Space Key to enter/exit inspector mode. Input '111230' for demo tree.");
			}
		}//Delete

		//Traverse
		if(x > 811 && x < 930 && y > 245 && y < 295){
			//Switch InputBox Off
			_bInput = 0;
			//Clear _lpszPromptText[1]
			memset(_lpszPromptText[1], 0, 512);

			//InOrder
			InOrderTraverse(_pT, NULL, &_bTraverse_CallBack);
			_lpszPromptText[1][strlen(_lpszPromptText[1]) - 2] = 0;
			sprintf_s(_lpszPromptText[2], 512, "InOrderTraverse: ");
			strcat_s(_lpszPromptText[2], 512, _lpszPromptText[1]);
			memset(_lpszPromptText[1], 0, 512);
			//PostOrder
			PostOrderTraverse(_pT, &_bTraverse_CallBack);
			_lpszPromptText[1][strlen(_lpszPromptText[1]) - 2] = 0;
			sprintf_s(_lpszPromptText[3], 512, "PostOrderTraverse: ");
			strcat_s(_lpszPromptText[3], 512, _lpszPromptText[1]);
			memset(_lpszPromptText[1], 0, 512);
			//LevelOrder
			LevelOrderTraverse(_pT, &_bTraverse_CallBack);
			_lpszPromptText[1][strlen(_lpszPromptText[1]) - 2] = 0;
			sprintf_s(_lpszPromptText[4], 512, "LevelOrderTraverse: ");
			strcat_s(_lpszPromptText[4], 512, _lpszPromptText[1]);
			memset(_lpszPromptText[1], 0, 512);
			//PreOrder
			sprintf_s(_lpszPromptText[1], 512, "PreOrderTraverse: ");
			PreOrderTraverse(_pT, &_bTraverse_CallBack);
			_lpszPromptText[1][strlen(_lpszPromptText[1]) - 2] = 0;
			//Turn Prompt2 On
			_bPromptTextOn = 2;_iPromptRemainingFrames = 1200;

		}//Traverse
	}//!_lockMouse && state == GLUT_UP
}

void _motionFunc(int x, int y)
{
	if(!_lockMouse)
	{
		_normVerticalDeg -= (float)(long)(y - _lastY) / 3.5f;
		_normHorizontalDeg += (float)(long)(x - _lastX) / 3.5f;
		if(_normVerticalDeg >= 90){
			_normVerticalDeg = 89;
		}
		if(_normVerticalDeg <= -90){
			_normVerticalDeg = -89;
		}
		_lastX = x; _lastY = y;
	}
}

void _passMotionFunc(int x, int y)
{
	/*
	char *_lpszTitle = new char[128];
	sprintf_s(_lpszTitle, 128, "Data-structure Course Design - Binary Sort Tree [(x,y): %d, %d]", (x), (y));
	glutSetWindowTitle(_lpszTitle);
	*/
	_lastX = x; _lastY = y;
}



// ********************* Math Functions *************************
double _m_sin_deg(double _deg){
	return (sin(((_deg) * 3.14159265358979324f) / 180.0f));
}

double _m_cos_deg(double _deg){
	return (cos(((_deg) * 3.14159265358979324f) / 180.0f));
}





// ********************* BST Functions *************************
// ********************* BST Functions *************************

void _bInitBST(char *_seq)
{

	//int _seq[] = {45, 12, 53, 0};
	//int _seq[] = {45, 12, 53, 3, 37, 100, 24, 61, 90, 78, 0};
	//int _seq[] = {45, 12, 53, 3, 32, 37, 50, 100, 24, 61, 89, 90, 0};
	int _iSeqDemo[] = {100, 50, 200, 40, 75, 150, 250, 30, 45, 65, 80, 120, 170, 220, 270, 0};
	int _iSeqDemo_2012[] = {
		100, 50, 200, 40, 75, 150, 250, 30, 45, 65, 80, 120, 170, 220, 270,
		25, 35, 42, 47, 60, 70, 77, 85, 110, 130, 160, 180, 210, 230, 260, 280, 0};
	int *_iSeq = new int[512];
	char *_lpszNum = new char[128]; //Temporary string(save 1 number)

	unsigned int _iChrPos = 0;
	unsigned int _iSeqPos = 0;
	unsigned int _iNumBeginPos = 0;
	
	while(_seq[_iChrPos])
	{
		if(_seq[_iChrPos] == 0x2C){
			//Copy to temporary string
			memcpy(_lpszNum, &_seq[_iNumBeginPos], (_iChrPos - _iNumBeginPos));
			//Add NULL termination
			_lpszNum[_iChrPos - _iNumBeginPos] = 0;
			//Array to integer
			_iSeq[_iSeqPos] = atoi(_lpszNum);
			//Sequence number ++
			_iSeqPos ++;
			//Set next number begin pos
			_iNumBeginPos = _iChrPos + 1;
		}
		_iChrPos++;
	}
	//Copy the last number
		//Copy to temporary string
		memcpy(_lpszNum, &_seq[_iNumBeginPos], (_iChrPos - _iNumBeginPos));
		//Add NULL termination
		_lpszNum[_iChrPos - _iNumBeginPos] = 0;
		//Array to integer
		_iSeq[_iSeqPos] = atoi(_lpszNum);
		//Sequence number ++
		_iSeqPos ++;
	//Add 0 to the end of sequence
	_iSeq[_iSeqPos] = 0;

	if(!_iSeq[0]){return;}
	
	//Demo Trees
	if(_iSeq[0] == 111230){
		_iSeq = _iSeqDemo;
	}
	if(_iSeq[0] == 121221){
		_iSeq = _iSeqDemo_2012;
	}
	if(_iSeq[0] == 41322){
		__loadTexture(_T("_textures\\_maple.bmp"), 5, 256, 0xFF);
	}

	_pT = new bstNode;
	_nodePoints = new _nodePoint[4096];
	memset(_pT, 0, sizeof(bstNode));
	memset(_nodePoints, 0, 4096 * sizeof(_nodePoint));

	CreateBST(_pT, _iSeq);
	_bGenBSTCoords(_pT, 1.0f, 0.5f, 2.0f, _nodePoints);

}

void _bTraverse_CallBack(bstNode *_node)
{
	//Print Traverse result to _lpszPromptText[1].
	char *_lpszTmpStr;

	if(_node)
	{
		_lpszTmpStr = new char[128];
		sprintf_s(_lpszTmpStr, 128, "%d, ", _node->nData.iKey);
		strcat_s(_lpszPromptText[1], 512, _lpszTmpStr);
	}
}


void _bGenBSTC_CallBack(bstNode *_node, unsigned int _curNum, void *_param)
{
	bstNode **_nodes = (bstNode **)_param;
	_nodes[_curNum] = _node;
}

void _bGenBSTCoords(bstNode *T, float _widthCuboid, float _widthSpace, float _heightSpace, _nodePoint *_points)
{

	unsigned char _depth;
	float _width;
	bstNode **_nodes;
	
	//Get Depth
	InOrderTraverse(T, &_depth, NULL);

	//Alloc Memory
	_nodes = new bstNode *[4096];
	memset(_nodes, 0, 4096 * sizeof(bstNode *));

	//Generate Width
	_width = (_widthCuboid + _widthSpace) * pow(2.0f, _depth - 1) - _widthSpace;

	//Get nodes by level order
	LevelOrderTraverseWithNullNodes(T, &_bGenBSTC_CallBack, _nodes);

	//Clear points array
	memset(_points, 0, 4096 * sizeof(_nodePoint));
	unsigned int _curNodeNum = 0;
	for(unsigned char _i = 0; _i < _depth; _i++)
	{
		unsigned int _curLevelNodes = pow(2.0f, _i);
		for (unsigned int _j = 0; _j < _curLevelNodes; _j++)
		{
			_points[_curNodeNum]._x = (_width / pow(2.0f, (int)(_i + 1))) * (2 * _j + 1);
			_points[_curNodeNum]._y = (_depth - _i) * _heightSpace + 1;
			_points[_curNodeNum]._node = _nodes[_curNodeNum];
			_curNodeNum++;
		}
	}
}






