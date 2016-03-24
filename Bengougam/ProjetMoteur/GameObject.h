#pragma once
/*
class GameObject
{
public:
	GameObject(int uniqueID);

	inline int GetGameObjectID() const { return m_UniqueID; }

	void AddComponent(CoreComponent* comp)
	{
		m_Components.push_back(comp);
	}

	bool SendMessage(CoreMessage* msg)
	{
		bool isSent = false;

		// GameObject has a switch for any messages it cares about
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
*/