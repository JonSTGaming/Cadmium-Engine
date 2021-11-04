#include <nds.h>
#include <gl2d.h>
#include <stdlib.h> 
#include <math.h>

#define mapWidth 8
#define mapHeight 8
#define screenWidth 256
#define screenHeight 192

int worldMap[mapWidth][mapHeight]=
{
  {1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,1},
  {1,0,3,0,0,2,0,1},
  {1,0,0,0,0,2,0,1},
  {4,0,0,0,0,2,0,1},
  {4,0,0,0,0,2,0,1},
  {4,0,0,0,0,2,0,1},
  {1,4,4,1,1,1,1,1},
};

int w = 256;
int h = 192;

void drawSky();

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	// Set Variables
	double posX = 5, posY = 5; // X and Y start pos
	double dirX = -1, dirY = 0; // Initial direction vector
	double planeX = 0, planeY = 0.66; // The 2D raycaster version of camera plane

	double time = 0; // Time of current frame
	double oldTime = 0; // Time of previous frame

	videoSetMode(MODE_5_3D);
    glScreen2D();

    while(1){
		for(int x=0; x < w; x++){
			double cameraX = 2 * x / (double)(screenWidth) - 1;
			double rayDirX = dirX + planeX * cameraX;
			double rayDirY = dirY + planeY * cameraX;

			// Which box
			int mapX = (int)(posX);
			int mapY = (int)(posY);

			// Length of ray from current position to next x or y-side
			double sideDistX;
			double sideDistY;

			// Length of ray from one x or y-side to next x or y-side
			double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
			double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
			double perpWallDist;

			// What direction to step in x or y-direction (either +1 or -1)
			int stepX;
			int stepY;

			int hit = 0; // Was there a wall hit?
			int side; // Was a NS or a EW wall hit?

			if(rayDirX < 0)
			{
				stepX = -1;
				sideDistX = (posX - mapX) * deltaDistX;
			}
			else
			{
				stepX = 1;
				sideDistX = (mapX + 1.0 - posX) * deltaDistX;
			}
			if(rayDirY < 0)
			{
				stepY = -1;
				sideDistY = (posY - mapY) * deltaDistY;
			}
			else
			{
				stepY = 1;
				sideDistY = (mapY + 1.0 - posY) * deltaDistY;
			}
			// Perfrom DDA
			while(hit==0){
				// JUmp to the next map square either in x-direction or in y-direction
				if (sideDistX < sideDistY){
					sideDistX += deltaDistX;
					mapX += stepX;
					side = 0;
				}
				else{
					sideDistY += deltaDistY;
					mapY += stepY;
					side = 1;
				}
				if (worldMap[mapX][mapY] > 0) hit = 1;
			}
			if(side == 0) perpWallDist = (sideDistX - deltaDistX);
			else          perpWallDist = (sideDistY - deltaDistY);
			
			//Calculate height of line to draw on screen
			int lineHeight = (int)(h / perpWallDist);

			//Calculate lowest and highest pixel to fill in current stripe
			int drawStart = -lineHeight / 2 + h / 2;
			if(drawStart < 0)drawStart = 0;
			int drawEnd = lineHeight / 2 + h / 2;
			if(drawEnd >= h)drawEnd = h - 1;

			int color[3];
			switch(worldMap[mapX][mapY]){
				case 1:		color[0] = 255; color[1] = 0; color[2] = 0; break;     //red
				case 2:		color[0] = 0; color[1] = 255; color[2] = 0; break;     //green
				case 3:		color[0] = 0; color[1] = 0; color[2] = 255; break;     //blue
				case 4:		color[0] = 255; color[1] = 255; color[2] = 255; break; //white
				default:	color[0] = 255; color[1] = 255; color[2] = 0; break;     //yellow
			}
			if(side == 1) {color[0] = color[0]/2; color[1] = color[1]/2; color[2] = color[2]/2;}	
			glBegin2D();
            	glLine(x, drawEnd, x, drawStart, RGB15(color[0], color[1], color[2]));
        	glEnd2D();
			
		}

		double moveSpeed = 0.0167 * 5.0;
		double rotSpeed = 0.0167 * 3.0;

        scanKeys();
		
		if (keysHeld() & KEY_UP){
			if(worldMap[(int)(posX + dirX * moveSpeed)][(int)(posY)] == false) posX += dirX * moveSpeed;
      		if(worldMap[(int)(posX)][(int)(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
		}
		if (keysHeld() & KEY_DOWN){
			if(worldMap[(int)(posX - dirX * moveSpeed)][(int)(posY)] == false) posX -= dirX * moveSpeed;
      		if(worldMap[(int)(posX)][(int)(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
		}
		if (keysHeld() & KEY_RIGHT){
			double oldDirX = dirX;
			dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
			dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
      		double oldPlaneX = planeX;
      		planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
      		planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    	}
		//rotate to the left
		if (keysHeld() & KEY_LEFT)
		{
			//both camera direction and camera plane must be rotated
			double oldDirX = dirX;
			dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
			dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
			double oldPlaneX = planeX;
			planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
			planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
		}
		
        glFlush(0);

        

        swiWaitForVBlank();
    }

}

void drawSky()
{
	glBegin2D();
		glBoxFilled(0, 0, 256, 96, RGB15(141, 213, 242));
		glBoxFilled(0, 96, 256, 256, RGB15(102, 102, 102));
	glEnd2D();
}