//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ========//
//
// Purpose: Core implementation of vgui
//
// $NoKeywords: $
//===========================================================================//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef PostMessage
#undef GetCursorPos

#include <vgui/VGUI2.h>
#include <vgui/Dar.h>
#include <vgui/IInputInternal.h>
#include <vgui/IPanel.h>
#include <vgui/ISystem.h>
#include <vgui/ISurfaceNext.h>
#include <vgui/IVGui.h>
#include <vgui/IClientPanel.h>
#include <vgui/ISchemeNext.h>
#include <KeyValues.h>
#include <string.h>
#include <Assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <tier0/dbg.h>
#include <tier1/utlhandletable.h>
#include "vgui_internal.h"
#include "VPanel.h"
#include "UtlLinkedList.h"
#include "UtlPriorityQueue.h"
#include "UtlVector.h"
#include "tier0/vprof.h"
#include "tier0/icommandline.h"

#undef GetCursorPos // protected_things.h defines this, and it makes it so we can't access g_pInput->GetCursorPos.

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui2;
static const int WARN_PANEL_NUMBER = 32768; // in DEBUG if more panels than this are created then throw an Assert, helps catch panel leaks

//-----------------------------------------------------------------------------
// Purpose: Single item in the message queue
//-----------------------------------------------------------------------------
struct MessageItem_t
{
    KeyValues* _params; // message data
    // _params->GetName() is the message name

    HPanel _messageTo; // the panel this message is to be sent to
    HPanel _from; // the panel this message is from (if any)
    float _arrivalTime; // time at which the message should be passed on to the recipient

    int _messageID; // incrementing message index
};


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool PriorityQueueComp(const MessageItem_t& x, const MessageItem_t& y)
{
    if (x._arrivalTime > y._arrivalTime)
    {
        return true;
    }
    else if (x._arrivalTime < y._arrivalTime)
    {
        return false;
    }

    // compare messageID's to ensure we have the messages in the correct order
    return (x._messageID > y._messageID);
}


//-----------------------------------------------------------------------------
// Purpose: Implementation of core vgui functionality
//-----------------------------------------------------------------------------
class CVGui : public IVGui
{
public:
    CVGui();
    ~CVGui();

    virtual bool Init(CreateInterfaceFn* factoryList, int numFactories);
    virtual void Shutdown();


    virtual void RunFrame();

    virtual void Start()
    {
        m_bRunning = true;
    }

    // signals vgui to Stop running
    virtual void Stop()
    {
        m_bRunning = false;
    }

    // returns true if vgui is current active
    virtual bool IsRunning()
    {
        return m_bRunning;
    }

    virtual void ShutdownMessage(unsigned int shutdownID);

    // safe-pointer handle methods
    virtual VPANEL AllocPanel();
    virtual void FreePanel(VPANEL ipanel);
    virtual HPanel PanelToHandle(VPANEL panel);
    virtual VPANEL HandleToPanel(HPanel index);
    virtual void MarkPanelForDeletion(VPANEL panel);

    virtual void AddTickSignal(VPANEL panel, int intervalMilliseconds = 0);
    virtual void RemoveTickSignal(VPANEL panel);

    // message pump method
    virtual void PostMessage(VPANEL target, KeyValues* params, VPANEL from, float delaySeconds = 0.0f);

    virtual void SetSleep(bool state) { m_bDoSleep = state; };
    virtual bool GetShouldVGuiControlSleep() { return m_bDoSleep; }

    virtual void DPrintf(const char* format, ...);
    virtual void DPrintf2(const char* format, ...);
    virtual void SpewAllActivePanelNames();

    // Creates/ destroys vgui contexts, which contains information
    // about which controls have mouse + key focus, for example.
    virtual HContext CreateContext();
    virtual void DestroyContext(HContext context);

    // Associates a particular panel with a vgui context
    // Associating NULL is valid; it disconnects the panel from the context
    virtual void AssociatePanelWithContext(HContext context, VPANEL pRoot);

    // Activates a particular input context, use DEFAULT_VGUI_CONTEXT
    // to get the one normally used by VGUI
    virtual void ActivateContext(HContext context);

    bool IsDispatchingMessages(void)
    {
        return m_InDispatcher;
    }

private:
    // VGUI contexts
    struct Context_t
    {
        HInputContext m_hInputContext;
    };

    struct Tick_t
    {
        VPANEL panel;
        int interval;
        int nexttick;

