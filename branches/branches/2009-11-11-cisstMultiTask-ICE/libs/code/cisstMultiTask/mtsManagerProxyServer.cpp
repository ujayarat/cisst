/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsManagerProxyServer.cpp 145 2009-03-18 23:32:40Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2010-01-20

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerProxyServer.h>

CMN_IMPLEMENT_SERVICES(mtsManagerProxyServer);

std::string mtsManagerProxyServer::ManagerCommunicatorID = "ManagerServerCommunicator";

//-----------------------------------------------------------------------------
//  Constructor, Destructor, Initializer
//-----------------------------------------------------------------------------
mtsManagerProxyServer::~mtsManagerProxyServer()
{
    //// Add any resource clean-up related methods here, if any.
    //TaskManagerMapType::iterator it = TaskManagerMap.begin();
    //for (; it != TaskManagerMap.end(); ++it) {
    //    delete it->second;
    //}
}

//-----------------------------------------------------------------------------
//  Proxy Start-up
//-----------------------------------------------------------------------------
bool mtsManagerProxyServer::Start(mtsManagerGlobal * proxyOwner)
{
    // Initialize Ice object.
    IceInitialize();
    
    if (!InitSuccessFlag) {
        LogError(mtsManagerProxyServer, "ICE proxy Initialization failed");
        return false;
    }

    // Set the owner of this proxy object
    SetProxyOwner(proxyOwner);

    // Register this proxy to the owner
    if (!proxyOwner->AddProcessObject(this, true)) {
        LogError(mtsManagerProxyServer, "Failed to register proxy server to the global component manager");
        return false;
    }

    // Create a worker thread here and returns immediately.
    //ThreadArgumentsInfo.ProxyOwner = proxyOwner;
    ThreadArgumentsInfo.Proxy = this;
    ThreadArgumentsInfo.Runner = mtsManagerProxyServer::Runner;

    // Note that a worker thread is created but is not yet running.
    WorkerThread.Create<ProxyWorker<mtsManagerGlobal>, ThreadArguments<mtsManagerGlobal>*>(
        &ProxyWorkerInfo, &ProxyWorker<mtsManagerGlobal>::Run, &ThreadArgumentsInfo,
        // Set the name of this thread as GMPS which means Global
        // Manager Proxy Server. Such a very short naming rule is
        // because sometimes there is a limitation of the total number 
        // of characters as a thread name on some systems (e.g. LINUX RTAI).
        "GMPS");

    return true;
}

void mtsManagerProxyServer::StartServer()
{
    Sender->Start();

    // This is a blocking call that should be run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsManagerProxyServer::Runner(ThreadArguments<mtsManagerGlobal> * arguments)
{
    mtsManagerProxyServer * ProxyServer = 
        dynamic_cast<mtsManagerProxyServer*>(arguments->Proxy);
    if (!ProxyServer) {
        CMN_LOG_RUN_ERROR << "mtsManagerProxyServer: Failed to create a proxy server." << std::endl;
        return;
    }

    ProxyServer->GetLogger()->trace("mtsManagerProxyServer", "Proxy server starts.....");

    try {
        ProxyServer->SetAsActiveProxy();
        ProxyServer->StartServer();
    } catch (const Ice::Exception& e) {
        std::string error("mtsManagerProxyServer: ");
        error += e.what();
        ProxyServer->GetLogger()->error(error);
    } catch (const char * msg) {
        std::string error("mtsManagerProxyServer: ");
        error += msg;
        ProxyServer->GetLogger()->error(error);
    }

    ProxyServer->GetLogger()->trace("mtsManagerProxyServer", "Proxy server terminates.....");

    ProxyServer->Stop();
}

void mtsManagerProxyServer::Stop()
{
    BaseServerType::Stop();

    Sender->Stop();
}

void mtsManagerProxyServer::OnClose()
{
    //
    //  TODO: Add OnClose() event handler.
    //

    // remove from TaskManagerMapByTaskName
    // remove from TaskManagerClient
    //RemoveTaskManagerByConnectionID();
}

mtsManagerProxyServer::ManagerClientProxyType * mtsManagerProxyServer::GetNetworkProxyClient(const ClientIDType clientID)
{
    ManagerClientProxyType * clientProxy = GetClientByClientID(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "GetNetworkProxyClient: no client proxy connected with client id: " << clientID);
        return NULL;
    }

    //
    // TODO: Check if the network proxy client is active
    //

    // Check if this network proxy server is active
    return (IsActiveProxy() ? clientProxy : NULL);
}

void mtsManagerProxyServer::ConstructConnectionStringSet(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
    ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    connectionStringSet.ClientProcessName = clientProcessName;
    connectionStringSet.ClientComponentName = clientComponentName;
    connectionStringSet.ClientRequiredInterfaceName = clientRequiredInterfaceName;
    connectionStringSet.ServerProcessName = serverProcessName;
    connectionStringSet.ServerComponentName = serverComponentName;
    connectionStringSet.ServerProvidedInterfaceName = serverProvidedInterfaceName;
}

