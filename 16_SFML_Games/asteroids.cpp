#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include "Physics.h"

using namespace sf;

const int W = 1200;
const int H = 800;

float DEGTORAD = 0.017453f;

class Animation
{
   public:
   float Frame, speed;
   Sprite sprite;
   std::vector<IntRect> frames;

   Animation(){}

   Animation (Texture &t, int x, int y, int w, int h, int count, float Speed)
   {
     Frame = 0;
     speed = Speed;

     for (int i=0;i<count;i++)
      frames.push_back( IntRect(x+i*w, y, w, h)  );

     sprite.setTexture(t);
     sprite.setOrigin(w/2,h/2);
     sprite.setTextureRect(frames[0]);
   }


   void update()
   {
     Frame += speed;
     int n = frames.size();
     if (Frame >= n) Frame -= n;
     if (n>0) sprite.setTextureRect( frames[int(Frame)] );
   }

   bool isEnd()
   {
     return Frame+speed>=frames.size();
   }

};

enum class EntityType
{
    Player,
    Asteroid,
    Bullet,
    Explosion
};

class Entity
{
   public:
   float x,y,dx,dy,R,angle;
   bool isAlive;
   EntityType type;
   Animation anim;

   Entity()
   {
       isAlive = 1;
   }

   void settings(Animation &a,int X,int Y,float Angle=0,int radius=1)
   {
     anim = a;
     x=X; y=Y;
     angle = Angle;
     R = radius;
   }

   virtual void update(){};

   void draw(RenderWindow &app)
   {
     anim.sprite.setPosition(x,y);
     anim.sprite.setRotation(angle+90);
     app.draw(anim.sprite);

     CircleShape circle(R);
     circle.setFillColor(Color(255,0,0,170));
     circle.setPosition(x,y);
     circle.setOrigin(R,R);
     //app.draw(circle);
   }

   virtual ~Entity(){};
};


class asteroid: public Entity
{
   public:
   asteroid()
   {
     dx=rand()%8-4;
     dy=rand()%8-4;
     type = EntityType::Asteroid;
   }

   void update()
   {
     x+=dx;
     y+=dy;

     if (x>W) x=0;  if (x<0) x=W;
     if (y>H) y=0;  if (y<0) y=H;
   }

};


class bullet: public Entity
{
   public:
   bullet()
   {
       type = EntityType::Bullet;
   }

   void  update()
   {
     dx=cos(angle*DEGTORAD)*6;
     dy=sin(angle*DEGTORAD)*6;
     // angle+=rand()%7-3;  /*try this*/
     x+=dx;
     y+=dy;

     if (x>W || x<0 || y>H || y<0) isAlive = false;
   }

};


class player : public Entity
{
public:
    bool thrust;

    player()
    {
        type = EntityType::Player;
    }

    void update() override
    {
        if (thrust)
            Physics::applyThrust(dx, dy, angle, 0.2f);
        else
            Physics::applyDrag(dx, dy, 0.99f);

        Physics::limitSpeed(dx, dy, 15.0f);

        x += dx;
        y += dy;

        if (x > W) x = 0;
        if (x < 0) x = W;
        if (y > H) y = 0;
        if (y < 0) y = H;
    }
};


