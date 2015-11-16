/*
 * Config.h
 *
 *  Created on: Aug 24, 2015
 *      Author: yankai
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#include "../Base/common.h"
#include "../Base/cvplatform.h"

#include "../Autopilot/AutoPilot.h"
#include "../Camera/CamStream.h"

namespace kai
{

class Config
{
public:
	Config();
	virtual ~Config();


	bool setJSON(JSON* pJson);
	bool setCamStream(CamStream* pCamStream);
	bool setAutoPilot(AutoPilot* pAuto);


	JSON* m_pJson;
};

} /* namespace kai */

#endif /* SRC_CONFIG_H_ */