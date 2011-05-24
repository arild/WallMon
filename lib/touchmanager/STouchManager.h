//
//  DSTouchManager.h
//  II-Demo
//
//  Created by Daniel St�dle on 26/10/2010.
//  Copyright 2010 Yellow Lemon Software. All rights reserved.
//

#ifndef STOUCHMANAGER_H
	#define STOUCHMANAGER_H

#include "WVGeometry.h"
#include "shout/shout.h"
#include "shout/event-types/touch-events.h"
#include <map>
#include <vector>

typedef struct {
	WVPoint2d		loc,
					delta;
	uint32_t		oid;
	float			radius,
					movedDistance,
					time;
	double			lastUpdated;
	bool			wasUpdated,
					remove;
} TT_touch_state_t;

typedef std::map<uint32_t, TT_touch_state_t*>	oidTouchMap_t;
typedef std::vector<TT_touch_state_t*>			touchVector_t;

typedef struct {
	TT_touch_state_t	*obj;
	double				distance;
} TT_object_distance_pair_t;

typedef struct {
	std::vector<TT_object_distance_pair_t>	closest;
	TT_touch_state_t						*newObj;
} TT_touch_pair_t;


typedef void	(*touchManagerCallback_t)(void *refcon, touchVector_t &down, touchVector_t &up);

class STouchManager {
	public:
		STouchManager(void *refcon, touchManagerCallback_t callback, uint32_t wantedSenderID=-1);
		virtual ~STouchManager(void);
		
		static STouchManager*		sharedTouchManager(void);
		void						handleEvent(shout_event_t *evt);
		double						maxDistanceBetweenTouches(size_t &a, size_t &b);
		void						runCallback(void);
		void						setCallback(touchManagerCallback_t newCallback) { lock(); callback = newCallback; unlock(); };
		
		void						lock(void);
		void						unlock(void);
	protected:
		void				updateTouch(TT_touch_state_t *obj, TT_touch_state_t *newObj);
		TT_touch_state_t*	newTouchStateForEvent(shout_event_t *evt, bool &isLast);
		
		void						*refcon;
		oidTouchMap_t				oidTouchMap;
		touchVector_t				touchesDown,
									touchesUp;
		pthread_mutex_t				mgrLock;
		int32_t						wantedSenderID;
		touchManagerCallback_t		callback;
};


#endif
