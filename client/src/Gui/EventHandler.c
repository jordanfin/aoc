////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Gui/EventHandler.h>
#include <Game/Login.h>
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct EventNode
{
	struct EventPtr Data;
	struct EventNode * Next;	
};

struct EventNode * First = NULL;
struct EventNode * Last = NULL;

////////////////////////////////////////////////////////////
/// Initialize all events
////////////////////////////////////////////////////////////
void GuiEventHandlerInitialize()
{
	// Login window
	GuiEventHandlerAdd("LogConnect_ButtonReleased", (void*)&LoginConnect_ButtonReleased);
	GuiEventHandlerAdd("LogExit_ButtonReleased", (void*)&LoginExit_ButtonReleased);

	// Account window
	GuiEventHandlerAdd("AccConnect_ButtonReleased", (void*)&AccountConnect_ButtonReleased);
	GuiEventHandlerAdd("AccExit_ButtonReleased", (void*)&AccountExit_ButtonReleased);
}

////////////////////////////////////////////////////////////
// Create an event for the list
////////////////////////////////////////////////////////////
struct EventNode * GuiEventHandlerCreate()
{
	struct EventNode * Event = (struct EventNode*)malloc(sizeof(struct EventNode));

	if (!Event)
		return NULL;

	// Event->Data.Ptr = NULL;
	Event->Data.Type = GUI_EVENT_ERROR;
	Event->Data.ID[0] = NULLCHAR;

	return Event;
}

////////////////////////////////////////////////////////////
/// Return the event type
////////////////////////////////////////////////////////////
UInt32 GuiEventHandlerType(char * EventType)
{
	if (strcmp(EventType, "KeyPressed") == 0)
		return GUI_EVENT_KEY_PRESSED;
	else if (strcmp(EventType, "KeyReleased") == 0)
		return GUI_EVENT_KEY_RELEASED;
	else if (strcmp(EventType, "TextEntered") == 0)
		return GUI_EVENT_TEXT_ENTERED;
	else if (strcmp(EventType, "ButtonPressed") == 0)
		return GUI_EVENT_BUTTON_PRESSED;
	else if (strcmp(EventType, "ButtonReleased") == 0)
		return GUI_EVENT_BUTTON_RELEASED;
	else if (strcmp(EventType, "MouseMove") == 0)
		return GUI_EVENT_MOUSE_MOVE;
	else if (strcmp(EventType, "Open") == 0)
		return GUI_EVENT_OPEN;
	else if (strcmp(EventType, "Close") == 0)
		return GUI_EVENT_CLOSE;
	else
		return GUI_EVENT_ERROR;
}


////////////////////////////////////////////////////////////
/// Get the event type of the function id
////////////////////////////////////////////////////////////
UInt32 GuiEventHandlerGetType(char * FunctionID)
{
	char EventTypeStr[0xFF];

	// Get the type
	sreadfield(EventTypeStr, FunctionID, '_', 1);
	
	// Check for the event
	return GuiEventHandlerType(EventTypeStr);
}

////////////////////////////////////////////////////////////
// Search an event by id
////////////////////////////////////////////////////////////
struct EventNode * GuiEventHandlerSearch(char * ID)
{
	struct EventNode * Iterator;

	for (Iterator = First; Iterator != NULL; Iterator = Iterator->Next)
	{
		if (strcmp(ID, Iterator->Data.ID) == 0)
		{
			return Iterator;
		}
	}

	return NULL;
}

////////////////////////////////////////////////////////////
// Add an event to the list
////////////////////////////////////////////////////////////
void GuiEventHandlerAdd(char * ID, void * Func)
{
	struct EventNode * Node = GuiEventHandlerCreate();

	if (Node)
	{
		// Set the type
		Node->Data.Type = GuiEventHandlerGetType(ID);

        // Set the function pointer
        switch (Node->Data.Type)
        {
            case GUI_EVENT_KEY_PRESSED :
                Node->Data.Ptr.KeyPressed = (void (*)(struct KeyEvent*))Func;
                break;

            case GUI_EVENT_KEY_RELEASED :
                Node->Data.Ptr.KeyReleased = (void (*)(struct KeyEvent*))Func;
                break;

            case GUI_EVENT_TEXT_ENTERED :
                Node->Data.Ptr.TextEntered = (void (*)(struct TextEvent*))Func;
                break;

            case GUI_EVENT_BUTTON_PRESSED :
                Node->Data.Ptr.ButtonPressed = (void (*)(struct MouseButtonEvent*))Func;
                break;

            case GUI_EVENT_BUTTON_RELEASED :
                Node->Data.Ptr.ButtonReleased = (void (*)(struct MouseButtonEvent*))Func;
                break;

            case GUI_EVENT_MOUSE_MOVE :
                Node->Data.Ptr.MouseMove = (void (*)(struct MouseMoveEvent*))Func;
                break;

            case GUI_EVENT_OPEN :
                Node->Data.Ptr.Open = (void (*)())Func;
                break;

            case GUI_EVENT_CLOSE :
                Node->Data.Ptr.Close = (void (*)())Func;
                break;

            default :       // By default, no add the function
				Delete(Node);
                return;
        }

        // Copy the function ID
        strcpy(Node->Data.ID, ID);

		// Add to the list
		if (First == NULL)
		{
			// If it is not initialized
			First = Last = Node;
		}
		else
		{
			Last->Next = Node;
			Node->Next = NULL;
			Last = Node;
		}
	}
}

////////////////////////////////////////////////////////////
// Remove an event to the list by id
////////////////////////////////////////////////////////////
void GuiEventHandlerRemove(char * ID)
{
	// Search for the event
	struct EventNode * Node = GuiEventHandlerSearch(ID);

	if (Node)
	{
		struct EventNode * Temp, * Prev;

		// Set to null the function ptr
		// Node->Data.Ptr = NULL;

		// Remove from the list
		Temp = Node; Prev = First;

		if (Temp == Prev)
		{
			First = First->Next;

			if (Last == Temp)
			{
				Last = Last->Next;
			}
		}
		else
		{
			while (Prev->Next != Temp)
			{
				Prev = Prev->Next;
			}

			Prev->Next = Temp->Next;

			if (Last == Temp)
				Last = Prev;
		}

		// Destroy it
		Delete(Temp);
	}
}

////////////////////////////////////////////////////////////
// Get an event by id
////////////////////////////////////////////////////////////
struct EventPtr * GuiEventHandlerGet(char * ID)
{
	// Return the event
	return &GuiEventHandlerSearch(ID)->Data;
}

////////////////////////////////////////////////////////////
// Destroy the list
////////////////////////////////////////////////////////////
void GuiEventHandlerDestroy()
{
	struct EventNode * Iterator = First;

	// Delete all one by one
	while (Iterator != NULL)
	{
		struct EventNode * Temp = Iterator->Next;

		// Set to null the function ptr
		// Iterator->Data.Ptr = NULL;

		// Delete it
		Delete(Iterator);

		Iterator = Temp;
	}
}