        // Debugging
        char panelname[64];
    };

    // Returns the current context
    Context_t* GetContext(HContext context);

    void PanelCreated(VPanel* panel);
    void PanelDeleted(VPanel* panel);
    bool DispatchMessages();
    void DestroyAllContexts();
    void ClearMessageQueues();

    // safe panel handle stuff
    CUtlHandleTable<VPanel, 20> m_HandleTable;
    int m_iCurrentMessageID;

    bool m_bRunning : 1;
    bool m_bDoSleep : 1;
    bool m_InDispatcher : 1;
    bool m_bDebugMessages : 1;
    bool m_bInitialized = false;

    CUtlVector<Tick_t*> m_TickSignalVec;
    CUtlLinkedList<Context_t> m_Contexts;

    HContext m_hContext;
    Context_t m_DefaultContext;

#ifdef DEBUG
	int m_iDeleteCount, m_iDeletePanelCount;
#endif

    // message queue. holds all vgui messages generated by windows events
    CUtlLinkedList<MessageItem_t, ushort> m_MessageQueue;

    // secondary message queue, holds all vgui messages generated by vgui
    CUtlLinkedList<MessageItem_t, ushort> m_SecondaryQueue;

    // timing queue, holds all the messages that have to arrive at a specified time
    CUtlPriorityQueue<MessageItem_t> m_DelayedMessageQueue;
};

CVGui g_VGui;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CVGui, IVGui, VGUI_IVGUI_INTERFACE_VERSION_GS, g_VGui);

bool IsDispatchingMessageQueue(void)
{
    return g_VGui.IsDispatchingMessages();
}

