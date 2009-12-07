/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsManagerGlobalTest.cpp 2009-03-05 mjung5 $
  
  Author(s):  Min Yang Jung
  Created on: 2009-11-17
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsManagerGlobalTest.h"

#include <cisstMultiTask/mtsManagerGlobal.h>
#include <string>

#define P1 "P1"
#define P2 "P2"
#define C1 "C1"
#define C2 "C2"
#define C3 "C3"
#define C4 "C4"
#define p1 "p1"
#define p2 "p2"
#define r1 "r1"
#define r2 "r2"

using namespace std;

void mtsManagerGlobalTest::TestAddProcess(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.FindItem(P1));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.GetItem(P1) == NULL);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.size());

    CPPUNIT_ASSERT(!managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.FindItem(P1));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.GetItem(P1) == NULL);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.size());

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P2));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.FindItem(P1));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.FindItem(P2));
    CPPUNIT_ASSERT_EQUAL((unsigned int) 2, managerGlobal.ProcessMap.size());
}

void mtsManagerGlobalTest::TestFindProcess(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(!managerGlobal.FindProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.FindProcess(P1));

    CPPUNIT_ASSERT(managerGlobal.RemoveProcess(P1));
    CPPUNIT_ASSERT(!managerGlobal.FindProcess(P1));
}

void mtsManagerGlobalTest::TestRemoveProcess(void)
{
    mtsManagerGlobal managerGlobal;

    // Case 1. When a process does not have any components
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.FindProcess(P1));

    CPPUNIT_ASSERT(managerGlobal.RemoveProcess(P1));
    CPPUNIT_ASSERT(!managerGlobal.FindProcess(P1));
    CPPUNIT_ASSERT_EQUAL((unsigned int) 0, managerGlobal.ProcessMap.size());

    //
    // TODO: Add tests for cases that a process is removed when the process has
    // multiple components and multiple interfaces.
    //

    // Case 2. When processes with components are registered

    // Case 3. When processes with components that have interfaces are registered

    // Case 4. When processes with components that have interfaces that have connection
    //         with other interfaces are registered
}

void mtsManagerGlobalTest::TestAddComponent(void)
{
    mtsManagerGlobal managerGlobal;

    // Test adding a component without adding a process first
    CPPUNIT_ASSERT(!managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT_EQUAL((unsigned int) 0, managerGlobal.ProcessMap.GetMap().size());

    // Add a process
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    {
        // Check changes in the process map        
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetMap().size());

        // Check changes in the component map
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1));
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(NULL == managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));
    }

    // Test if a same component name in the same process is not allowed
    CPPUNIT_ASSERT(!managerGlobal.AddComponent(P1, C1));

    // Test addind another component
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C2));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetMap().size());

        // Check changes in the component map
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1));
        CPPUNIT_ASSERT_EQUAL((unsigned int) 2, managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(NULL == managerGlobal.ProcessMap.GetItem(P1)->GetItem(C2));
    }
}

void mtsManagerGlobalTest::TestFindComponent(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(!managerGlobal.FindComponent(P1, C1));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(!managerGlobal.FindComponent(P1, C1));

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.FindComponent(P1, C1));

    CPPUNIT_ASSERT(managerGlobal.RemoveComponent(P1, C1));
    CPPUNIT_ASSERT(!managerGlobal.FindComponent(P1, C1));
}

void mtsManagerGlobalTest::TestRemoveComponent(void)
{
    //
    // TODO:
    //
    // Case 1. When only components are registered

    // Case 2. When components that have interfaces are registered

    // Case 3. When components that have interfaces that have connection
    //         with other interfaces are registered
}

void mtsManagerGlobalTest::TestAddProvidedInterface(void)
{
    mtsManagerGlobal managerGlobal;

    // Test adding a provided interface before adding a component
    CPPUNIT_ASSERT(!managerGlobal.AddProvidedInterface(P1, C1, p1));

    // Test adding a provided interface after adding a component
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.AddProvidedInterface(P1, C1, p1));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.GetItem(p1));
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.GetItem(p1));
    }

    // Test if a same provided interface name in the same component is not allowed
    CPPUNIT_ASSERT(!managerGlobal.AddProvidedInterface(P1, C1, p1));

    // Test addind another component
    CPPUNIT_ASSERT(managerGlobal.AddProvidedInterface(P1, C1, p2));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 2, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.GetItem(p2));
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.GetItem(p1));
    }
}

