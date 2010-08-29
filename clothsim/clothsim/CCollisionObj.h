// TAKEN STRAIGHT FROM HIS CODE!!


class CCollisionObj
{
public:
	bool m_bActive;
	int	 m_iType;
	vector3d m_size;
	vector3d m_center;
	CCollisionObj *m_next;
public:
	CCollisionObj(){m_bActive=true;m_iType=0;m_next=NULL;};
	~CCollisionObj(){};
};