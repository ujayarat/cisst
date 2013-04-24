/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2007-09-05

  (C) Copyright 2007-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a mailbox for communication between tasks.
*/

#ifndef _mtsMailBox_h
#define _mtsMailBox_h

#include <cisstMultiTask/mtsQueue.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>


class CISST_EXPORT mtsMailBox
{
    mtsQueue<mtsCommandBase *> CommandQueue;

    /*! Name provided for logs */
    std::string Name;

    /*! Command to execute after a command is queued.  This command has
      to be provided when the mail box is constructed.  This
      mechanism is used by mtsTaskFromSignal to wake up the task's
      thread. */
    mtsCallableVoidBase * PostCommandQueuedCallable;

    /*! Command executed after a command is de-queued.  This is
      used for blocking commands in order to trigger an event sent
      back to the caller.  The caller's required interface needs to
      provide an event handler that is not queued. */
    mtsCommandVoid * PostCommandDequeuedCommand;

    /*! Command executed after a command with return is de-queued.
      This is used for blocking commands in order to trigger an event
      sent back to the caller.  The caller's required interface needs
      to provide an event handler that is not queued. */
    mtsCommandVoid * PostCommandReturnDequeuedCommand;

    /*! Method to determine which post queued command needs to be triggered. */
    void TriggerPostQueuedCommandIfNeeded(bool isBlocking, bool isBlockingReturn);

public:
    mtsMailBox(const std::string & name,
               size_t size,
               mtsCallableVoidBase * postCommandQueuedCallable = 0);

    ~mtsMailBox(void);

    /*! Get the mailbox's name */
    const std::string & GetName(void) const;

    /*! Write a command to the mailbox.  If a post command queued
      command has been provided, the command is executed. */
    bool Write(mtsCommandBase * command);

    /*! Execute the oldest command queued. */
    bool ExecuteNext(void);

    /*! Resize the mailbox, i.e. resizes the underlying queue of
      commands.  This command is not thread safe and shouldn't be used
      if commands are already queued or can be queued.  The SetSize
      methods deletes whatever command has been queued. */
    void SetSize(size_t size);

    /*! Returns true if mailbox is empty. */
    bool IsEmpty(void) const;

    /*! Returns true if mailbox is full. */
    bool IsFull(void) const;

    /*! Set the command to be called after a blocking command is
      de-queued and executed.  This can be used to call a trigger for
      event.  The event handler on the client site can then raise a
      thread signal. */
    void SetPostCommandDequeuedCommand(mtsCommandVoid * command);

    /*! Set the command to be called after a blocking command with
      return value is de-queued and executed.  This can be used to
      call a trigger for event.  The event handler on the client site
      can then raise a thread signal. */
    void SetPostCommandReturnDequeuedCommand(mtsCommandVoid * command);

};


#endif // _mtsMailbox_h

