#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <SFML/Network.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <time.h>
#include <math.h>

#define TIMESTEP 0.0005
#define SPEED 1/TIMESTEP

using namespace std;

typedef enum
{
	NONE = 0,
	HOR,
	VER,
	TTOP,
	TBOT,
	TLEFT,
	TRIGHT,
	CTL,
	CTR,
	CBL,
	CBR,
	CROSS
	
}tRoadTileType; //yol parçalarý

typedef enum
{
	DOWN = 1,
	LEFT,
	UP,
	RIGHT
	
}tWaypointDir; // yönler

typedef enum
{
	RED = 0,
	YELLOW,
	GREEN 
}tLightState; // ýþýk durumlarý

sf::RenderWindow* window; //iþletim sistemi penceresi açar

class Bus
{
	float x;
	float y;
	float angle;
	bool origin_set;
	sf::Texture texture; // doku
	sf::Sprite sprite; // doku bileþenleri
	vector<int>stops; // duraklarý saklama
	int currentStop; // son durak yeri
	
	public:
		Bus(string image, float x, float y, float angle); // özellikleri
		void move(float &x, float &y, float &angle); // yetenek
		addStop(int index);	// yetenek
};

Bus::Bus(string image, float x, float y, float angle) // ->bus classýndan bus fonksiyonunu çaðýrma
{
	this->x = x;
	this->y = y;
	this->angle = angle;
	if(texture.loadFromFile(image.c_str()) == 0) // resim yükleme
	{
		cout << "Could not load image file." << endl;                           
	}
	
	sprite.setTexture(texture); // dokuyu resme uygula 
	sf::FloatRect boundingBox = sprite.getGlobalBounds(); 									// rectangle box bounds
	sprite.setOrigin(sf::Vector2f(boundingBox.width/2, boundingBox.height/2));			//rectangle origin 
}

void Bus::move(float &x, float &y, float &angle)
{
	this->x = x;
	this->y = y;
	this->angle = angle;                                         
	sprite.setPosition(sf::Vector2f(this->x, this->y));     
	sprite.setRotation(angle);                             
	window->draw(sprite);
}

class Vehicle
{
	float x;
	float y;
	float angle;
	bool origin_set;
	sf::Texture texture;
	sf::Sprite sprite;
	
	public:
		Vehicle(string image, float x, float y, float angle);
		void move(float &x, float &y, float &angle);
};

Vehicle::Vehicle(string image, float x, float y, float angle)
{
	this->x = x;
	this->y = y;
	this->angle = angle;
	if(texture.loadFromFile(image.c_str()) == 0)
	{
		cout << "Could not load image file." << endl;                            // the vehicle was printed on the screen in this section.
	}
	
	sprite.setTexture(texture);
	sf::FloatRect boundingBox = sprite.getGlobalBounds();
	sprite.setOrigin(sf::Vector2f(boundingBox.width/2, boundingBox.height/2));    //The MEDIUM POINT of the vehicle have been assigned to the origin.
}

void Vehicle::move(float &x, float &y, float &angle)              // We failed at this stage. 
{
	this->x = x;
	this->y = y;
	this->angle = angle;                                         
	sprite.setPosition(sf::Vector2f(this->x, this->y));     
	sprite.setRotation(angle);                             
	window->draw(sprite);
}

class RoadTile
{
	float x;
	float y;
	sf::Texture texture;
	sf::Sprite sprite;
	
	public:
		RoadTile(tRoadTileType t, int row, int col);
		void draw();
};

