#include "Game2D.h"
#include "fmod.hpp"
#include "WalkingPerson.h"
#include <random>
#include <vector>
#include <memory>
#include <time.h>
#include "ImageObject.h"


#define MaxBulletNumber 300  // 최대 총알 갯수 300
#define MaxEnemyNumber 50  // 최대 적
#define MaxgunitemNumber 20 //총 아이템 최대 수 
#define MaxhealitemNumber 20 //총 아이템 최대 수 
#define MaxSEnemyNumber 50 //총 super 적 수


using namespace jm;

namespace jm
{
	static const auto gravity = vec2(0.0f, -9.8f);


	using namespace std;


	class Random
	{
	public:
		std::mt19937_64 mesenne;
		Random()
		{
			std::random_device rd; //random_device를 만듬
			std::mt19937_64 mesenne(rd()); //random_device를 넣음
		}
		vec2 random() //적출현 좌표random
		{
			int r_n = 0;
			//1부터 6까지 나올 확률이 균일 함
			std::uniform_int_distribution<> dice(1, 6); //확률분포를 이용하여 난수를 만듬
			r_n = dice(mesenne);
			if (r_n == 1)
				return vec2(-1.2f, -0.55f);
			else if (r_n == 2)
				return vec2(-1.2f, 0.2f);
			else if (r_n == 3)
				return vec2(-1.2f, 0.7f);
			else if (r_n == 4)
				return vec2(+1.2f, -0.55f);
			else if (r_n == 5)
				return vec2(+1.2f, 0.2f);
			else if (r_n == 6)
				return vec2(+1.2f, 0.7f);
		}
		int random_move() //적 움직임 random
		{
			int r_n = 0;
			//1부터 180까지 나올 확률이 균일 함
			std::uniform_int_distribution<> dice(1, 180); //확률분포를 이용하여 난수를 만듬
			r_n = dice(mesenne);
			return r_n;
		}
		int enemyshow() //적이 나타낼 확률
		{
			int e_n = 0;
			//1부터 600까지 나올 확률이 균일 함
			std::uniform_int_distribution<> dice(1, 600); //확률분포를 이용하여 난수를 만듬
			e_n = dice(mesenne);
			return e_n;
		}
		int setrandom(int n) //적이 공격할 확률
		{
			int s_n = 0;
			//확률이 균일 함
			std::uniform_int_distribution<> dice(0, n - 1); //확률분포를 이용하여 난수를 만듬
			s_n = dice(mesenne);
			return s_n;
		}
		float getFloat(float input1, float input2)
		{
			float r = 0;
			r = input1 + ((float)rand() / (float)(RAND_MAX)) * input2;
			return r;
		}
	};

	class Particle
	{
	public:
		vec2 pos; //위치
		vec2 vel; //속도
		RGB color{ 0.0f,0.0f, 0.0f }; //색

		float age = 0;
		float life = 0;
		Particle()
		{}
		void update(const float &dt)
		{
			const auto acc = gravity;

			vel += acc * dt;
			pos += vel * dt;

			//update age.
			age += dt;

		}
	};

	class ParticleSystem
	{
	public:
		Random rn;
		std::vector<Particle> particles;

		ParticleSystem()
		{
			reset();
		}

		auto getRandominitVector()
		{
			const float theta = rn.getFloat(0.0f, 3.141592f*2.0f);
			return vec2{ cos(theta),-sin(theta) };
		}

		auto getRandomColor()
		{
			return RGB{ rn.getFloat(0.0f,1.0f),rn.getFloat(0.0f,1.0f),rn.getFloat(0.0f,1.0f) };
		}

