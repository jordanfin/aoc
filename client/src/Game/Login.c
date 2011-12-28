////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Game/Login.h>
#include <Game/User.h>
#include <Game/Application.h>

#include <Audio/SoundManager.h>

#include <Graphics/OpenGLHelper.h>

#include <Network/Protocol.h>

#include <Window/Window.h>

#include <Gui/WindowManager.h>

#include <System/Timer.h>
#include <System/Error.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define LOGIN_SOUND_THEME				"data/audio/1.ogg"

//struct GuiWindow * LoginWindow, * AccountWindow;
//struct GuiWidget * PasswordTextBox, * UserTextBox, * AccountList;

////////////////////////////////////////////////////////////
/// Connect button released event
////////////////////////////////////////////////////////////
void LoginConnect_ButtonReleased(struct MouseButtonEvent * ButtonEvent)
{
	// Copy the data
	//strcpy(UserGetData()->Name, UserTextBox->Text);
	//strcpy(UserGetData()->Password, PasswordTextBox->Text);

	// Login
	//ProtocolConnect(); // Show in render "Connecting..."

	// Terrain test
	GameSetStatus(GAME_RUN);
}

////////////////////////////////////////////////////////////
/// Exit button released event
////////////////////////////////////////////////////////////
void LoginExit_ButtonReleased(struct MouseButtonEvent * ButtonEvent)
{
	// Exit
	GameSetStatus(GAME_DESTROY);
}

////////////////////////////////////////////////////////////
/// Initialize the login
////////////////////////////////////////////////////////////
bool LoginOpen()
{
	// Play sound
	SoundBufferPlay(LOGIN_SOUND_THEME, false, 0, 0, 0);
	GuiOpenWindow(GuiGetWindow("frmAccLogin"));
	return true;
}

////////////////////////////////////////////////////////////
/// Renders the logo of the game
////////////////////////////////////////////////////////////
void LoginRenderLogo()
{
	TextureRenderGrh(LOGIN_GRH_LOGO,
					(WindowWidth / 2) - (IndexGetGrh(LOGIN_GRH_LOGO)->PixelWidth / 2),
					(100 - (IndexGetGrh(LOGIN_GRH_LOGO)->PixelHeight / 2)));
}

////////////////////////////////////////////////////////////
/// Close the login
////////////////////////////////////////////////////////////
void LoginClose()
{
	// Close the window
	GuiCloseWindow(GuiGetWindow("frmAccLogin"));
}

////////////////////////////////////////////////////////////
/// Connect button released event
////////////////////////////////////////////////////////////
void AccountConnect_ButtonReleased(struct MouseButtonEvent * ButtonEvent)
{
	// Login the player
	struct PacketPlayer PlayerStruct = {{0}};;

	// PlayerStruct.Index = (Int8)AccountList->Attributes.List.ItemSelected;

	if (PlayerStruct.Index < 0)
	{
		// Show alert

		return;
	}

	// Send packet
	ProtocolSendData(NETWORK_TYPE_LOGIN, NETWORK_CODE_PLAYER, &PlayerStruct, sizeof(struct PacketPlayer));
}

////////////////////////////////////////////////////////////
/// Exit button released event
////////////////////////////////////////////////////////////
void AccountExit_ButtonReleased(struct MouseButtonEvent * ButtonEvent)
{
	// Exit
	GuiCloseWindow(GuiGetWindow("frmAccMain"));
	// ProtocolDisconnect();
	GameSetStatus(GAME_LOGIN);
}


////////////////////////////////////////////////////////////
/// Initialize the account
////////////////////////////////////////////////////////////
void AccountOpen()
{
	/*if (GuiWindowStatus(AccountWindow) == GUI_WINDOW_NOTLOADED)
	{
		UInt32 i;
		struct GuiWidget * ConnectButton, * ExitButton;

		AccountWindow = GuiWindowCreate();

		ConnectButton = GuiButtonCreate(80, 200, 8, "Login");
		//ConnectButton->ButtonReleased = AccountConnect_ButtonReleased;

		ExitButton = GuiButtonCreate(250, 200, 8, "Exit");
		//ExitButton->ButtonReleased = AccountExit_ButtonReleased;

		GuiWindowAddWidget(AccountWindow, GuiFormCreate(GUI_FORM_STYLE_2, GUI_FORM_CENTERED, GUI_FORM_CENTERED, 450, 250, 8, NULL));
		GuiWindowAddWidget(AccountWindow, ConnectButton);
		GuiWindowAddWidget(AccountWindow, ExitButton);

		AccountList = GuiListCreate(20, 20, 200, 160, 8);

		for (i = 0; i < UserGetData()->PlayersSize; i++)
		{
			GuiListAddItem(&AccountList->Attributes.List, &UserGetData()->Players[i].Name[0]);
		}

		GuiWindowAddWidget(AccountWindow, AccountList);
	}

	GuiOpenWindow(AccountWindow);*/
}

////////////////////////////////////////////////////////////
/// Close the account
////////////////////////////////////////////////////////////
void AccountClose()
{
	// Close the window
	GuiCloseWindow(GuiGetWindow("frmAccMain"));
}
