/*
 * _ObjectBase.h
 *
 *  Created on: Aug 17, 2016
 *      Author: yankai
 */

#ifndef OpenKAI_src_Base__ObjectBase_H_
#define OpenKAI_src_Base__ObjectBase_H_

#include "../Base/common.h"
#include "../Base/_ThreadBase.h"
#include "../Vision/_VisionBase.h"
#include "../Tracker/_SingleTracker.h"

#define OBJECT_N_CLASS 128
#define OBJECT_N_OBJ 256

namespace kai
{

enum detectorMode
{
	det_thread, det_noThread, det_batch
};

struct OBJECT
{
	vDouble4	m_fBBox;
	vInt4		m_bbox;
	double		m_dist;
	vDouble3	m_velo;
	uint64_t	m_trackID;
	double		m_speed;
	_SingleTracker*	m_pTracker;
	vInt2		m_camSize;
	int	 		m_topClass;		//most probable class
	double		m_topProb;
	uint64_t 	m_mClass;		//all candidate class mask
	bool	 	m_bCluster;		//clustered class mask
	int64_t 	m_tStamp;

	void init(void)
	{
		m_fBBox.init();
		m_bbox.init();
		m_camSize.init();
		m_dist = -1.0;
		m_velo.init();
		m_trackID = 0;
		m_speed = 0.0;
		m_pTracker = NULL;
		m_tStamp = -1;
		m_bCluster = false;
		resetClass();
	}

	void addClassIdx(int iClass)
	{
		m_mClass |= 1 << iClass;
	}

	void setClassMask(uint64_t mClass)
	{
		m_mClass = mClass;
	}

	void setTopClass(int iClass, double prob)
	{
		m_topClass = iClass;
		m_topProb = prob;
		addClassIdx(iClass);
	}

	bool bClass(int iClass)
	{
		return (m_mClass & (1 << iClass));
	}

	bool bClassMask(uint64_t mClass)
	{
		return m_mClass & mClass;
	}

	void resetClass(void)
	{
		m_topClass = -1;
		m_topProb = 0.0;
		m_mClass = 0;
	}

	void f2iBBox(void)
	{
		m_bbox.x = m_fBBox.x * m_camSize.x;
		m_bbox.z = m_fBBox.z * m_camSize.x;
		m_bbox.y = m_fBBox.y * m_camSize.y;
		m_bbox.w = m_fBBox.w * m_camSize.y;
	}

	void i2fBBox(void)
	{
		double base;
		base = 1.0/m_camSize.x;
		m_fBBox.x = m_bbox.x * base;
		m_fBBox.z = m_bbox.z * base;
		base = 1.0/m_camSize.y;
		m_fBBox.y = m_bbox.y * base;
		m_fBBox.w = m_bbox.w * base;
	}
};

struct OBJECT_ARRAY
{
	OBJECT m_pObj[OBJECT_N_OBJ];
	int m_nObj;

	void reset(void)
	{
		m_nObj = 0;
	}

	OBJECT* add(OBJECT* pO)
	{
		NULL_N(pO);
		IF_N(m_nObj >= OBJECT_N_OBJ);

		m_pObj[m_nObj++] = *pO;

		return &m_pObj[m_nObj-1];
	}

	OBJECT* at(int i)
	{
		IF_N(i >= m_nObj);
		return &m_pObj[i];
	}

	int size(void)
	{
		return m_nObj;
	}
};

struct OBJECT_DARRAY
{
	OBJECT_ARRAY m_objArr[2];
	OBJECT_ARRAY* m_pPrev;
	OBJECT_ARRAY* m_pNext;
	int m_iSwitch;

	void reset(void)
	{
		m_iSwitch = 0;
		update();
		m_pPrev->reset();
		m_pNext->reset();
	}

	void update(void)
	{
		m_iSwitch = 1 - m_iSwitch;
		m_pPrev = &m_objArr[m_iSwitch];
		m_pNext = &m_objArr[1 - m_iSwitch];

		m_pNext->reset();
	}

	OBJECT* add(OBJECT* pO)
	{
		return m_pNext->add(pO);
	}

	OBJECT* at(int i)
	{
		return m_pPrev->at(i);
	}

	int size(void)
	{
		return m_pPrev->size();
	}
};

struct CLASS_STATISTICS
{
	string  m_name;
	int		m_n;

	void init(void)
	{
		m_n = 0;
		m_name = "";
	}
};

class _ObjectBase: public _ThreadBase
{
public:
	_ObjectBase();
	virtual ~_ObjectBase();

	virtual bool init(void* pKiss);
	virtual bool link(void);
	virtual bool draw(void);
	virtual void update(void);
	virtual bool cli(int& iY);
	virtual int getClassIdx(string& className);
	virtual string getClassName(int iClass);

	void bSetActive(bool bActive);

	OBJECT* add(OBJECT* pNewObj);
	OBJECT* at(int i);
	void merge(_ObjectBase* pO);
	void updateStatistics(void);

	int size(void);
	bool bReady(void);

public:
	//input
	_VisionBase* m_pVision;
	OBJECT_DARRAY m_obj;

	//control
	bool m_bActive;
	bool m_bReady;
	uint64_t m_tStamp;
	detectorMode m_mode;
	uint64_t m_trackID;

	//config
	double m_minOverlap;
	double m_minConfidence;
	double m_minArea;
	double m_maxArea;
	vDouble4 m_roi;

	//model
	string m_modelFile;
	string m_trainedFile;
	string m_meanFile;
	string m_labelFile;
	int	   m_nClass;
	CLASS_STATISTICS m_pClassStatis[OBJECT_N_CLASS];

	//show
	std::bitset<64> m_bitSet;
	bool m_bDrawStatistics;
	vInt3 m_classLegendPos;
	bool m_bDrawSegment;
	double m_segmentBlend;
	double m_drawVeloScale;
	bool m_bDrawObjClass;
	bool m_bDrawObjVelo;

};

}
#endif