		void reset()
		{
			particles.clear();

			for (int i = 0; i < 300; ++i)
			{
				Particle new_particle;
				new_particle.pos = vec2(0.0f, 0.5f) + getRandominitVector() * rn.getFloat(0.001f, 0.03f);
				new_particle.vel = getRandominitVector()* rn.getFloat(0.01f, 1.0f);
				new_particle.color = getRandomColor();
				new_particle.age = 0.0f;
				new_particle.life = rn.getFloat(0.1f, 5.0f);
				particles.push_back(new_particle);
			}
		}

		void update(const float &dt)
		{
			for (auto &pt : particles)
			{
				pt.update(dt);
			}
		}

		void draw()
		{
			for (const auto &pt : particles)
			{
				if (pt.age > pt.life) continue;
				beginTransformation();
				translate(pt.pos);
				drawFilledStar(pt.color, 0.015f, 0.01f);
				endTransformation();
			}
		}
	};


	class item
	{
	public:
		vec2 center = vec2(-0.5f, -0.70f);

		void draw()
		{
			beginTransformation();
			{
				translate(-0.5f, -0.70f);
				drawWiredBox(Colors::black, 0.15f, 0.1f);
				drawFilledStar(Colors::yellow, 0.07f, 0.035f);
			}
			endTransformation();
		}
		void update()
		{
			draw();
		}
	};

	class heal
	{
	public:
		vec2 center = vec2(+0.5f, -0.70f);
		vec2 line1 = vec2(-0.05f, -0.0f);
		vec2 line2 = vec2(+0.05f, -0.0f);
		vec2 line3 = vec2(+0.00f, -0.03f);
		vec2 line4 = vec2(-0.00f, +0.03f);
		void draw()
		{
			beginTransformation();
			{
				translate(+0.5f, -0.70f);
				drawWiredBox(Colors::black, 0.15f, 0.1f);
				setLineWidth(6.0f);
				drawLine(Colors::red, line1, line2);
				drawLine(Colors::red, line3, line4);
			}
			endTransformation();
		}
		void update()
		{
			draw();
		}
	};

	class Mygame : public virtual WalkingPerson, public virtual MyBullet, public Random, public virtual Main, public virtual MySuperBullet, public virtual heal, public virtual item, public virtual SuperEnemy, public virtual SuperBullet
	{
	public:
		const float dt = getTimeStep() * 2.5f;
		FMOD::System *system = nullptr;
		FMOD::Sound *sound[2];
		FMOD::Channel *channel = nullptr;
		FMOD_RESULT result;
		FMOD_RESULT result2;
		Timer timer;
		ParticleSystem ps;

		vec2 line1 = vec2(-0.05f, -0.0f);
		vec2 line2 = vec2(+0.05f, -0.0f);
		vec2 line3 = vec2(+0.00f, -0.05f);
		vec2 line4 = vec2(-0.00f, +0.05f);

		void *extradriverdata = nullptr;

		bool item1 = false; ///////총 종류
		bool item1_time = false;
		bool flag = true; //

		int b_Num = 0;
		int b2_Num = 0;
		int s_b_Num = 0;
		int e_Num = 0;
		int s_e_Num = 0;
		int g_i_Num = 0;
		int h_i_Num = 0;
		int count = 0;

		Random dice;
		Timer time;
		item *gun_item[MaxgunitemNumber];
		heal *heal_item[MaxgunitemNumber];
		MyBullet *bullet[MaxBulletNumber];
		SuperBullet *e_s_bullet[MaxBulletNumber];
		MySuperBullet *s_bullet[MaxBulletNumber];
		WalkingPerson *enemy[MaxEnemyNumber];
		SuperEnemy *super_enemy[MaxSEnemyNumber];
		ImageObject background;
		ImageObject score;
		ImageObject info;
		ImageObject num[10];
		ImageObject dot;
		ImageObject sec;
		ImageObject survival;
		Main person;

