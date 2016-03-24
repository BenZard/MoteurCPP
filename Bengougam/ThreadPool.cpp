//
// example03.cpp
//

// --- Includes --------------------------------------------------------------

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>
#include "Common/EsgiTimer.h"



class GameObject
{

private:
	static int LastGameObjectID;

protected:
	int id;

public:

	GameObject()
	{
		id = LastGameObjectID;
		LastGameObjectID++;
	}

	virtual void Update(float dt)
	{
		std::cout << id << std::endl;
	}
};

class Creator
{


};

class Factory
{
public:
	GameObject* CreateObject(std::string type);
	void Register(std::string type, Creator *creator);
	std::map<std::string, Creator* > m_Factory;
	std::map < std::string, std::vector<GameObject*> > m_Reserve;
};



class GOIterator
{
	
	int index;
	std::vector<GameObject*> * objects;
public:

	std::mutex mutex;

	GOIterator(std::vector<GameObject*>* obj) : objects(obj), mutex()
	{
		index = 0;
	}

	GameObject* get()
	{
		if (finished())
		{
			return NULL;
		}

		return (*objects)[index];
	}

	bool finished()
	{
		return index >= objects->size();
	}

	void operator++()
	{
		index++;
	}
};

class Worker
{
	std::condition_variable condWork;
	GOIterator *iterator;
	std::atomic<bool> working;
	std::thread thread;
public: 
	
	Worker() : thread(&Worker::work, this)
	{
		working.store(false);	
		iterator = NULL;
	}

	void addWork(GOIterator *p_iterator)
	{
		if (working)
		{
			assert(false);
		}

		iterator = p_iterator;
		working.store(true);
		condWork.notify_all();		
	}

	void waitForEndOfWork()
	{
		while (working)
		{

		}
	}

	void work()
	{
		while (1)
		{
			
			iterator = NULL;
			std::mutex mutex;	
			working.store(false);

			while (!working)
			{
				condWork.wait_for(std::unique_lock<std::mutex>(mutex), std::chrono::milliseconds(10));
			}
		
			while (1)
			{
				GameObject* obj = NULL;
				{
					std::lock_guard<std::mutex> lock(iterator->mutex);

					if (iterator->finished())
					{
						break;
					}

					obj = iterator->get();
					(*iterator)++;
				}

				if (obj != NULL)
				{
					obj->Update(0.03);
				}
			}

		}
	}


};


class Boss
{
	int nb_worker;
	Worker* workers;

public:
	Boss(int p_nb_worker = 8) : nb_worker(p_nb_worker)
	{		
		workers = new Worker[nb_worker];
	}

	~Boss()
	{
		delete[] workers;
	}

	void work(GOIterator *iterator)
	{

		for (int i = 0; i < nb_worker; i++)
		{
			workers[i].addWork(iterator);
		}
		
		for (int i = 0; i < nb_worker; i++)
		{
			workers[i].waitForEndOfWork();
		}
	}
};



class GameEngine
{

private:
	Boss boss;
	std::vector<GameObject*> objects;
public:

	void AddObject(GameObject* obj)
	{
		objects.push_back(obj);
	}

	GameEngine()
	{
	
	}

	void Update()
	{
		GOIterator iterator(&objects);
		boss.work(&iterator);
	}

};

int GameObject::LastGameObjectID = 0;

int main(int argc, char* argv[])
{
	GameEngine engine;

	for (int i = 0; i < 100; i++)
	{
		GameObject * obj = new GameObject();
		engine.AddObject(obj);
	}
	
	while (1)
	{	
		engine.Update();
	}

	return 0;
}

