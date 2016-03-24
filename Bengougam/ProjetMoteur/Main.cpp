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
			// Ici on update la position du mesh...
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

		// Si on reçoit un message set ou get : deux cas 
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

		// If the GameObject didn't handle the message but the component
		// did, we return true to signify it was handled by something.
		isSent = isSent | PassMessageToComponents(msg);

		return isSent;
	}
private: // Members
	int m_UniqueID;
	std::list<CoreComponent*> m_Components;
	Vector3 m_Position;
	Vector3 m_Velocity;
	Vector3 m_BoundingBox;  // dx,dy,dz 
 

private: // Methods
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
	// Returns true if the GameObject or any components handled the message
	bool SendMessage(CoreMessage* msg)
	{
		// We look for the GameObject in the scene by its ID
		std::map<int, GameObject*>::iterator objIt = m_GameObjects.find(msg->m_targetGameObject);
		if (objIt != m_GameObjects.end())
		{
			// GameObject was found, so send it the message
			return objIt->second->SendMessage(msg);
		}

		// GameObject with the specified ID wasn't found
		return false;
	}

	GameObject* CreateGameObject()
	{
		GameObject* newObj = new GameObject(nextGameObjectID++);
		m_GameObjects[newObj->GetGameObjectID()] = newObj;

		return newObj;
	}

private:
	std::map<int, GameObject*> m_GameObjects;
	static int nextGameObjectID;
};

// Initialize our static unique GameObjectID generator
int SceneManager::nextGameObjectID = 0;

int main()
{
	// Create a scene manager
	SceneManager sceneMgr;

	// Have scene manager create an GameObject for us, which
	// automatically puts the GameObject into the scene as well
	GameObject* myObj = sceneMgr.CreateGameObject();

	// Create a render component
	RenderComponent* renderComp = new RenderComponent();

	// Attach render component to the GameObject we made
	myObj->AddComponent(renderComp);

	// Set 'myObj' position to (1, 2, 3)
	SetPositionMessage msgSetPos(myObj->GetGameObjectID(), 1.0f, 2.0f, 3.0f);
	sceneMgr.SendMessage(&msgSetPos);
	cout << "Position set to (1, 2, 3) on GameObject with ID: " << myObj->GetGameObjectID() << '\n';

	// Get 'myObj' position to verify it was set properly
	cout << "Retreiving position from GameObject with ID: " << myObj->GetGameObjectID() << '\n';
	GetPositionMessage msgGetPos(myObj->GetGameObjectID());
	sceneMgr.SendMessage(&msgGetPos);
	cout << "X: " << msgGetPos.x << '\n';
	cout << "Y: " << msgGetPos.y << '\n';
	cout << "Z: " << msgGetPos.z << '\n';

	system("Pause");

}