void mtsManagerGlobalTest::TestAddRequiredInterface(void)
{
    mtsManagerGlobal managerGlobal;

    // Test adding a required interface before adding a component
    CPPUNIT_ASSERT(!managerGlobal.AddRequiredInterface(P1, C1, r1));

    // Test adding a required interface after adding a component
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.AddRequiredInterface(P1, C1, r1));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.GetItem(r1));
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.GetItem(r1));
    }

    // Test if a same required interface name in the same component is not allowed
    CPPUNIT_ASSERT(!managerGlobal.AddRequiredInterface(P1, C1, r1));

    // Test addind another component
    CPPUNIT_ASSERT(managerGlobal.AddRequiredInterface(P1, C1, r2));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 2, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.GetItem(r2));
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.GetItem(r1));
    }
}

void mtsManagerGlobalTest::TestFindProvidedInterface(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(!managerGlobal.FindProvidedInterface(P1, C1, p1));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(!managerGlobal.FindProvidedInterface(P1, C1, p1));

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(!managerGlobal.FindProvidedInterface(P1, C1, p1));

    CPPUNIT_ASSERT(managerGlobal.AddProvidedInterface(P1, C1, p1));
    CPPUNIT_ASSERT(managerGlobal.FindProvidedInterface(P1, C1, p1));

    CPPUNIT_ASSERT(managerGlobal.RemoveProvidedInterface(P1, C1, p1));
    CPPUNIT_ASSERT(!managerGlobal.FindProvidedInterface(P1, C1, p1));
}

void mtsManagerGlobalTest::TestFindRequiredInterface(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(!managerGlobal.FindRequiredInterface(P1, C1, r1));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(!managerGlobal.FindRequiredInterface(P1, C1, r1));

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(!managerGlobal.FindRequiredInterface(P1, C1, r1));

    CPPUNIT_ASSERT(managerGlobal.AddRequiredInterface(P1, C1, r1));
    CPPUNIT_ASSERT(managerGlobal.FindRequiredInterface(P1, C1, r1));

    CPPUNIT_ASSERT(managerGlobal.RemoveRequiredInterface(P1, C1, r1));
    CPPUNIT_ASSERT(!managerGlobal.FindRequiredInterface(P1, C1, r1));
}

void mtsManagerGlobalTest::TestRemoveProvidedInterface(void)
{
    mtsManagerGlobal managerGlobal;

    // Case 1. When only interfaces that have no connection are registered
    // Test removing a provided interface before adding a component
    CPPUNIT_ASSERT(!managerGlobal.RemoveProvidedInterface(P1, C1, p1));

    // Test adding a provided interface after adding a component
    CPPUNIT_ASSERT(!managerGlobal.FindProvidedInterface(P1, C1, p1));
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.AddProvidedInterface(P1, C1, p1));

    CPPUNIT_ASSERT(managerGlobal.FindProvidedInterface(P1, C1, p1));
    CPPUNIT_ASSERT(managerGlobal.RemoveProvidedInterface(P1, C1, p1));
    CPPUNIT_ASSERT(!managerGlobal.FindProvidedInterface(P1, C1, p1));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.GetItem(p1));
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.GetItem(p1));
    }

    //
    // TODO:
    // 
    // Case 2. When interfaces have connection with other interfaces
}
         
void mtsManagerGlobalTest::TestRemoveRequiredInterface(void)
{
    mtsManagerGlobal managerGlobal;

    // Case 1. When only interfaces that have no connection are registered
    // Test removing a provided interface before adding a component
    CPPUNIT_ASSERT(!managerGlobal.RemoveRequiredInterface(P1, C1, r1));

    // Test adding a provided interface after adding a component
    CPPUNIT_ASSERT(!managerGlobal.FindRequiredInterface(P1, C1, r1));
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.AddRequiredInterface(P1, C1, r1));

    CPPUNIT_ASSERT(managerGlobal.FindRequiredInterface(P1, C1, r1));
    CPPUNIT_ASSERT(managerGlobal.RemoveRequiredInterface(P1, C1, r1));
    CPPUNIT_ASSERT(!managerGlobal.FindRequiredInterface(P1, C1, r1));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(NULL != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->ProvidedInterfaceMap.GetItem(r1));
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.size());
        CPPUNIT_ASSERT(NULL == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->RequiredInterfaceMap.GetItem(r1));
    }

    //
    // TODO:
    // 
    // Case 2. When interfaces have connection with other interfaces
}

