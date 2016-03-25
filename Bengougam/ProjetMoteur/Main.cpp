// ProjetMoteur.cpp : définit le point d'entrée pour l'application console.
//
#include "stdafx.h"
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
#include <stdio.h>
#include <list>
#include <map>
#include "assert.h"
#include "Common/EsgiTimer.h"

using namespace std;
struct Vector3
{
public:
	Vector3() : x(0.0f), y(0.0f), z(0.0f)
	{}

	float x, y, z;
};

enum eMsgGameObj
{
	SetPosition,
	GetPosition,
};

class CoreMessage
{
protected: // Classe abstraite pour les messages : un destinataire, un type de message
	CoreMessage(int targetID, eMsgGameObj messageTypeID)
		: m_targetGameObject(targetID)
		, m_messageType(messageTypeID)
	{}

public:
	int m_targetGameObject;
	eMsgGameObj m_messageType;
};

class PositionMessage : public CoreMessage
{
protected: // Classe mère poru les messages de type Get/Set position 
	PositionMessage(int targetID, eMsgGameObj messageType,
		float X = 0.0f, float Y = 0.0f, float Z = 0.0f)
		: CoreMessage(targetID, messageType)
		, x(X)
		, y(Y)
		, z(Z)
	{}

public:
	float x, y, z;
};

class SetPositionMessage : public PositionMessage
{
public:
	SetPositionMessage(int targetID, float X, float Y, float Z)
		: PositionMessage(targetID, SetPosition, X, Y, Z)
	{}
};

class GetPositionMessage : public PositionMessage
{
public:
	GetPositionMessage(int targetID)
		: PositionMessage(targetID, GetPosition)
	{}
};

// Component peut accéder à son parent. 
class Component
{
private:
	int m_ComponentID;
	static int LastComponentID;
public:
	Component()
	{
		m_ComponentID = LastComponentID;
		LastComponentID++;
	}


	virtual bool SendMessage(CoreMessage* msg) { return false; }

	virtual void update(float dt)
	{
		std::cout << m_ComponentID << std::endl;
	}

};


class RenderComponent : public Component
{
public:
	bool SendMessage(CoreMessage* msg)
	{
		if (msg->m_messageType == SetPosition)
		{
			// Ici on pourra mettr eà jour la position du Mesh
			cout << "RenderComponent handling SetPosition\n";
			return true;
		}
		else
		{
			return Component::SendMessage(msg); // False
		}
	}
};

// fonction qui renvoie le component at un index. 

class GameObject
{

private:
	static int LastGameObjectID;
	std::vector<Component*> m_Components;
	Vector3 m_Position;
	Vector3 m_Velocity;
	Vector3 m_BoundingBox;  // dx,dy,dz 
	bool PassMessageToComponents(CoreMessage* msg)
	{
		bool isSent = false;

		std::vector<Component*>::iterator compIt = m_Components.begin();
		for (compIt; compIt != m_Components.end(); ++compIt)
		{
			isSent |= (*compIt)->SendMessage(msg);
		}

		return isSent;
	}

protected:
	int m_GameObjectID;

public:

	GameObject()
	{
		m_GameObjectID = LastGameObjectID;
		LastGameObjectID++;
	}

	virtual void Update(float dt)
	{
		std::cout << m_GameObjectID << std::endl;
	}

	int GetGameObjectID() const { return m_GameObjectID; }

	int GetComponentsSize()
	{
		return m_Components.size();
	}
	
	Component * GetComponentAt(int index)
	{
		if (index < 0 || index >= m_Components.size())
		{
			return NULL;
		}
		else
		{
			return m_Components.at(index);
		}
	}

	void AddComponent(Component* comp)
	{
		m_Components.push_back(comp);
	}

	bool SendMessage(CoreMessage* msg)
	{
		bool isSent = false;

		// Si on reçoit un message set ou get : deux cas à traiter 
		switch (msg->m_messageType)
		{
		case SetPosition:
		{
			SetPositionMessage* msgSetPos = static_cast<SetPositionMessage*>(msg);
			m_Position.x = msgSetPos->x;
			m_Position.y = msgSetPos->y;
			m_Position.z = msgSetPos->z;

			isSent = true;
			cout << "GameObject has SetPosition\n";
		}
		break;
		case GetPosition:
		{
			GetPositionMessage* msgSetPos = static_cast<GetPositionMessage*>(msg);
			msgSetPos->x = m_Position.x;
			msgSetPos->y = m_Position.y;
			msgSetPos->z = m_Position.z;

			isSent = true;
			cout << "GameObject is trying to GetPosition\n";
		}
		break;
		default:
			return PassMessageToComponents(msg);
		}

		// Si le message est traité par un composant on renvoie true
		isSent = isSent | PassMessageToComponents(msg);

		return isSent;
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

	int indexObject;
	int indexComponent;
	std::vector<GameObject*> * objects;
public:

	std::mutex mutex;

	GOIterator(std::vector<GameObject*>* obj) : objects(obj), mutex()
	{
		indexObject = 0;
		indexComponent = 0;
	}
	/*
	GameObject* get()
	{
		if (finished())
		{
			return NULL;
		}

		return (*objects)[index];
	}
	*/
	Component* getComponent()
	{
		if (finished())
		{
			return NULL;
		}

		while (indexComponent >= (*objects)[indexObject]->GetComponentsSize())
		{
			indexObject++;
			indexComponent++;
			if (finished())
			{
				return NULL;
			}
		}

		return (*objects)[indexObject]->GetComponentAt(indexComponent);
	}

	bool finished()
	{
		return indexObject >= objects->size();
	}

	void plusplus()
	{
		if (finished())
		{
			return;
		}

		indexComponent++;
		if (indexComponent >= (*objects)[indexObject]->GetComponentsSize())
		{
			indexComponent = 0;
			indexObject++;
		}
	}
};
// Ajouter components au game object au moment ou je le créé avec une méthode add component (envoyer un component *) 
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
				Component* comp = NULL;
				{
					std::lock_guard<std::mutex> lock(iterator->mutex);

					if (iterator->finished())
					{
						break;
					}

					comp = iterator->getComponent();
					(*iterator).plusplus();
				}

				if (comp != NULL)
				{
					comp->update(0.03);
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
int Component::LastComponentID = 0;
int main(int argc, char* argv[])
{
	GameEngine engine;
	
	for (int i = 0; i < 100; i++)
	{
		RenderComponent* renderer = new RenderComponent();
		GameObject * obj = new GameObject();
		obj->AddComponent(renderer);
		engine.AddObject(obj);
	}

	while (1)
	{
		engine.Update();
	}

	return 0;
}