		Mygame() : Game2D("This is game made by js!", 1920, 1080, false)
		{
			Soundinit_once(); //PlaySound를 하기위한 함수들 초기화
			background.init("배경3.jpg");
			score.init("score.png");
			info.init("info.png");
			num[0].init("0.png");
			num[1].init("1.png");
			num[2].init("2.png");
			num[3].init("3.png");
			num[4].init("4.png");
			num[5].init("5.png");
			num[6].init("6.png");
			num[7].init("7.png");
			num[8].init("8.png");
			num[9].init("9.png");
			dot.init("dot.png");
			sec.init("sec.png");
			survival.init("survival.png");
			time.start();
			enemyinit();
			s_enemyinit();
			bulletinit();
			e_s_bulletinit();
			s_bulletinit();
			g_iteminit();
			h_iteminit();
		}
		void Soundinit_once()
		{
			result = FMOD::System_Create(&system);
			result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
			result = system->createSound("gunshot.wav", FMOD_LOOP_OFF, 0, &sound[0]);
			result2 = system->createSound("starshot.wav", FMOD_LOOP_OFF, 0, &sound[1]);
		}
		void enemyinit()
		{
			////적 초기화
			for (int i = 0; i < MaxEnemyNumber; i++)
				enemy[i] = nullptr;
			e_Num = 0;
		}
		void s_enemyinit()
		{
			////적 초기화
			for (int i = 0; i < MaxSEnemyNumber; i++)
				super_enemy[i] = nullptr;
			s_e_Num = 0;
		}
		void bulletinit()
		{

			for (int i = 0; i < MaxBulletNumber; i++)
				bullet[i] = nullptr;
			b_Num = 0;
		}
		void e_s_bulletinit()
		{

			for (int i = 0; i < MaxBulletNumber; i++)
				e_s_bullet[i] = nullptr;
			b2_Num = 0;
		}
		void s_bulletinit()
		{

			for (int i = 0; i < MaxBulletNumber; i++)
				s_bullet[i] = nullptr;
			s_b_Num = 0;
		}

		void g_iteminit()
		{

			for (int i = 0; i < MaxgunitemNumber; i++)
				gun_item[i] = nullptr;
			g_i_Num = 0;
		}

		void h_iteminit()
		{
			for (int i = 0; i < MaxgunitemNumber; i++)
				heal_item[i] = nullptr;
			h_i_Num = 0;
		}

