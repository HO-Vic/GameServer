#include <windows.h>  // Including necessary headers directly
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>

extern HDC hDC;              // Private GDI Device Context
extern HGLRC hRC;            // Permanent Rendering Context
extern HWND hWnd;            // Holds Our Window Handle
extern HINSTANCE hInstance;  // Holds The Instance Of The Application

extern GLuint base;   // Base Display List For The Font Set
extern GLfloat cnt1;  // 1st Counter Used To Move Text & For Coloring
extern GLfloat cnt2;  // 2nd Counter Used To Move Text & For Coloring

extern bool keys[256];   // Array Used For The Keyboard Routine
extern bool active;      // Window Active Flag Set To TRUE By Default
extern bool fullscreen;  // Fullscreen Flag Set To Fullscreen Mode By Default

GLvoid BuildFont(GLvoid);
GLvoid KillFont(GLvoid);
GLvoid glPrint(const char* fmt, ...);

GLvoid ReSizeGLScene(GLsizei width, GLsizei height);

int InitGL(GLvoid);
int DrawGLScene(GLvoid);
GLvoid KillGLWindow(GLvoid);
BOOL CreateGLWindow(const wchar_t* title, int width, int height, BYTE bits, bool fullscreenflag, int posX = 0, int posY = 0);
LRESULT CALLBACK WndProc(HWND hWnd,      // Handle For This Window
                         UINT uMsg,      // Message For This Window
                         WPARAM wParam,  // Additional Message Information
                         LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,      // Instance
                   HINSTANCE hPrevInstance,  // Previous Instance
                   LPSTR lpCmdLine,          // Command Line Parameters
                   int nCmdShow);