void mtsManagerProxyServer::ConvertProvidedInterfaceDescription(
    const ::mtsManagerProxy::ProvidedInterfaceDescription & src, ProvidedInterfaceDescription & dest)
{
    // Initialize destination structure
    dest.CommandsVoid.clear();
    dest.CommandsWrite.clear();
    dest.CommandsRead.clear();
    dest.CommandsQualifiedRead.clear();
    dest.EventsVoid.clear();
    dest.EventsWrite.clear();

    // Conversion of provided interface name
    dest.ProvidedInterfaceName = src.ProvidedInterfaceName;

    // Conversion of command void vector
    mtsInterfaceCommon::CommandVoidElement commandVoid;
    ::mtsManagerProxy::CommandVoidSequence::const_iterator itVoid = src.CommandsVoid.begin();
    const ::mtsManagerProxy::CommandVoidSequence::const_iterator itVoidEnd = src.CommandsVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandVoid.Name = itVoid->Name;
        dest.CommandsVoid.push_back(commandVoid);
    }

    // Conversion of command write vector
    mtsInterfaceCommon::CommandWriteElement commandWrite;
    ::mtsManagerProxy::CommandWriteSequence::const_iterator itWrite = src.CommandsWrite.begin();
    const ::mtsManagerProxy::CommandWriteSequence::const_iterator itWriteEnd = src.CommandsWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        commandWrite.Name = itWrite->Name;
        commandWrite.ArgumentPrototypeSerialized = itWrite->ArgumentPrototypeSerialized;
        dest.CommandsWrite.push_back(commandWrite);
    }

    // Conversion of command read vector
    mtsInterfaceCommon::CommandReadElement commandRead;
    ::mtsManagerProxy::CommandReadSequence::const_iterator itRead = src.CommandsRead.begin();
    const ::mtsManagerProxy::CommandReadSequence::const_iterator itReadEnd = src.CommandsRead.end();
    for (; itRead != itReadEnd; ++itRead) {
        commandRead.Name = itRead->Name;
        commandRead.ArgumentPrototypeSerialized = itRead->ArgumentPrototypeSerialized;
        dest.CommandsRead.push_back(commandRead);
    }

    // Conversion of command qualified read vector
    mtsInterfaceCommon::CommandQualifiedReadElement commandQualifiedRead;
    ::mtsManagerProxy::CommandQualifiedReadSequence::const_iterator itQualifiedRead = src.CommandsQualifiedRead.begin();
    const ::mtsManagerProxy::CommandQualifiedReadSequence::const_iterator itQualifiedReadEnd = src.CommandsQualifiedRead.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandQualifiedRead.Name = itQualifiedRead->Name;
        commandQualifiedRead.Argument1PrototypeSerialized = itQualifiedRead->Argument1PrototypeSerialized;
        commandQualifiedRead.Argument2PrototypeSerialized = itQualifiedRead->Argument2PrototypeSerialized;
        dest.CommandsQualifiedRead.push_back(commandQualifiedRead);
    }

    // Conversion of event void generator vector
    mtsInterfaceCommon::EventVoidElement eventVoid;
    ::mtsManagerProxy::EventVoidSequence::const_iterator itEventVoid = src.EventsVoid.begin();
    const ::mtsManagerProxy::EventVoidSequence::const_iterator itEventVoidEnd = src.EventsVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoid.Name = itEventVoid->Name;
        dest.EventsVoid.push_back(eventVoid);
    }

    // Conversion of event write generator vector
    mtsInterfaceCommon::EventWriteElement eventWrite;
    ::mtsManagerProxy::EventWriteSequence::const_iterator itEventWrite = src.EventsWrite.begin();
    const ::mtsManagerProxy::EventWriteSequence::const_iterator itEventWriteEnd = src.EventsWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventWrite.Name = itEventWrite->Name;
        eventWrite.ArgumentPrototypeSerialized = itEventWrite->ArgumentPrototypeSerialized;
        dest.EventsWrite.push_back(eventWrite);
    }
}

void mtsManagerProxyServer::ConvertRequiredInterfaceDescription(
    const ::mtsManagerProxy::RequiredInterfaceDescription & src, RequiredInterfaceDescription & dest)
{
    // Initialize destination structure
    dest.FunctionVoidNames.clear();
    dest.FunctionWriteNames.clear();
    dest.FunctionReadNames.clear();
    dest.FunctionQualifiedReadNames.clear();
    dest.EventHandlersVoid.clear();
    dest.EventHandlersWrite.clear();

    // Conversion of required interface name
    dest.RequiredInterfaceName = src.RequiredInterfaceName;
    
    // Conversion of function void vector
    dest.FunctionVoidNames.insert(dest.FunctionVoidNames.begin(), src.FunctionVoidNames.begin(), src.FunctionVoidNames.end());

    // Conversion of function write vector
    dest.FunctionWriteNames.insert(dest.FunctionWriteNames.begin(), src.FunctionWriteNames.begin(), src.FunctionWriteNames.end());

    // Conversion of function read vector
    dest.FunctionReadNames.insert(dest.FunctionReadNames.begin(), src.FunctionReadNames.begin(), src.FunctionReadNames.end());

    // Conversion of function qualified read vector
    dest.FunctionQualifiedReadNames.insert(dest.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.end());

    // Conversion of event void handler vector
    mtsInterfaceCommon::CommandVoidElement eventVoidHandler;
    ::mtsManagerProxy::CommandVoidSequence::const_iterator itEventVoid = src.EventHandlersVoid.begin();
    const ::mtsManagerProxy::CommandVoidSequence::const_iterator itEventVoidEnd = src.EventHandlersVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoidHandler.Name = itEventVoid->Name;
        dest.EventHandlersVoid.push_back(eventVoidHandler);
    }

    // Conversion of event write handler vector
    mtsInterfaceCommon::CommandWriteElement eventWriteHandler;
    ::mtsManagerProxy::CommandWriteSequence::const_iterator itEventWrite = src.EventHandlersWrite.begin();
    const ::mtsManagerProxy::CommandWriteSequence::const_iterator itEventWriteEnd = src.EventHandlersWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventWriteHandler.Name = itEventWrite->Name;
        eventWriteHandler.ArgumentPrototypeSerialized = itEventWrite->ArgumentPrototypeSerialized;
        dest.EventHandlersWrite.push_back(eventWriteHandler);
    }
}

