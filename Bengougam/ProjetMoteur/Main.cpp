// ProjetMoteur.cpp : définit le point d'entrée pour l'application console.
//
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include "Common/EsgiTimer.h"
#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <list>
#include <map>

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

class CoreComponent
{
public:
	virtual bool SendMessage(CoreMessage* msg) { return false; }
};

class RenderComponent : public CoreComponent
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
			return CoreComponent::SendMessage(msg); // False
		}
	}
};

class GameObject
{
public:
	GameObject(int uniqueID): m_UniqueID(uniqueID){	}

	int GetGameObjectID() const { return m_UniqueID; }

	void AddComponent(CoreComponent* comp)
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

private: 
	int m_UniqueID;
	std::list<CoreComponent*> m_Components;
	Vector3 m_Position;
	Vector3 m_Velocity;
	Vector3 m_BoundingBox;  // dx,dy,dz 
 

private:
	bool PassMessageToComponents(CoreMessage* msg)
	{
		bool isSent = false;

		std::list<CoreComponent*>::iterator compIt = m_Components.begin();
		for (compIt; compIt != m_Components.end(); ++compIt)
		{
			isSent |= (*compIt)->SendMessage(msg);
		}

		return isSent;
	}


};

class SceneManager
{
public:
	// Returns true if the GameObject or any components handled the message.
	bool SendMessage(CoreMessage* msg)
	{
	//Recherche du GameObject par son ID. 
		std::map<int, GameObject*>::iterator gameObjIt = m_GameObjects.find(msg->m_targetGameObject);
		if (gameObjIt != m_GameObjects.end())
		{
			// On envoie le message au GameObject.
			return gameObjIt->second->SendMessage(msg);
		}

		// Si aucun GameObject n'a été trouvé.
		return false;
	}

	// Fonction de création de GameObject avec ID. 
	GameObject* CreateGameObject()
	{
		// remplacer le new. 
		GameObject* newObj = new GameObject(nextID++);
		m_GameObjects[newObj->GetGameObjectID()] = newObj;

		return newObj;
	}

private:
	std::map<int, GameObject*> m_GameObjects;
	static int nextID;
};

// Initialization du generateur. 
int SceneManager::nextID = 0;

int main()
{
	SceneManager sceneMgr;
	// On créé un nouveau game object et le place dans la scène
	GameObject* TestGameObject = sceneMgr.CreateGameObject();

	// Creer le component (new à enlever plus rard) 
	RenderComponent* renderer = new RenderComponent();
	TestGameObject->AddComponent(renderer);

	// Test Set Pos
	SetPositionMessage msgSetPos(TestGameObject->GetGameObjectID(), 1.0f, 2.0f, 3.0f);
	sceneMgr.SendMessage(&msgSetPos);
	cout << "Position set to (1, 2, 3) on GameObject with ID: " << TestGameObject->GetGameObjectID() << '\n';

	// Test Get pos
	cout << "Retreiving position from GameObject with ID: " << TestGameObject->GetGameObjectID() << '\n';
	GetPositionMessage msgGetPos(TestGameObject->GetGameObjectID());
	sceneMgr.SendMessage(&msgGetPos);
	cout << "X: " << msgGetPos.x << '\n';
	cout << "Y: " << msgGetPos.y << '\n';
	cout << "Z: " << msgGetPos.z << '\n';

	system("Pause");

}