RoadTile::RoadTile(tRoadTileType t, int row,int col)
{
	this->x=col*239; // yol resimleri 239x239 piksel
	this->y=row*239;
	switch (t)
	{
		case CTL:
			texture.loadFromFile("images/roadpieces/corner-topleft.png");
			break;
		case HOR:
			texture.loadFromFile("images/roadpieces/straight-horizontal.png");
			break;
		case TTOP:
			texture.loadFromFile("images/roadpieces/t-top.png");
			break;
		case CTR:                                                                  
			texture.loadFromFile("images/roadpieces/corner-topright.png");
			break;
		case VER:
			texture.loadFromFile("images/roadpieces/straight-vertical.png");
			break;
		case TLEFT:
			texture.loadFromFile("images/roadpieces/t-left.png");
			break;
		case CROSS:
			texture.loadFromFile("images/roadpieces/cross.png");
			break;
		case TRIGHT:
			texture.loadFromFile("images/roadpieces/t-right.png");
			break;
		case CBL:
			texture.loadFromFile("images/roadpieces/corner-bottomleft.png");
			break;
		case TBOT:
			texture.loadFromFile("images/roadpieces/t-bottom.png");
			break;
		case CBR:
			texture.loadFromFile("images/roadpieces/corner-bottomright.png");
			break;
	}
	sprite.setTexture(texture);
	sprite.setPosition(sf::Vector2f(this->x,this->y));
}

void RoadTile::draw()
{
	window->draw(sprite);
}


class Waypoint
{
	float x;
	float y;
	int dir; //direction, olasý gideceði yön
	int next1;
	int next2;
	int next3;
	sf::Texture texture;
	sf::Sprite sprite;
	
	public:
		Waypoint(tWaypointDir dir, tRoadTileType type,int row, int col, int idx, int next1, int next2, int next3);
		int getNext();
		int busNext(); // sonra bak
		void getPosition(float &x, float &y, float &dir) {x = this->x; y = this->y; dir = this->dir;}
		void setPosition(float x, float y, float dir) {this->x = x; this->y = y; this->dir = dir; }
		void draw();
};

Waypoint::Waypoint(tWaypointDir dir, tRoadTileType type,int row, int col, int idx, int next1, int next2, int next3)
{
	this->x = col*239;
	this->y = row*239;
	this->dir = dir;
	this->next1 = next1;
	this->next2 = next2;
	this->next3 = next3;
	switch (type)
	{                                            // Waypoints are printed on screen according to their index number.
		case CTL:
			x += (idx == 0) ? 118 : 218;
			y += (idx == 0) ? 218 : 121;
			break;
		case HOR:
			x += (idx == 0) ? 20 : 172;
			y += (idx == 0) ? 121 : 121;
			break;
		case TTOP:
			if(idx == 0)
			{
				x += 20;
				y += 121;
			}
			if(idx == 1)
			{
				x += 118;
				y += 218;
			}
			if(idx == 2)
			{
				x += 218;
				y += 121;
			}
			break;
		case CTR:
			x += (idx == 0) ? 20 : 118;
			y += (idx == 0) ? 121 : 218;
			break;
		case TLEFT:
			if(idx == 0)
			{
				x += 118;
				y += 20;
			}
			if(idx == 1)
			{
				x += 218;
				y += 121;
			}
			if(idx == 2)
			{
				x += 118;
				y += 218;
			}
			break;
		case CROSS:
			if(idx == 0)
			{
				x += 20;
				y += 121;
			}
			if(idx == 1)
			{
				x += 118;
				y += 20;
			}
			if(idx == 2)
			{
				x += 218;
				y += 121;
			}
			if(idx == 3)
			{
				x += 118;
				y += 218;
			}
			break;
		case VER:
			x += (idx == 0) ? 118 : 118;
			y += (idx == 0) ? 20 : 172;
			break;
		case TRIGHT:
			if(idx == 0)
			{
				x += 118;
				y += 20;
			}
			if(idx == 1)
			{
				x += 20;
				y += 121;
			}
			if(idx == 2)
			{
				x += 118;
				y += 218;
			}
			break;
		case CBL:
			x += (idx == 0) ? 118 : 218;
			y += (idx == 0) ? 20 : 121;
			break;
		case TBOT:
			if(idx == 0)
			{
				x += 20;
				y += 121;
			}
			if(idx == 1)
			{
				x += 118;
				y += 20;
			}
			if(idx == 2)
			{
				x += 218;
				y += 121;
			}
			break;
		case CBR:
			x += (idx == 0) ? 20 : 118;
			y += (idx == 0) ? 121 :	20;				
			break;
	}
	
	switch (dir)
	{
		case UP:
			texture.loadFromFile("images/waypoints/up.png");
			break;
		case DOWN:
			texture.loadFromFile("images/waypoints/down.png");
			break;
		case LEFT:
			texture.loadFromFile("images/waypoints/left.png");
			break;
		case RIGHT:
			texture.loadFromFile("images/waypoints/right.png");
			break;
	}
	
	sprite.setTexture(texture);
	sprite.setPosition(sf :: Vector2f(this->x, this->y));
}