		void bulletshot()
		{
			for (int i = 0; i < MaxBulletNumber; i++)
			{
				if (bullet[i] != nullptr)
				{
					bullet[i]->update(getTimeStep());
					bullet[i]->draw();
					if (bullet[i]->center.x <= -3.0f || bullet[i]->center.x >= 3.0f || bullet[i]->center.y >= 2.0f || bullet[i]->center.y <= -2.0f)
					{
						bullet[i] = nullptr;
					}
				}
			}

			for (int i = 0; i < MaxBulletNumber; i++)
			{
				if (e_s_bullet[i] != nullptr)
				{
					e_s_bullet[i]->update(getTimeStep());
					e_s_bullet[i]->draw();
					if (e_s_bullet[i]->center.x <= -3.0f || e_s_bullet[i]->center.x >= 3.0f || e_s_bullet[i]->center.y >= 2.0f || e_s_bullet[i]->center.y <= -2.0f)
					{
						e_s_bullet[i] = nullptr;
					}
				}
			}

			if (item1 == true)
			{
				for (int i = 0; i < MaxBulletNumber; i++)
				{
					if (s_bullet[i] != nullptr)
					{
						s_bullet[i]->update(getTimeStep());
						s_bullet[i]->draw();
						if (s_bullet[i]->center.x <= -3.0f || s_bullet[i]->center.x >= 3.0f || s_bullet[i]->center.y >= 2.0f || s_bullet[i]->center.y <= -2.0f)
						{
							s_bullet[i] = nullptr;
						}
					}
				}
			}
		}
		void drawenemy() //적 움직임
		{
			for (int i = 0; i < MaxEnemyNumber; i++)
			{
				if (enemy[i] != nullptr)
				{
					enemy[i]->draw();
					if (dice.random_move() == 1)
					{
						enemy[i]->center.x -= 7.0f * getTimeStep();
					}
					else if (dice.random_move() == 2)
					{
						enemy[i]->center.x += 7.0f * getTimeStep();
					}
					else if (dice.random_move() == 3)
					{
						enemy[i]->center.x -= 5.0f * getTimeStep();
					}
					else if (dice.random_move() == 4)
					{
						enemy[i]->center.x += 5.0f * getTimeStep();
					}
					else if (dice.random_move() == 5)
					{
						enemy[i]->center.x += 5.0f * getTimeStep();
					}
					else if (dice.random_move() == 6)
					{
						enemy[i]->center.x -= 5.0f * getTimeStep();
					}

					if (enemy[i]->center.x <= -1.875f || enemy[i]->center.x >= 1.875f || (enemy[i]->center.x <= 1.0f && enemy[i]->center.x >= -1.0f))
					{
						enemy[i]->center = dice.random(); //적이 지정된 맵의 범위를 넘어갔을 때
					}
				}
			}

			for (int i = 0; i < MaxSEnemyNumber; i++)
			{
				if (super_enemy[i] != nullptr)
				{
					super_enemy[i]->draw();
					if (dice.random_move() == 1)
					{
						super_enemy[i]->center.x -= 7.0f * getTimeStep();
					}
					else if (dice.random_move() == 2)
					{
						super_enemy[i]->center.x += 7.0f * getTimeStep();
					}
					else if (dice.random_move() == 3)
					{
						super_enemy[i]->center.x -= 5.0f * getTimeStep();
					}
					else if (dice.random_move() == 4)
					{
						super_enemy[i]->center.x += 5.0f * getTimeStep();
					}
					else if (dice.random_move() == 5)
					{
						super_enemy[i]->center.x += 5.0f * getTimeStep();
					}
					else if (dice.random_move() == 6)
					{
						super_enemy[i]->center.x -= 5.0f * getTimeStep();
					}
					if (super_enemy[i]->center.x <= -1.875f || super_enemy[i]->center.x >= 1.875f || (super_enemy[i]->center.x <= 1.0f && super_enemy[i]->center.x >= -1.0f))
					{
						super_enemy[i]->center = dice.random(); //적이 지정된 맵의 범위를 넘어갔을 때
					}
				}
			}
		}
		void collisionT() //충돌감지, item 획득
		{
			//적이 총에 맞을 경우
			for (int j = 0; j < MaxEnemyNumber; ++j)
				for (int i = 0; i < MaxBulletNumber; ++i)
					if (bullet[i] != nullptr && enemy[j] != nullptr)
					{
						if ((abs(bullet[i]->center.x - enemy[j]->center.x) <= 0.05f) && (abs(bullet[i]->center.y - enemy[j]->center.y) <= 0.05f))
						{
							bullet[i] = nullptr;//bullet 초기화
							enemy[j] = nullptr;
							++count;
							////gun item
							if (dice.setrandom(5) == 3)
							{
								gun_item[g_i_Num] = (item*)new item;
								g_i_Num++;
							}

						}
					}


			///super적이 총에 맞을 경우
			for (int j = 0; j < MaxSEnemyNumber; ++j)
				for (int i = 0; i < MaxBulletNumber; ++i)
					if (bullet[i] != nullptr && super_enemy[j] != nullptr)
					{
						if ((abs(bullet[i]->center.x - super_enemy[j]->center.x) <= 0.05f) && (abs(bullet[i]->center.y - super_enemy[j]->center.y) <= 0.05f))
						{
							super_enemy[j]->enegy -= 30;
							bullet[i] = nullptr;//bullet 초기화
							if (super_enemy[j]->enegy <= 0)
							{
								super_enemy[j] = nullptr;
								++count;
							}

							///heal
							if (dice.setrandom(4) == 3)
							{
								heal_item[g_i_Num] = (heal*)new heal;
								h_i_Num++;
							}
						}
					}

			//아이템 총
			if (item1 == true)
			{
				for (int j = 0; j < MaxEnemyNumber; ++j)
					for (int i = 0; i < MaxBulletNumber; ++i)
						if (s_bullet[i] != nullptr && enemy[j] != nullptr)
						{
							if ((abs(s_bullet[i]->center.x - enemy[j]->center.x) <= 0.05f) && (abs(s_bullet[i]->center.y - enemy[j]->center.y) <= 0.05f))
							{
								enemy[j] = nullptr;
								++count;
							}
						}

				for (int j = 0; j < MaxSEnemyNumber; ++j)
					for (int i = 0; i < MaxBulletNumber; ++i)
						if (s_bullet[i] != nullptr && super_enemy[j] != nullptr)
						{
							if ((abs(s_bullet[i]->center.x - super_enemy[j]->center.x) <= 0.05f) && (abs(s_bullet[i]->center.y - super_enemy[j]->center.y) <= 0.05f))
							{
								super_enemy[j]->enegy -= 50;
								if (super_enemy[j]->enegy <= 0)
								{
									super_enemy[j] = nullptr;
									++count;
								}
							}
						}
			}

			//주인공이 총에 맞을 경우
			for (int i = 0; i < MaxBulletNumber; ++i)
				if (bullet[i] != nullptr)
				{
					if ((abs(bullet[i]->center.x - person.center.x) <= 0.05f) && (abs(bullet[i]->center.y - person.center.y) <= 0.05f))
					{
						bullet[i] = nullptr;//bullet 초기화
						person.enegy -= 10;
					}
				}

			for (int i = 0; i < MaxBulletNumber; ++i)
				if (e_s_bullet[i] != nullptr)
				{
					if ((abs(e_s_bullet[i]->center.x - person.center.x) <= 0.05f) && (abs(e_s_bullet[i]->center.y - person.center.y) <= 0.05f))
					{
						e_s_bullet[i] = nullptr;//bullet 초기화
						person.enegy -= 30;
					}
				}

			//주인공 총 아이템 획득
			for (int i = 0; i < MaxgunitemNumber; ++i)
				if (gun_item[i] != nullptr)
				{
					if ((abs(gun_item[i]->center.x - person.center.x) <= 0.1f) && (abs(gun_item[i]->center.y - person.center.y) <= 0.15f))
					{
						gun_item[i] = nullptr;//item 초기화
						item1 = true;
					}
				}
			// 주인공 힐 아이템 획득
			for (int i = 0; i < MaxgunitemNumber; ++i)
				if (heal_item[i] != nullptr)
				{
					if ((abs(heal_item[i]->center.x - person.center.x) <= 0.1f) && (abs(heal_item[i]->center.y - person.center.y) <= 0.15f))
					{
						heal_item[i] = nullptr;//item 초기화
						person.enegy = 1000;
					}
				}
		}