namespace vgui2
{
    IVGui* g_pIVgui = &g_VGui;
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CVGui::CVGui() : m_DelayedMessageQueue(0, 4, PriorityQueueComp)
{
    m_bRunning = false;
    m_InDispatcher = false;
    m_bDebugMessages = false;
    m_bDoSleep = true;
    m_hContext = DEFAULT_VGUI_CONTEXT;
    m_DefaultContext.m_hInputContext = DEFAULT_INPUT_CONTEXT;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CVGui::~CVGui()
{
#ifdef _DEBUG
    int nCount = m_HandleTable.GetHandleCount();
    int nActualCount = 0;
    for (int i = 0; i < nCount; ++i)
    {
        UtlHandle_t h = m_HandleTable.GetHandleFromIndex(i);
        if (m_HandleTable.IsHandleValid(h))
        {
            ++nActualCount;
        }
    }

    if (nActualCount > 0)
    {
        Msg("Memory leak: panels left in CVGui::m_PanelList: %d\n", nActualCount);
    }
#endif // _DEBUG
}

//-----------------------------------------------------------------------------
// Purpose: Dumps out list of all active panels
//-----------------------------------------------------------------------------
void CVGui::SpewAllActivePanelNames()
{
    int nCount = m_HandleTable.GetHandleCount();
    for (int i = 0; i < nCount; ++i)
    {
        UtlHandle_t h = m_HandleTable.GetHandleFromIndex(i);
        if (m_HandleTable.IsHandleValid(h))
        {
            VPanel* pPanel = m_HandleTable.GetHandle(h);
            Msg("\tpanel '%s' of type '%s' leaked\n", g_pIPanel->GetName((VPANEL)pPanel), ((VPanel*)pPanel)->GetClassName());
        }
    }
}


//-----------------------------------------------------------------------------
// Creates/ destroys "input" contexts, which contains information
// about which controls have mouse + key focus, for example.
//-----------------------------------------------------------------------------
HContext CVGui::CreateContext()
{
    HContext i = m_Contexts.AddToTail();
    m_Contexts[i].m_hInputContext = g_pInput->CreateInputContext();
    return i;
}

void CVGui::DestroyContext(HContext context)
{
    Assert(context != DEFAULT_VGUI_CONTEXT);

    if (m_hContext == context)
    {
        ActivateContext(DEFAULT_VGUI_CONTEXT);
    }

    g_pInput->DestroyInputContext(GetContext(context)->m_hInputContext);
    m_Contexts.Remove(context);
}

void CVGui::DestroyAllContexts()
{
    HContext next;
    HContext i = m_Contexts.Head();
    while (i != m_Contexts.InvalidIndex())
    {
        next = m_Contexts.Next(i);
        DestroyContext(i);
        i = next;
    }
}


//-----------------------------------------------------------------------------
// Returns the current context
//-----------------------------------------------------------------------------
CVGui::Context_t* CVGui::GetContext(HContext context)
{
    if (context == DEFAULT_VGUI_CONTEXT)
        return &m_DefaultContext;
    return &m_Contexts[context];
}


//-----------------------------------------------------------------------------
// Associates a particular panel with a context
// Associating NULL is valid; it disconnects the panel from the context
//-----------------------------------------------------------------------------
void CVGui::AssociatePanelWithContext(HContext context, VPANEL pRoot)
{
    Assert(context != DEFAULT_VGUI_CONTEXT);
    g_pInput->AssociatePanelWithInputContext(GetContext(context)->m_hInputContext, pRoot);
}


//-----------------------------------------------------------------------------
// Activates a particular context, use DEFAULT_VGUI_CONTEXT
// to get the one normally used by VGUI
//-----------------------------------------------------------------------------
void CVGui::ActivateContext(HContext context)
{
    Assert((context == DEFAULT_VGUI_CONTEXT) || m_Contexts.IsValidIndex(context));

    if (m_hContext != context)
    {
        DispatchMessages();

        m_hContext = context;
        g_pInput->ActivateInputContext(GetContext(m_hContext)->m_hInputContext);

        if (context != DEFAULT_VGUI_CONTEXT)
        {
            g_pInput->RunFrame();
        }
    }
}

//-----------------------------------------------------------------------------
// Purpose: Runs a single vgui frame, pumping all message to panels
//-----------------------------------------------------------------------------
void CVGui::RunFrame()
{
#ifdef DEBUG
    //  memory allocation debug helper
    //	DPrintf( "Delete Count:%i,%i\n", m_iDeleteCount, m_iDeletePanelCount );
    //	m_iDeleteCount = 	m_iDeletePanelCount = 0;
#endif

    // this will generate all key and mouse events as well as make a real repaint
    {
        VPROF("surface()->RunFrame()");
        g_pSurfaceNext->RunFrame();
    }

    // give the system a chance to process
    {
        VPROF("system()->RunFrame()");
        g_pSystem->RunFrame();
    }

    // update cursor positions
    {
        VPROF("update cursor positions");
        int cursorX, cursorY;
        g_pInput->GetCursorPosition(cursorX, cursorY);

        // this does the actual work given a x,y and a surface
        g_pInput->UpdateMouseFocus(cursorX, cursorY);
    }

    {
        VPROF("input()->RunFrame()");
        g_pInput->RunFrame();
    }

    // messenging
    {
        VPROF("messenging");
        // send all the messages waiting in the queue
        DispatchMessages();

        int time = g_pSystem->GetTimeMillis();

        // directly invoke tick all who asked to be ticked
        int count = m_TickSignalVec.Count();
        for (int i = count - 1; i >= 0; i--)
        {
            Tick_t* t = m_TickSignalVec[i];

            VPANEL tickTarget = t->panel;
            if (!tickTarget)
            {
                m_TickSignalVec.Remove(i);
                delete t;
                continue;
            }

            if (t->interval != 0)
            {
                if (time < t->nexttick)
                    continue;

                t->nexttick = time + t->interval;
            }

            PostMessage(tickTarget, new KeyValues("Tick"), NULL);
        }
    }

    {
        VPROF("SolveTraverse");
        // make sure the hierarchy is up to date
        g_pSurfaceNext->SolveTraverse(g_pSurfaceNext->GetEmbeddedPanel());
        g_pSurfaceNext->ApplyChanges();
        Assert(_heapchk() == _HEAPOK);
    }
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
VPANEL CVGui::AllocPanel()
{
#ifdef DEBUG
	m_iDeleteCount++;
#endif

    VPanel* panel = new VPanel;
    PanelCreated(panel);
    return (VPANEL)panel;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CVGui::FreePanel(VPANEL ipanel)
{
    PanelDeleted((VPanel*)ipanel);
    delete (VPanel*)ipanel;
#ifdef DEBUG
	m_iDeleteCount--;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Returns the safe index of the panel
//-----------------------------------------------------------------------------
HPanel CVGui::PanelToHandle(VPANEL panel)
{
    if (panel)
        return ((VPanel*)panel)->GetHPanel();
    return INVALID_PANEL;
}


//-----------------------------------------------------------------------------
// Purpose: Returns the panel at the specified index
//-----------------------------------------------------------------------------
VPANEL CVGui::HandleToPanel(HPanel index)
{
    if (!m_HandleTable.IsHandleValid(index))
    {
        return NULL;
    }
    return (VPANEL)m_HandleTable.GetHandle((UtlHandle_t)index);
}


//-----------------------------------------------------------------------------
// Purpose: Called whenever a panel is constructed
//-----------------------------------------------------------------------------
void CVGui::PanelCreated(VPanel* panel)
{
    UtlHandle_t h = m_HandleTable.AddHandle();
    m_HandleTable.SetHandle(h, panel);

#if DUMP_PANEL_LIST
	int nCount = m_HandleTable.GetHandleCount();
	int nActualCount = 0;
	for ( int i = 0; i < nCount; ++i )
	{
		UtlHandle_t h = m_HandleTable.GetHandleFromIndex( i );
		if ( m_HandleTable.IsHandleValid( h ) )
		{
			++nActualCount;
		}
	}

	if ( nActualCount >= WARN_PANEL_NUMBER )
	{
		FILE *file1 = fopen("panellist.txt", "w");
		if (file1 != NULL)
		{
			fprintf(file1, "Too many panels...listing them all.\n");
			int panelIndex;
			for (panelIndex = 0; panelIndex < nCount; ++panelIndex)
			{
				UtlHandle_t h = m_HandleTable.GetHandleFromIndex( i );
				VPanel *pPanel = m_HandleTable.GetHandle( h );
				IClientPanel *ipanel = ( pPanel ) ? pPanel->Client() : NULL;
				if ( ipanel )
					fprintf(file1, "panel %d: name: %s   classname: %s\n", panelIndex, ipanel->GetName(), ipanel->GetClassName());
				else
					fprintf(file1, "panel %d: can't get ipanel\n", panelIndex);
			}

			fclose(file1);
		}
	}

	Assert( nActualCount < WARN_PANEL_NUMBER );
#endif // DUMP_PANEL_LIST

    ((VPanel*)panel)->SetHPanel(h);

    g_pSurfaceNext->AddPanel((VPANEL)panel);
}

//-----------------------------------------------------------------------------
// Purpose: instantly stops the app from pointing to the focus'd object
//			used when an object is being deleted
//-----------------------------------------------------------------------------
void CVGui::PanelDeleted(VPanel* focus)
{
    Assert(focus);
    g_pSurfaceNext->ReleasePanel((VPANEL)focus);
    g_pInput->PanelDeleted((VPANEL)focus);

    // remove from safe handle list
    UtlHandle_t h = ((VPanel*)focus)->GetHPanel();

    Assert(m_HandleTable.IsHandleValid(h));
    if (m_HandleTable.IsHandleValid(h))
    {
        m_HandleTable.RemoveHandle(h);
    }

    ((VPanel*)focus)->SetHPanel(INVALID_PANEL);

    // remove from tick signal dar
    RemoveTickSignal((VPANEL)focus);
}


//-----------------------------------------------------------------------------
// Purpose: Adds the panel to a tick signal list, so the panel receives a message every frame
//-----------------------------------------------------------------------------
void CVGui::AddTickSignal(VPANEL panel, int intervalMilliseconds /*=0*/)
{
    Tick_t* t;
    // See if it's already in list
    int count = m_TickSignalVec.Count();
    for (int i = 0; i < count; i++)
    {
        Tick_t* t = m_TickSignalVec[i];
        if (t->panel == panel)
        {
            // Go ahead and update intervals
            t->interval = intervalMilliseconds;
            t->nexttick = g_pSystem->GetTimeMillis() + t->interval;
            return;
        }
    }

    // Add to list
    t = new Tick_t;

    t->panel = panel;
    t->interval = intervalMilliseconds;
    t->nexttick = g_pSystem->GetTimeMillis() + t->interval;

    if (strlen(((VPanel*)panel)->Client()->GetName()) > 0)
    {
        strncpy(t->panelname, ((VPanel*)panel)->Client()->GetName(), sizeof(t->panelname));
    }
    else
    {
        strncpy(t->panelname, ((VPanel*)panel)->Client()->GetClassName(), sizeof(t->panelname));
    }

    // simply add the element to the list
    m_TickSignalVec.AddToTail(t);
    // panel is removed from list when deleted
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CVGui::RemoveTickSignal(VPANEL panel)
{
    // remove from tick signal dar
    int count = m_TickSignalVec.Count();

    for (int i = 0; i < count; i++)
    {
        Tick_t* tick = m_TickSignalVec[i];
        if (tick->panel == panel)
        {
            m_TickSignalVec.Remove(i);
            delete tick;
            return;
        }
    }
}


//-----------------------------------------------------------------------------
// Purpose: message pump
//			loops through and sends all active messages
//			note that more messages may be posted during the process
//-----------------------------------------------------------------------------
bool CVGui::DispatchMessages()
{
    int time = g_pSystem->GetTimeMillis();

    m_InDispatcher = true;
    bool doneWork = (m_MessageQueue.Count() > 12);

    bool bUsingDelayedQueue = (m_DelayedMessageQueue.Count() > 0);

    while (m_MessageQueue.Count() > 0 || (m_SecondaryQueue.Count() > 0) || bUsingDelayedQueue)
    {
        // get the first message
        MessageItem_t* messageItem = NULL;
        int messageIndex = 0;

        // use the secondary queue until it empties. empty it after each message in the
        // primary queue. this makes primary messages completely resolve
        bool bUsingSecondaryQueue = (m_SecondaryQueue.Count() > 0);
        if (bUsingSecondaryQueue)
        {
            doneWork = true;
            messageIndex = m_SecondaryQueue.Head();
            messageItem = &m_SecondaryQueue[messageIndex];
        }
        else if (bUsingDelayedQueue)
        {
            if (m_DelayedMessageQueue.Count() > 0)
            {
                messageItem = (MessageItem_t*)&m_DelayedMessageQueue.ElementAtHead();
            }
            if (!messageItem || messageItem->_arrivalTime > time)
            {
                // no more items in the delayed message queue, move to the system queue
                bUsingDelayedQueue = false;
                continue;
            }
        }
        else
        {
            messageIndex = m_MessageQueue.Head();
            messageItem = &m_MessageQueue[messageIndex];
        }

        // message debug code

        if (m_bDebugMessages)
        {
            const char* qname = bUsingSecondaryQueue ? "Secondary" : "Primary";

            if (strcmp(messageItem->_params->GetName(), "Tick")
                && strcmp(messageItem->_params->GetName(), "MouseFocusTicked")
                && strcmp(messageItem->_params->GetName(), "KeyFocusTicked")
                && strcmp(messageItem->_params->GetName(), "CursorMoved"))
            {
                if (!stricmp(messageItem->_params->GetName(), "command"))
                {
                    g_pIVgui->DPrintf2("%s Queue dispatching command( %s, %s -- %i )\n", qname, messageItem->_params->GetName(), messageItem->_params->GetString("command"),
                                       messageItem->_messageID);
                }
                else
                {
                    g_pIVgui->DPrintf2("%s Queue dispatching( %s -- %i )\n", qname, messageItem->_params->GetName(), messageItem->_messageID);
                }
            }
        }

        // send it
        KeyValues* params = messageItem->_params;

        VPanel* vto = (VPanel*)g_pIVgui->HandleToPanel(messageItem->_messageTo);
        if (vto)
        {
            //			Msg("Sending message: %s to %s\n", params ? params->GetName() : "\"\"", vto->GetName() ? vto->GetName() : "\"\"");
            vto->SendMessage(params, g_pIVgui->HandleToPanel(messageItem->_from));
        }

        // free the keyvalues memory
        // we can't reference the messageItem pointer anymore since the queue might have moved in memory
        if (params)
        {
            params->deleteThis();
        }

        // remove it from the queue
        if (bUsingSecondaryQueue)
        {
            m_SecondaryQueue.Remove(messageIndex);
        }
        else if (bUsingDelayedQueue)
        {
            m_DelayedMessageQueue.RemoveAtHead();
        }
        else
        {
            m_MessageQueue.Remove(messageIndex);
        }
    }

    m_InDispatcher = false;
    return doneWork;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CVGui::MarkPanelForDeletion(VPANEL panel)
{
    PostMessage(panel, new KeyValues("Delete"), NULL);
}

//-----------------------------------------------------------------------------
// Purpose: Adds a message to the queue to be sent to a user
//-----------------------------------------------------------------------------
void CVGui::PostMessage(VPANEL target, KeyValues* params, VPANEL from, float delay)
{
    if (!target)
    {
        if (params)
        {
            params->deleteThis();
        }
        return;
    }

    MessageItem_t messageItem;
    messageItem._messageTo = (target != (VPANEL)-1) ? g_pIVgui->PanelToHandle(target) : 0xFFFFFFFF;
    messageItem._params = params;
    Assert(params->GetName());
    messageItem._from = g_pIVgui->PanelToHandle(from);
    messageItem._arrivalTime = 0;
    messageItem._messageID = m_iCurrentMessageID++;

    // add the message to the correct message queue
    if (delay > 0.0f)
    {
        messageItem._arrivalTime = g_pSystem->GetTimeMillis() + (delay * 1000);
        m_DelayedMessageQueue.Insert(messageItem);
    }
    else if (m_InDispatcher)
    {
        m_SecondaryQueue.AddToTail(messageItem);
    }
    else
    {
        m_MessageQueue.AddToTail(messageItem);
    }
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CVGui::ShutdownMessage(unsigned int shutdownID)
{
    // broadcast Shutdown to all the top level windows, and see if any take notice
    VPANEL panel = g_pSurfaceNext->GetEmbeddedPanel();
    for (int i = 0; i < ((VPanel*)panel)->GetChildCount(); i++)
    {
        g_pIVgui->PostMessage((VPANEL)((VPanel*)panel)->GetChild(i), new KeyValues("ShutdownRequest", "id", shutdownID), NULL);
    }

    // post to the top level window as well
    g_pIVgui->PostMessage(panel, new KeyValues("ShutdownRequest", "id", shutdownID), NULL);
}

//-----------------------------------------------------------------------------
// Purpose: Clears all the memory queues and free's their memory
//-----------------------------------------------------------------------------
void CVGui::ClearMessageQueues()
{
    Assert(!m_InDispatcher);

    {
        FOR_EACH_LL(m_MessageQueue, i)
        {
            if (m_MessageQueue[i]._params)
            {
                m_MessageQueue[i]._params->deleteThis();
            }
        }
    }
    m_MessageQueue.RemoveAll();

    // secondary message queue, holds all vgui messages generated by vgui
    {
        FOR_EACH_LL(m_SecondaryQueue, i)
        {
            if (m_SecondaryQueue[i]._params)
            {
                m_SecondaryQueue[i]._params->deleteThis();
            }
        }
    }
    m_SecondaryQueue.RemoveAll();

    // timing queue, holds all the messages that have to arrive at a specified time
    while (m_DelayedMessageQueue.Count() > 0)
    {
        if (m_DelayedMessageQueue.ElementAtHead()._params)
        {
            m_DelayedMessageQueue.ElementAtHead()._params->deleteThis();
        }
        m_DelayedMessageQueue.RemoveAtHead();
    }
}

void CVGui::DPrintf(const char* format, ...)
{
    char buf[2048];
    va_list argList;

    va_start(argList, format);
    Q_vsnprintf(buf, sizeof(buf), format, argList);
    va_end(argList);

    ::OutputDebugString(buf);
}

void CVGui::DPrintf2(const char* format, ...)
{
    char buf[2048];
    va_list argList;
    static int ctr = 0;

    Q_snprintf(buf, sizeof(buf), "%d:", ctr++);

    va_start(argList, format);
    Q_vsnprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), format, argList);
    va_end(argList);

    ::OutputDebugString(buf);
}

void vgui2::vgui_strcpy(char* dst, int dstLen, const char* src)
{
    Assert(dst!=nullptr);
    Assert(dstLen>=0);
    Assert(src!=nullptr);

    int srcLen = strlen(src) + 1;
    if (srcLen > dstLen)
    {
        srcLen = dstLen;
    }

    memcpy(dst, src, srcLen - 1);
    dst[srcLen - 1] = 0;
}

//-----------------------------------------------------------------------------
// Init, shutdown
//-----------------------------------------------------------------------------
bool CVGui::Init(CreateInterfaceFn* factoryList, int numFactories)
{
    if (m_bInitialized)
        return true;

    m_bInitialized = true;

    m_hContext = DEFAULT_VGUI_CONTEXT;
    m_bDebugMessages = CommandLine()->FindParm("-vguimessages") ? true : false;

    return VGui_InternalLoadInterfaces(factoryList, numFactories);
}

void CVGui::Shutdown()
{
    g_pSystem->SaveUserConfigFile();

    DestroyAllContexts();
    ClearMessageQueues();

    g_pSystem->Shutdown();
    g_pScheme->Shutdown(true);
    g_pSurfaceNext->Shutdown();

    m_bInitialized = false;
}