void Waypoint::draw()
{
	window->draw(sprite);
}
// tWaypointDir dir, tRoadTileType type, int row, int col,int idx, int next1, int next2, int next3
int Waypoint::getNext() // sonra bak anlamadým
{
	if(next1 == 6 && next2 == 5)
	{
		return 5 + rand() %2;
	}
	else if(next1 == 17 && next2 == 18)
	{
		return 17 + rand() %2; 
	}
	else if(next1 == 41 && next2 == 43)
	{
		int next = 41 + rand() %3;
		if(next == 42)
		{
			return next1;
		}
		else
		{
			return next;
		}
	}
	else
	{
        return next1;
    }
}
int Waypoint::busNext()
{
	return next1;
}

class TrafficLight
{
	float x;
	float y;
	float dir;
	tLightState state;
//	sf::Texture redTexture;
//	sf::Texture greenTexture;
    sf::Texture texture;
	sf::Sprite sprite;
	TrafficLight *next;
	
	public:
	    TrafficLight(float x, float y, float dir, tLightState state); 
	    void getPosition(float &x, float &y, float &dir);
	    void draw();
	    tLightState getState();
	    void setState(tLightState state);
};

TrafficLight::TrafficLight(float x, float y, float dir, tLightState state)
{
	this->x = x;
	this->y = y;
	this->dir = dir;
	switch(state) 
	{
		case RED:
			texture.loadFromFile("images/trafficlights/red.png");
			break;
		case YELLOW:
			texture.loadFromFile("images/trafficlights/green.png");
			break;
		case GREEN:
			texture.loadFromFile("images/trafficlights/green.png");
			break;
	}
	sprite.setTexture(texture);
	sprite.setPosition(sf::Vector2f(this->x,this->y));
	sprite.setRotation(this->dir *90.0f);
}

void TrafficLight::getPosition(float &x, float &y, float &dir)
{
	x = this->x; 
	y = this->y; 
	dir = this->dir;
}

void TrafficLight::draw()
{
	window->draw(sprite);
}

tLightState TrafficLight::getState(){
	return state;
}

void TrafficLight::setState(tLightState state){
	
	state=state;
}

/////////////////////// TRAFÝK IÞIKLARI 2.CLASS KISMI ///////////////////////
class TrafficLightGroup{
	
	private:
	TrafficLight *next; 
	TrafficLight *head;
	TrafficLight *greenLight;
	float time;
	float duration;
		
	public:
		TrafficLightGroup(float duration);
		void add(TrafficLight *light);
		void simulate(float timestep);
};

TrafficLightGroup::TrafficLightGroup(float duration){
	
	this->duration=duration;
}	

void TrafficLightGroup::add(TrafficLight *light){
	
	light=light;
}

void TrafficLightGroup::simulate(float timestep){
	
	timestep=timestep;
	
}


//BUS STOP

class BusStop
{
	float x;
	float y;
	float dir;
	sf::Texture texture;
	sf::Sprite sprite;
	
	public:
		BusStop(string image, float x, float y, float dir);
		void getPosition(float &x, float &y, float &dir);
		void draw();		
};

