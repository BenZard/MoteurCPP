//
// example03.cpp
//

// --- Includes --------------------------------------------------------------

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include "Common/EsgiTimer.h"

// --- Classes ----------------------------------------------------------------

struct Vec2
{
	float x, y;
	
	Vec2() 
	{
		printf("Vec2::constructeur vide\n");
	}

	~Vec2() 
	{
		printf("Vec2::destructeur\n");
	}

	Vec2(float _x, float _y) 
	{
		printf("Vec2::constructeur _x = %f,_y = %f\n", _x, _y);
		x = _x; y = _y;
	}

	void copy(Vec2 v) 
	{
		printf("Vec2::copy (%f,%f)\n", v.x, v.y);
		x = v.x; y = v.y;
	}

	Vec2& operator=(const Vec2 &v) 
	{
		printf("Vec2::operator=(%f,%f)\n", v.x, v.y);
		x = v.x; y = v.y;		
		return *this;
	}

	bool operator!=(const Vec2 &v) 
	{
		return ((v.x != x) || (v.y != y));
	}

	static Vec2 g_42;
}; 

Vec2 Vec2::g_42 = Vec2(42.f, 42.f);

// ---

struct ObjectA
{	
	Vec2 value;

	ObjectA() 
	{
		printf("---A::constructeur\n");
	}

	virtual ~ObjectA() 
	{
		printf("ObjectA::destructeur\n");
	}
};

struct ObjectB : public ObjectA
{
	ObjectB()
	{
	}

	ObjectB(Vec2 vec) 
	{
		printf("---B::constructeur par copie\n");
		value.copy(vec);
	}

	~ObjectB() 
	{
		printf("ObjectB::destructeur\n");
	}

	/*ObjectB operator++() const
	{
	}*/
};

// --- Functions -------------------------------------------------------------

int main(int argc, char* argv[])
{
	printf("cpp01.cpp");
	// objets crees dans la pile (stack) ---

	ObjectA A;
	A.value = Vec2::g_42;
	
	ObjectB B(Vec2::g_42);
	
	if (B.value != A.value) {
		printf("A=%s B=%s\n", A.value.x, B.value.x);
	}

	getchar();
	return 0;

}