void mtsManagerProxyServer::ConstructProvidedInterfaceDescriptionFrom(
    const ProvidedInterfaceDescription & src, ::mtsManagerProxy::ProvidedInterfaceDescription & dest)
{
    // Initialize destination structure
    dest.CommandsVoid.clear();
    dest.CommandsWrite.clear();
    dest.CommandsRead.clear();
    dest.CommandsQualifiedRead.clear();
    dest.EventsVoid.clear();
    dest.EventsWrite.clear();

    // Construct provided interface name
    dest.ProvidedInterfaceName = src.ProvidedInterfaceName;

    // Construct command void vector
    ::mtsManagerProxy::CommandVoidElement commandVoid;
    CommandVoidVector::const_iterator itVoid = src.CommandsVoid.begin();
    const CommandVoidVector::const_iterator itVoidEnd = src.CommandsVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandVoid.Name = itVoid->Name;
        dest.CommandsVoid.push_back(commandVoid);
    }

    // Construct command write vector
    ::mtsManagerProxy::CommandWriteElement commandWrite;
    CommandWriteVector::const_iterator itWrite = src.CommandsWrite.begin();
    const CommandWriteVector::const_iterator itWriteEnd = src.CommandsWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        commandWrite.Name = itWrite->Name;
        commandWrite.ArgumentPrototypeSerialized = itWrite->ArgumentPrototypeSerialized;
        dest.CommandsWrite.push_back(commandWrite);
    }

    // Construct command read vector
    ::mtsManagerProxy::CommandReadElement commandRead;
    CommandReadVector::const_iterator itRead = src.CommandsRead.begin();
    const CommandReadVector::const_iterator itReadEnd = src.CommandsRead.end();
    for (; itRead != itReadEnd; ++itRead) {
        commandRead.Name = itRead->Name;
        commandRead.ArgumentPrototypeSerialized = itRead->ArgumentPrototypeSerialized;
        dest.CommandsRead.push_back(commandRead);
    }

    // Construct command QualifiedRead vector
    ::mtsManagerProxy::CommandQualifiedReadElement commandQualifiedRead;
    CommandQualifiedReadVector::const_iterator itQualifiedRead = src.CommandsQualifiedRead.begin();
    const CommandQualifiedReadVector::const_iterator itQualifiedReadEnd = src.CommandsQualifiedRead.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandQualifiedRead.Name = itQualifiedRead->Name;
        commandQualifiedRead.Argument1PrototypeSerialized = itQualifiedRead->Argument1PrototypeSerialized;
        commandQualifiedRead.Argument2PrototypeSerialized = itQualifiedRead->Argument2PrototypeSerialized;
        dest.CommandsQualifiedRead.push_back(commandQualifiedRead);
    }

    // Construct event void generator vector
    ::mtsManagerProxy::EventVoidElement eventVoidGenerator;
    EventVoidVector::const_iterator itEventVoid = src.EventsVoid.begin();
    const EventVoidVector::const_iterator itEventVoidEnd = src.EventsVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoidGenerator.Name = itEventVoid->Name;
        dest.EventsVoid.push_back(eventVoidGenerator);
    }

    // Construct event write generator vector
    ::mtsManagerProxy::EventWriteElement eventWriteGenerator;
    EventWriteVector::const_iterator itEventWrite = src.EventsWrite.begin();
    const EventWriteVector::const_iterator itEventWriteEnd = src.EventsWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventWriteGenerator.Name = itEventWrite->Name;
        eventWriteGenerator.ArgumentPrototypeSerialized = itEventWrite->ArgumentPrototypeSerialized;
        dest.EventsWrite.push_back(eventWriteGenerator);
    }
}