void mtsManagerGlobalTest::TestConnect(void)
{
    mtsManagerGlobal globalManager;
    mtsManagerGlobal::ConnectionMapType * connectionMap;
    mtsManagerGlobal::ConnectedInterfaceInfo * connectedInterfaceInfo;

    //-----------------------------------------------------
    // Test if invalid arguments are handled properly
    //
    CPPUNIT_ASSERT(!globalManager.Connect(P1, C1, r1, P2, C2, p1));

    CPPUNIT_ASSERT(globalManager.AddProcess(P1));
    CPPUNIT_ASSERT(!globalManager.Connect(P1, C1, r1, P2, C2, p1));

    CPPUNIT_ASSERT(globalManager.AddComponent(P1, C1));
    CPPUNIT_ASSERT(!globalManager.Connect(P1, C1, r1, P2, C2, p1));

    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P1, C1, r1));
    CPPUNIT_ASSERT(!globalManager.Connect(P1, C1, r1, P2, C2, p1));

    CPPUNIT_ASSERT(globalManager.AddProcess(P2));
    CPPUNIT_ASSERT(!globalManager.Connect(P1, C1, r1, P2, C2, p1));

    CPPUNIT_ASSERT(globalManager.AddComponent(P2, C2));
    CPPUNIT_ASSERT(!globalManager.Connect(P1, C1, r1, P2, C2, p1));

    CPPUNIT_ASSERT(globalManager.AddProvidedInterface(P2, C2, p1));

    CPPUNIT_ASSERT(globalManager.Connect(P1, C1, r1, P2, C2, p1));

    // Test if connection is established correctly
    // Check required interface's connection information
    connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r1);
    CPPUNIT_ASSERT(connectionMap);

    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
    string interfaceUID = globalManager.GetInterfaceUID(P2, C2, p1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo);

    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == p1);

    // Check provided interface's connection information
    connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p1);
    CPPUNIT_ASSERT(connectionMap);

    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
    interfaceUID = globalManager.GetInterfaceUID(P1, C1, r1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo);

    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == r1);

    //-----------------------------------------------------
    // Test with various configurations to see if 
    // connection  information are managed correctly
    // 
    // Test cases used here are described visually in the project wiki page.
    // (see https://trac.lcsr.jhu.edu/cisst/wiki/Private/cisstMultiTaskNetwork)
    //
    globalManager.CleanUp();

    // Prepare components
    CPPUNIT_ASSERT(globalManager.AddProcess(P1));
    CPPUNIT_ASSERT(globalManager.AddComponent(P1, C1));
    CPPUNIT_ASSERT(globalManager.AddComponent(P1, C2));
    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P1, C1, r1));
    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P1, C1, r2));
    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P1, C2, r1));

    CPPUNIT_ASSERT(globalManager.AddProcess(P2));
    CPPUNIT_ASSERT(globalManager.AddComponent(P2, C2));
    CPPUNIT_ASSERT(globalManager.AddComponent(P2, C3));
    CPPUNIT_ASSERT(globalManager.AddProvidedInterface(P2, C2, p1));
    CPPUNIT_ASSERT(globalManager.AddProvidedInterface(P2, C2, p2));
    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P2, C3, r1));

    // Establish connections
    // Connection: (P1, C1, r1) ~ (P2, C2, p1)
    CPPUNIT_ASSERT(globalManager.Connect(P1, C1, r1, P2, C2, p1));
    // Connection: (P1, C1, r2) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(globalManager.Connect(P1, C1, r2, P2, C2, p2));
    // Connection: (P1, C2, r1) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(globalManager.Connect(P1, C2, r1, P2, C2, p2));
    // Connection: (P2, C3, r1) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(globalManager.Connect(P2, C3, r1, P2, C2, p2));

    // Check if connection information is correct
    mtsManagerGlobal::ConnectionMapType::const_iterator it;

    // P1 : C1 : r1 - P2 : C2 : p1
    connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r1);
    CPPUNIT_ASSERT(connectionMap);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
    for (it = connectionMap->begin(); it != connectionMap->end(); ++it) {
        CPPUNIT_ASSERT(it->second->GetProcessName() == P2);
        CPPUNIT_ASSERT(it->second->GetComponentName() == C2);
        CPPUNIT_ASSERT(it->second->GetInterfaceName() == p1);
    }
    
    // P1 : C1 : r2 - P2 : C2 : p2
    connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r2);
    CPPUNIT_ASSERT(connectionMap);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
    for (it = connectionMap->begin(); it != connectionMap->end(); ++it) {
        CPPUNIT_ASSERT(it->second->GetProcessName() == P2);
        CPPUNIT_ASSERT(it->second->GetComponentName() == C2);
        CPPUNIT_ASSERT(it->second->GetInterfaceName() == p2);
    }

    // P1 : C2 : r1 - P2 : C2 : p2
    connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C2, r1);
    CPPUNIT_ASSERT(connectionMap);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
    for (it = connectionMap->begin(); it != connectionMap->end(); ++it) {
        CPPUNIT_ASSERT(it->second->GetProcessName() == P2);
        CPPUNIT_ASSERT(it->second->GetComponentName() == C2);
        CPPUNIT_ASSERT(it->second->GetInterfaceName() == p2);
    }

    // P2 : C2 : p1 - P1 : C1 : r1
    connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p1);
    CPPUNIT_ASSERT(connectionMap);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
    for (it = connectionMap->begin(); it != connectionMap->end(); ++it) {
        CPPUNIT_ASSERT(it->second->GetProcessName() == P1);
        CPPUNIT_ASSERT(it->second->GetComponentName() == C1);
        CPPUNIT_ASSERT(it->second->GetInterfaceName() == r1);
    }

    // P2 : C2 : p2 - P1 : C1 : r2
    // P2 : C2 : p2 - P1 : C2 : r1
    // P2 : C2 : p2 - P2 : C3 : r1
    connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p2);
    CPPUNIT_ASSERT(connectionMap);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 3, connectionMap->size());
    for (it = connectionMap->begin(); it != connectionMap->end(); ++it) {
        if (it->second->GetProcessName() == P1) {
            if (it->second->GetComponentName() == C1) {
                if (it->second->GetInterfaceName() != r2) {
                    CPPUNIT_ASSERT(false);
                }
            } else if (it->second->GetComponentName() == C2) {
                if (it->second->GetInterfaceName() != r1) {
                    CPPUNIT_ASSERT(false);
                }
            } else {
                CPPUNIT_ASSERT(false);
            }
        } else if (it->second->GetProcessName() == P2) {
            if (it->second->GetComponentName() == C3) {
                if (it->second->GetInterfaceName() != r1) {
                    CPPUNIT_ASSERT(false);
                }
            } else {
                CPPUNIT_ASSERT(false);
            }
        } else {
            CPPUNIT_ASSERT(false);
        }
    }

    //-----------------------------------------------------
    // Test if connection is cleaned up correctly when
    // a required interface is successfully connected to
    // provided interface while a provided interface failed
    // to establish connection with a required interface
    // (for any reason).
    //
}

