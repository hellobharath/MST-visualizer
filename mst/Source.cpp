#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRACATE
#include<GL/glut.h>
#include<stdio.h>
#include<math.h>
#include<string.h>
#include<time.h>
#include<limits.h>
#include<stdlib.h>

//Window Size
GLsizei wh = 1020, ww = 1080;

//Used in drawSquare, square length
GLfloat size = 3.0;

float PI = 3.14285714286;

//Radius of circle
int radius = 30;

//No. of nodes in window
int nodeCount = 0;

//Max No. of nodes that can be created
int maxNodes = 26;

//2X2 matrix between the nodes 
int adjMatrix[26][26];

//No. of nodes selected to draw a line
int lineNodesCount = 0;

//Source node created
int sourceNodeCreated = 0;

//Destination node created
int destinationNodeCreated = 0;

//Program begin
int PgmRun = 0;

//instruction display
int instruction = 1;

int cost = 0;

int parent[26];

int flag = 0;

//Create node struct
struct node
{
	int id;
	int xCoordinate;
	int yCoordinate;
}nodes[26], lineNodes[2], sourceNode, destinationNode;

//return distance between two nodes
int computeDistance(struct node a, struct node b)
{
	return (int)sqrt(pow(abs(a.xCoordinate - b.xCoordinate), 2) + pow(abs(a.yCoordinate - b.yCoordinate), 2));
}
//Check if two nodes intersect i.e if to create a new node or select a node 
int noRepeat(int x, int y)
{
	//if no nodes on screen, dont bother
	if (!nodeCount)
		return 1;

	//create a temp node for comparison on the clicked coordinate
	struct node temp = { nodeCount, x, y };

	for (int i = 0; i < nodeCount; i++)
	{
		//if the nodes intersect, assume a node is clicked
		if (computeDistance(temp, nodes[i]) < 2 * radius)
		{
			//push the clicked node to the stack
			lineNodes[lineNodesCount] = nodes[i];
			return 0;
		}
	}
	//no node was pressed, create a new node at x,y
	return 1;
}

//get perpendicular coordinates to display the text
void getPerpendicularCoordinates(int* x, int* y)
{
	int x1, x2, y1, y2;

	x1 = lineNodes[0].xCoordinate;
	x2 = lineNodes[1].xCoordinate;
	y1 = lineNodes[0].yCoordinate;
	y2 = lineNodes[1].yCoordinate;

	float diff = (x2 - x1) ? 0.0001 : x2 - x1;

	float slope = ((float)(y2 - y1)) / (diff);

	float perpendicularSlope = -1 * (1 / slope);

	*x = *x + 25;
	*y = *y + perpendicularSlope * (*x) + 7;

}

//write Distance between the nodes to screen
void writeDistance(const char* text, int x, int y, float r, float g, float b)
{
	if (instruction)
		glColor3f(r, g, b);

	glRasterPos2i(x, y);
	int len = strlen(text);
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
	}
	glFlush();
}

//write Label to nodes to screen
void writeLabel(char text, int x, int y)
{
	//change the color to yellow
	glColor3f(1.0, 1.0, 0.0);
	glRasterPos2i(x, y);

	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text);
}

//draw Circle at the given coordinates
void drawCircle(float cx, float cy, float r, float num_segments, char colorCharacter)
{
	//theta- angle in radians of the point 
	//x- x coordinate of point
	//y- y coordinate of point
	float theta, x, y;

	//Give the center of the circle a label [A-Z]
	if (colorCharacter == 'N')
		writeLabel((char)65 + nodeCount, cx - 5, cy - 14);

	//change the color to green if colorCharacter = 'N'
	if (colorCharacter == 'N')
		glColor3f(0.0, 1.0, 0.0);

	//line loop- connect all the points below 
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < num_segments; i++) {
		//compute new angle and get the x, y coordinate
		theta = 2.0f * PI * i / num_segments;
		x = r * cosf(theta);
		y = r * sinf(theta);

		//draw the point
		glVertex2f(x + cx, y + cy);
	}
	glEnd();

	//flush to output screen
	glFlush();

}

//sets the mid point coordinates to the arguments
void getMidPoint(int* midX, int* midY)
{
	*midX = (lineNodes[0].xCoordinate + lineNodes[1].xCoordinate) / 2;
	*midY = (lineNodes[0].yCoordinate + lineNodes[1].yCoordinate) / 2;
}
int input()
{
	int a = 0;
	printf("Enter the edge cost :");
	scanf("%d", &a);
	return a;
}