void mtsManagerProxyServer::ConstructRequiredInterfaceDescriptionFrom(
    const RequiredInterfaceDescription & src, ::mtsManagerProxy::RequiredInterfaceDescription & dest)
{
    // Initialize destination structure
    dest.FunctionVoidNames.clear();
    dest.FunctionWriteNames.clear();
    dest.FunctionReadNames.clear();
    dest.FunctionQualifiedReadNames.clear();
    dest.EventHandlersVoid.clear();
    dest.EventHandlersWrite.clear();

    // Construct required interface name
    dest.RequiredInterfaceName = src.RequiredInterfaceName;
    
    // Construct function void vector
    dest.FunctionVoidNames.insert(dest.FunctionVoidNames.begin(), src.FunctionVoidNames.begin(), src.FunctionVoidNames.end());

    // Construct function write vector
    dest.FunctionWriteNames.insert(dest.FunctionWriteNames.begin(), src.FunctionWriteNames.begin(), src.FunctionWriteNames.end());

    // Construct function read vector
    dest.FunctionReadNames.insert(dest.FunctionReadNames.begin(), src.FunctionReadNames.begin(), src.FunctionReadNames.end());

    // Construct function qualified read vector
    dest.FunctionQualifiedReadNames.insert(dest.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.end());

    // Construct event void handler vector
    ::mtsManagerProxy::CommandVoidElement eventVoidHandler;
    mtsInterfaceCommon::CommandVoidVector::const_iterator itEventVoid = src.EventHandlersVoid.begin();
    const mtsInterfaceCommon::CommandVoidVector::const_iterator itEventVoidEnd = src.EventHandlersVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoidHandler.Name = itEventVoid->Name;
        dest.EventHandlersVoid.push_back(eventVoidHandler);
    }

    // Construct event write handler vector
    ::mtsManagerProxy::CommandWriteElement eventWriteHandler;
    mtsInterfaceCommon::CommandWriteVector::const_iterator itEventWrite = src.EventHandlersWrite.begin();
    const mtsInterfaceCommon::CommandWriteVector::const_iterator itEventWriteEnd = src.EventHandlersWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventWriteHandler.Name = itEventWrite->Name;
        eventWriteHandler.ArgumentPrototypeSerialized = itEventWrite->ArgumentPrototypeSerialized;
        dest.EventHandlersWrite.push_back(eventWriteHandler);
    }
}

//-------------------------------------------------------------------------
//  Implementation of mtsManagerLocalInterface
//  (See mtsManagerLocalInterface.h for comments)
//-------------------------------------------------------------------------
bool mtsManagerProxyServer::CreateComponentProxy(const std::string & componentProxyName, const std::string & listenerID)
{
    return SendCreateComponentProxy(componentProxyName, listenerID);
}

bool mtsManagerProxyServer::RemoveComponentProxy(const std::string & componentProxyName, const std::string & listenerID)
{
    return SendRemoveComponentProxy(componentProxyName, listenerID);
}

bool mtsManagerProxyServer::CreateProvidedInterfaceProxy(const std::string & serverComponentProxyName,
    const ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & listenerID)
{
    // Convert providedInterfaceDescription to an object of type mtsManagerProxy::ProvidedInterfaceDescription
    mtsManagerProxy::ProvidedInterfaceDescription interfaceDescription;
    mtsManagerProxyServer::ConstructProvidedInterfaceDescriptionFrom(providedInterfaceDescription, interfaceDescription);

    return SendCreateProvidedInterfaceProxy(serverComponentProxyName, interfaceDescription, listenerID);
}

bool mtsManagerProxyServer::CreateRequiredInterfaceProxy(const std::string & clientComponentProxyName,
    const RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & listenerID)
{
    // Convert requiredInterfaceDescription to an object of type mtsManagerProxy::RequiredInterfaceDescription
    mtsManagerProxy::RequiredInterfaceDescription interfaceDescription;
    mtsManagerProxyServer::ConstructRequiredInterfaceDescriptionFrom(requiredInterfaceDescription, interfaceDescription);
    
    return SendCreateRequiredInterfaceProxy(clientComponentProxyName, interfaceDescription, listenerID);
}

bool mtsManagerProxyServer::RemoveProvidedInterfaceProxy(
    const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName, const std::string & listenerID)
{
    return SendRemoveProvidedInterfaceProxy(clientComponentProxyName, providedInterfaceProxyName, listenerID);
}

bool mtsManagerProxyServer::RemoveRequiredInterfaceProxy(
    const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName, const std::string & listenerID)
{
    return SendRemoveRequiredInterfaceProxy(serverComponentProxyName, requiredInterfaceProxyName, listenerID);
}

void mtsManagerProxyServer::ProxyCreationCompleted(const std::string & listenerID)
{
    return SendProxyCreationCompleted(listenerID);
}

bool mtsManagerProxyServer::ConnectServerSideInterface(const unsigned int providedInterfaceProxyInstanceId,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, const std::string & listenerID)
{
    // Create an instance of mtsManagerProxy::ConnectionStringSet out of a set of strings given
    ::mtsManagerProxy::ConnectionStringSet connectionStringSet;
    ConstructConnectionStringSet(
        clientProcessName, clientComponentName, clientRequiredInterfaceName,
        serverProcessName, serverComponentName, serverProvidedInterfaceName,
        connectionStringSet);

    return SendConnectServerSideInterface(providedInterfaceProxyInstanceId, connectionStringSet, listenerID);
}