void mtsManagerGlobalTest::TestDisconnect(void)
{
    mtsManagerGlobal globalManager;

    // Prepare components
    CPPUNIT_ASSERT(globalManager.AddProcess(P1));
    CPPUNIT_ASSERT(globalManager.AddComponent(P1, C1));
    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P1, C1, r1));
    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P1, C1, r2));

    CPPUNIT_ASSERT(globalManager.AddProcess(P2));
    CPPUNIT_ASSERT(globalManager.AddComponent(P2, C2));
    CPPUNIT_ASSERT(globalManager.AddProvidedInterface(P2, C2, p1));
    CPPUNIT_ASSERT(globalManager.AddProvidedInterface(P2, C2, p2));

    // Establish connections
    // (P1, C1, r1) ~ (P2, C2, p1)
    CPPUNIT_ASSERT(globalManager.Connect(P1, C1, r1, P2, C2, p1));
    // (P1, C1, r1) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(globalManager.Connect(P1, C1, r1, P2, C2, p2));
    // (P1, C1, r2) ~ (P2, C2, p1)
    CPPUNIT_ASSERT(globalManager.Connect(P1, C1, r2, P2, C2, p1));
    // (P1, C1, r2) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(globalManager.Connect(P1, C1, r2, P2, C2, p2));

    mtsManagerGlobal::ConnectionMapType * connectionMap;
    connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r1);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 2, connectionMap->size());
    connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r2);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 2, connectionMap->size());
    connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p1);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 2, connectionMap->size());
    connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p2);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 2, connectionMap->size());

    // Check if connection information is correct while disconnecting a connection one by one
    mtsManagerGlobal::ConnectedInterfaceInfo * connectionInfo;

    // Disconnect P1:C1:r1 - P2:C2:p1
    globalManager.Disconnect(P1, C1, r1, P2, C2, p1);
    {
        // Check required interface's connection information
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r1);
        CPPUNIT_ASSERT(connectionMap->begin()->first == globalManager.GetInterfaceUID(P2, C2, p2));
        connectionInfo = connectionMap->begin()->second;
        CPPUNIT_ASSERT(connectionInfo->GetProcessName() == P2);
        CPPUNIT_ASSERT(connectionInfo->GetComponentName() == C2);
        CPPUNIT_ASSERT(connectionInfo->GetInterfaceName() == p2);

        // Check provided interface's connection information
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p1);
        CPPUNIT_ASSERT(connectionMap->begin()->first == globalManager.GetInterfaceUID(P1, C1, r2));
        connectionInfo = connectionMap->begin()->second;
        CPPUNIT_ASSERT(connectionInfo->GetProcessName() == P1);
        CPPUNIT_ASSERT(connectionInfo->GetComponentName() == C1);
        CPPUNIT_ASSERT(connectionInfo->GetInterfaceName() == r2);

        // Check connection count
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 2, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 2, connectionMap->size());
    }

    // Disconnect P1:C1:r1 - P2:C2:p2
    globalManager.Disconnect(P1, C1, r1, P2, C2, p2);
    {
        // Check required interface's connection information
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap->size());

        // Check provided interface's connection information
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());

        CPPUNIT_ASSERT(connectionMap->begin()->first == globalManager.GetInterfaceUID(P1, C1, r2));
        connectionInfo = connectionMap->begin()->second;
        CPPUNIT_ASSERT(connectionInfo->GetProcessName() == P1);
        CPPUNIT_ASSERT(connectionInfo->GetComponentName() == C1);
        CPPUNIT_ASSERT(connectionInfo->GetInterfaceName() == r2);

        // Check connection status
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 2, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
    }

    // Disconnect P1:C1:r2 - P2:C2:p1
    globalManager.Disconnect(P1, C1, r2, P2, C2, p1);
    {
        // Check required interface's connection information
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r2);
        CPPUNIT_ASSERT(connectionMap->begin()->first == globalManager.GetInterfaceUID(P2, C2, p2));
        connectionInfo = connectionMap->begin()->second;
        CPPUNIT_ASSERT(connectionInfo->GetProcessName() == P2);
        CPPUNIT_ASSERT(connectionInfo->GetComponentName() == C2);
        CPPUNIT_ASSERT(connectionInfo->GetInterfaceName() == p2);

        // Check provided interface's connection information
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap->size());

        // Check connection status
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap->size());
    }

    // Disconnect P1:C1:r2 - P2:C2:p2
    globalManager.Disconnect(P1, C1, r2, P2, C2, p2);
    {
        // Check required interface's connection information
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap->size());

        // Check provided interface's connection information
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap->size());

        // Check connection status
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfRequiredInterface(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap->size());
        connectionMap = globalManager.GetConnectionsOfProvidedInterface(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap->size());
    }
}