bool isCollide(Entity *a,Entity *b)
{
  return (b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
}


void handleAsteroidBulletCollision(Entity* rock, Entity* bullet, std::list<Entity*>& entities, Animation& explosionAnim, Animation& smallRockAnim)
{
    rock->isAlive = false;
    bullet->isAlive = false;

    // Add explosion animation
    Entity* explosion = new Entity();
    explosion->settings(explosionAnim, rock->x, rock->y);
    explosion->type = EntityType::Explosion;
    entities.push_back(explosion);

    // Create 2 smaller asteroids
    for (int i = 0; i < 2; i++)
    {
        if (rock->R == 15) continue;

        asteroid* small = new asteroid();
        small->settings(smallRockAnim, rock->x, rock->y, rand() % 360, 15);
        entities.push_back(small);
    }
}

void handlePlayerAsteroidCollision(player* p, Entity* asteroid, std::list<Entity*>& entities, Animation& explosionAnim, int respawnX, int respawnY)
{
    asteroid->isAlive = false;

    // Create ship explosion
    Entity* explosion = new Entity();
    explosion->settings(explosionAnim, p->x, p->y);
    explosion->type = EntityType::Explosion;
    entities.push_back(explosion);

    // Reset player position
    p->settings(p->anim, respawnX, respawnY, 0, 20);
    p->dx = 0;
    p->dy = 0;
}

int asteroids()
{
    srand(time(0));

    RenderWindow app(VideoMode(W, H), "Asteroids!");
    app.setFramerateLimit(60);

    Texture t1,t2,t3,t4,t5,t6,t7;
    t1.loadFromFile("images/asteroids/spaceship.png");
    t2.loadFromFile("images/asteroids/background.jpg");
    t3.loadFromFile("images/asteroids/explosions/type_C.png");
    t4.loadFromFile("images/asteroids/rock.png");
    t5.loadFromFile("images/asteroids/fire_blue.png");
    t6.loadFromFile("images/asteroids/rock_small.png");
    t7.loadFromFile("images/asteroids/explosions/type_B.png");

    t1.setSmooth(true);
    t2.setSmooth(true);

    Sprite background(t2);

    Animation sExplosion(t3, 0,0,256,256, 48, 0.5);
    Animation sRock(t4, 0,0,64,64, 16, 0.2);
    Animation sRock_small(t6, 0,0,64,64, 16, 0.2);
    Animation sBullet(t5, 0,0,32,64, 16, 0.8);
    Animation sPlayer(t1, 40,0,40,40, 1, 0);
    Animation sPlayer_go(t1, 40,40,40,40, 1, 0);
    Animation sExplosion_ship(t7, 0,0,192,192, 64, 0.5);


    std::list<Entity*> entities;

    for(int i=0;i<15;i++)
    {
      asteroid *a = new asteroid();
      a->settings(sRock, rand()%W, rand()%H, rand()%360, 25);
      entities.push_back(a);
    }

    player *p = new player();
    p->settings(sPlayer,200,200,0,20);
    entities.push_back(p);

    /////main loop/////
    while (app.isOpen())
    {
        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == Event::Closed)
                app.close();

            if (event.type == Event::KeyPressed)
             if (event.key.code == Keyboard::Space)
              {
                bullet *b = new bullet();
                b->settings(sBullet,p->x,p->y,p->angle,10);
                entities.push_back(b);
              }
        }

     const float PLAYER_ROTATION_SPEED = 3.0f;

    if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += PLAYER_ROTATION_SPEED;
    if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle -= PLAYER_ROTATION_SPEED;
    if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust=true;
    else p->thrust=false;
    for (auto a : entities)
        for (auto b : entities)
        {
            if (a == b) continue;

            // Asteroid gets hit by bullet
            if (a->type == EntityType::Asteroid && b->type == EntityType::Bullet)
            {
                if (isCollide(a, b))
                {
                    handleAsteroidBulletCollision(
                        a, b, entities,
                        sExplosion, sRock_small
                    );
                }
            }

            // Player hits asteroid
            if (a->type == EntityType::Player && b->type == EntityType::Asteroid)
            {
                if (isCollide(a, b))
                {
                    handlePlayerAsteroidCollision(
                        p,
                        b,
                        entities,
                        sExplosion_ship,
                        W / 2, H / 2
                    );
                }
            }
        }
    if (p->thrust)  p->anim = sPlayer_go;
    else   p->anim = sPlayer;


    for(auto e:entities)
     if (e->type == EntityType::Explosion)
      if (e->anim.isEnd()) e->isAlive = 0;

    if (rand()%150==0)
     {
       asteroid *a = new asteroid();
       a->settings(sRock, 0,rand()%H, rand()%360, 25);
       entities.push_back(a);
     }

    for(auto i=entities.begin();i!=entities.end();)
    {
      Entity *e = *i;

      e->update();
      e->anim.update();

      if (!e->isAlive) {i=entities.erase(i); delete e;}
      else i++;
    }

   //////draw//////
   app.draw(background);
   for(auto i:entities) i->draw(app);
   app.display();
    }

    return 0;
}
