// GUI.h
#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "HospitalSystem.h"

// Button 
struct Button {
    sf::RectangleShape shape;
    sf::Text           label;
    sf::Color          normalColor;
    sf::Color          hoverColor;

    Button(const std::string& text, sf::Font& font,
        float x, float y, float w, float h,
        sf::Color normal, sf::Color hover);

    bool isHovered(sf::RenderWindow& window);
    bool isClicked(sf::RenderWindow& window, sf::Event& event);
    void draw(sf::RenderWindow& window);
    void setPosition(float x, float y);
};

// InputField 
struct InputField {
    sf::RectangleShape box;
    sf::Text           text;
    sf::Text           placeholder;
    std::string        value;
    bool               active;
    bool               isPassword;

    InputField(const std::string& hint, sf::Font& font,
        float x, float y, float w, float h,
        bool password = false);

    void        handleEvent(sf::Event& event);
    void        draw(sf::RenderWindow& window);
    void        clear();
    const char* getValue() const;
};

//  GUI 
class GUI {
private:
    sf::RenderWindow window;
    sf::Font         font;
    sf::Texture      bgTexture;
    sf::Sprite       bgSprite;
    HospitalSystem& sys;
    bool pendingFormAfterPanel;
    
    //Enumerations
    enum Screen {
        SCREEN_LOGIN,
        SCREEN_PATIENT_MENU,
        SCREEN_DOCTOR_MENU,
        SCREEN_ADMIN_MENU,
        SCREEN_FORM,
        SCREEN_EXIT,
        SCREEN_BOOK_APPOINTMENT,
    };

    enum FormType {
        FORM_NONE,
        FORM_BOOK_APPOINTMENT,
        FORM_PAY_BILL,
        FORM_ADD_PRESCRIPTION,
        FORM_ADD_DOCTOR,
        FORM_ADD_PATIENT,
        FORM_REMOVE_PATIENT,
        FORM_DISCHARGE_PATIENT,
        FORM_CANCEL_APPOINTMENT,
        FORM_TOP_UP,
        FORM_MARK_COMPLETE,
        FORM_MARK_NOSHOW,
        FORM_VIEW_PAT_HISTORY,
        FORM_REMOVE_DOCTOR     
    };
    FormType pendingFormType;
    //  Session state 
    Screen      currentScreen;
    FormType    currentForm;
    int         loggedInId;
    char        loggedInRole[20];

    //   Status bar        
    std::string statusMessage;
    sf::Color   statusColor;

    //  Info panel    
    bool                     showPanel;
    std::string              panelTitle;
    std::vector<std::string> panelLines;

    // Form state
    std::vector<InputField*> formFields;
    std::vector<std::string> formLabels;
    std::string              formTitle;
    std::string              formError;
    int                      topUpAttempts;

    // Cached layout values set once in run() and reused by drawForm()
    // so both functions agree on field height without recomputing.
    float formFieldH;   // = H * 0.07f
    float formStartY;   // = H * 0.18f
    float formPanelW;   // = W * 0.55f

    // Date / time picker state
    bool showTimeSlotPicker;
    bool showDatePicker;
    int  selectedDay;
    int  selectedMonth;
    int  selectedYear;
    // Book appointment multi-step state
    int bookStep;           
    char bookSpec[50];
    int  bookDocId;
    char bookDate[15];
    int  bookDateAttempts;
    std::vector<std::string> availableSlots;

    // Draw helpers
    void drawBackground();
    void drawHeader(const std::string& title);
    void drawStatus();
    void drawInfoPanel();
    void drawLoginScreen();
    void drawPatientMenu();
    void drawDoctorMenu();
    void drawAdminMenu();
    void drawForm();

    //Form helpers 
    void openForm(FormType type);
    void submitForm();
    void clearForm();

    // Action helpers 
    void showData(const std::string& title,
        const std::vector<std::string>& lines);

public:
    GUI(HospitalSystem& sys);
    ~GUI();
    void run();
};

#endif