void mtsManagerGlobalTest::TestIsAlreadyConnected(void)
{
    mtsManagerGlobal globalManager;

    // Because it is assumed that the existence of provided and required interface 
    // has already been checked before calling this method, 
    // FindRequiredInterface() and FindProvidedInterface() should be checked first.

    // Required interface
    CPPUNIT_ASSERT(globalManager.AddProcess(P1));
    CPPUNIT_ASSERT(globalManager.AddComponent(P1, C1));
    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P1, C1, r1));
    // Provided interface
    CPPUNIT_ASSERT(globalManager.AddProcess(P2));
    CPPUNIT_ASSERT(globalManager.AddComponent(P2, C2));
    CPPUNIT_ASSERT(globalManager.AddProvidedInterface(P2, C2, p1));

    // Check if both interfaces exist
    CPPUNIT_ASSERT(globalManager.FindRequiredInterface(P1, C1, r1));
    CPPUNIT_ASSERT(globalManager.FindProvidedInterface(P2, C2, p1));

    // Check if this method can detect connection correctly after connection.
    CPPUNIT_ASSERT(!globalManager.IsAlreadyConnected(P1, C1, r1, P2, C2, p1));
    CPPUNIT_ASSERT(globalManager.Connect(P1, C1, r1, P2, C2, p1));
    CPPUNIT_ASSERT(globalManager.IsAlreadyConnected(P1, C1, r1, P2, C2, p1));

    // Check if this method can detect connection correctly after disconnection.
    globalManager.Disconnect(P1, C1, r1, P2, C2, p1);
    CPPUNIT_ASSERT(!globalManager.IsAlreadyConnected(P1, C1, r1, P2, C2, p1));
}

