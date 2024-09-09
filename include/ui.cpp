#include <ESPUI.h>
#include <EEPROM.h>
#include <esp_system.h>

#include <WiFi.h>
#include <ESPmDNS.h>

//Settings
#define SLOW_BOOT 0
#define HOSTNAME "CoffeValidator"
#define FORCE_USE_HOTSPOT 0


//Function Prototypes
void connectWifi();
void setUpUI();
void enterWifiDetailsCallback(Control *sender, int type);
void textCallback(Control *sender, int type);
void generalCallback(Control *sender, int type);
void styleCallback(Control *sender, int type);
void updateCallback(Control *sender, int type);
void getTimeCallback(Control *sender, int type);
void graphAddCallback(Control *sender, int type);
void graphClearCallback(Control *sender, int type);
void extendedCallback(Control* sender, int type, void* param);
void restartSystem(Control *sender, int typ);
void updateCoffePrice(Control *sender, int typ);
void uiShootDown(Control *sender, int typ);


int readCoffePrice();
//UI handles
uint16_t wifi_ssid_text, wifi_pass_text;
int coffeePrice;
uint16_t mainLabel, mainSwitcher, mainSlider, mainText, mainNumber, mainScrambleButton, mainTime;
uint16_t styleButton, styleLabel, styleSwitcher, styleSlider, styleButton2, styleLabel2, styleSlider2;
uint16_t graph;
volatile bool updates = false;



// This is the main function which builds our GUI
void setUpUI() {



	//Turn off verbose debugging
	ESPUI.setVerbosity(Verbosity::Quiet);

	//Make sliders continually report their position as they are being dragged.
	ESPUI.sliderContinuous = true;

	//This GUI is going to be a tabbed GUI, so we are adding most controls using ESPUI.addControl
	//which allows us to set a parent control. If we didn't need tabs we could use the simpler add
	//functions like:
	//    ESPUI.button()
	//    ESPUI.label()


	/*
	 * Tab: User Control
	 * This tab contains all the basic ESPUI controls, and shows how to read and update them at runtime.
	 *-----------------------------------------------------------------------------------------------------------*/
	auto userCtab = ESPUI.addControl(Tab, "", "User Control");
  ESPUI.addControl(Button, "Update", "Update", Carrot, userCtab, generalCallback);
  ESPUI.addControl(Separator, "Search User", "", None, userCtab);
	mainText = ESPUI.addControl(Text, "USER RFID", "00:00:00:00", Alizarin, userCtab, generalCallback);
  mainText = ESPUI.addControl(Text, "USER NAME", "Csirke Csaba", Alizarin, userCtab, generalCallback);

	/*
	 * Tab: Machine Controls
	 * This tab contains all the basic ESPUI controls, and shows how to read and update them at runtime.
	 *-----------------------------------------------------------------------------------------------------------*/
	auto machineTab = ESPUI.addControl(Tab, "", "Machine Controls");
  mainSwitcher = ESPUI.addControl(Switcher, "Free", "0", Sunflower, machineTab, generalCallback);
  
  auto groupPrice = ESPUI.addControl(Label, "Coffee price $","", Dark, machineTab, generalCallback);
  String price = String(readCoffePrice());
  

  coffeePrice=ESPUI.addControl(Number, "", price, Emerald, groupPrice, generalCallback);
  ESPUI.addControl(Button, "", "Update", Carrot, groupPrice, updateCoffePrice);
  

	/*
	 * Tab: WiFi Credentials
	 * You use this tab to enter the SSID and password of a wifi network to autoconnect to.
	 *-----------------------------------------------------------------------------------------------------------*/
	auto wifitab = ESPUI.addControl(Tab, "", "WiFi Credentials");
	wifi_ssid_text = ESPUI.addControl(Text, "SSID", "", Alizarin, wifitab, textCallback);
	//Note that adding a "Max" control to a text control sets the max length
	ESPUI.addControl(Max, "", "32", None, wifi_ssid_text);
	wifi_pass_text = ESPUI.addControl(Text, "Password", "", Alizarin, wifitab, textCallback);
	ESPUI.addControl(Max, "", "64", None, wifi_pass_text);
	ESPUI.addControl(Button, "Save", "Save", Peterriver, wifitab, enterWifiDetailsCallback);
  ESPUI.addControl(Button, "Restart", "Restart", Peterriver, wifitab, restartSystem);
  ESPUI.addControl(Button, "shoot down the ui", "shoot down the UI", Peterriver, wifitab, uiShootDown);

	//Finally, start up the UI. 
	//This should only be called once we are connected to WiFi.
	ESPUI.begin(HOSTNAME);

}

//This callback generates and applies inline styles to a bunch of controls to change their colour.
//The styles created are of the form: 
//	"border-bottom: #999 3px solid; background-color: #aabbcc;"
//	"background-color: #aabbcc;"
void styleCallback(Control *sender, int type) {
	//Declare space for style strings. These have to be static so that they are always available
	//to the websocket layer. If we'd not made them static they'd be allocated on the heap and
	//will be unavailable when we leave this function.
	static char stylecol1[60], stylecol2[30]; 
	if(type == B_UP) {
		//Generate two random HTML hex colour codes, and print them into CSS style rules
		sprintf(stylecol1, "border-bottom: #999 3px solid; background-color: #%06X;", (unsigned int) random(0x0, 0xFFFFFF));
		sprintf(stylecol2, "background-color: #%06X;", (unsigned int) random(0x0, 0xFFFFFF));

		//Apply those styles to various elements to show how controls react to styling
		ESPUI.setPanelStyle(styleButton, stylecol1);
		ESPUI.setElementStyle(styleButton, stylecol2);
		ESPUI.setPanelStyle(styleLabel, stylecol1);
		ESPUI.setElementStyle(styleLabel, stylecol2);
		ESPUI.setPanelStyle(styleSwitcher, stylecol1);
		ESPUI.setElementStyle(styleSwitcher, stylecol2);
		ESPUI.setPanelStyle(styleSlider, stylecol1);
		ESPUI.setElementStyle(styleSlider, stylecol2);
	}
}