BusStop::BusStop(string image, float x, float y, float dir)
{
	this->x = x;
	this->y = y;
	this->dir = dir;	
	if(texture.loadFromFile(image.c_str()) == 0)
	{
		cout << "Could not load image file." << endl;                            // the vehicle was printed on the screen in this section.
	}
	
	sprite.setTexture(texture);
    sprite.setPosition(sf::Vector2f(this->x,this->y));
	sprite.setRotation(this->dir *90.0f);
}

void BusStop::getPosition(float &x, float &y, float &dir)
{
	x = this->x;
	y = this->y;
	dir = this->dir;
}

void BusStop::draw()
{
	window->draw(sprite);
}

int main()
{
	srand(time(NULL));
	float x,y,dir;
	float carx, cary, carangle;
	float bus1x, bus1y, bus1angle;
	float bus2x, bus2y, bus2angle;
	int nextwaypoint = 1;
	
	window = new sf::RenderWindow(sf::VideoMode(1920, 1240), "Traffic Simulator");
	window->setSize(sf::Vector2u(1024, 864));                                       
	sf::Vector2i position(10, 10);
	static_cast<sf::Window *> (window)->setPosition(position);
	
	RoadTile road[] = {{CTL,0,0},{HOR,0,1},{TTOP,0,2},{HOR,0,3},{CTR,0,4},
			//		   {HOR,0,5},{CTR,0,6},
	                   {VER,1,0},{NONE,1,1},{VER,1,2},{NONE,1,3},{VER,1,4},
			//		   {NONE,1,5},{VER,1,6},
					   {TLEFT,2,0},{HOR,2,1},{CROSS,2,2},{HOR,2,3},{TRIGHT,2,4},
			//		   {HOR,2,5},{TRIGHT,2,6},
					   {VER,3,0},{NONE,3,1},{VER,3,2},{NONE,3,3},{VER,3,4},
			//		   {NONE,3,5},{VER,3,6},
					   {CBL,4,0},{HOR,4,1},{TBOT,4,2},{HOR,4,3},{CBR,4,4},
			//		   {HOR,4,5},{CBR,4,6}
			};
	
	Waypoint points[] = {{UP, CTL, 0, 0, 0, 1, -1, -1}, {RIGHT, CTL, 0, 0, 1, 2, -1, -1}, 
						 {RIGHT, HOR, 0, 1, 0, 3, -1, -1}, {RIGHT, HOR, 0, 1, 1, 4, -1, -1}, 
						 {RIGHT, TTOP, 0, 2, 0, 5, 6, -1},{DOWN, TTOP, 0, 2, 1, 13, -1, -1}, {RIGHT, TTOP, 0, 2, 2, 7, -1, -1}, 
						 {RIGHT, HOR, 0, 3 , 0, 8, -1, -1}, {RIGHT, HOR, 0, 3, 1, 9, -1, -1},
//						 {RIGHT, TTOP, 0, 4, 0, 10, 11, -1},{DOWN, TTOP, 0, 4, 1, 21, -1, -1},{RIGHT, TTOP, 0, 4, 2, 12, -1, -1}, 
//						 {RIGHT, HOR, 0, 5, 0, 13, -1, -1}, {RIGHT, HOR, 0, 5, 1, 14, -1, -1},
						 {RIGHT, CTR, 0, 4, 0, 10, -1, -1},{DOWN, CTR, 0, 4, 1, 15, -1, -1}, 
						 
						 {UP, VER, 1, 0, 0, 0, -1, -1}, {UP, VER, 1, 0, 1, 11, -1, -1}, 
						 {DOWN, VER, 1, 2, 0, 14, -1, -1}, {DOWN, VER, 1, 2, 1, 23, -1, -1},
						 {DOWN, VER, 1, 4, 0, 16, -1, -1}, {DOWN, VER, 1, 4, 1, 28, -1, -1},
//						 {DOWN, VER, 1, 6, 0, 24, -1, -1}, {DOWN, VER, 1, 6, 1, 25, -1, -1}, 
						 
						 {UP, TLEFT, 2, 0, 0, 12, -1, -1}, {RIGHT, TLEFT, 2, 0, 1, 20, -1, -1}, {UP, TLEFT, 2, 0, 2, 17, 18, -1},
						 {RIGHT, HOR, 2, 1, 0, 21, -1, -1}, {RIGHT, HOR, 2, 1, 1, 22, -1, -1},  
						 {RIGHT,CROSS, 2, 2, 0, 25, -1, -1}, {DOWN, CROSS, 2, 2, 1, 25, -1, -1}, {LEFT, CROSS, 2, 2, 2, 25, -1, -1},{DOWN, CROSS, 2, 2, 3, 33, -1, -1}, 						 
						 {LEFT,  HOR, 2, 3, 0, 24, -1, -1}, {LEFT, HOR, 2, 3, 1, 26, -1, -1},
//						 {LEFT,CROSS, 2, 4, 0, 36, -1, -1}, {DOWN, CROSS, 2, 4, 1, 36, -1, -1}, {LEFT, CROSS, 2, 4, 2, 36, -1, -1},{DOWN, CROSS, 2, 4, 3, 37, -1, -1}, 
//						 {LEFT,  HOR, 2, 5, 0, 38, -1, -1}, {LEFT, HOR, 2, 5, 1, 39, -1, -1}, 
						 {DOWN, TRIGHT, 2, 4, 0, 29, -1, -1},{LEFT,TRIGHT, 2, 4, 1, 27, -1, -1},{UP, TRIGHT, 2, 4, 2, 29, -1, -1}, 
						 
						 {UP, VER, 3, 0, 0, 19, -1, -1}, {UP, VER, 3, 0, 1, 31, -1, -1}, 
						 {DOWN, VER, 3, 2, 0, 34, -1, -1}, {DOWN, VER, 3, 2, 1, 42, -1, -1},
						 {UP, VER, 3, 4, 0, 30, -1, -1}, {UP, VER, 3, 4, 1, 35, -1, -1}, 
//						 {UP, VER, 3, 6, 0, 48, -1, -1}, {UP, VER, 3, 6, 1, 49, -1, -1},
						 
						 {UP, CBL, 4, 0, 0, 32, -1, -1}, {LEFT, CBL, 4, 0, 1, 37, -1, -1}, 
						 {LEFT, HOR, 4, 1, 0, 38, -1, -1}, {LEFT, HOR, 4, 1, 1, 39, -1, -1},
						 {LEFT, TBOT, 4, 2, 0, 40, -1, -1}, {DOWN, TBOT, 4, 2, 1, 41, 43, -1}, {RIGHT, TBOT, 4, 2, 2, 44, -1, -1}, 
						 {RIGHT, HOR, 4, 3, 0, 45, -1, -1}, {RIGHT, HOR, 4, 3, 1, 46, -1, -1},
//						 {RIGHT, TBOT, 4, 4, 0, 61, -1, -1}, {DOWN, TBOT, 4, 4, 1, 62, 63, -1}, {RIGHT, TBOT, 4, 4, 2, 64, -1, -1}, 
//						 {RIGHT, HOR, 4, 5, 0, 65, -1, -1}, {RIGHT, HOR, 4, 5, 1, 66, -1, -1},
						 {RIGHT, CBR, 4, 4, 0, 47, -1, -1}, {UP, CBR, 4, 4, 1, 36, -1, -1}
						};
	                               // tWaypointDir dir, tRoadTileType type, int row, int col,int idx, int next1, int next2, int next3
		
	points[0].getPosition(x,y,dir);
	carx = x;
	cary = y;
	carangle = dir*90.0f;
	Vehicle car("images/vehicles/car5.png", carx, cary, carangle);
	points[1].getPosition(x,y,dir);
	carx = x;
	cary = y;
	carangle = dir*90.0f;
	Vehicle car2("images/vehicles/car2.png", carx, cary, carangle);	
	
	points[1].getPosition(x,y,dir);
	bus1x = x;
	bus1y = y;
	bus1angle = dir*90.0f;	
	Bus bus[] = {{"images/vehicles/bus.png", bus1x, bus1y, bus1angle}};
	
	TrafficLight lights[] = {{545, 520, LEFT, YELLOW}, {675, 550, UP, GREEN}, {520, 650, DOWN, RED}, {1020, 525, LEFT, GREEN}, {1125, 650, RIGHT, RED}};                 // TRAFÝK IÞIKLARI
	
	BusStop stop[] = {{"images/busstop/busstop.png", 200, 360, DOWN}, {"images/busstop/busstop.png", 875, 210, LEFT}, {"images/busstop/busstop.png", 515, 850, UP},    // ROTASYON ÝÞLEMÝ 90derece
	                  {"images/busstop/busstop.png", 1155, 850, DOWN}, {"images/busstop/busstop.png", 200, 850, DOWN},{"images/busstop/busstop.png", 1155, 360, DOWN}};                                                // farkla çalýþýyor.
                                                                                                                                                                       //90->180,0->90,180->270,270->0
	                  
	while(window->isOpen())
	{
		sf::Event event;
		while(window->pollEvent(event))
		{
			switch(event.type)
			{
				case sf::Event::Closed:
					window->close();
					break;
				case sf::Event::KeyPressed:
					if(event.key.code == sf::Keyboard::Escape)
					window->close();
					break;
			}
		}
		
		window->clear(sf::Color::White);
		
		for(int i = 0 ; i < sizeof(road)/sizeof(RoadTile) ; i++) // yollarý bastýr
		{
			road[i].draw();
		}
		
		for(int i = 0 ; i < sizeof(points)/sizeof(Waypoint) ; i++) //yönleri bastýr
		{
			points[i].draw();
		}
		
		for(int i = 0 ; i < sizeof(lights)/sizeof(TrafficLight); i++) 
		{
			lights[i].draw();
		}
		
		for(int i= 0 ; i < sizeof(stop)/sizeof(BusStop) ; i++)
		{
			stop[i].draw();
		}
		
		points[nextwaypoint].getPosition(x,y,dir);
		
		if(carx != x || cary != y)
		{
			if(carangle == 270.0f)
			{
				cary -= SPEED * TIMESTEP;
			}
			else if(carangle == 360.0f || carangle == 0.0f)
			{
				carx += SPEED * TIMESTEP;
			}
			else if(carangle == 90.0f)
			{
				cary += SPEED * TIMESTEP;
			}
			else if(carangle == 180)
			{
				carx -= SPEED * TIMESTEP;
			}
			
		    if(cary == y)
		    {
			    carangle = dir * 90.0f;			
		    }
		    if(carx == x)
		    {
		    	carangle = dir * 90.0f;
			}
		}

		else
		{
			nextwaypoint = points[nextwaypoint].getNext();
		}
		
		car.move(carx, cary, carangle);
		
		points[nextwaypoint].getPosition(x,y,dir);
		
		if(bus1x != x || bus1y != y)
		{
			if(bus1angle == 270.0f)
			{
				bus1y -= SPEED * TIMESTEP;
			}
			else if(bus1angle == 360.0f || bus1angle == 0.0f)
			{
				bus1x += SPEED * TIMESTEP;
			}
			else if(bus1angle == 90.0f)
			{
				bus1y += SPEED * TIMESTEP;
			}
			else if(bus1angle == 180)
			{
				bus1x -= SPEED * TIMESTEP;
			}
			
		    if(bus1y == y)
		    {
			    bus1angle = dir * 90.0f;			
		    }
		    if(bus1x == x)
		    {
		    	bus1angle = dir * 90.0f;
			}
		}

		else
		{
			nextwaypoint = points[nextwaypoint].busNext();
		}
		
		for(int i=0; i<sizeof(bus)/sizeof(Bus); i++)
		{
		    bus[i].move(bus1x,bus1y,bus1angle);
	    }
		window->display();
		
		sf::sleep(sf::seconds(TIMESTEP));
	}
}
