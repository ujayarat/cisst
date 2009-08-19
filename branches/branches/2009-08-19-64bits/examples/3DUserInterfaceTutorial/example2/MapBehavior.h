/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: MapBehavior.h 612 2009-07-29 22:38:37Z adeguet1 $

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisst3DUserInterface.h>
#include <list>
#include <limits>

class MapMarker;
struct MarkerType;

class MapBehavior : public ui3BehaviorBase
{
    public:
        MapBehavior(const std::string & name);
        ~MapBehavior();

        void Startup(void);
        void Cleanup(void);
        void ConfigureMenuBar(void);
        bool RunForeground(void);
        bool RunBackground(void);
        bool RunNoInput(void);
        void Configure(const std::string & configFile);
        bool SaveConfiguration(const std::string & configFile);
        inline ui3VisibleObject * GetVisibleObject(void) {
            return this->VisibleList;
        }

    protected:
        unsigned long Ticker;
        void FirstButtonCallback(void);
        void PrimaryMasterButtonCallback(const prmEventButton & event);
        void SecondaryMasterButtonCallback(const prmEventButton & event);
        void MasterClutchPedalCallback(const prmEventButton & payload);
        void CameraControlPedalCallback(const prmEventButton & payload);
        StateType PreviousState;
        bool PreviousMaM;
        vctDouble3 PreviousCursorPosition;
        vctDouble3 Offset;
        vctFrm3 Position;
        bool Following;
        bool Transition;

        ui3SlaveArm * Slave1;
        ui3SlaveArm * ECM1;
        prmPositionCartesianGet Slave1Position;
        prmPositionCartesianGet ECM1Position;

        mtsFunctionRead GetJointPositionSlave;
        mtsFunctionRead GetCartesianPositionSlave;
        mtsFunctionRead GetJointPositionECM;
        prmPositionJointGet JointsSlave;
        prmPositionJointGet JointsECM;

        void UpdateCursorPosition(void);
        vctFrm3 GetCurrentCursorPositionWRTECMRCM(void);
        void UpdateVisibleMap(void);
        void AddMarker();
        void RemoveLastMarker();
        int FindClosestMarker();

        typedef  std::list<MarkerType*> MarkersType;
        MarkersType Markers;




    private:

        ui3VisibleList * VisibleList;
        ui3VisibleObject * MapCursor;
        ui3VisibleList * MarkerList;
        MapMarker * MyMarkers[20];
        bool RightMTMOpen, LeftMTMOpen;
        bool CameraPressed, ClutchPressed;
        bool MarkerDropped, MarkerRemoved;
        int MarkerCount;
        vctDouble3 PreviousSlavePosition;

        vctFrm3 ECMtoECMRCM, ECMRCMtoVTK;
        vctDouble3 centerRotated;
};