		void fullinit()
		{
			if (e_Num == MaxEnemyNumber) //적의 객체들 초기화
				enemyinit();
			if (b_Num == MaxBulletNumber)//총알의 객체 초기화
				bulletinit();
			if (b2_Num == MaxBulletNumber)
				e_s_bulletinit();
			if (s_b_Num == MaxBulletNumber)
				s_bulletinit();
			if (g_i_Num == MaxgunitemNumber)
				g_iteminit();
			if (h_i_Num == MaxgunitemNumber)
				h_iteminit();
			if (s_e_Num == MaxSEnemyNumber)
				s_enemyinit();
		}

		void gun_item_check()
		{

			if (item1 == true && item1_time == false)
			{
				timer.start();
				item1_time = true;
				bulletinit();
			}
			if (item1_time == true)
			{
				timer.stop();
				if (timer.getElapsedMilli() >= 10000)
				{
					item1 = false;
					item1_time = false;
					s_bulletinit();
				}
			}
		}

		void enemyshow_pro()
		{
			if (dice.enemyshow() == 500) //적이 나타날 확률
			{
				enemy[e_Num] = (WalkingPerson*)new WalkingPerson;
				enemy[e_Num]->center = dice.random();
				e_Num++;
			}

			if (dice.enemyshow() == 500) //super적이 나타날 확률
			{
				super_enemy[s_e_Num] = (SuperEnemy*)new SuperEnemy;
				super_enemy[s_e_Num]->center = dice.random();
				s_e_Num++;
			}
		}