//draw lines between two  nodes
void drawLine(char color)
{
	//set color to white
	if (color == 'N' || color == 'R')
		glColor3f(0.3, 0.3, 0.3);

	if (color == 'D')
		glColor3f(1.0, 0.0, 0.0);

	if (color == 'M')
		glColor3f(0.0, 0.0, 1.0);

	//draw lines between selected nodes (stored in lineNodes)
	glBegin(GL_LINES);
	glVertex2f(lineNodes[0].xCoordinate, lineNodes[0].yCoordinate);
	glVertex2f(lineNodes[1].xCoordinate, lineNodes[1].yCoordinate);
	glEnd();
	glFlush();

	//Mid Point of the line segment to display distance
	int midX, midY;
	char distanceString[10];
	//get the mid coordinates 
	getMidPoint(&midX, &midY);

	//get nodes id, keys for the adjMatrix 
	int id1 = lineNodes[0].id;
	int id2 = lineNodes[1].id;
	//compute distance between the nodes.
	if (!PgmRun)
	{
		cost = input();

		//store it in adjMatrix
		adjMatrix[id1][id2] = cost;
		adjMatrix[id2][id1] = cost;

		//Convert distance to string
		sprintf(distanceString, "%d", cost);
		//Display to the window
		getPerpendicularCoordinates(&midX, &midY);
		writeDistance(distanceString, midX, midY, 1.0, 1.0, 0.0);
	}
	glFlush();

	//write both circles' labels again
	writeLabel('A' + lineNodes[0].id, lineNodes[0].xCoordinate - 5, lineNodes[0].yCoordinate - 14);
	writeLabel('A' + lineNodes[1].id, lineNodes[1].xCoordinate - 5, lineNodes[1].yCoordinate - 14);

	glFlush();

	//Display the updated matrix to the console
	if (color == 'N')
	{
		printf("\n");
		for (int i = 0; i < nodeCount; i++)
		{
			for (int j = 0; j < nodeCount; j++)
				printf("%d \t", adjMatrix[i][j]);

			printf("\n");
		}
	}
}

//mouse click handler
void myMouse(int btn, int state, int x, int y)
{
	if (flag == 1) {
		//Clear Instructions first
		if (instruction)
		{
			instruction = 0;
			glClear(GL_COLOR_BUFFER_BIT);
		}

		//dont listen to mouse events if algorithm is running
		if (PgmRun)
			return;

		//fix coordinate system
		y = wh - y;

		//left click: create nodes, join nodes
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			//if not clicked on the circle 
			if (noRepeat(x, y))
			{
				//dont draw if nodeCount > 26;
				if (nodeCount >= 26)
					return;

				//draw new circle
				drawCircle(x, y, radius, 200, 'N');
				lineNodesCount = 0;
				//set the values of the new node
				nodes[nodeCount].xCoordinate = x;
				nodes[nodeCount].yCoordinate = y;
				nodes[nodeCount].id = nodeCount;

				//increase the node count
				nodeCount++;
			}
			//draw line
			else
			{
				if (lineNodesCount == 1)
				{
					drawLine('N');
					lineNodesCount = 0;
				}
				else
				{
					lineNodesCount += 1;
				}
			}
		}
	}
}

//myReshape to Handle change in window size 
void myReshape(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
	ww = w;
	wh = h;
}

//window initilization
void myInit()
{
	glViewport(0, 0, ww, wh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, ww, 0, wh);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(85.0 / 255.0, 203.0 / 255.0, 171.0 / 255.0, 1);
	glColor3f(1.0, 1.0, 1.0);
	// glPointSize(0);
	glLineWidth(5);
}

void frontscreen() {
	glClear(GL_COLOR_BUFFER_BIT);
	writeDistance("RNS INSTITUTE OF TECHNOLOGY", 300, 550, 1.0, 1.0, 1.0);
	writeDistance("DEPARTMENT OF COMPUTER SCIENCE AND ENGINEERING", 200, 520, 1.0, 1.0, 1.0);
	writeDistance("A MINI PROJECT ON MST VISUALIZER BY:", 270, 400, 1.0, 1.0, 1.0);
	writeDistance("ARJUNA REDDY G (1RN17CS017)", 100, 340, 1.0, 1.0, 1.0);
	writeDistance("6th SEMESTER, CSE", 100, 310, 1.0, 1.0, 1.0);
	writeDistance("BHARATH G SHENOY (1RN17CS026)", 550, 340, 1.0, 1.0, 1.0);
	writeDistance("6th SEMESTER, CSE", 550, 310, 1.0, 1.0, 1.0);
	writeDistance("GUIDE:", 350, 240, 1.0, 1.0, 1.0);
	writeDistance("MAMATA JAJUR", 430, 240, 1.0, 1.0, 1.0);
	writeDistance("ASSISTANT PROFESSOR, CSE", 430, 210, 1.0, 1.0, 1.0);
	glColor3f(1, 0, 0);
	writeDistance("PRESS s TO START", 350, 120, 1.0, 0.0, 0.0);
	glFlush();
}

