#ifndef GUI_H_
#define GUI_H_

void GUI_Init(void);
void GUI_Update(void);
void GUI_Clear(unsigned int color);
void GUI_Destroy(void);
void GUI_SetRGB565(const char* pic[]);
void GUI_SetText(const char* text, int x, int y, unsigned int color);
void GUI_ClearAllText(void);
void GUI_Log(const char* text, unsigned int color);

#endif