		void enemyattack_pro()
		{
			if (dice.setrandom(70) == 1) //적이 공격할 확률
			{
				int i = dice.setrandom(300);
				for (int j = 0; j < MaxEnemyNumber; ++j)
					if (enemy[j] != nullptr)
					{
						bullet[i] = (MyBullet*)new MyBullet;
						vec2 set(0.0f, -0.1f);
						bullet[i]->center = enemy[j]->center + set;
						vec2 enemyshow = person.center - enemy[j]->center - set;
						bullet[i]->velocity = enemyshow;
					}
			}

			if (dice.setrandom(70) == 1)
			{
				int i = dice.setrandom(300);
				for (int j = 0; j < MaxSEnemyNumber; ++j)
					if (super_enemy[j] != nullptr)
					{
						e_s_bullet[i] = (SuperBullet*)new SuperBullet;
						vec2 set(0.0f, -0.1f);
						e_s_bullet[i]->center = super_enemy[j]->center + set;
						vec2 enemyshow = person.center - super_enemy[j]->center - set;
						e_s_bullet[i]->velocity = enemyshow;
					}
			}
		}

		void mousedraw()
		{
			beginTransformation(); //마우스 Cursor 표시
			{
				const vec2 mouse_pos = getCursorPos(true);
				translate(mouse_pos);
				setLineWidth(0.2f);
				drawWiredCircle(Colors::black, 0.02f);
				drawLine(Colors::red, line1, line2);
				drawLine(Colors::red, line3, line4);
			}
			endTransformation();
		}

		void itemdraw()
		{
			for (int i = 0; i < MaxgunitemNumber; ++i)
			{
				if (gun_item[i] != nullptr)
				{
					gun_item[i]->update();
				}
				if (heal_item[i] != nullptr)
				{
					heal_item[i]->update();
				}
			}
		}
		void scoreshow()
		{
			int i = count;
			int j = count;
			beginTransformation();
			translate(-0.1f, 0.4f);
			scale(0.1f, 0.1f);
			score.draw();
			bool flag = false;
			if (person.center.y > -0.649f)
			{
				translate(+2.5f, 0.0f); //백자리
				if (i >= 100)
				{
					num[i / 100].draw();
					i = i % 100;
				}
				else
					num[0].draw();
				translate(+0.3f, 0.0f); //십자리
				if (i >= 10)
				{
					num[i / 10].draw();
					i = i % 10;
				}
				else
					num[0].draw();
				translate(+0.3f, 0.0f); //일자리
				num[i].draw();
			}
			else
			{
				scale(0.1f, 0.1f);
				translate(+1.5f, 4.0f); //앉을 때 백자리
				if (j >= 100)
				{
					num[j / 100].draw();
					j = j % 100;
				}
				else
					num[0].draw();
				translate(+0.3f, 0.0f); //십자리
				if (j >= 10)
				{
					num[j / 10].draw();
					j = j % 10;
				}
				else
					num[0].draw();
				translate(+0.3f, 0.0f); //일자리
				num[j].draw();
			}
			endTransformation();
		}

