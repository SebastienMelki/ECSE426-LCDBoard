#include "LCDPathDrawing.h"

LCDPoint path[100]; // Array that will contain the entire path in Cartesian coordinates


/**
  * @brief  Computes x coordinate from polar coordinates
  * @param  angle The angle of the polar vector
  * @param  r The radius (or number of steps) of the polar vector
  * @retval x coordinate of polar vector.
  */
int getXFromPolar(float angle, int r) {
	float temp;
	temp = r * cos((180 - angle) * PI / 180);
	if (temp < 0) {
		temp = ceil(temp - 0.5);
	} else {
		temp = floor(temp + 0.5);
	}
	return (int)(temp);
}

/**
  * @brief  Computes y coordinate from polar coordinates
  * @param  angle The angle of the polar vector
  * @param  r The radius (or number of steps) of the polar vector
  * @retval y coordinate of polar vector.
  */
int getYFromPolar(float angle, int r) {
	float temp;
	temp = r * sin((180 - angle) * PI / 180);
	if (temp < 0) {
		temp = ceil(temp - 0.5);
	} else {
		temp = floor(temp + 0.5);
	}
	return (int)(temp);
}

/**
  * @brief  Converts a polar path into a Cartesian path
  * @param  heading The Polar path
  * @param  r The radius (or number of steps) of the polar vector
  * @retval x coordinate of polar vector.
  */
void convertHeadingToCartesian(heading_pair * heading, int length) {	
	int i;
	int gtrdn = 0;
	for (i = 0; i < length; i++) {
		if (i == 0) {
			path->x = 0;
			path->y = 0;
			(path + 1)->x = getXFromPolar(heading->heading, heading->step_count);
			(path + 1)->y = getYFromPolar(heading->heading, heading->step_count);
		} else {
			(path + i + 1)->x = getXFromPolar((heading + i)->heading, (heading + i)->step_count) + (path + i)->x;
			(path + i + 1)->y = getYFromPolar((heading + i)->heading, (heading + i)->step_count) + (path + i)->y;
		}
	}
}

/**
  * @brief  Draws a border along defined screen mins and mass
  */
void drawBorder() {
	LCD_DrawUniLine(X_MIN, Y_MIN, X_MIN, Y_MAX);
	LCD_DrawUniLine(X_MIN, Y_MAX, X_MAX, Y_MAX);
	LCD_DrawUniLine(X_MAX, Y_MAX, X_MAX, Y_MIN);
	LCD_DrawUniLine(X_MAX, Y_MIN, X_MIN, Y_MIN);
}

/**
  * @brief  Finds minimum y coordinate in Cartesian path
  * @param  path The Cartesian path
  * @param  length The size of the path
  * @retval min y coordinate in path
  */
int getMinY(LCDPoint * path, int length) {
	int min = path->y;
	int i;	
	for (i = 0; i < length; i++) {
		if ((path + i)->y < min) {
			min = (path + i)->y;
		}			
	}
	return min;
}

/**
  * @brief  Finds minimum x coordinate in Cartesian path
  * @param  path The Cartesian path
  * @param  length The size of the path
  * @retval min x coordinate in path
  */
int getMinX(LCDPoint * path, int length) {
	int min = path->x;
	int i;	
	for (i = 0; i < length; i++) {
		if ((path + i)->x < min) {
			min = (path + i)->x;
		}			
	}	
	return min;
}

/**
  * @brief  Finds maximum y coordinate in Cartesian path
  * @param  path The Cartesian path
  * @param  length The size of the path
  * @retval max y coordinate in path
  */
int getMaxY(LCDPoint * path, int length) {
	int max = path->y;
	int i;
	for (i = 0; i < length; i++) {
		if ((path + i)->y > max) {
			max = (path + i)->y;
		}			
	}
	
	return max;	
}

/**
  * @brief  Finds maximum x coordinate in Cartesian path
  * @param  path The Cartesian path
  * @param  length The size of the path
  * @retval max x coordinate in path
  */
int getMaxX(LCDPoint * path, int length) {
	int max = path->x;
	int i;
	for (i = 0; i < length; i++) {
		if ((path + i)->x > max) {
			max = (path + i)->x;
		}			
	}
	
	return max;		
}

/**
  * @brief  Scales an x value depending on the defined screen borders and the given extremums 
  * @param  pathX The Cartesian x coordinate to be scaled
  * @param  pathMinX The minimum x coordinate in the path
  * @param  pathMaxX The maximum x coordinate in the path
  * @retval Scaled x coordinate
  */
int getScaledX(int pathX, int pathMinX, int pathMaxX) {
	float scaleFactor = (X_MAX - X_MIN) / (pathMaxX - pathMinX);
	return X_MIN + (scaleFactor * (pathX - pathMinX));
}

/**
  * @brief  Scales an y value depending on the defined screen borders and the given extremums 
  * @param  pathX The Cartesian y coordinate to be scaled
  * @param  pathMinX The minimum y coordinate in the path
  * @param  pathMaxX The maximum y coordinate in the path
  * @retval Scaled y coordinate
  */
int getScaledY(int pathY, int pathMinY, int pathMaxY) { // Takes care of transformation from LCD coordinate system to real coodinate system
	float scaleFactor = (Y_MAX - Y_MIN) / (pathMaxY - pathMinY);
	return Y_MAX - (scaleFactor * (pathY - pathMinY));
}	

/**
  * @brief  Draws the given path on the LCD screen 
  * @param  length The size of the path
  */
void drawPath(int length) {	
	int pathMinX = getMinX(path, length);
	int pathMinY = getMinY(path, length);
	int pathMaxX = getMaxX(path, length);
	int pathMaxY = getMaxY(path, length);
	int i;
	int x;
	int y;
	
	int max;
	int min;
	
	if (pathMinX < 0) {
		int negativeOffsetX = -pathMinX;
		pathMinX += negativeOffsetX;
		pathMaxX += negativeOffsetX;
		for (i = 0; i < length; i++) {
			(path + i)->x += negativeOffsetX;
		}
	}
	
	if (pathMinY < 0) {
		int negativeOffsetY = -pathMinY;
		pathMinY += negativeOffsetY;
		pathMaxY += negativeOffsetY;
		for (i = 0; i < length; i++) {
			(path + i)->y += negativeOffsetY;
		}
	}
	
	if (pathMaxX > pathMaxY) {
		max = pathMaxX;
	} else {
		max = pathMaxY;
	}
	
	if (pathMinX < pathMinY) {
		min = pathMinX;
	} else {
		min = pathMinY;
	}
	
	for (i = 0; i < length - 1; i++) {
		int x1 = getScaledX((path + i)->x, min, max);
		int y1 = getScaledY((path + i)->y, min, max);
		int x2 = getScaledX((path + i + 1)->x, min, max);
		int y2 = getScaledY((path + i + 1)->y, min, max);
		LCD_DrawUniLine(x1, y1, x2, y2);
	}		
}

/**
  * @brief  Takes care of initializations necessary for drawing on the LCD screen and calls the drawPath() method 
  * @param  pair The polar path
  * @param  length The size of the polar path
  */
void drawTrajectory(heading_pair * pair, int length) {
		/* LCD initiatization */
		LCD_Init();
		/* LCD Layer initiatization */
		LCD_LayerInit();
		/* Enable the LTDC controler */
		LTDC_Cmd(ENABLE);
		/* Set LCD foreground layer as the current layer */
		LCD_SetLayer(LCD_FOREGROUND_LAYER);
		LCD_Clear(0xFFFFFF);
		
		convertHeadingToCartesian(pair, length);
		drawPath(length + 1);
}
	