void updateCallback(Control *sender, int type) {
	updates = (sender->value.toInt() > 0);
}



void graphClearCallback(Control *sender, int type) {
	if(type == B_UP) {
		ESPUI.clearGraph(graph);
	}
}


//Most elements in this test UI are assigned this generic callback which prints some
//basic information. Event types are defined in ESPUI.h
void generalCallback(Control *sender, int type) {
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
}

// Most elements in this test UI are assigned this generic callback which prints some
// basic information. Event types are defined in ESPUI.h
// The extended param can be used to hold a pointer to additional information
// or for C++ it can be used to return a this pointer for quick access
// using a lambda function
void extendedCallback(Control* sender, int type, void* param)
{
    Serial.print("CB: id(");
    Serial.print(sender->id);
    Serial.print(") Type(");
    Serial.print(type);
    Serial.print(") '");
    Serial.print(sender->label);
    Serial.print("' = ");
    Serial.println(sender->value);
    Serial.print("param = ");
    Serial.println((long)param);
}


void readStringFromEEPROM(String& buf, int baseaddress, int size) {
	buf.reserve(size);
	for (int i = baseaddress; i < baseaddress+size; i++) {
		char c = EEPROM.read(i);
		buf += c;
		if(!c) break;
	}	
}

void connectWifi() {
	int connect_timeout;

	WiFi.setHostname(HOSTNAME);

	Serial.println("Begin wifi...");

	//Load credentials from EEPROM 
	if(!(FORCE_USE_HOTSPOT)) {
		yield();
		EEPROM.begin(100);
		String stored_ssid, stored_pass;
		readStringFromEEPROM(stored_ssid, 0, 32);
		readStringFromEEPROM(stored_pass, 32, 96);
		EEPROM.end();
	
		//Try to connect with stored credentials, fire up an access point if they don't work.
		WiFi.begin(stored_ssid.c_str(), stored_pass.c_str());

		connect_timeout = 28; //7 seconds
		while (WiFi.status() != WL_CONNECTED && connect_timeout > 0) {
			delay(250);
			Serial.print(".");
			connect_timeout--;
		}
	}
	
	if (WiFi.status() == WL_CONNECTED) {
		Serial.println(WiFi.localIP());
		Serial.println("Wifi started");

		if (!MDNS.begin(HOSTNAME)) {
			Serial.println("Error setting up MDNS responder!");
		}
	} else {
		Serial.println("\nCreating access point...");
		WiFi.mode(WIFI_AP);
		WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
		WiFi.softAP(HOSTNAME);

		connect_timeout = 20;
		do {
			delay(250);
			Serial.print(",");
			connect_timeout--;
		} while(connect_timeout);
	}
}

void enterWifiDetailsCallback(Control *sender, int type) {
	if(type == B_UP) {
		Serial.println("Saving credentials to EPROM...");
		Serial.println(ESPUI.getControl(wifi_ssid_text)->value);
		Serial.println(ESPUI.getControl(wifi_pass_text)->value);
		unsigned int i;
		EEPROM.begin(100);
		for(i = 0; i < ESPUI.getControl(wifi_ssid_text)->value.length(); i++) {
			EEPROM.write(i, ESPUI.getControl(wifi_ssid_text)->value.charAt(i));
			if(i==30) break; //Even though we provided a max length, user input should never be trusted
		}
		EEPROM.write(i, '\0');

		for(i = 0; i < ESPUI.getControl(wifi_pass_text)->value.length(); i++) {
			EEPROM.write(i + 32, ESPUI.getControl(wifi_pass_text)->value.charAt(i));
			if(i==94) break; //Even though we provided a max length, user input should never be trusted
		}
		EEPROM.write(i + 32, '\0');
		EEPROM.end();
	}
}

void restartSystem(Control *sender, int type) {
	if(type == B_UP){
  esp_restart();
  }
}

void uiShootDown(Control *sender, int type) {
	if(type == B_UP){
    Serial.println("Stop UI");
  }
}

 void updateCoffePrice (Control *sender, int type){
    if(type == B_UP){
      Serial.println("Saving Coffee price to EPROM...");
      Serial.println(ESPUI.getControl(coffeePrice)->value);

      unsigned int i;
      EEPROM.begin(100);

      for(i = 0; i < ESPUI.getControl(coffeePrice)->value.length(); i++) {
        EEPROM.write(i + 96, ESPUI.getControl(coffeePrice)->value.charAt(i));
        if(i==30) break; //Even though we provided a max length, user input should never be trusted
      }
      EEPROM.write(i + 96, '\0');
      EEPROM.end();
      
	}
    Serial.println("Saved Coffee price");
    Serial.println(ESPUI.getControl(coffeePrice)->value);
}


int readCoffePrice(){

    EEPROM.begin(100);
    String coffeePrice;
    readStringFromEEPROM(coffeePrice, 96, 30);
    EEPROM.end();
    Serial.print("EEPROM price: ");
    Serial.println(coffeePrice);
    int x=coffeePrice.toInt();
    uint16_t y = static_cast<uint16_t>(x);
    return x;
	
}
 

void textCallback(Control *sender, int type) {
	//This callback is needed to handle the changed values, even though it doesn't do anything itself.
}