		void timeshow()
		{
			int k = time.getElapsedMilli();
			beginTransformation();
			scale(0.1f, 0.1f);
			translate(-2.5f, 9.5f);
			survival.draw();
			translate(+3.5f, 0.0f); //백자리
			if (k >= 100000)
			{
				num[k / 100000].draw();
				k = k % 100000;
				dot.draw();
			}
			else
			{
				num[0].draw();
				dot.draw();
			}
			translate(+0.3f, 0.0f); //십자리
			if (k >= 10000)
			{
				num[k / 10000].draw();
				k = k % 10000;
				//dot.draw();
			}
			else	num[0].draw();
			translate(+0.3f, 0.0f); //일자리
			if (k >= 1000)
			{
				num[k / 1000].draw();
				k = k % 1000;
			}
			else	num[0].draw();
			translate(+0.3f, 0.0f); //0.1자리
			translate(+0.3f, 0.0f); 
			if (k >= 100)
			{
				num[k / 100].draw();
				k = k % 100;
				//dot.draw();
			}
			else	num[0].draw();
			translate(+0.3f, 0.0f); //0.01자리
			if (k >= 100)
			{
				num[k / 100].draw();
				k = k % 100;
			}
			else	num[0].draw();
			translate(+0.3f, 0.0f); //0.001자리
			if (k >= 10)
			{
				num[k / 10].draw();
				k = k % 10;
			}
			else	num[0].draw();
			sec.draw();
			endTransformation();
		}
		void update() override
		{
			background.draw();
			scoreshow();
			drawMymap(Colors::black);

			if (person.enegy >= 0)
			{
				time.stop();
				mousedraw();
				fullinit();
				gun_item_check();
				itemdraw();
				timeshow();
				if (isKeyPressed(GLFW_KEY_LEFT))
					person.center.x -= 0.8f * getTimeStep();
				if (isKeyPressed(GLFW_KEY_RIGHT))
					person.center.x += 0.8f * getTimeStep();
				if (isKeyPressed(GLFW_KEY_DOWN))
				{
					person.center.y = -0.65f;
					if (isKeyPressed(GLFW_KEY_RIGHT))
						person.center.y = -0.649f;
				}
				else
					person.center.y = -0.55f;

				if (isMouseButtonPressedAndReleased(GLFW_MOUSE_BUTTON_1) == true) //주인공이 총쏠때
				{
					if (item1 == false)
					{
						result = system->playSound(sound[0], 0, false, &channel);

						bullet[b_Num] = (MyBullet*)new MyBullet;
						vec2 set(0.00f, +0.1f);
						bullet[b_Num]->center = person.center + set;
						vec2 bulletdir = getCursorPos(true);
						bullet[b_Num]->velocity = bulletdir;
						b_Num++;
					}
					else
					{
						result2 = system->playSound(sound[1], 0, false, &channel);

						s_bullet[s_b_Num] = (MySuperBullet*)new MySuperBullet;
						vec2 set(0.00f, +0.1f);
						s_bullet[s_b_Num]->center = person.center + set;
						vec2 bulletdir = getCursorPos(true);
						s_bullet[s_b_Num]->velocity = bulletdir;
						s_b_Num++;
					}

				}

				enemyshow_pro();
				enemyattack_pro();
				bulletshot();
				drawenemy();
				person.draw();
				collisionT();
			}
			else
			{
				ps.update(dt);
				ps.draw();
				beginTransformation();
				translate(0.0f, -0.5f);
				scale(0.17f, 0.17f);
				info.draw();
				endTransformation();
				timeshow();
				if ((isKeyPressed(GLFW_KEY_R)))
				{
					ps.reset();
					time.reset();
					enemyinit();
					s_enemyinit();
					bulletinit();
					e_s_bulletinit();
					s_bulletinit();
					g_iteminit();
					h_iteminit();
					count = 0;
					person.enegy = 1000;
				}
				else if ((isKeyPressed(GLFW_KEY_F)))
				{
					exit(0);
				}
			}
		}
	};
}

int main(void)
{
	jm::Mygame().run();

	return 0;
}
