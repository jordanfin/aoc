////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Core/Console.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
Thread	ConsoleThread;	///< Thread console
Mutex	ConsoleMutex;	///< Mutex console
bool	CmdListSorted;	///< Console command list array sorting value

////////////////////////////////////////////////////////////
// Initialize command console
////////////////////////////////////////////////////////////
bool ConsoleInitialize()
{
	Int32 i, CommandSize;
	char Section[0xFF];

	struct Dictionary * Data = ParserLoad(COMMAND_LIST_PATH);

	if (Data == NULL)
	{
		MessageError("ConsoleInitialize", "Error when the program tried to load the command list: %s", COMMAND_LIST_PATH);
		return false;
	}

	CommandSize = ParserGetInt(Data, "INIT:CmdSize", -1);

	// Load the command list
	for (i = 0; i < CommandSize; i++)
	{
		sprintf(Section, "CMDLST:%d", i);
		CommandList[i].Index = i;
		strcpy(CommandList[i].Data, ParserGetString(Data, Section, NULL));
	}

	ParserFreeDict(Data);

	// Set switch
	ConsoleRun = true;

	// Set sorted value
	CmdListSorted = false;

	// Initialize the mutex
	MutexInitialize(&ConsoleMutex);

	// Run the thread
	ThreadLaunch(&ConsoleThread, (FuncType)&ConsoleRoutine, NULL);

	return true;
}

////////////////////////////////////////////////////////////
// Console command helper comparator
////////////////////////////////////////////////////////////
static Int32 ConsoleCommandCompare(const void * p1, const void * p2)
{
	if (p1 == NULL || p2 == NULL)
		return CONSOLE_ERROR;

    return strcmp( (const char*)p1, (const char*)p2 );
}

////////////////////////////////////////////////////////////
// Console command parser
////////////////////////////////////////////////////////////
Int32 ConsoleParseCommand(char * BufferCommand)
{
	struct CommandData * Command;

    if (!CmdListSorted)
    {
        qsort(CommandList, CONSOLE_COMMAND_SIZE, sizeof(*CommandList), ConsoleCommandCompare);
        CmdListSorted = true;
    }

    Command = bsearch(BufferCommand, CommandList, CONSOLE_COMMAND_SIZE, sizeof(*CommandList), ConsoleCommandCompare);

    return Command ? Command->Index : CONSOLE_ERROR;
}

////////////////////////////////////////////////////////////
// Console command parser
////////////////////////////////////////////////////////////
void ConsoleParseArguments(struct CommandData * Command, char * Arguments)
{
	UInt32 i, j;
	UInt32 LastPos = 0;

	for (i = 0; i < CONSOLE_COMMAND_ARG_SIZE; i++)
	{
		for (j = LastPos; j < CONSOLE_BUFFER_SIZE; j++)
		{
			if ( (Arguments[j] == ':') || (Arguments[j] == NULLCHAR) )
			{
				if (LastPos == 0)
				{
					memcpy(Command->Arguments[i], &Arguments[0], j);
					Command->Arguments[i][j] = NULLCHAR;
				}
				else
				{
					memcpy(Command->Arguments[i], &Arguments[LastPos], (j - LastPos));
					Command->Arguments[i][j] = NULLCHAR;
				}

				LastPos = (j + 1);

				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////
// Input buffer parser
////////////////////////////////////////////////////////////
Int32 ConsoleParseBuffer(char * Buffer)
{
	char Command[CONSOLE_BUFFER_SIZE];
	Int32 i;

	for (i = 0; i < CONSOLE_BUFFER_SIZE; i++)
	{
		if (Buffer[i] == NULLCHAR)
		{
			return ConsoleParseCommand(Buffer);
		}

		if (Buffer[i] == ':')
		{
			Int32 CommandID;

			// Copy command
			memcpy(Command, Buffer, i);

			// Set finally null char
			Command[i] = NULLCHAR;

			// Copy arguments
			memcpy(&Buffer[0], &Buffer[i+1], strlen(Buffer - i));

			// Parse command
			CommandID = ConsoleParseCommand(Command);

			// Parse arguments
			ConsoleParseArguments(&CommandList[CommandID], Buffer);

			// Return ID
			return CommandID;
		}
	}

	return CONSOLE_ERROR;	
}

////////////////////////////////////////////////////////////
// Command console method
////////////////////////////////////////////////////////////
void ConsoleRoutine()
{
	char Buffer[CONSOLE_BUFFER_SIZE];

	while (ConsoleRun)
	{
		printf("Server>");

		#ifdef SYSTEM_WINDOWS
			scanf_s("%s", Buffer, CONSOLE_BUFFER_SIZE);
		#else
			scanf("%s", Buffer);
		#endif

		switch (ConsoleParseBuffer(Buffer))
		{
			case CONSOLE_CMD_HELP :
				printf("Command console help :\n");
				printf("     time : show info about server time\n");
				printf("     exit : close the application\n");
				break;

			case CONSOLE_CMD_TIME :
				printf("Server time : 00:00:00\n");
				break;

			case CONSOLE_CMD_EXIT :
				printf("Closing, wait...\n");
				ConsoleRun = false;
				AppSetRun(false);
				break;

			case CONSOLE_ERROR :
				MessageError("ConsoleRoutine", "Error when the program tried to manage the command: %s", Buffer);
				break;

			default : break;
		}
	}

	ThreadWait(&ConsoleThread);
}

////////////////////////////////////////////////////////////
// Destroy command console
////////////////////////////////////////////////////////////
void ConsoleDestroy()
{
	ThreadTerminate(&ConsoleThread);
}