bool mtsManagerProxyServer::ConnectClientSideInterface(const unsigned int connectionID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, const std::string & listenerID)
{
    // Create an instance of mtsManagerProxy::ConnectionStringSet out of a set of strings given
    ::mtsManagerProxy::ConnectionStringSet connectionStringSet;
    ConstructConnectionStringSet(
        clientProcessName, clientComponentName, clientRequiredInterfaceName,
        serverProcessName, serverComponentName, serverProvidedInterfaceName,
        connectionStringSet);

    return SendConnectClientSideInterface(connectionID, connectionStringSet, listenerID);
}

bool mtsManagerProxyServer::GetProvidedInterfaceDescription(const std::string & componentName, const std::string & providedInterfaceName, 
    ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & listenerID)
{
    mtsManagerProxy::ProvidedInterfaceDescription src;

    if (!SendGetProvidedInterfaceDescription(componentName, providedInterfaceName, src, listenerID)) {
        LogError(mtsManagerProxyServer, "GetProvidedInterfaceDescription() failed");
        return false;
    }

    // Construct an instance of type ProvidedInterfaceDescription from an object of type mtsManagerProxy::ProvidedInterfaceDescription
    mtsManagerProxyServer::ConvertProvidedInterfaceDescription(src, providedInterfaceDescription);

    return true;
}

bool mtsManagerProxyServer::GetRequiredInterfaceDescription(const std::string & componentName, const std::string & requiredInterfaceName, 
    RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & listenerID)
{
    mtsManagerProxy::RequiredInterfaceDescription src;

    if (!SendGetRequiredInterfaceDescription(componentName, requiredInterfaceName, src, listenerID)) {
        LogError(mtsManagerProxyServer, "GetRequiredInterfaceDescription() failed");
        return false;
    }

    // Construct an instance of type RequiredInterfaceDescription from an object of type mtsManagerProxy::RequiredInterfaceDescription
    mtsManagerProxyServer::ConvertRequiredInterfaceDescription(src, requiredInterfaceDescription);

    return true;
}

const std::string mtsManagerProxyServer::GetProcessName(const std::string & listenerID)
{
    return SendGetProcessName(listenerID);
}

const int mtsManagerProxyServer::GetCurrentInterfaceCount(const std::string & componentName, const std::string & listenerID)
{
    return SendGetCurrentInterfaceCount(componentName, listenerID);
}

//-------------------------------------------------------------------------
//  Event Handlers (Client -> Server)
//-------------------------------------------------------------------------
void mtsManagerProxyServer::ReceiveTestMessageFromClientToServer(
    const ConnectionIDType & connectionID, const std::string & str)
{
    const ClientIDType clientID = GetClientID(connectionID);

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer,
             "ReceiveTestMessageFromClientToServer: " 
             << "\n..... ConnectionID: " << connectionID
             << "\n..... Message: " << str);
#endif

    std::cout << "Server: received from Client " << clientID << ": " << str << std::endl;
}

bool mtsManagerProxyServer::ReceiveAddClient(
    const ConnectionIDType & connectionID, const std::string & processName,
    ManagerClientProxyType & clientProxy)
{
    if (!AddProxyClient(processName, processName, connectionID, clientProxy)) {
        LogError(mtsManagerProxyServer, "ReceiveAddClient: failed to add proxy client: " << processName);
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer,
             "ReceiveAddClient: added proxy client: " 
             << "\n..... ConnectionID: " << connectionID 
             << "\n..... Process name: " << processName);
#endif

    return true;
}

//
// TODO: Implement ReceiveShutdown()
//

bool mtsManagerProxyServer::ReceiveAddProcess(const std::string & processName)
{
    return ProxyOwner->AddProcess(processName);
}

bool mtsManagerProxyServer::ReceiveFindProcess(const std::string & processName) const
{
    return ProxyOwner->FindProcess(processName);
}

bool mtsManagerProxyServer::ReceiveRemoveProcess(const std::string & processName)
{
    return ProxyOwner->RemoveProcess(processName);
}

bool mtsManagerProxyServer::ReceiveAddComponent(const std::string & processName, const std::string & componentName)
{
    return ProxyOwner->AddComponent(processName, componentName);
}

bool mtsManagerProxyServer::ReceiveFindComponent(const std::string & processName, const std::string & componentName) const
{
    return ProxyOwner->FindComponent(processName, componentName);
}

bool mtsManagerProxyServer::ReceiveRemoveComponent(const std::string & processName, const std::string & componentName)
{
    return ProxyOwner->RemoveComponent(processName, componentName);
}