//Function to display instructions
void display_hello()
{
	writeDistance(" Welcome To The Simulation of Prim's and Kruskal's Algorithn.", 200, 500, 1.0, 1.0, 1.0);
	writeDistance("#  Use Mouse Left Button to Create Nodes.", 250, 470, 1.0, 1.0, 1.0);
	writeDistance("#  Select any two Nodes to form a Path.", 250, 440, 1.0, 1.0, 1.0);
	writeDistance("#  Enter the cost in the terminal.", 250, 410, 1.0, 1.0, 1.0);
	writeDistance("#  Press '1' for Prim's Algorithm.", 250, 380, 1.0, 1.0, 1.0);
	writeDistance("#  Press '2' for Kruskal's Algorithm.", 250, 350, 1.0, 1.0, 1.0);
	writeDistance("#  Press '0' to Exit.", 250, 320, 1.0, 1.0, 1.0);

}

//display function
void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (flag == 0)
		frontscreen();
	if (flag == 1)
		display_hello();
	glFlush();

}

//fix adjacency matrix: set the distance among the nodes to highest value (99999)
void fixAdjMatrix()
{
	for (int i = 0; i < maxNodes; i++)
	{
		for (int j = 0; j < maxNodes; j++)
		{
			adjMatrix[i][j] = 99999;
		}
	}
}

void delay(int number_of_seconds)
{
	// Converting time into milli_seconds
	int milli_seconds = 1000 * number_of_seconds;

	// Stroing start time
	clock_t start_time = clock();

	// looping till required time is not acheived
	while (clock() < start_time + milli_seconds);
}
// Function to implement prims algorithm
void prim()
{
	printf("\nPrims\n");
	int visited[26];
	PgmRun = 1;
	int n = nodeCount;
	for (int i = 0; i < n; i++)
	{
		visited[i] = false;
	}

	printf("Enter Source Vertex :");
	int srcVertex;
	scanf("%d", &srcVertex);

	visited[srcVertex - 1] = true;

	int source = 0, cost = 0, target = 0;
	printf("Edges: ");

	for (int i = 1; i < n; i++)
	{
		int min = 99999;
		
		for (int j = 0; j < n; j++)
		{
			if (visited[j])
			{
				for (int k = 0; k < n; k++)
				{
					if (!visited[k] && min > adjMatrix[j][k])
					{
						min = adjMatrix[j][k];
						source = j;
						target = k;
					}
				}
			}
		}
		visited[target] = true;

		printf("(%d , %d)", (source + 1), (target + 1));
		lineNodes[0] = nodes[source];
		lineNodes[1] = nodes[target];
		delay(1);
		drawLine('M');
		cost += min;
	}
	printf("\nMinimum cost of Spanning Tree: %d", cost);
}

int find(int i)
{
	while (parent[i] != i)
		i = parent[i];
	return i;
}

// Does union of i and j. It returns 
// false if i and j are already in same 
// set. 
void union1(int i, int j)
{
	int a = find(i);
	int b = find(j);
	parent[a] = b;
}

// Finds MST using Kruskal's algorithm 
void kruskal()
{
	printf("\nKruskal\n");
	PgmRun = 1;
	int mincost = 0; 

	// Initialize sets of disjoint sets. 
	for (int i = 0; i < nodeCount; i++)
		parent[i] = i;

	// Include minimum weight edges one by one 
	int edge_count = 0;
	while (edge_count < nodeCount - 1) {
		int min = INT_MAX, a = -1, b = -1;
		for (int i = 0; i < nodeCount; i++) {
			for (int j = 0; j < nodeCount; j++) {
				if (find(i) != find(j) && adjMatrix[i][j] < min) {
					min = adjMatrix[i][j];
					a = i;
					b = j;
				}
			}
		}
		union1(a, b);
		printf("Edge %d:(%d, %d) cost:%d \n", edge_count++, a + 1, b + 1, min);
		lineNodes[0] = nodes[a];
		lineNodes[1] = nodes[b];
		delay(1);
		drawLine('D');
		mincost += min;
	}
	printf("\n Minimum cost= %d \n", mincost);

}

void beginProgram(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 's':if (flag == 0)
		flag = 1;
		myDisplay();
		break;
	case '1':prim();
		break;
	case '2':kruskal();
		break;
	case '0': exit(0);
	}
}

int main(int argc, char** argv)
{
	fixAdjMatrix();
	printf("Hello\n\n");
	glutInit(&argc, argv);
	glutInitWindowSize(900, 600);
	glutInitWindowPosition(200, 100);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow(" Minimum Spanning Tree");
	myInit();
	glutReshapeFunc(myReshape);
	glutKeyboardFunc(beginProgram);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutMainLoop();
	return 0;
}