void mtsManagerGlobalTest::TestGetConnectionsOfProvidedInterface(void)
{
    mtsManagerGlobal globalManager;
    mtsManagerGlobal::ConnectionMapType * connectionMap; 
    mtsManagerGlobal::InterfaceMapType * interfaceMap;
    mtsManagerGlobal::ConnectedInterfaceInfo * connectedInterfaceInfo;

    // 1. Test first type method (with InterfaceMapType argument)
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfProvidedInterface(P1, C1, p1, &interfaceMap));

    CPPUNIT_ASSERT(globalManager.AddProcess(P1));
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfProvidedInterface(P1, C1, p1, &interfaceMap));

    CPPUNIT_ASSERT(globalManager.AddComponent(P1, C1));
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfProvidedInterface(P1, C1, p1, &interfaceMap));

    CPPUNIT_ASSERT(globalManager.AddProvidedInterface(P1, C1, p1));
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfProvidedInterface(P1, C1, p1, &interfaceMap));

    // Add required interface to establish connection
    CPPUNIT_ASSERT(globalManager.AddProcess(P2));
    CPPUNIT_ASSERT(globalManager.AddComponent(P2, C2));
    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P2, C2, r1));

    // Connect two interfaces
    CPPUNIT_ASSERT(globalManager.Connect(P2, C2, r1, P1, C1, p1));

    // Check if connection information is correct
    connectionMap = globalManager.GetConnectionsOfProvidedInterface(P1, C1, p1, &interfaceMap);
    CPPUNIT_ASSERT(connectionMap);
    string interfaceUID = globalManager.GetInterfaceUID(P2, C2, r1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == r1);

    // 2. Test second type method (without InterfaceMapType argument)
    globalManager.CleanUp();

    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfProvidedInterface(P1, C1, p1));

    CPPUNIT_ASSERT(globalManager.AddProcess(P1));
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfProvidedInterface(P1, C1, p1));

    CPPUNIT_ASSERT(globalManager.AddComponent(P1, C1));
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfProvidedInterface(P1, C1, p1));

    CPPUNIT_ASSERT(globalManager.AddProvidedInterface(P1, C1, p1));
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfProvidedInterface(P1, C1, p1));

    // Add required interface to establish connection
    CPPUNIT_ASSERT(globalManager.AddProcess(P2));
    CPPUNIT_ASSERT(globalManager.AddComponent(P2, C2));
    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P2, C2, r1));

    // Connect two interfaces
    CPPUNIT_ASSERT(globalManager.Connect(P2, C2, r1, P1, C1, p1));

    // Check if connection information is correct
    connectionMap = globalManager.GetConnectionsOfProvidedInterface(P1, C1, p1, &interfaceMap);
    CPPUNIT_ASSERT(connectionMap);
    interfaceUID = globalManager.GetInterfaceUID(P2, C2, r1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == r1);
}