bool mtsManagerProxyServer::ReceiveAddProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface)
{
    return ProxyOwner->AddProvidedInterface(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyServer::ReceiveFindProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const
{
    return ProxyOwner->FindProvidedInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ReceiveRemoveProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return ProxyOwner->RemoveProvidedInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ReceiveAddRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface)
{
    return ProxyOwner->AddRequiredInterface(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyServer::ReceiveFindRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const
{
    return ProxyOwner->FindRequiredInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ReceiveRemoveRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return ProxyOwner->RemoveRequiredInterface(processName, componentName, interfaceName);
}

::Ice::Int mtsManagerProxyServer::ReceiveConnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->Connect(connectionStringSet.RequestProcessName,
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName);
}

bool mtsManagerProxyServer::ReceiveConnectConfirm(::Ice::Int connectionSessionID)
{
    return ProxyOwner->ConnectConfirm(connectionSessionID);
}

bool mtsManagerProxyServer::ReceiveDisconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->Disconnect(
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName);
}

bool mtsManagerProxyServer::ReceiveSetProvidedInterfaceProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const std::string & endpointInfo, const std::string & communicatorID)
{
    return ProxyOwner->SetProvidedInterfaceProxyAccessInfo(
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName,
        endpointInfo, communicatorID);
}

bool mtsManagerProxyServer::ReceiveGetProvidedInterfaceProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, std::string & endpointInfo, std::string & communicatorID)
{
    return ProxyOwner->GetProvidedInterfaceProxyAccessInfo(
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName,
        endpointInfo, communicatorID);
}

bool mtsManagerProxyServer::ReceiveInitiateConnect(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->InitiateConnect(connectionID,
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName);
}

bool mtsManagerProxyServer::ReceiveConnectServerSideInterface(::Ice::Int providedInterfaceProxyInstanceId, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->ConnectServerSideInterface(providedInterfaceProxyInstanceId,
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName);
}

//-------------------------------------------------------------------------
//  Event Generators (Event Sender) : Server -> Client
//-------------------------------------------------------------------------
void mtsManagerProxyServer::SendTestMessageFromServerToClient(const std::string & str)
{
    if (!this->IsActiveProxy()) return;

    // iterate client map -> send message to ALL clients (broadcasts)
    ManagerClientProxyType * clientProxy;
    ClientIDMapType::iterator it = ClientIDMap.begin();
    ClientIDMapType::const_iterator itEnd = ClientIDMap.end();
    for (; it != itEnd; ++it) {
        clientProxy = &(it->second.ClientProxy);
        try 
        {
            (*clientProxy)->TestMessageFromServerToClient(str);

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendMessageFromServerToClient");
#endif
        }
        catch (const ::Ice::Exception & ex)
        {
            std::cerr << "Error: " << ex << std::endl;

            //
            // TODO: Ice::ConnectionLostException -> is thrown when a connected
            // client disconnects abruptly (ctrl+C, etc.).
            // When this exception is thrown, it can be used for handling disconnect()
            // properly.
            //
            continue;
        }
    }
}

/*
bool mtsManagerProxyServer::SendFetchFunctionProxyPointers(
    const ClientIDType clientID, const std::string & requiredInterfaceName,
    mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendFetchFunctionProxyPointers: no proxy client found or inactive proxy: " << clientID);
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendMessageFromServerToClient: provided interface proxy instance id: " << clientID);
#endif

    return (*clientProxy)->FetchFunctionProxyPointers(requiredInterfaceName, functionProxyPointers);
}
*/

bool mtsManagerProxyServer::SendCreateComponentProxy(const std::string & componentProxyName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendCreateComponentProxy: invalid client id (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->CreateComponentProxy(componentProxyName);
}

bool mtsManagerProxyServer::SendRemoveComponentProxy(const std::string & componentProxyName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendRemoveComponentProxy: invalid client id (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->RemoveComponentProxy(componentProxyName);
}

bool mtsManagerProxyServer::SendCreateProvidedInterfaceProxy(
    const std::string & serverComponentProxyName, 
    const ::mtsManagerProxy::ProvidedInterfaceDescription & providedInterfaceDescription, 
    const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendCreateProvidedInterfaceProxy: invalid client id (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->CreateProvidedInterfaceProxy(serverComponentProxyName, providedInterfaceDescription);
}

bool mtsManagerProxyServer::SendCreateRequiredInterfaceProxy(
    const std::string & clientComponentProxyName, 
    const ::mtsManagerProxy::RequiredInterfaceDescription & requiredInterfaceDescription,
    const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendCreateRequiredInterfaceProxy: invalid client id (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->CreateRequiredInterfaceProxy(clientComponentProxyName, requiredInterfaceDescription);
}

bool mtsManagerProxyServer::SendRemoveProvidedInterfaceProxy(
    const std::string & clientComponentProxyName, 
    const std::string & providedInterfaceProxyName, 
    const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendRemoveProvidedInterfaceProxy: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->RemoveProvidedInterfaceProxy(clientComponentProxyName, providedInterfaceProxyName);
}

bool mtsManagerProxyServer::SendRemoveRequiredInterfaceProxy(
    const std::string & serverComponentProxyName, 
    const std::string & requiredInterfaceProxyName, 
    const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendRemoveRequiredInterfaceProxy: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->RemoveRequiredInterfaceProxy(serverComponentProxyName, requiredInterfaceProxyName);
}

void mtsManagerProxyServer::SendProxyCreationCompleted(const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendProxyCreationCompleted: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

    (*clientProxy)->ProxyCreationCompleted();
}

bool mtsManagerProxyServer::SendConnectServerSideInterface(
    ::Ice::Int providedInterfaceProxyInstanceId, 
    const ::mtsManagerProxy::ConnectionStringSet & connectionStrings, 
    const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendConnectServerSideInterface: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->ConnectServerSideInterface(providedInterfaceProxyInstanceId, connectionStrings);
}

bool mtsManagerProxyServer::SendConnectClientSideInterface(
    ::Ice::Int connectionID, 
    const ::mtsManagerProxy::ConnectionStringSet & connectionStrings, 
    const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendConnectClientSideInterface: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->ConnectClientSideInterface(connectionID, connectionStrings);
}

bool mtsManagerProxyServer::SendGetProvidedInterfaceDescription(
    const std::string & componentName, 
    const std::string & providedInterfaceName,
    ::mtsManagerProxy::ProvidedInterfaceDescription & providedInterfaceDescription,
    const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetProvidedInterfaceDescription: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->GetProvidedInterfaceDescription(componentName, providedInterfaceName, providedInterfaceDescription);
}

bool mtsManagerProxyServer::SendGetRequiredInterfaceDescription(
    const std::string & componentName,
    const std::string & requiredInterfaceName,
    ::mtsManagerProxy::RequiredInterfaceDescription & requiredInterfaceDescription,
    const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetRequiredInterfaceDescription: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->GetRequiredInterfaceDescription(componentName, requiredInterfaceName, requiredInterfaceDescription);
}

std::string mtsManagerProxyServer::SendGetProcessName(const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetProcessName: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->GetProcessName();
}

::Ice::Int mtsManagerProxyServer::SendGetCurrentInterfaceCount(const std::string & componentName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetCurrentInterfaceCount: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

    return (*clientProxy)->GetCurrentInterfaceCount(componentName);
}

//-------------------------------------------------------------------------
//  Definition by mtsComponentInterfaceProxy.ice
//-------------------------------------------------------------------------
mtsManagerProxyServer::ManagerServerI::ManagerServerI(
    const Ice::CommunicatorPtr& communicator, const Ice::LoggerPtr& logger,
    mtsManagerProxyServer * managerProxyServer)
    : Communicator(communicator),
      SenderThreadPtr(new SenderThread<ManagerServerIPtr>(this)),
      IceLogger(logger),
      Runnable(true),
      ManagerProxyServer(managerProxyServer)
{
}

void mtsManagerProxyServer::ManagerServerI::Start()
{
    ManagerProxyServer->GetLogger()->trace(
        "mtsManagerProxyServer", "Send thread starts");

    SenderThreadPtr->start();
}

// TODO: Remove this
//#define _COMMUNICATION_TEST_

void mtsManagerProxyServer::ManagerServerI::Run()
{
#ifdef _COMMUNICATION_TEST_
    int count = 0;

    while (Runnable) 
    {
        osaSleep(1 * cmn_s);
        std::cout << "\tServer [" << ManagerProxyServer->GetProxyName() << "] running (" << ++count << ")" << std::endl;

        std::stringstream ss;
        ss << "Msg " << count << " from Server";

        ManagerProxyServer->SendTestMessageFromServerToClient(ss.str());
    }
#else
    while(Runnable) 
    {
        osaSleep(1 * cmn_ms);

        /*
        if(!clients.empty())
        {
            ++num;
            for(std::set<mtsTaskManagerProxy::TaskManagerClientPrx>::iterator p 
                = clients.begin(); p != clients.end(); ++p)
            {
                try
                {
                    std::cout << "server sends: " << num << std::endl;
                }
                catch(const IceUtil::Exception& ex)
                {
                    std::cerr << "removing client `" << Communicator->identityToString((*p)->ice_getIdentity()) << "':\n"
                        << ex << std::endl;

                    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
                    _clients.erase(*p);
                }
            }
        }
        */
    }
#endif
}

void mtsManagerProxyServer::ManagerServerI::Stop()
{
    if (!ManagerProxyServer->IsActiveProxy()) return;

    IceUtil::ThreadPtr callbackSenderThread;
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

        Runnable = false;

        notify();

        callbackSenderThread = SenderThreadPtr;
        SenderThreadPtr = 0; // Resolve cyclic dependency.
    }
    callbackSenderThread->getThreadControl().join();
}

//-----------------------------------------------------------------------------
//  Network Event Handlers
//-----------------------------------------------------------------------------
void mtsManagerProxyServer::ManagerServerI::TestMessageFromClientToServer(
    const std::string & str, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: TestMessageFromClientToServer");
#endif

    const ConnectionIDType connectionID = current.ctx.find(ConnectionIDKey)->second;

    ManagerProxyServer->ReceiveTestMessageFromClientToServer(connectionID, str);
}

bool mtsManagerProxyServer::ManagerServerI::AddClient(
    const std::string & processName, const ::Ice::Identity & identity, const ::Ice::Current& current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
   LogPrint(ManagerServerI, "<<<<< RECV: AddClient: " << processName << " (" << Communicator->identityToString(identity) << ")");
#endif

    const ConnectionIDType connectionID = current.ctx.find(ConnectionIDKey)->second;

    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    ManagerClientProxyType clientProxy = 
        ManagerClientProxyType::uncheckedCast(current.con->createProxy(identity));
    
    return ManagerProxyServer->ReceiveAddClient(connectionID, processName, clientProxy);
}

void mtsManagerProxyServer::ManagerServerI::Shutdown(const ::Ice::Current& current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: Shutdown");
#endif

    const ConnectionIDType connectionID = current.ctx.find(ConnectionIDKey)->second;

    // TODO:
    // Set as true to represent that this connection (session) is going to be closed.
    // After this flag is set, no message is allowed to be sent to a server.
    //ComponentInterfaceProxyServer->ShutdownSession(current);
}

bool mtsManagerProxyServer::ManagerServerI::AddProcess(const std::string & processName, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: AddProcess");
#endif

    return ManagerProxyServer->ReceiveAddProcess(processName);
}

bool mtsManagerProxyServer::ManagerServerI::FindProcess(const std::string & processName, const ::Ice::Current & current) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: FindProcess");
#endif

    return ManagerProxyServer->ReceiveFindProcess(processName);
}

bool mtsManagerProxyServer::ManagerServerI::RemoveProcess(const std::string & processName, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: RemoveProcess");
#endif

    return ManagerProxyServer->ReceiveRemoveProcess(processName);
}

bool mtsManagerProxyServer::ManagerServerI::AddComponent(const std::string & processName, const std::string & componentName, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: AddComponent");
#endif

    return ManagerProxyServer->ReceiveAddComponent(processName, componentName);
}

bool mtsManagerProxyServer::ManagerServerI::FindComponent(const std::string & processName, const std::string & componentName, const ::Ice::Current & current) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: FindComponent ");
#endif

    return ManagerProxyServer->ReceiveFindComponent(processName, componentName);
}

bool mtsManagerProxyServer::ManagerServerI::RemoveComponent(const std::string & processName, const std::string & componentName, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: RemoveComponent ");
#endif

    return ManagerProxyServer->ReceiveRemoveComponent(processName, componentName);
}

bool mtsManagerProxyServer::ManagerServerI::AddProvidedInterface(
    const std::string & processName, const std::string & componentName, 
    const std::string & interfaceName, bool isProxyInterface, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: AddProvidedInterface");
#endif

    return ManagerProxyServer->ReceiveAddProvidedInterface(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyServer::ManagerServerI::FindProvidedInterface(
    const std::string & processName, const std::string & componentName, 
    const std::string & interfaceName, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: FindProvidedInterface");
#endif

    return ManagerProxyServer->ReceiveFindProvidedInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ManagerServerI::RemoveProvidedInterface(
    const std::string & processName, const std::string & componentName, 
    const std::string & interfaceName, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: RemoveProvidedInterface");
#endif

    return ManagerProxyServer->ReceiveRemoveProvidedInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ManagerServerI::AddRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, bool isProxyInterface, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: AddRequiredInterface");
#endif

    return ManagerProxyServer->ReceiveAddRequiredInterface(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyServer::ManagerServerI::FindRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current &) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: FindRequiredInterface");
#endif

    return ManagerProxyServer->ReceiveFindRequiredInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ManagerServerI::RemoveRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: RemoveRequiredInterface");
#endif

    return ManagerProxyServer->ReceiveRemoveRequiredInterface(processName, componentName, interfaceName);
}

::Ice::Int mtsManagerProxyServer::ManagerServerI::Connect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: Connect");
#endif

    return ManagerProxyServer->ReceiveConnect(connectionStringSet);
}

bool mtsManagerProxyServer::ManagerServerI::ConnectConfirm(::Ice::Int connectionSessionID, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: ConnectConfirm");
#endif

    return ManagerProxyServer->ReceiveConnectConfirm(connectionSessionID);
}

bool mtsManagerProxyServer::ManagerServerI::Disconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: Disconnect");
#endif

    return ManagerProxyServer->ReceiveDisconnect(connectionStringSet);
}

bool mtsManagerProxyServer::ManagerServerI::SetProvidedInterfaceProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const std::string & endpointInfo, const std::string & communicatorID, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: SetProvidedInterfaceProxyAccessInfo");
#endif

    return ManagerProxyServer->ReceiveSetProvidedInterfaceProxyAccessInfo(connectionStringSet, endpointInfo, communicatorID);
}

bool mtsManagerProxyServer::ManagerServerI::GetProvidedInterfaceProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, std::string & endpointInfo, std::string & communicatorID, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: GetProvidedInterfaceProxyAccessInfo");
#endif

    return ManagerProxyServer->ReceiveGetProvidedInterfaceProxyAccessInfo(connectionStringSet, endpointInfo, communicatorID);
}

bool mtsManagerProxyServer::ManagerServerI::InitiateConnect(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: InitiateConnect");
#endif

    return ManagerProxyServer->ReceiveInitiateConnect(connectionID, connectionStringSet);
}

bool mtsManagerProxyServer::ManagerServerI::ConnectServerSideInterface(::Ice::Int providedInterfaceProxyInstanceId, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: ConnectServerSideInterface");
#endif

    return ManagerProxyServer->ReceiveConnectServerSideInterface(providedInterfaceProxyInstanceId, connectionStringSet);
}