void mtsManagerGlobalTest::TestGetConnectionsOfRequiredInterface(void)
{
    mtsManagerGlobal globalManager;
    mtsManagerGlobal::ConnectionMapType * connectionMap; 
    mtsManagerGlobal::InterfaceMapType * interfaceMap;
    mtsManagerGlobal::ConnectedInterfaceInfo * connectedInterfaceInfo;

    // 1. Test first method (with InterfaceMapType as argument)
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfRequiredInterface(P2, C2, r1, &interfaceMap));

    CPPUNIT_ASSERT(globalManager.AddProcess(P2));
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfRequiredInterface(P2, C2, r1, &interfaceMap));

    CPPUNIT_ASSERT(globalManager.AddComponent(P2, C2));
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfRequiredInterface(P2, C2, r1, &interfaceMap));

    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P2, C2, r1));
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfRequiredInterface(P2, C2, r1, &interfaceMap));

    // Add provided interface to establish connection
    CPPUNIT_ASSERT(globalManager.AddProcess(P1));
    CPPUNIT_ASSERT(globalManager.AddComponent(P1, C1));
    CPPUNIT_ASSERT(globalManager.AddProvidedInterface(P1, C1, p1));

    // Connect two interfaces
    CPPUNIT_ASSERT(globalManager.Connect(P2, C2, r1, P1, C1, p1));

    // Check if connection information is correct
    connectionMap = globalManager.GetConnectionsOfRequiredInterface(P2, C2, r1, &interfaceMap);
    CPPUNIT_ASSERT(connectionMap);
    string interfaceUID = globalManager.GetInterfaceUID(P1, C1, p1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == p1);

    // 2. Test second method (without InterfaceMapType as argument)
    globalManager.CleanUp();

    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfRequiredInterface(P2, C2, r1));
                                                                           
    CPPUNIT_ASSERT(globalManager.AddProcess(P2));                          
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfRequiredInterface(P2, C2, r1));
                                                                           
    CPPUNIT_ASSERT(globalManager.AddComponent(P2, C2));                    
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfRequiredInterface(P2, C2, r1));
                                                                           
    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P2, C2, r1));        
    CPPUNIT_ASSERT(NULL == globalManager.GetConnectionsOfRequiredInterface(P2, C2, r1));

    // Add provided interface to establish connection
    CPPUNIT_ASSERT(globalManager.AddProcess(P1));
    CPPUNIT_ASSERT(globalManager.AddComponent(P1, C1));
    CPPUNIT_ASSERT(globalManager.AddProvidedInterface(P1, C1, p1));

    // Connect two interfaces
    CPPUNIT_ASSERT(globalManager.Connect(P2, C2, r1, P1, C1, p1));

    // Check if connection information is correct
    connectionMap = globalManager.GetConnectionsOfRequiredInterface(P2, C2, r1);
    CPPUNIT_ASSERT(connectionMap);
    interfaceUID = globalManager.GetInterfaceUID(P1, C1, p1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == p1);
}

void mtsManagerGlobalTest::TestAddConnectedInterface(void)
{
    mtsManagerGlobal globalManager;

    //-----------------------------------------------------
    // Test if invalid arguments are handled properly
    //
    CPPUNIT_ASSERT(!globalManager.AddConnectedInterface(NULL, P1, C1, r1));

    // Prepare components
    CPPUNIT_ASSERT(globalManager.AddProcess(P1));
    CPPUNIT_ASSERT(globalManager.AddComponent(P1, C1));
    CPPUNIT_ASSERT(globalManager.AddRequiredInterface(P1, C1, r1));

    mtsManagerGlobal::ConnectionMapType connectionMap;
    CPPUNIT_ASSERT_EQUAL((unsigned int) 0, connectionMap.size());

    CPPUNIT_ASSERT(globalManager.AddConnectedInterface(&connectionMap, P1, C1, r1));
    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, connectionMap.size());

    string interfaceUID = globalManager.GetInterfaceUID(P1, C1, r1);
    CPPUNIT_ASSERT(connectionMap.FindItem(interfaceUID));
    
    mtsManagerGlobal::ConnectionMapType::const_iterator it = connectionMap.begin();
    mtsManagerGlobal::ConnectedInterfaceInfo * connectedInterfaceInfo = it->second;

    CPPUNIT_ASSERT_EQUAL(interfaceUID, it->first);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == r1);

    // TODO: REMOVE THIS
    mtsManagerGlobal::AllocatedPointerType::iterator _it = 
        globalManager.AllocatedPointers.find(reinterpret_cast<unsigned int>(connectedInterfaceInfo));
    globalManager.AllocatedPointers.erase(_it);

    // Should release an internal object that AddConnectedInterface() creates dynamically.
    delete connectedInterfaceInfo;
}

CPPUNIT_TEST_SUITE_REGISTRATION(mtsManagerGlobalTest);
