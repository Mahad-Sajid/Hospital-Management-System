#include "GUI.h"
#include "FileHandler.h"
#include "Validator.h"
#include "SlotUnavailableException.h"
#include "InsufficientFundsException.h"
#include <fstream>
#include <ctime>
static const sf::Color BG_DARK(12, 12, 14);
static const sf::Color BG_CARD(28, 18, 18);
static const sf::Color ACCENT(210, 30, 40);
static const sf::Color ACCENT_HOVER(160, 18, 25);
static const sf::Color BTN_WHITE(240, 240, 245);
static const sf::Color BTN_WHITE_HOVER(200, 200, 210);
static const sf::Color BTN_DARK(38, 22, 22);
static const sf::Color BTN_DARK_HOVER(55, 30, 30);
static const sf::Color BTN_DANGER(160, 18, 25);
static const sf::Color BTN_DANGER_HOVER(120, 10, 18);
static const sf::Color TEXT_WHITE(255, 255, 255);
static const sf::Color TEXT_DARK(12, 12, 14);
static const sf::Color TEXT_MUTED(150, 110, 110);
static const sf::Color INPUT_BG(35, 20, 20);
static const sf::Color INPUT_ACTIVE(55, 30, 30);
static const sf::Color PANEL_OVERLAY(0, 0, 0, 190);

//to get current date and time as string for security log and daily report
static void getCurrentDateTime(char* buf, int maxLen) {
    time_t t = time(nullptr);
    struct tm tm_info;
    localtime_s(&tm_info, &t);
    strftime(buf, maxLen, "%Y-%m-%d %H:%M:%S", &tm_info);
}

// Button
Button::Button(const std::string& text, sf::Font& font,
    float x, float y, float w, float h,
    sf::Color normal, sf::Color hover)
    : normalColor(normal), hoverColor(hover)
{
    shape.setPosition(x, y);
    shape.setSize({ w, h });
    shape.setFillColor(normal);
    shape.setOutlineThickness(1);
    shape.setOutlineColor(sf::Color(80, 40, 40));

    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(16);
    // Dark text on white/light buttons, white text on dark/red buttons
    bool lightBg = (normal.r > 180);
    label.setFillColor(lightBg ? TEXT_DARK : TEXT_WHITE);

    sf::FloatRect tb = label.getLocalBounds();
    label.setPosition(x + (w - tb.width) / 2.f - tb.left,
        y + (h - tb.height) / 2.f - tb.top);
}

void Button::setPosition(float x, float y) {
    shape.setPosition(x, y);
    sf::FloatRect tb = label.getLocalBounds();
    float w = shape.getSize().x, h = shape.getSize().y;
    label.setPosition(x + (w - tb.width) / 2.f - tb.left,
        y + (h - tb.height) / 2.f - tb.top);
}

bool Button::isHovered(sf::RenderWindow& window) {
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    return shape.getGlobalBounds().contains(
        static_cast<float>(mouse.x), static_cast<float>(mouse.y));
}

bool Button::isClicked(sf::RenderWindow& window, sf::Event& event) {
    return event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left &&
        isHovered(window);
}

void Button::draw(sf::RenderWindow& window) {
    bool hov = isHovered(window);
    shape.setFillColor(hov ? hoverColor : normalColor);
    // Brighten outline on hover
    shape.setOutlineColor(hov ? ACCENT : sf::Color(80, 40, 40));
    window.draw(shape);
    window.draw(label);
}

// InputField 
InputField::InputField(const std::string& hint, sf::Font& font,
    float x, float y, float w, float h, bool password)
    : active(false), isPassword(password)
{
    box.setPosition(x, y);
    box.setSize({ w, h });
    box.setFillColor(INPUT_BG);
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color(80, 40, 40));

    text.setFont(font);
    text.setCharacterSize(16);
    text.setFillColor(TEXT_WHITE);
    text.setPosition(x + 12.f, y + (h - 20.f) / 2.f);

    placeholder.setFont(font);
    placeholder.setString(hint);
    placeholder.setCharacterSize(16);
    placeholder.setFillColor(TEXT_MUTED);
    placeholder.setPosition(x + 12.f, y + (h - 20.f) / 2.f);
}

void InputField::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::TextEntered && active) {
        if (event.text.unicode == 8) {
            if (!value.empty()) value.pop_back();
        }
        else if (event.text.unicode >= 32 && event.text.unicode < 128) {
            if (value.size() < 100)
                value += static_cast<char>(event.text.unicode);
        }
        std::string display = isPassword
            ? std::string(value.size(), '*') : value;
        text.setString(display);
    }
}

void InputField::draw(sf::RenderWindow& window) {
    box.setFillColor(active ? INPUT_ACTIVE : INPUT_BG);
    box.setOutlineColor(active ? ACCENT : sf::Color(80, 40, 40));
    window.draw(box);
    if (value.empty() && !active) window.draw(placeholder);
    else window.draw(text);
}

void InputField::clear() {
    value = "";
    text.setString("");
    active = false;
}

const char* InputField::getValue() const { return value.c_str(); }

// GUI Constructor 
GUI::GUI(HospitalSystem& sys)
    : sys(sys),
    window(sf::VideoMode::getDesktopMode(),
        "MediCore Hospital System", sf::Style::Fullscreen),
    currentScreen(SCREEN_LOGIN),
    currentForm(FORM_NONE),
    loggedInId(-1),
    showPanel(false),
    statusColor(ACCENT),
    formFieldH(0.f),
    formStartY(0.f),
    formPanelW(0.f),
    showDatePicker(false),
    showTimeSlotPicker(false),
    selectedDay(1),
    selectedMonth(1),
    selectedYear(2024),
    pendingFormAfterPanel(false),
    pendingFormType(FORM_NONE),
    topUpAttempts(0),
    bookStep(0),
    bookDocId(-1),
    bookDateAttempts(0)
{
    window.setFramerateLimit(60);
    loggedInRole[0] = '\0';
    bookSpec[0] = '\0';
    bookDate[0] = '\0';

    font.loadFromFile("C:\\Users\\GNG\\Desktop\\project oop\\x64\\Debug\\Roboto-Regular.ttf");
    if (bgTexture.loadFromFile("C:\\Users\\GNG\\Desktop\\project oop\\x64\\Debug\\hospital.jpg")); {
        bgSprite.setTexture(bgTexture);
        float winW = static_cast<float>(window.getSize().x);
        float winH = static_cast<float>(window.getSize().y);
        bgSprite.setScale(
            winW / static_cast<float>(bgTexture.getSize().x),
            winH / static_cast<float>(bgTexture.getSize().y));
    }
}
GUI::~GUI() { clearForm(); }

void GUI::clearForm() {
    for (auto f : formFields) delete f;
    formFields.clear();
    formLabels.clear();
    formError = "";
}

// Draw Helpers
void GUI::drawBackground() {
    window.clear(BG_DARK);
    // Top red stripe
    sf::RectangleShape strip(
        { static_cast<float>(window.getSize().x), 3.f });
    strip.setFillColor(ACCENT);
    window.draw(strip);
    // Bottom red stripe
    sf::RectangleShape strip2(
        { static_cast<float>(window.getSize().x), 2.f });
    strip2.setPosition(0.f, static_cast<float>(window.getSize().y) - 2.f);
    strip2.setFillColor(ACCENT);
    window.draw(strip2);
}

void GUI::drawHeader(const std::string& title) {
    float W = static_cast<float>(window.getSize().x);

    sf::RectangleShape bar({ W, 52.f });
    bar.setPosition(0.f, 3.f);
    bar.setFillColor(sf::Color(20, 12, 12));
    window.draw(bar);

    sf::RectangleShape border({ W, 2.f });
    border.setPosition(0.f, 55.f);
    border.setFillColor(ACCENT);
    window.draw(border);

    // Red cross icon
    sf::RectangleShape cv({ 6.f, 22.f });
    cv.setPosition(22.f, 18.f);
    cv.setFillColor(ACCENT);
    window.draw(cv);
    sf::RectangleShape ch({ 22.f, 6.f });
    ch.setPosition(14.f, 26.f);
    ch.setFillColor(ACCENT);
    window.draw(ch);

    sf::Text logo;
    logo.setFont(font);
    logo.setString("MEDICORE");
    logo.setCharacterSize(18);
    logo.setFillColor(ACCENT);
    logo.setStyle(sf::Text::Bold);
    logo.setPosition(46.f, 17.f);
    window.draw(logo);

    sf::RectangleShape sep({ 1.f, 22.f });
    sep.setPosition(138.f, 17.f);
    sep.setFillColor(sf::Color(80, 40, 40));
    window.draw(sep);

    sf::Text titleText;
    titleText.setFont(font);
    titleText.setString(title);
    titleText.setCharacterSize(14);
    titleText.setFillColor(TEXT_MUTED);
    titleText.setPosition(150.f, 20.f);
    window.draw(titleText);
}

void GUI::drawStatus() {
    if (statusMessage.empty()) return;
    float W = static_cast<float>(window.getSize().x);
    float H = static_cast<float>(window.getSize().y);

    sf::RectangleShape bar({ W, 36.f });
    bar.setPosition(0.f, H - 38.f);
    bool isErr = (statusColor.r > 150 && statusColor.g < 80);
    bar.setFillColor(isErr
        ? sf::Color(60, 15, 15, 220)
        : sf::Color(15, 40, 15, 220));
    window.draw(bar);

    sf::Text msg;
    msg.setFont(font);
    msg.setString(statusMessage);
    msg.setCharacterSize(15);
    msg.setFillColor(statusColor);
    sf::FloatRect tb = msg.getLocalBounds();
    msg.setPosition((W - tb.width) / 2.f, H - 32.f);
    window.draw(msg);
}

void GUI::drawInfoPanel() {
    if (!showPanel) return;
    float W = static_cast<float>(window.getSize().x);
    float H = static_cast<float>(window.getSize().y);

    sf::RectangleShape overlay({ W, H });
    overlay.setFillColor(PANEL_OVERLAY);
    window.draw(overlay);

    float pw = W * 0.76f, ph = H * 0.76f;
    float px = (W - pw) / 2.f, py = (H - ph) / 2.f;

    sf::RectangleShape panel({ pw, ph });
    panel.setPosition(px, py);
    panel.setFillColor(BG_CARD);
    panel.setOutlineThickness(2);
    panel.setOutlineColor(ACCENT);
    window.draw(panel);

    // Red accent top bar
    sf::RectangleShape topBar({ pw, 4.f });
    topBar.setPosition(px, py);
    topBar.setFillColor(ACCENT);
    window.draw(topBar);

    sf::Text title;
    title.setFont(font);
    title.setString(panelTitle);
    title.setCharacterSize(20);
    title.setFillColor(ACCENT);
    title.setStyle(sf::Text::Bold);
    title.setPosition(px + 20.f, py + 14.f);
    window.draw(title);

    sf::RectangleShape divider({ pw - 40.f, 1.f });
    divider.setPosition(px + 20.f, py + 48.f);
    divider.setFillColor(sf::Color(80, 40, 40));
    window.draw(divider);

    float lineY = py + 58.f;
    for (int i = 0; i < static_cast<int>(panelLines.size()); i++) {
        if (lineY + 24.f > py + ph - 36.f) break;
        // Alternating row shading
        if (i > 1 && i % 2 == 0) {
            sf::RectangleShape row({ pw - 40.f, 22.f });
            row.setPosition(px + 20.f, lineY - 2.f);
            row.setFillColor(sf::Color(40, 22, 22, 120));
            window.draw(row);
        }
        sf::Text line;
        line.setFont(font);
        line.setString(panelLines[i]);
        line.setCharacterSize(14);
        line.setFillColor(panelLines[i].empty() ? TEXT_MUTED : TEXT_WHITE);
        line.setPosition(px + 20.f, lineY);
        window.draw(line);
        lineY += 24.f;
    }

    sf::Text hint;
    hint.setFont(font);
    hint.setString("Press ESC or click to close");
    hint.setCharacterSize(13);
    hint.setFillColor(TEXT_MUTED);
    hint.setPosition(px + 20.f, py + ph - 26.f);
    window.draw(hint);
}

//Screen Renderers
void GUI::drawLoginScreen() {
    float W = static_cast<float>(window.getSize().x);
    float H = static_cast<float>(window.getSize().y);
    float cx = W / 2.0f, cy = H / 2.0f;

    window.draw(bgSprite);
    sf::RectangleShape overlay({ W, H });
    overlay.setFillColor(sf::Color(0, 0, 0, 175));
    window.draw(overlay);

    float cw = W * 0.34f, ch = H * 0.62f;
    float cardLeft = cx - cw / 2.f;
    float cardTop = cy - ch / 2.f - 20.f;

    sf::RectangleShape card({ cw, ch });
    card.setPosition(cardLeft, cardTop);
    card.setFillColor(BG_CARD);
    card.setOutlineThickness(1);
    card.setOutlineColor(sf::Color(80, 40, 40));
    window.draw(card);

    sf::RectangleShape cardAccent({ cw, 3.f });
    cardAccent.setPosition(cardLeft, cardTop);
    cardAccent.setFillColor(ACCENT);
    window.draw(cardAccent);

    // Cross icon
    sf::RectangleShape icV({ 8.f, 30.f });
    icV.setPosition(cx - 4.f, cardTop + 28.f);
    icV.setFillColor(ACCENT);
    window.draw(icV);
    sf::RectangleShape icH({ 30.f, 8.f });
    icH.setPosition(cx - 15.f, cardTop + 39.f);
    icH.setFillColor(ACCENT);
    window.draw(icH);

    sf::Text welcome;
    welcome.setFont(font);
    welcome.setString("Welcome to MediCore");
    welcome.setCharacterSize(20);
    welcome.setFillColor(TEXT_WHITE);
    welcome.setStyle(sf::Text::Bold);
    sf::FloatRect wb = welcome.getLocalBounds();
    welcome.setPosition(cx - wb.width / 2.f, cardTop + 78.f);
    window.draw(welcome);

    sf::Text sub;
    sub.setFont(font);
    sub.setString("Hospital Management System");
    sub.setCharacterSize(13);
    sub.setFillColor(TEXT_MUTED);
    sf::FloatRect sb = sub.getLocalBounds();
    sub.setPosition(cx - sb.width / 2.f, cardTop + 106.f);
    window.draw(sub);

    // Field labels
    auto drawLabel = [&](const std::string& lbl, float y) {
        sf::Text t;
        t.setFont(font);
        t.setString(lbl);
        t.setCharacterSize(11);
        t.setFillColor(TEXT_MUTED);
        t.setPosition(cardLeft + 28.f, y);
        window.draw(t);
        };
    drawLabel("USER ID", cardTop + ch * 0.40f - 18.f);
    drawLabel("PASSWORD", cardTop + ch * 0.55f - 18.f);
    drawLabel("SELECT ROLE TO LOGIN", cardTop + ch * 0.72f - 18.f);
}

void GUI::drawPatientMenu() {
    float W = static_cast<float>(window.getSize().x);
    float H = static_cast<float>(window.getSize().y);
    float cardW = (W - 160.f) / 3.0f;
    float cardH = H * 0.12f;
    float cardY = H * 0.08f;

    const char* patName = "Patient";
    const char* patBal = "--";
    static char balBuf[30];
    for (int i = 0; i < static_cast<int>(sys.patients.size()); i++) {
        if (sys.patients[i].getId() == loggedInId) {
            patName = sys.patients[i].getName();
            MyString::copy(balBuf, "PKR ");
            char tmp[20]; MyString::fromFloat(sys.patients[i].getBalance(), tmp);
            MyString::concat(balBuf, tmp);
            patBal = balBuf;
            break;
        }
    }

    struct StatCard { const char* label; const char* value; sf::Color accent; };
    StatCard cards[3] = {
        { "PATIENT", patName, ACCENT },
        { "BALANCE", patBal,  sf::Color(180, 30, 40) },
        { "STATUS",  "Active",sf::Color(140, 20, 28) }
    };

    for (int i = 0; i < 3; i++) {
        float x = 80.f + static_cast<float>(i) * (cardW + 10.f);
        sf::RectangleShape card({ cardW, cardH });
        card.setPosition(x, cardY);
        card.setFillColor(BG_CARD);
        card.setOutlineThickness(1);
        card.setOutlineColor(sf::Color(60, 30, 30));
        window.draw(card);
        sf::RectangleShape accent({ 4.f, cardH });
        accent.setPosition(x, cardY);
        accent.setFillColor(cards[i].accent);
        window.draw(accent);
        sf::Text lbl; lbl.setFont(font); lbl.setString(cards[i].label);
        lbl.setCharacterSize(11); lbl.setFillColor(TEXT_MUTED);
        lbl.setPosition(x + 16.f, cardY + 14.f); window.draw(lbl);
        sf::Text val; val.setFont(font); val.setString(cards[i].value);
        val.setCharacterSize(15); val.setFillColor(TEXT_WHITE);
        val.setStyle(sf::Text::Bold);
        val.setPosition(x + 16.f, cardY + 34.f); window.draw(val);
    }

    float cardBottom = cardY + cardH;

    // Show ALL pending appointments for this patient
    std::string apptHint;
    bool anyPending = false;
    sf::Text apptInfo; apptInfo.setFont(font); apptInfo.setString(apptHint);
    apptInfo.setCharacterSize(13); apptInfo.setFillColor(sf::Color(220, 120, 120));
    apptInfo.setPosition(80.f, cardBottom + 6.f); window.draw(apptInfo);

    // Show ALL unpaid bills for this patient
    std::string billHint;
    bool anyUnpaid = false;
    sf::Text billInfo; billInfo.setFont(font); billInfo.setString(billHint);
    billInfo.setCharacterSize(13); billInfo.setFillColor(sf::Color(200, 100, 100));
    billInfo.setPosition(80.f, cardBottom + 26.f); window.draw(billInfo);

    sf::Text section; section.setFont(font); section.setString("QUICK ACTIONS");
    section.setCharacterSize(11); section.setFillColor(TEXT_MUTED);
    section.setPosition(80.f, cardBottom + 48.f); window.draw(section);
}

void GUI::drawDoctorMenu() {
    float W = static_cast<float>(window.getSize().x);
    float H = static_cast<float>(window.getSize().y);
    float cardY = H * 0.08f, cardH = H * 0.12f;
    float cardW = (W - 160.f) / 2.0f;

    const char* docName = "Doctor", * docSpec = "";
    for (int i = 0; i < static_cast<int>(sys.doctors.size()); i++) {
        if (sys.doctors[i].getId() == loggedInId) {
            docName = sys.doctors[i].getName();
            docSpec = sys.doctors[i].getSpecialization();
            break;
        }
    }

    auto drawStatCard = [&](float x, const char* lbl, const char* val,
        sf::Color acc) {
            sf::RectangleShape card({ cardW, cardH });
            card.setPosition(x, cardY);
            card.setFillColor(BG_CARD);
            card.setOutlineThickness(1);
            card.setOutlineColor(sf::Color(60, 30, 30));
            window.draw(card);
            sf::RectangleShape a({ 4.f, cardH });
            a.setPosition(x, cardY); a.setFillColor(acc);
            window.draw(a);
            sf::Text l; l.setFont(font); l.setString(lbl);
            l.setCharacterSize(11); l.setFillColor(TEXT_MUTED);
            l.setPosition(x + 16.f, cardY + 14.f); window.draw(l);
            sf::Text v; v.setFont(font); v.setString(val);
            v.setCharacterSize(15); v.setFillColor(TEXT_WHITE);
            v.setStyle(sf::Text::Bold);
            v.setPosition(x + 16.f, cardY + 34.f); window.draw(v);
        };

    drawStatCard(80.f, "DOCTOR", docName, ACCENT);
    drawStatCard(90.f + cardW, "SPECIALIZATION", docSpec,
        sf::Color(180, 30, 40));

    float cardBottom = cardY + cardH;
    sf::Text section; section.setFont(font);
    section.setString("QUICK ACTIONS");
    section.setCharacterSize(11); section.setFillColor(TEXT_MUTED);
    section.setPosition(80.f, cardBottom + 28.f); window.draw(section);
}

void GUI::drawAdminMenu() {
    float W = static_cast<float>(window.getSize().x);
    float H = static_cast<float>(window.getSize().y);
    float cardY = H * 0.08f, cardH = H * 0.12f;
    float cardW = (W - 160.f) / 3.0f;

    const char* labels[3] = { "PATIENTS", "DOCTORS", "APPOINTMENTS" };
    int values[3] = {
        static_cast<int>(sys.patients.size()),
        static_cast<int>(sys.doctors.size()),
        static_cast<int>(sys.appointments.size())
    };
    sf::Color accents[3] = { ACCENT, sf::Color(180,30,40), sf::Color(140,20,28) };

    for (int i = 0; i < 3; i++) {
        float x = 80.f + static_cast<float>(i) * (cardW + 10.f);
        sf::RectangleShape card({ cardW, cardH });
        card.setPosition(x, cardY);
        card.setFillColor(BG_CARD);
        card.setOutlineThickness(1);
        card.setOutlineColor(sf::Color(60, 30, 30));
        window.draw(card);
        sf::RectangleShape acc({ 4.f, cardH });
        acc.setPosition(x, cardY); acc.setFillColor(accents[i]);
        window.draw(acc);
        sf::Text lbl; lbl.setFont(font); lbl.setString(labels[i]);
        lbl.setCharacterSize(11); lbl.setFillColor(TEXT_MUTED);
        lbl.setPosition(x + 16.f, cardY + 14.f); window.draw(lbl);
        char num[10]; MyString::fromInt(values[i], num);
        sf::Text val; val.setFont(font); val.setString(num);
        val.setCharacterSize(22); val.setFillColor(TEXT_WHITE);
        val.setStyle(sf::Text::Bold);
        val.setPosition(x + 16.f, cardY + 34.f); window.draw(val);
    }

    sf::Text section; section.setFont(font); section.setString("ADMIN ACTIONS");
    section.setCharacterSize(11); section.setFillColor(TEXT_MUTED);
    section.setPosition(80.f, cardY + cardH + 18.f); window.draw(section);
}

void GUI::drawForm() {
    float W = static_cast<float>(window.getSize().x);
    float H = static_cast<float>(window.getSize().y);
    float pw = formPanelW;
    float px = (W - pw) / 2.0f;
    float startY = formStartY;

    float formH = H * 0.08f
        + static_cast<float>(formFields.size()) * (formFieldH + H * 0.04f)
        + H * 0.14f;

    sf::RectangleShape formCard({ pw + 60.f, formH });
    formCard.setPosition(px - 30.f, startY - 42.f);
    formCard.setFillColor(BG_CARD);
    formCard.setOutlineThickness(1);
    formCard.setOutlineColor(sf::Color(80, 40, 40));
    window.draw(formCard);

    sf::RectangleShape topAccent({ pw + 60.f, 3.f });
    topAccent.setPosition(px - 30.f, startY - 42.f);
    topAccent.setFillColor(ACCENT);
    window.draw(topAccent);

    sf::Text title; title.setFont(font); title.setString(formTitle);
    title.setCharacterSize(20); title.setFillColor(TEXT_WHITE);
    title.setStyle(sf::Text::Bold);
    title.setPosition(px, startY - 20.f); window.draw(title);

    bool twoCol = formFields.size() >= 4;
    float colW = twoCol ? (pw - 16.f) / 2.0f : pw;

    for (int i = 0; i < static_cast<int>(formFields.size()); i++) {
        float fx = twoCol
            ? px + static_cast<float>(i % 2) * (colW + 16.f)
            : px;
        float fy = twoCol
            ? startY + static_cast<float>(i / 2) * (formFieldH + H * 0.04f)
            : startY + static_cast<float>(i) * 78.f;

        sf::Text label; label.setFont(font); label.setString(formLabels[i]);
        label.setCharacterSize(11); label.setFillColor(TEXT_MUTED);
        label.setPosition(fx, fy); window.draw(label);

        formFields[i]->box.setPosition(fx, fy + 18.f);
        formFields[i]->box.setSize({ colW, formFieldH });
        formFields[i]->text.setPosition(
            fx + 14.f, fy + 18.f + (formFieldH - 20.f) / 2.f);
        formFields[i]->placeholder.setPosition(
            fx + 14.f, fy + 18.f + (formFieldH - 20.f) / 2.f);

        if (formFields[i]->active) {
            sf::RectangleShape activeBar({ 3.f, formFieldH });
            activeBar.setPosition(fx, fy + 18.f);
            activeBar.setFillColor(ACCENT);
            window.draw(activeBar);
        }
        formFields[i]->draw(window);
    }

    //
    // Draw clickable slot buttons when on time slot step
    if (currentForm == FORM_BOOK_APPOINTMENT && bookStep == 3
        && !availableSlots.empty()) {
        float slotBtnW = 90.f, slotBtnH = 36.f, slotGap = 8.f;
        // Place below the single input field
        float slotY = formStartY + formFieldH + H * 0.02f;
        sf::Text slotLabel; slotLabel.setFont(font);
        slotLabel.setString("Click a slot to select:");
        slotLabel.setCharacterSize(13); slotLabel.setFillColor(TEXT_MUTED);
        slotLabel.setPosition(px, slotY - 20.f); window.draw(slotLabel);

        for (int s = 0; s < static_cast<int>(availableSlots.size()); s++) {
            float sx = px + static_cast<float>(s) * (slotBtnW + slotGap);
            sf::Vector2i mouse = sf::Mouse::getPosition(window);
            bool hov = (mouse.x >= sx && mouse.x <= sx + slotBtnW
                && mouse.y >= slotY && mouse.y <= slotY + slotBtnH);

            sf::RectangleShape slotBtn({ slotBtnW, slotBtnH });
            slotBtn.setPosition(sx, slotY);
            slotBtn.setFillColor(hov ? ACCENT_HOVER : ACCENT);
            slotBtn.setOutlineThickness(1);
            slotBtn.setOutlineColor(sf::Color(255, 255, 255, 60));
            window.draw(slotBtn);

            sf::Text slotTxt; slotTxt.setFont(font);
            slotTxt.setString(availableSlots[s]);
            slotTxt.setCharacterSize(14);
            slotTxt.setFillColor(TEXT_WHITE);
            sf::FloatRect tb = slotTxt.getLocalBounds();
            slotTxt.setPosition(
                sx + (slotBtnW - tb.width) / 2.f - tb.left,
                slotY + (slotBtnH - tb.height) / 2.f - tb.top);
            window.draw(slotTxt);
        }
    }

    if (!formError.empty()) {
        float errY = twoCol
            ? startY
            + static_cast<float>((formFields.size() + 1) / 2)
            * (formFieldH + H * 0.04f) + 8.f
            : startY
            + static_cast<float>(formFields.size()) * 78.f + 8.f;

        sf::RectangleShape errBg({ pw, 36.f });
        errBg.setPosition(px, errY);
        errBg.setFillColor(sf::Color(60, 15, 15, 200));
        errBg.setOutlineThickness(1);
        errBg.setOutlineColor(sf::Color(220, 38, 38, 160));
        window.draw(errBg);

        sf::Text err; err.setFont(font); err.setString(formError);
        err.setCharacterSize(14); err.setFillColor(sf::Color(255, 160, 160));
        err.setPosition(px + 10.f, errY + 9.f); window.draw(err);
    }
}

//Form Setup
void GUI::openForm(FormType type) {
    clearForm();
    currentForm = type;
    currentScreen = SCREEN_FORM;
    formError = "";

    float W = static_cast<float>(window.getSize().x);
    float pw = formPanelW;
    float px = (W - pw) / 2.f;

    switch (type) {
    case FORM_BOOK_APPOINTMENT:
        formTitle = "Book Appointment";
        if (bookStep == 0)      formLabels = { "Specialization (e.g. Cardiology)" };
        else if (bookStep == 1) formLabels = { "Doctor ID" };
        else if (bookStep == 2) formLabels = { "Date (DD-MM-YYYY)" };
        else if (bookStep == 3) formLabels = { "Time Slot (click a button below or type)" };
        break;
    case FORM_PAY_BILL:
        formTitle = "Pay a Bill"; formLabels = { "Bill ID" }; break;
    case FORM_ADD_PRESCRIPTION:
        formTitle = "Add Prescription";
        formLabels = { "Appointment ID", "Patient ID",
                       "Medicines (semicolon-separated)", "Notes" };
        break;
    case FORM_ADD_DOCTOR:
        formTitle = "Add Doctor";
        formLabels = { "Name", "Specialization", "Contact",
                       "Password", "Fee (Rs.)" };
        break;
    case FORM_ADD_PATIENT:
        formTitle = "Add Patient";
        formLabels = { "Name", "Age", "Gender (M/F)",
                       "Contact", "Password", "Initial Balance" };
        break;
    case FORM_DISCHARGE_PATIENT:
        formTitle = "Discharge Patient"; formLabels = { "Patient ID" }; break;
    case FORM_CANCEL_APPOINTMENT:
        formTitle = "Cancel Appointment"; formLabels = { "Appointment ID" }; break;
    case FORM_TOP_UP:
        formTitle = "Top Up Balance"; formLabels = { "Amount (PKR)" }; break;
    case FORM_MARK_COMPLETE:
        formTitle = "Mark Appointment Complete"; formLabels = { "Appointment ID" }; break;
    case FORM_MARK_NOSHOW:
        formTitle = "Mark Appointment No-Show"; formLabels = { "Appointment ID" }; break;
    case FORM_VIEW_PAT_HISTORY:
        formTitle = "View Patient Medical History"; formLabels = { "Patient ID" }; break;
    case FORM_REMOVE_DOCTOR:
        formTitle = "Remove Doctor"; formLabels = { "Doctor ID" }; break;
    case FORM_REMOVE_PATIENT:
        formTitle = "Remove Patient"; formLabels = { "Patient ID" }; break;
    default: break;
    }

    float H = static_cast<float>(window.getSize().y);
    for (int i = 0; i < static_cast<int>(formLabels.size()); i++) {
        float y = formStartY + static_cast<float>(i) * 80.f;
        formFields.push_back(
            new InputField(formLabels[i], font, px, y, pw, 50.f));
    }
}

// Security log helper 
// Append a log entry with real date/time; keep chronological order
static void appendSecurityLogEntry(const char* logPath,
    const char* role, const char* idStr,
    const char* result)
{
    char dt[30];
    getCurrentDateTime(dt, 30);

    // Read existing entries
    std::vector<std::string> existing;
    {
        std::ifstream fin(logPath);
        if (fin.is_open()) {
            char line[400];
            while (fin.getline(line, 400))
                if (line[0] != '\0') existing.push_back(line);
            fin.close();
        }
    }

    // Build new entry
    std::string entry = std::string(dt) + " | " + role
        + " | ID:" + idStr + " | " + result;

    existing.push_back(entry);

    // Write back (already chronological since we append)
    std::ofstream fout(logPath);
    if (fout.is_open()) {
        for (auto& e : existing) fout << e << "\n";
        fout.close();
    }
}

// Form Submission
void GUI::submitForm() {
    formError = "";
    auto val = [&](int i) -> std::string {
        return i < static_cast<int>(formFields.size())
            ? formFields[i]->value : "";
        };

    switch (currentForm) {

        // Book Appointment 
    case FORM_BOOK_APPOINTMENT: {
        auto val0 = val(0);

        // Step 0 — specialization
        if (bookStep == 0) {
            if (val0.empty()) { formError = "Enter a specialization."; return; }
            MyString::copy(bookSpec, val0.c_str(), 50);

            panelLines.clear();
            panelTitle = "Doctors — " + val0;
            panelLines.push_back("ID  |  Name  |  Fee (Rs.)");
            panelLines.push_back("------------------------------------");
            bool anyFound = false;
            for (int i = 0; i < static_cast<int>(sys.doctors.size()); i++) {
                const char* ds = sys.doctors[i].getSpecialization();
                // case-insensitive compare
                int j = 0; bool match = true;
                while (ds[j] && bookSpec[j]) {
                    char a = ds[j], b = bookSpec[j];
                    if (a >= 'A' && a <= 'Z') a += 32;
                    if (b >= 'A' && b <= 'Z') b += 32;
                    if (a != b) { match = false; break; }
                    j++;
                }
                if (match && ds[j] == '\0' && bookSpec[j] == '\0') {
                    char buf[200], did[10], fee[20];
                    MyString::fromInt(sys.doctors[i].getId(), did);
                    MyString::fromFloat(sys.doctors[i].getFee(), fee);
                    MyString::copy(buf, did); MyString::concat(buf, "  |  ");
                    MyString::concat(buf, sys.doctors[i].getName());
                    MyString::concat(buf, "  |  Rs.");
                    MyString::concat(buf, fee);
                    panelLines.push_back(buf);
                    anyFound = true;
                }
            }
            if (!anyFound) { formError = "No doctors for that specialization."; return; }
            showPanel = true;
            bookStep = 1; clearForm(); openForm(FORM_BOOK_APPOINTMENT);
            return;
        }

        // Step 1 — doctor ID
        if (bookStep == 1) {
            bookDocId = atoi(val0.c_str());
            bool found = false;
            for (int i = 0; i < static_cast<int>(sys.doctors.size()); i++) {
                if (sys.doctors[i].getId() == bookDocId) {
                    const char* ds = sys.doctors[i].getSpecialization();
                    int j = 0; bool match = true;
                    while (ds[j] && bookSpec[j]) {
                        char a = ds[j], b = bookSpec[j];
                        if (a >= 'A' && a <= 'Z') a += 32;
                        if (b >= 'A' && b <= 'Z') b += 32;
                        if (a != b) { match = false; break; }
                        j++;
                    }
                    if (match && ds[j] == '\0' && bookSpec[j] == '\0')
                        found = true;
                    break;
                }
            }
            if (!found) { formError = "Doctor not found for that specialization."; return; }
            bookStep = 2; bookDateAttempts = 0;
            clearForm(); openForm(FORM_BOOK_APPOINTMENT);
            return;
        }

        // Step 2 — date
        if (bookStep == 2) {
            if (!Validator::isValidDate(val0.c_str())) {
                bookDateAttempts++;
                if (bookDateAttempts >= 3) {
                    statusMessage = "Too many invalid date attempts.";
                    statusColor = ACCENT;
                    currentScreen = SCREEN_PATIENT_MENU; clearForm(); return;
                }
                formError = "Invalid date (DD-MM-YYYY). Attempt "
                    + std::to_string(bookDateAttempts) + "/3";
                return;
            }
            MyString::copy(bookDate, val0.c_str(), 15);

            const char* allSlots[] = {
                "09:00","10:00","11:00","12:00",
                "13:00","14:00","15:00","16:00" };
            availableSlots.clear();
            for (int s = 0; s < 8; s++) {
                bool taken = false;
                for (int i = 0; i < static_cast<int>(sys.appointments.size()); i++) {
                    if (sys.appointments[i].getDoctorId() == bookDocId &&
                        MyString::compare(sys.appointments[i].getDate(), bookDate) == 0 &&
                        MyString::compare(sys.appointments[i].getTimeSlot(), allSlots[s]) == 0 &&
                        MyString::compare(sys.appointments[i].getStatus(), "cancelled") != 0) {
                        taken = true; break;
                    }
                }
                if (!taken) availableSlots.push_back(allSlots[s]);
            }

            panelLines.clear();
            panelTitle = "Available Slots — " + std::string(bookDate);
            if (availableSlots.empty()) {
                panelLines.push_back("No slots available. Choose another date.");
                showPanel = true;
                bookStep = 2; clearForm(); openForm(FORM_BOOK_APPOINTMENT);
                return;
            }
            panelLines.push_back("Click one of the slot buttons to select:");
            for (auto& s : availableSlots) panelLines.push_back("  " + s);
            showPanel = true;
            bookStep = 3; clearForm(); openForm(FORM_BOOK_APPOINTMENT);
            return;
        }

        // Step 3 — time slot & confirm booking
        if (bookStep == 3) {
            std::string slot = val0;
            if (slot.empty()) { formError = "Select or type a time slot."; return; }
            bool slotOk = false;
            for (auto& s : availableSlots)
                if (s == slot) { slotOk = true; break; }
            if (!slotOk) {
                try { throw SlotUnavailableException("Slot not available."); }
                catch (SlotUnavailableException& e) { formError = e.what(); return; }
            }

            float fee = 0;
            for (int i = 0; i < static_cast<int>(sys.doctors.size()); i++)
                if (sys.doctors[i].getId() == bookDocId)
                {
                    fee = sys.doctors[i].getFee(); break;
                }

            float balance = 0; int patIdx = -1;
            for (int i = 0; i < static_cast<int>(sys.patients.size()); i++) {
                if (sys.patients[i].getId() == loggedInId)
                {
                    balance = sys.patients[i].getBalance(); patIdx = i; break;
                }
            }
            if (balance < fee) {
                try { throw InsufficientFundsException(fee, balance); }
                catch (InsufficientFundsException& e) {
                    formError = e.what();
                    currentScreen = SCREEN_PATIENT_MENU; clearForm(); return;
                }
            }

            int maxApptId = 0;
            for (int i = 0; i < static_cast<int>(sys.appointments.size()); i++)
                if (sys.appointments[i].getId() > maxApptId)
                    maxApptId = sys.appointments[i].getId();

            Appointment a(maxApptId + 1, loggedInId, bookDocId,
                bookDate, slot.c_str(), "pending");
            sys.appointments.add(a);
            FileHandler::appendAppointment(FilePaths::APPOINTMENTS, a);

            int maxBillId = 0;
            for (int i = 0; i < static_cast<int>(sys.bills.size()); i++)
                if (sys.bills[i].getId() > maxBillId)
                    maxBillId = sys.bills[i].getId();

            Bill b(maxBillId + 1, loggedInId, maxApptId + 1,
                fee, "unpaid", bookDate);
            sys.bills.add(b);
            FileHandler::appendBill(FilePaths::BILLS, b);

            sys.patients[patIdx] -= fee;
            FileHandler::savePatients(FilePaths::PATIENTS, sys.patients);

            char idStr[10]; MyString::fromInt(maxApptId + 1, idStr);
            statusMessage = "Appointment booked! ID: " + std::string(idStr);
            statusColor = sf::Color(100, 220, 100);
            currentScreen = SCREEN_PATIENT_MENU; clearForm();
        }
        break;
    }

                              //Pay Bill
    case FORM_PAY_BILL: {
        int billId = atoi(val(0).c_str());
        if (billId <= 0) { formError = "Enter a valid Bill ID."; return; }
        bool found = false;
        for (int i = 0; i < static_cast<int>(sys.bills.size()); i++) {
            if (sys.bills[i].getId() == billId &&
                sys.bills[i].getPatientId() == loggedInId) {
                if (MyString::compare(sys.bills[i].getStatus(), "paid") == 0)
                {
                    formError = "Bill already paid."; return;
                }
                float amt = sys.bills[i].getAmount();
                for (int j = 0; j < static_cast<int>(sys.patients.size()); j++) {
                    if (sys.patients[j].getId() == loggedInId) {
                        if (sys.patients[j].getBalance() < amt)
                        {
                            formError = "Insufficient balance."; return;
                        }
                        sys.patients[j] -= amt;
                        FileHandler::savePatients(FilePaths::PATIENTS, sys.patients);
                        break;
                    }
                }
                sys.bills[i].setStatus("paid");
                FileHandler::saveBills(FilePaths::BILLS, sys.bills);
                found = true;
                statusMessage = "Bill paid successfully!";
                statusColor = sf::Color(100, 220, 100);
                break;
            }
        }
        if (!found) formError = "Bill not found for your account.";
        else { currentScreen = SCREEN_PATIENT_MENU; clearForm(); }
        break;
    }

                      // Add Prescription
    case FORM_ADD_PRESCRIPTION: {
        int apptId = atoi(val(0).c_str());
        int patId = atoi(val(1).c_str());
        std::string meds = val(2), notes = val(3);
        if (apptId <= 0 || patId <= 0 || meds.empty())
        {
            formError = "Appt ID, Patient ID and Medicines required."; return;
        }
        int maxId = 0;
        for (int i = 0; i < static_cast<int>(sys.prescriptions.size()); i++)
            if (sys.prescriptions[i].getId() > maxId)
                maxId = sys.prescriptions[i].getId();
        char dateBuf[15]; Validator::getTodayDate(dateBuf);
        Prescription p(maxId + 1, apptId, patId, loggedInId,
            dateBuf, meds.c_str(), notes.c_str());
        sys.prescriptions.add(p);
        FileHandler::appendPrescription(FilePaths::PRESCRIPTIONS, p);
        statusMessage = "Prescription added!";
        statusColor = sf::Color(100, 220, 100);
        currentScreen = SCREEN_DOCTOR_MENU; clearForm(); break;
    }

                              // Add Doctor
    case FORM_ADD_DOCTOR: {
        std::string name = val(0), spec = val(1), contact = val(2),
            pass = val(3), feeStr = val(4);
        float fee = static_cast<float>(atof(feeStr.c_str()));
        if (name.empty() || spec.empty() || contact.empty()
            || pass.empty() || fee <= 0)
        {
            formError = "All fields required. Fee must be > 0."; return;
        }
        int maxId = 0;
        for (int i = 0; i < static_cast<int>(sys.doctors.size()); i++)
            if (sys.doctors[i].getId() > maxId) maxId = sys.doctors[i].getId();
        Doctor d(maxId + 1, name.c_str(), spec.c_str(),
            contact.c_str(), pass.c_str(), fee);
        sys.doctors.add(d);
        FileHandler::appendDoctor(FilePaths::DOCTORS, d);
        statusMessage = "Doctor added! ID: " + std::to_string(maxId + 1);
        statusColor = sf::Color(100, 220, 100);
        currentScreen = SCREEN_ADMIN_MENU; clearForm(); break;
    }

                        // Add Patient
    case FORM_ADD_PATIENT: {
        std::string name = val(0), ageStr = val(1), gender = val(2),
            contact = val(3), pass = val(4), balStr = val(5);
        int age = atoi(ageStr.c_str());
        float balance = static_cast<float>(atof(balStr.c_str()));
        if (name.empty() || age <= 0 || gender.empty()
            || contact.empty() || pass.empty())
        {
            formError = "All fields required."; return;
        }
        int maxId = 0;
        for (int i = 0; i < static_cast<int>(sys.patients.size()); i++)
            if (sys.patients[i].getId() > maxId) maxId = sys.patients[i].getId();
        Patient p(maxId + 1, name.c_str(), age, gender.c_str(),
            contact.c_str(), pass.c_str(), balance);
        sys.patients.add(p);
        FileHandler::appendPatient(FilePaths::PATIENTS, p);
        statusMessage = "Patient added! ID: " + std::to_string(maxId + 1);
        statusColor = sf::Color(100, 220, 100);
        currentScreen = SCREEN_ADMIN_MENU; clearForm(); break;
    }

                         // Remove Doctor 
    case FORM_REMOVE_DOCTOR: {
        int docId = atoi(val(0).c_str());
        if (docId <= 0) { formError = "Enter a valid Doctor ID."; return; }
        if (!sys.doctors.findById(docId))
        {
            formError = "Doctor not found."; return;
        }
        for (int i = 0; i < static_cast<int>(sys.appointments.size()); i++) {
            if (sys.appointments[i].getDoctorId() == docId &&
                MyString::compare(sys.appointments[i].getStatus(), "pending") == 0)
            {
                formError = "Cannot remove: doctor has pending appointments."; return;
            }
        }
        sys.doctors.removeById(docId);
        FileHandler::saveDoctors(FilePaths::DOCTORS, sys.doctors);
        statusMessage = "Doctor removed.";
        statusColor = sf::Color(100, 220, 100);
        currentScreen = SCREEN_ADMIN_MENU; clearForm(); break;
    }

                           // Remove Patient 
    case FORM_REMOVE_PATIENT: {
        int patId = atoi(val(0).c_str());
        if (patId <= 0) { formError = "Enter a valid Patient ID."; return; }
        int patIdx = -1;
        for (int i = 0; i < static_cast<int>(sys.patients.size()); i++)
            if (sys.patients[i].getId() == patId) { patIdx = i; break; }
        if (patIdx == -1) { formError = "Patient not found."; return; }
        for (int i = 0; i < static_cast<int>(sys.appointments.size()); i++)
            if (sys.appointments[i].getPatientId() == patId &&
                MyString::compare(sys.appointments[i].getStatus(), "pending") == 0)
            {
                formError = "Patient has pending appointments."; return;
            }
        for (int i = 0; i < static_cast<int>(sys.bills.size()); i++)
            if (sys.bills[i].getPatientId() == patId &&
                MyString::compare(sys.bills[i].getStatus(), "unpaid") == 0)
            {
                formError = "Patient has unpaid bills."; return;
            }
        sys.patients.removeById(patId);
        FileHandler::savePatients(FilePaths::PATIENTS, sys.patients);
        for (int i = static_cast<int>(sys.appointments.size()) - 1; i >= 0; i--)
            if (sys.appointments[i].getPatientId() == patId)
                sys.appointments.removeById(sys.appointments[i].getId());
        FileHandler::saveAppointments(FilePaths::APPOINTMENTS, sys.appointments);
        for (int i = static_cast<int>(sys.bills.size()) - 1; i >= 0; i--)
            if (sys.bills[i].getPatientId() == patId)
                sys.bills.removeById(sys.bills[i].getId());
        FileHandler::saveBills(FilePaths::BILLS, sys.bills);
        for (int i = static_cast<int>(sys.prescriptions.size()) - 1; i >= 0; i--)
            if (sys.prescriptions[i].getPatientId() == patId)
                sys.prescriptions.removeById(sys.prescriptions[i].getId());
        FileHandler::savePrescriptions(FilePaths::PRESCRIPTIONS, sys.prescriptions);
        statusMessage = "Patient removed.";
        statusColor = sf::Color(100, 220, 100);
        currentScreen = SCREEN_ADMIN_MENU; clearForm(); break;
    }

                            // Discharge Patient
    case FORM_DISCHARGE_PATIENT: {
        int patId = atoi(val(0).c_str());
        if (patId <= 0) { formError = "Enter a valid Patient ID."; return; }
        int patIdx = -1;
        for (int i = 0; i < static_cast<int>(sys.patients.size()); i++)
            if (sys.patients[i].getId() == patId) { patIdx = i; break; }
        if (patIdx == -1) { formError = "Patient not found."; return; }
        for (int i = 0; i < static_cast<int>(sys.bills.size()); i++)
            if (sys.bills[i].getPatientId() == patId &&
                MyString::compare(sys.bills[i].getStatus(), "unpaid") == 0)
            {
                formError = "Patient has unpaid bills."; return;
            }
        for (int i = 0; i < static_cast<int>(sys.appointments.size()); i++)
            if (sys.appointments[i].getPatientId() == patId &&
                MyString::compare(sys.appointments[i].getStatus(), "pending") == 0)
            {
                formError = "Patient has pending appointments."; return;
            }
        char buffer[500];
        sys.patients[patIdx].toCSV(buffer);
        FileHandler::appendLine(FilePaths::DISCHARGED, buffer);
        sys.patients.removeById(patId);
        FileHandler::savePatients(FilePaths::PATIENTS, sys.patients);
        statusMessage = "Patient discharged and archived.";
        statusColor = sf::Color(100, 220, 100);
        currentScreen = SCREEN_ADMIN_MENU; clearForm(); break;
    }

                               // Cancel Appointment
    case FORM_CANCEL_APPOINTMENT: {
        int apptId = atoi(val(0).c_str());
        if (apptId <= 0) { formError = "Enter a valid Appointment ID."; return; }
        Appointment* appt = sys.appointments.findById(apptId);
        if (!appt || appt->getPatientId() != loggedInId ||
            MyString::compare(appt->getStatus(), "pending") != 0)
        {
            formError = "Invalid or non-pending appointment."; return;
        }
        Doctor* doc = sys.doctors.findById(appt->getDoctorId());
        float refund = doc ? doc->getFee() : 0.f;
        appt->setStatus("cancelled");
        FileHandler::saveAppointments(FilePaths::APPOINTMENTS, sys.appointments);
        for (int i = 0; i < static_cast<int>(sys.patients.size()); i++) {
            if (sys.patients[i].getId() == loggedInId) {
                sys.patients[i] += refund;
                FileHandler::savePatients(FilePaths::PATIENTS, sys.patients);
                break;
            }
        }
        for (int i = 0; i < static_cast<int>(sys.bills.size()); i++) {
            if (sys.bills[i].getAppointmentId() == apptId) {
                sys.bills[i].setStatus("cancelled"); break;
            }
        }
        FileHandler::saveBills(FilePaths::BILLS, sys.bills);
        char ref[20]; MyString::fromFloat(refund, ref);
        statusMessage = "Cancelled. Rs." + std::string(ref) + " refunded.";
        statusColor = sf::Color(100, 220, 100);
        currentScreen = SCREEN_PATIENT_MENU; clearForm(); break;
    }

                                // Top Up
    case FORM_TOP_UP: {
        float amount = static_cast<float>(atof(val(0).c_str()));
        if (amount <= 0) { formError = "Enter a positive amount."; return; }
        for (int i = 0; i < static_cast<int>(sys.patients.size()); i++) {
            if (sys.patients[i].getId() == loggedInId) {
                sys.patients[i] += amount;
                FileHandler::savePatients(FilePaths::PATIENTS, sys.patients);
                char bal[20]; MyString::fromFloat(sys.patients[i].getBalance(), bal);
                statusMessage = "Balance updated. New: Rs." + std::string(bal);
                statusColor = sf::Color(100, 220, 100);
                break;
            }
        }
        currentScreen = SCREEN_PATIENT_MENU; clearForm(); break;
    }

                    // Mark Complete
    case FORM_MARK_COMPLETE: {
        int apptId = atoi(val(0).c_str());
        if (apptId <= 0) { formError = "Enter a valid Appointment ID."; return; }
        Appointment* appt = sys.appointments.findById(apptId);
        if (!appt || appt->getDoctorId() != loggedInId ||
            MyString::compare(appt->getStatus(), "pending") != 0)
        {
            formError = "Invalid or not your pending appointment."; return;
        }
        appt->setStatus("completed");
        FileHandler::saveAppointments(FilePaths::APPOINTMENTS, sys.appointments);
        statusMessage = "Marked as completed.";
        statusColor = sf::Color(100, 220, 100);
        currentScreen = SCREEN_DOCTOR_MENU; clearForm(); break;
    }

                           // Mark No-Show
    case FORM_MARK_NOSHOW: {
        int apptId = atoi(val(0).c_str());
        if (apptId <= 0) { formError = "Enter a valid Appointment ID."; return; }
        Appointment* appt = sys.appointments.findById(apptId);
        if (!appt || appt->getDoctorId() != loggedInId ||
            MyString::compare(appt->getStatus(), "pending") != 0)
        {
            formError = "Invalid or not your pending appointment."; return;
        }
        appt->setStatus("noshow");
        for (int i = 0; i < static_cast<int>(sys.bills.size()); i++)
            if (sys.bills[i].getAppointmentId() == apptId)
            {
                sys.bills[i].setStatus("cancelled"); break;
            }
        FileHandler::saveAppointments(FilePaths::APPOINTMENTS, sys.appointments);
        FileHandler::saveBills(FilePaths::BILLS, sys.bills);
        statusMessage = "Marked as no-show.";
        statusColor = ACCENT;
        currentScreen = SCREEN_DOCTOR_MENU; clearForm(); break;
    }

                         // Patient History
    case FORM_VIEW_PAT_HISTORY: {
        int patId = atoi(val(0).c_str());
        if (patId <= 0) { formError = "Enter a valid Patient ID."; return; }
        Patient* pat = sys.patients.findById(patId);
        if (!pat) { formError = "Patient not found."; return; }
        bool hasCompleted = false;
        for (int i = 0; i < static_cast<int>(sys.appointments.size()); i++)
            if (sys.appointments[i].getPatientId() == patId &&
                sys.appointments[i].getDoctorId() == loggedInId &&
                MyString::compare(sys.appointments[i].getStatus(), "completed") == 0)
            {
                hasCompleted = true; break;
            }
        if (!hasCompleted)
        {
            formError = "No completed appointments with this patient."; return;
        }
        std::vector<std::string> lines;
        for (int i = 0; i < static_cast<int>(sys.prescriptions.size()); i++) {
            if (sys.prescriptions[i].getPatientId() == patId &&
                sys.prescriptions[i].getDoctorId() == loggedInId) {
                char buf[300];
                MyString::copy(buf, sys.prescriptions[i].getDate());
                MyString::concat(buf, "  |  ");
                MyString::concat(buf, sys.prescriptions[i].getMedicines());
                lines.push_back(buf);
                char buf2[300]; MyString::copy(buf2, "   Notes: ");
                MyString::concat(buf2, sys.prescriptions[i].getNotes());
                lines.push_back(buf2);
                lines.push_back("");
            }
        }
        if (lines.empty()) lines.push_back("No prescriptions found.");
        showData(std::string("History: ") + pat->getName(), lines);
        currentScreen = SCREEN_DOCTOR_MENU; clearForm(); break;
    }

    default: break;
    }
}

void GUI::showData(const std::string& title,
    const std::vector<std::string>& lines) {
    panelTitle = title;
    panelLines = lines;
    showPanel = true;
}

// Main Loop 
void GUI::run()
{
    float W = static_cast<float>(window.getSize().x);
    float H = static_cast<float>(window.getSize().y);
    float cx = W / 2.0f, cy = H / 2.0f;

    float cw = W * 0.34f, ch = H * 0.62f;
    float cardTop = cy - ch / 2.0f - 20.f;

    // Login fields
    InputField idField("Enter your ID...", font,
        cx - cw * 0.43f, cardTop + ch * 0.40f, cw * 0.86f, H * 0.07f);
    InputField passField("Enter password", font,
        cx - cw * 0.43f, cardTop + ch * 0.55f, cw * 0.86f, H * 0.07f, true);

    // Login role buttons 
    Button patientBtn("Patient", font,
        cx - cw * 0.43f, cardTop + ch * 0.72f, cw * 0.26f, H * 0.06f,
        BTN_DARK, BTN_DARK_HOVER);
    Button doctorBtn("Doctor", font,
        cx - cw * 0.13f, cardTop + ch * 0.72f, cw * 0.26f, H * 0.06f,
        BTN_DARK, BTN_DARK_HOVER);
    Button adminBtn("Admin", font,
        cx + cw * 0.17f, cardTop + ch * 0.72f, cw * 0.26f, H * 0.06f,
        BTN_DARK, BTN_DARK_HOVER);

    Button logoutBtn("Logout", font,
        W - 110.f, 12.f, 90.f, 32.f, ACCENT, ACCENT_HOVER);

    // Menu button layout
    float bw = W * 0.18f, bh = H * 0.08f, gap = W * 0.003f;
    float totalBW = 4.f * bw + 3.f * gap;
    float col1 = cx - totalBW / 2.0f;
    float col2 = col1 + bw + gap;
    float col3 = col2 + bw + gap;
    float col4 = col3 + bw + gap;

    float patRow1 = H * 0.08f + H * 0.12f + 62.f;
    float patRow2 = patRow1 + bh + H * 0.008f;
    float docRow1 = H * 0.08f + H * 0.12f + 42.f;
    float docRow2 = docRow1 + bh + H * 0.008f;
    float row1 = H * 0.30f, row2 = row1 + bh + H * 0.008f,
        row3 = row2 + bh + H * 0.008f;

    // Patient buttons — white for primary, red for danger
    Button bookBtn("Book Appointment", font, col1, patRow1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button cancelAppBtn("Cancel Appointment", font, col2, patRow1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button viewAppBtn("View Appointments", font, col3, patRow1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button viewMedBtn("Medical Records", font, col4, patRow1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button viewBillBtn("View Bills", font, col1, patRow2, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button payBillBtn("Pay a Bill", font, col2, patRow2, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button topUpBtn("Top Up Balance", font, col3, patRow2, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button viewPresBtn("Prescriptions", font, col4, patRow2, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);

    // Doctor buttons
    Button viewPatientsBtn("View All Patients", font, col1, docRow1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button viewDocAppBtn("View Appointments", font, col2, docRow1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button markCompleteBtn("Mark Complete", font, col3, docRow1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button markNoshowBtn("Mark No-Show", font, col4, docRow1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button addPresBtn("Write Prescription", font, col1, docRow2, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button viewPatHistBtn("Patient History", font, col3, docRow2, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);

    // Admin buttons
    Button addDocBtn("Add Doctor", font, col1, row1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button removeDocBtn("Remove Doctor", font, col2, row1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button addPatBtn("Add Patient", font, col3, row1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button viewAllPatsBtn("View Patients", font, col4, row1, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button viewAllDocsBtn("View Doctors", font, col1, row2, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button viewAllApptsBtn("View Appointments", font, col2, row2, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button viewUnpaidBtn("Unpaid Bills", font, col3, row2, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button dischargePatBtn("Discharge Patient", font, col4, row2, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button viewAllBtn("All Records", font, col1, row3, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button reportBtn("Daily Report", font, col2, row3, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button secLogBtn("Security Log", font, col3, row3, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);
    Button removePatBtn("Remove Patient", font, col4, row3, bw, bh, BTN_WHITE, BTN_WHITE_HOVER);

    // Form layout
    formPanelW = W * 0.55f;
    formFieldH = H * 0.07f;
    formStartY = H * 0.18f;
    float& pw = formPanelW;

    Button submitBtn("Submit", font, cx - pw * 0.5f, 0.f,
        pw * 0.46f, H * 0.07f, ACCENT, ACCENT_HOVER);
    Button cancelBtn("Cancel", font, cx + pw * 0.02f, 0.f,
        pw * 0.46f, H * 0.07f, BTN_DARK, BTN_DARK_HOVER);

    // Event / render loop 
    while (window.isOpen() && currentScreen != SCREEN_EXIT) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); break; }
            bool eventHandled = false;

            // ESC
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape) {
                if (showPanel) { showPanel = false; continue; }
                if (currentScreen == SCREEN_FORM) {
                    clearForm();
                    if (MyString::compare(loggedInRole, "patient") == 0)
                        currentScreen = SCREEN_PATIENT_MENU;
                    else if (MyString::compare(loggedInRole, "doctor") == 0)
                        currentScreen = SCREEN_DOCTOR_MENU;
                    else currentScreen = SCREEN_ADMIN_MENU;
                    continue;
                }
                if (currentScreen != SCREEN_LOGIN) {
                    currentScreen = SCREEN_LOGIN;
                    loggedInId = -1; loggedInRole[0] = '\0';
                    idField.clear(); passField.clear();
                }
                continue;
            }

            // Close panel on click
            if (showPanel) {
                if (event.type == sf::Event::MouseButtonReleased) {
                    showPanel = false;
                    if (pendingFormAfterPanel) {
                        pendingFormAfterPanel = false;
                        openForm(pendingFormType);
                        pendingFormType = FORM_NONE;
                    }
                }
                continue;
            }

            // Logout (any screen except login/form)
            if (currentScreen != SCREEN_LOGIN && currentScreen != SCREEN_FORM) {
                if (logoutBtn.isClicked(window, event)) {
                    currentScreen = SCREEN_LOGIN;
                    loggedInId = -1; loggedInRole[0] = '\0';
                    idField.clear(); passField.clear();
                    statusMessage = "Logged out.";
                    statusColor = sf::Color(200, 200, 200);
                    showPanel = false;
                    eventHandled = true;
                }
            }
            if (eventHandled) continue;

            //  LOGIN 
            if (currentScreen == SCREEN_LOGIN) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i m = sf::Mouse::getPosition(window);
                    idField.active = idField.box.getGlobalBounds().contains(
                        static_cast<float>(m.x), static_cast<float>(m.y));
                    passField.active = passField.box.getGlobalBounds().contains(
                        static_cast<float>(m.x), static_cast<float>(m.y));
                }
                idField.handleEvent(event);
                passField.handleEvent(event);

                auto tryLogin = [&](auto& collection, const char* role,
                    Screen nextScreen) {
                        bool found = false;
                        for (int i = 0; i < static_cast<int>(collection.size()); i++) {
                            char idBuf[20];
                            MyString::fromInt(collection[i].getId(), idBuf);
                            if (MyString::compare(idBuf, idField.getValue()) == 0 &&
                                MyString::compare(collection[i].getPassword(),
                                    passField.getValue()) == 0) {
                                loggedInId = collection[i].getId();
                                MyString::copy(loggedInRole, role, 20);
                                currentScreen = nextScreen;
                                statusMessage = "";
                                found = true;
                                // log success with real time
                                appendSecurityLogEntry(FilePaths::SECURITY_LOG,
                                    role, idBuf, "SUCCESS");
                                break;
                            }
                        }
                        if (!found) {
                            statusMessage = std::string("Invalid ") + role
                                + " ID or password.";
                            statusColor = ACCENT;
                            // log failure with real time
                            appendSecurityLogEntry(FilePaths::SECURITY_LOG,
                                role, idField.getValue(), "FAILED");
                        }
                    };

                if (patientBtn.isClicked(window, event))
                    tryLogin(sys.patients, "patient", SCREEN_PATIENT_MENU);
                if (doctorBtn.isClicked(window, event))
                    tryLogin(sys.doctors, "doctor", SCREEN_DOCTOR_MENU);
                if (adminBtn.isClicked(window, event))
                    tryLogin(sys.admins, "admin", SCREEN_ADMIN_MENU);
            }

            //FORM 
            if (currentScreen == SCREEN_FORM) {
                float formBottom = formStartY
                    + static_cast<float>(formFields.size())
                    * (formFieldH + H * 0.04f) + H * 0.03f;
                submitBtn.setPosition(cx - pw * 0.5f, formBottom);
                cancelBtn.setPosition(cx + pw * 0.02f, formBottom);

                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i m = sf::Mouse::getPosition(window);
                    for (auto f : formFields)
                        f->active = f->box.getGlobalBounds().contains(
                            static_cast<float>(m.x), static_cast<float>(m.y));

                    // slot button click detection
                    if (currentForm == FORM_BOOK_APPOINTMENT
                        && bookStep == 3
                        && !availableSlots.empty()) {
                        float slotBtnW = 90.f, slotBtnH = 36.f, slotGap = 8.f;
                        float slotY = formStartY + formFieldH + H * 0.02f;
                        for (int s = 0;
                            s < static_cast<int>(availableSlots.size()); s++) {
                            float sx = (W - pw) / 2.f
                                + static_cast<float>(s) * (slotBtnW + slotGap);
                            if (m.x >= sx && m.x <= sx + slotBtnW
                                && m.y >= slotY && m.y <= slotY + slotBtnH) {
                                // Fill the input field with the clicked slot
                                if (!formFields.empty()) {
                                    formFields[0]->value = availableSlots[s];
                                    formFields[0]->text.setString(
                                        availableSlots[s]);
                                }
                                eventHandled = true;
                                break;
                            }
                        }
                    }
                }

                for (auto f : formFields) f->handleEvent(event);

                if (!eventHandled && submitBtn.isClicked(window, event)) {
                    submitForm(); eventHandled = true;
                }
                if (!eventHandled && cancelBtn.isClicked(window, event)) {
                    clearForm();
                    if (MyString::compare(loggedInRole, "patient") == 0)
                        currentScreen = SCREEN_PATIENT_MENU;
                    else if (MyString::compare(loggedInRole, "doctor") == 0)
                        currentScreen = SCREEN_DOCTOR_MENU;
                    else currentScreen = SCREEN_ADMIN_MENU;
                    eventHandled = true;
                }
                if (eventHandled) continue;
            }

            // PATIENT MENU 
            if (!eventHandled && currentScreen == SCREEN_PATIENT_MENU) {
                if (bookBtn.isClicked(window, event)) {
                    bookStep = 0; bookSpec[0] = '\0'; bookDocId = -1;
                    bookDate[0] = '\0'; bookDateAttempts = 0;
                    availableSlots.clear();
                    openForm(FORM_BOOK_APPOINTMENT); eventHandled = true;
                }
                if (payBillBtn.isClicked(window, event)) {
                    //  show all unpaid bills in panel before form
                    panelTitle = "Your Unpaid Bills";
                    panelLines.clear();
                    panelLines.push_back("Bill ID  |  Amount (Rs.)  |  Date");
                    panelLines.push_back("------------------------------------");
                    bool any = false;
                    for (int i = 0; i < static_cast<int>(sys.bills.size()); i++) {
                        if (sys.bills[i].getPatientId() == loggedInId &&
                            MyString::compare(sys.bills[i].getStatus(), "unpaid") == 0) {
                            char buf[200], bid[10], amt[20];
                            MyString::fromInt(sys.bills[i].getId(), bid);
                            MyString::fromFloat(sys.bills[i].getAmount(), amt);
                            MyString::copy(buf, bid);
                            MyString::concat(buf, "  |  Rs.");
                            MyString::concat(buf, amt);
                            MyString::concat(buf, "  |  ");
                            MyString::concat(buf, sys.bills[i].getDate());
                            panelLines.push_back(buf);
                            any = true;
                        }
                    }
                    if (!any) panelLines.push_back("No unpaid bills.");
                    showPanel = true;
                    pendingFormAfterPanel = true;
                    pendingFormType = FORM_PAY_BILL;
                    eventHandled = true;
                }
                if (topUpBtn.isClicked(window, event)) {
                    openForm(FORM_TOP_UP); eventHandled = true;
                }
                if (cancelAppBtn.isClicked(window, event)) {
                    // show ALL pending appointments before cancel form
                    panelTitle = "Your Pending Appointments";
                    panelLines.clear();
                    panelLines.push_back(
                        "Appt ID  |  Doctor ID  |  Date  |  Time");
                    panelLines.push_back("-------------------------------------");
                    bool any = false;
                    for (int i = 0;
                        i < static_cast<int>(sys.appointments.size()); i++) {
                        if (sys.appointments[i].getPatientId() == loggedInId &&
                            MyString::compare(
                                sys.appointments[i].getStatus(), "pending") == 0) {
                            char buf[200], aid[10], did[10];
                            MyString::fromInt(sys.appointments[i].getId(), aid);
                            MyString::fromInt(
                                sys.appointments[i].getDoctorId(), did);
                            MyString::copy(buf, "#"); MyString::concat(buf, aid);
                            MyString::concat(buf, "  |  Dr.");
                            MyString::concat(buf, did);
                            MyString::concat(buf, "  |  ");
                            MyString::concat(buf, sys.appointments[i].getDate());
                            MyString::concat(buf, "  ");
                            MyString::concat(buf,
                                sys.appointments[i].getTimeSlot());
                            panelLines.push_back(buf);
                            any = true;
                        }
                    }
                    if (!any) panelLines.push_back("No pending appointments.");
                    showPanel = true;
                    pendingFormAfterPanel = true;
                    pendingFormType = FORM_CANCEL_APPOINTMENT;
                    eventHandled = true;
                }

                if (!eventHandled && viewAppBtn.isClicked(window, event)) {
                    std::vector<std::string> lines;
                    for (int i = 0;
                        i < static_cast<int>(sys.appointments.size()); i++) {
                        if (sys.appointments[i].getPatientId() == loggedInId) {
                            char buf[200], aid[10], did[10];
                            MyString::fromInt(sys.appointments[i].getId(), aid);
                            MyString::fromInt(
                                sys.appointments[i].getDoctorId(), did);
                            MyString::copy(buf, "Appt #");
                            MyString::concat(buf, aid);
                            MyString::concat(buf, "  |  Dr.");
                            MyString::concat(buf, did);
                            MyString::concat(buf, "  |  ");
                            MyString::concat(buf, sys.appointments[i].getDate());
                            MyString::concat(buf, " ");
                            MyString::concat(buf,
                                sys.appointments[i].getTimeSlot());
                            MyString::concat(buf, "  |  ");
                            MyString::concat(buf, sys.appointments[i].getStatus());
                            lines.push_back(buf);
                        }
                    }
                    if (lines.empty()) lines.push_back("No appointments found.");
                    showData("My Appointments", lines);
                }
                if (!eventHandled && viewMedBtn.isClicked(window, event)) {
                    std::vector<std::string> lines;
                    for (int i = 0;
                        i < static_cast<int>(sys.prescriptions.size()); i++) {
                        if (sys.prescriptions[i].getPatientId() == loggedInId) {
                            char buf[300];
                            MyString::copy(buf, sys.prescriptions[i].getDate());
                            MyString::concat(buf, "  |  ");
                            MyString::concat(buf,
                                sys.prescriptions[i].getMedicines());
                            lines.push_back(buf);
                            char buf2[300];
                            MyString::copy(buf2, "   Notes: ");
                            MyString::concat(buf2,
                                sys.prescriptions[i].getNotes());
                            lines.push_back(buf2);
                            lines.push_back("");
                        }
                    }
                    if (lines.empty()) lines.push_back("No records found.");
                    showData("My Medical Records", lines);
                }
                if (!eventHandled && viewBillBtn.isClicked(window, event)) {
                    std::vector<std::string> lines;
                    float totalUnpaid = 0;
                    for (int i = 0; i < static_cast<int>(sys.bills.size()); i++) {
                        if (sys.bills[i].getPatientId() == loggedInId) {
                            char buf[200], bid[10], amt[20];
                            MyString::fromInt(sys.bills[i].getId(), bid);
                            MyString::fromFloat(sys.bills[i].getAmount(), amt);
                            MyString::copy(buf, "Bill #");
                            MyString::concat(buf, bid);
                            MyString::concat(buf, "  |  Rs.");
                            MyString::concat(buf, amt);
                            MyString::concat(buf, "  |  ");
                            MyString::concat(buf, sys.bills[i].getStatus());
                            MyString::concat(buf, "  |  ");
                            MyString::concat(buf, sys.bills[i].getDate());
                            lines.push_back(buf);
                            if (MyString::compare(
                                sys.bills[i].getStatus(), "unpaid") == 0)
                                totalUnpaid += sys.bills[i].getAmount();
                        }
                    }
                    if (lines.empty()) lines.push_back("No bills found.");
                    else {
                        char tot[20]; MyString::fromFloat(totalUnpaid, tot);
                        lines.push_back("");
                        lines.push_back(
                            "Total Outstanding: Rs." + std::string(tot));
                    }
                    showData("My Bills", lines);
                }
                if (!eventHandled && viewPresBtn.isClicked(window, event)) {
                    std::vector<std::string> lines;
                    for (int i = 0;
                        i < static_cast<int>(sys.prescriptions.size()); i++) {
                        if (sys.prescriptions[i].getPatientId() == loggedInId) {
                            char buf[300], pid[10];
                            MyString::fromInt(sys.prescriptions[i].getId(), pid);
                            MyString::copy(buf, "Rx #");
                            MyString::concat(buf, pid);
                            MyString::concat(buf, "  |  ");
                            MyString::concat(buf,
                                sys.prescriptions[i].getMedicines());
                            lines.push_back(buf);
                        }
                    }
                    if (lines.empty()) lines.push_back("No prescriptions.");
                    showData("My Prescriptions", lines);
                }
            }

            // DOCTOR MENU 
            if (!eventHandled && currentScreen == SCREEN_DOCTOR_MENU) {
                if (addPresBtn.isClicked(window, event)) {
                    openForm(FORM_ADD_PRESCRIPTION); eventHandled = true;
                }
                if (viewPatHistBtn.isClicked(window, event)) {
                    openForm(FORM_VIEW_PAT_HISTORY); eventHandled = true;
                }

                auto showTodayPending = [&]() {
                    char todayBuf[15]; Validator::getTodayDate(todayBuf);
                    panelLines.clear();
                    panelTitle = std::string("Today's Pending — ") + todayBuf;
                    panelLines.push_back("Appt ID  |  Patient ID  |  Time");
                    panelLines.push_back("----------------------------------");
                    bool any = false;
                    // filter by today's date
                    for (int i = 0;
                        i < static_cast<int>(sys.appointments.size()); i++) {
                        if (sys.appointments[i].getDoctorId() == loggedInId &&
                            MyString::compare(
                                sys.appointments[i].getDate(), todayBuf) == 0 &&
                            MyString::compare(
                                sys.appointments[i].getStatus(), "pending") == 0) {
                            char buf[200], aid[10], pid[10];
                            MyString::fromInt(sys.appointments[i].getId(), aid);
                            MyString::fromInt(
                                sys.appointments[i].getPatientId(), pid);
                            MyString::copy(buf, "#");
                            MyString::concat(buf, aid);
                            MyString::concat(buf, "  |  Patient ");
                            MyString::concat(buf, pid);
                            MyString::concat(buf, "  |  ");
                            MyString::concat(buf,
                                sys.appointments[i].getTimeSlot());
                            panelLines.push_back(buf);
                            any = true;
                        }
                    }
                    if (!any) panelLines.push_back("No pending today.");
                    showPanel = true;
                    };

                if (!eventHandled && markCompleteBtn.isClicked(window, event)) {
                    showTodayPending();
                    pendingFormAfterPanel = true;
                    pendingFormType = FORM_MARK_COMPLETE;
                    eventHandled = true;
                }
                if (!eventHandled && markNoshowBtn.isClicked(window, event)) {
                    showTodayPending();
                    pendingFormAfterPanel = true;
                    pendingFormType = FORM_MARK_NOSHOW;
                    eventHandled = true;
                }

                //  View today's appointments only
                if (!eventHandled && viewDocAppBtn.isClicked(window, event)) {
                    char todayBuf[15]; Validator::getTodayDate(todayBuf);
                    std::vector<std::string> lines;
                    lines.push_back(std::string("Today: ") + todayBuf);
                    lines.push_back("-------------------------------------");
                    for (int i = 0;
                        i < static_cast<int>(sys.appointments.size()); i++) {
                        if (sys.appointments[i].getDoctorId() == loggedInId &&
                            MyString::compare(
                                sys.appointments[i].getDate(), todayBuf) == 0) {
                            char buf[200], aid[10], pid[10];
                            MyString::fromInt(sys.appointments[i].getId(), aid);
                            MyString::fromInt(
                                sys.appointments[i].getPatientId(), pid);
                            MyString::copy(buf, "Appt #");
                            MyString::concat(buf, aid);
                            MyString::concat(buf, "  |  Patient ");
                            MyString::concat(buf, pid);
                            MyString::concat(buf, "  |  ");
                            MyString::concat(buf,
                                sys.appointments[i].getTimeSlot());
                            MyString::concat(buf, "  |  ");
                            MyString::concat(buf, sys.appointments[i].getStatus());
                            lines.push_back(buf);
                        }
                    }
                    if (lines.size() <= 2) lines.push_back("None today.");
                    showData("Today's Appointments", lines);
                }
                if (!eventHandled && viewPatientsBtn.isClicked(window, event)) {
                    std::vector<std::string> lines;
                    for (int i = 0;
                        i < static_cast<int>(sys.patients.size()); i++) {
                        char buf[200], pid[10];
                        MyString::fromInt(sys.patients[i].getId(), pid);
                        MyString::copy(buf, "ID: "); MyString::concat(buf, pid);
                        MyString::concat(buf, "  |  ");
                        MyString::concat(buf, sys.patients[i].getName());
                        MyString::concat(buf, "  |  ");
                        MyString::concat(buf, sys.patients[i].getContact());
                        lines.push_back(buf);
                    }
                    if (lines.empty()) lines.push_back("No patients.");
                    showData("All Patients", lines);
                }
            }

            // ADMIN MENU 
            if (!eventHandled && currentScreen == SCREEN_ADMIN_MENU) {
                if (addDocBtn.isClicked(window, event)) {
                    openForm(FORM_ADD_DOCTOR); eventHandled = true;
                }
                if (addPatBtn.isClicked(window, event)) {
                    openForm(FORM_ADD_PATIENT); eventHandled = true;
                }
                if (dischargePatBtn.isClicked(window, event)) {
                    // FIX 6: Show all patients before discharge form
                    panelTitle = "All Patients — Select ID to Discharge";
                    panelLines.clear();
                    panelLines.push_back(
                        "ID  |  Name  |  Age  |  Balance");
                    panelLines.push_back("----------------------------------");
                    for (int i = 0;
                        i < static_cast<int>(sys.patients.size()); i++) {
                        char buf[200], pid[10], age[10], bal[20];
                        MyString::fromInt(sys.patients[i].getId(), pid);
                        MyString::fromInt(sys.patients[i].getAge(), age);
                        MyString::fromFloat(sys.patients[i].getBalance(), bal);
                        MyString::copy(buf, pid);
                        MyString::concat(buf, "  |  ");
                        MyString::concat(buf, sys.patients[i].getName());
                        MyString::concat(buf, "  |  ");
                        MyString::concat(buf, age);
                        MyString::concat(buf, "  |  Rs.");
                        MyString::concat(buf, bal);
                        panelLines.push_back(buf);
                    }
                    if (sys.patients.size() == 0)
                        panelLines.push_back("No patients.");
                    showPanel = true;
                    pendingFormAfterPanel = true;
                    pendingFormType = FORM_DISCHARGE_PATIENT;
                    eventHandled = true;
                }
                if (!eventHandled && removeDocBtn.isClicked(window, event)) {
                    // FIX 5: Show all doctors before remove form
                    panelTitle = "All Doctors — Select ID to Remove";
                    panelLines.clear();
                    panelLines.push_back(
                        "ID  |  Name  |  Specialization  |  Fee");
                    panelLines.push_back("------------------------------------");
                    for (int i = 0;
                        i < static_cast<int>(sys.doctors.size()); i++) {
                        char buf[200], did[10], fee[20];
                        MyString::fromInt(sys.doctors[i].getId(), did);
                        MyString::fromFloat(sys.doctors[i].getFee(), fee);
                        MyString::copy(buf, did);
                        MyString::concat(buf, "  |  ");
                        MyString::concat(buf, sys.doctors[i].getName());
                        MyString::concat(buf, "  |  ");
                        MyString::concat(buf, sys.doctors[i].getSpecialization());
                        MyString::concat(buf, "  |  Rs.");
                        MyString::concat(buf, fee);
                        panelLines.push_back(buf);
                    }
                    if (sys.doctors.size() == 0)
                        panelLines.push_back("No doctors.");
                    showPanel = true;
                    pendingFormAfterPanel = true;
                    pendingFormType = FORM_REMOVE_DOCTOR;
                    eventHandled = true;
                }
                if (!eventHandled && removePatBtn.isClicked(window, event)) {
                    panelTitle = "All Patients — Select ID to Remove";
                    panelLines.clear();
                    panelLines.push_back(
                        "ID  |  Name  |  Age  |  Balance");
                    panelLines.push_back("----------------------------------");
                    for (int i = 0;
                        i < static_cast<int>(sys.patients.size()); i++) {
                        char buf[200], pid[10], age[10], bal[20];
                        MyString::fromInt(sys.patients[i].getId(), pid);
                        MyString::fromInt(sys.patients[i].getAge(), age);
                        MyString::fromFloat(sys.patients[i].getBalance(), bal);
                        MyString::copy(buf, pid);
                        MyString::concat(buf, "  |  ");
                        MyString::concat(buf, sys.patients[i].getName());
                        MyString::concat(buf, "  |  ");
                        MyString::concat(buf, age);
                        MyString::concat(buf, "  |  Rs.");
                        MyString::concat(buf, bal);
                        panelLines.push_back(buf);
                    }
                    if (sys.patients.size() == 0)
                        panelLines.push_back("No patients.");
                    showPanel = true;
                    pendingFormAfterPanel = true;
                    pendingFormType = FORM_REMOVE_PATIENT;
                    eventHandled = true;
                }
                if (!eventHandled && reportBtn.isClicked(window, event)) {
                    char todayBuf[15]; Validator::getTodayDate(todayBuf);
                    std::string date = todayBuf;
                    panelTitle = "Daily Report — " + date;
                    panelLines.clear();
                    int pending = 0, completed = 0, noshow = 0, cancelled = 0;
                    float revenue = 0;
                    for (int i = 0;
                        i < static_cast<int>(sys.appointments.size()); i++) {
                        if (MyString::compare(
                            sys.appointments[i].getDate(), date.c_str()) == 0) {
                            if (MyString::compare(
                                sys.appointments[i].getStatus(), "pending") == 0)
                                pending++;
                            else if (MyString::compare(
                                sys.appointments[i].getStatus(), "completed") == 0)
                                completed++;
                            else if (MyString::compare(
                                sys.appointments[i].getStatus(), "noshow") == 0)
                                noshow++;
                            else if (MyString::compare(
                                sys.appointments[i].getStatus(), "cancelled") == 0)
                                cancelled++;
                        }
                    }
                    for (int i = 0; i < static_cast<int>(sys.bills.size()); i++)
                        if (MyString::compare(
                            sys.bills[i].getDate(), date.c_str()) == 0 &&
                            MyString::compare(
                                sys.bills[i].getStatus(), "paid") == 0)
                            revenue += sys.bills[i].getAmount();
                    char tmp[20];
                    MyString::fromInt(
                        pending + completed + noshow + cancelled, tmp);
                    panelLines.push_back(
                        "Total appointments: " + std::string(tmp));
                    MyString::fromInt(pending, tmp);
                    std::string s = "  Pending:" + std::string(tmp);
                    MyString::fromInt(completed, tmp);
                    s += "  Completed:" + std::string(tmp);
                    MyString::fromInt(noshow, tmp);
                    s += "  No-show:" + std::string(tmp);
                    MyString::fromInt(cancelled, tmp);
                    s += "  Cancelled:" + std::string(tmp);
                    panelLines.push_back(s);
                    char rev[20]; MyString::fromFloat(revenue, rev);
                    panelLines.push_back(
                        "Revenue today: Rs." + std::string(rev));
                    panelLines.push_back("");
                    panelLines.push_back("Unpaid outstanding:");
                    for (int i = 0;
                        i < static_cast<int>(sys.patients.size()); i++) {
                        float owed = 0;
                        for (int j = 0;
                            j < static_cast<int>(sys.bills.size()); j++)
                            if (sys.bills[j].getPatientId()
                                == sys.patients[i].getId() &&
                                MyString::compare(
                                    sys.bills[j].getStatus(), "unpaid") == 0)
                                owed += sys.bills[j].getAmount();
                        if (owed > 0) {
                            char buf[200], amt[20];
                            MyString::fromFloat(owed, amt);
                            MyString::copy(buf, sys.patients[i].getName());
                            MyString::concat(buf, "  |  Rs.");
                            MyString::concat(buf, amt);
                            panelLines.push_back(buf);
                        }
                    }
                    showPanel = true; eventHandled = true;
                }
                if (!eventHandled && secLogBtn.isClicked(window, event)) {
                    std::vector<std::string> lines;
                    std::ifstream f(FilePaths::SECURITY_LOG);
                    if (f.is_open()) {
                        char line[400];
                        while (f.getline(line, 400))
                            lines.push_back(line);
                        f.close();
                    }
                    if (lines.empty()) lines.push_back("No entries.");
                    showData("Security Log", lines);
                }
                if (!eventHandled && viewAllPatsBtn.isClicked(window, event)) {
                    std::vector<std::string> lines;
                    for (int i = 0;
                        i < static_cast<int>(sys.patients.size()); i++) {
                        char buf[200], pid[10], age[10], bal[20];
                        MyString::fromInt(sys.patients[i].getId(), pid);
                        MyString::fromInt(sys.patients[i].getAge(), age);
                        MyString::fromFloat(sys.patients[i].getBalance(), bal);
                        MyString::copy(buf, "ID:"); MyString::concat(buf, pid);
                        MyString::concat(buf, "  ");
                        MyString::concat(buf, sys.patients[i].getName());
                        MyString::concat(buf, "  Age:");
                        MyString::concat(buf, age);
                        MyString::concat(buf, "  Rs.");
                        MyString::concat(buf, bal);
                        lines.push_back(buf);
                    }
                    if (lines.empty()) lines.push_back("No patients.");
                    showData("All Patients", lines);
                }
                if (!eventHandled && viewAllDocsBtn.isClicked(window, event)) {
                    std::vector<std::string> lines;
                    for (int i = 0;
                        i < static_cast<int>(sys.doctors.size()); i++) {
                        char buf[200], did[10], fee[20];
                        MyString::fromInt(sys.doctors[i].getId(), did);
                        MyString::fromFloat(sys.doctors[i].getFee(), fee);
                        MyString::copy(buf, "ID:"); MyString::concat(buf, did);
                        MyString::concat(buf, "  ");
                        MyString::concat(buf, sys.doctors[i].getName());
                        MyString::concat(buf, "  ");
                        MyString::concat(buf, sys.doctors[i].getSpecialization());
                        MyString::concat(buf, "  Rs.");
                        MyString::concat(buf, fee);
                        lines.push_back(buf);
                    }
                    if (lines.empty()) lines.push_back("No doctors.");
                    showData("All Doctors", lines);
                }
                if (!eventHandled && viewAllApptsBtn.isClicked(window, event)) {
                    std::vector<std::string> lines;
                    for (int i = 0;
                        i < static_cast<int>(sys.appointments.size()); i++) {
                        char buf[200], aid[10], pid[10], did[10];
                        MyString::fromInt(sys.appointments[i].getId(), aid);
                        MyString::fromInt(
                            sys.appointments[i].getPatientId(), pid);
                        MyString::fromInt(
                            sys.appointments[i].getDoctorId(), did);
                        MyString::copy(buf, "#"); MyString::concat(buf, aid);
                        MyString::concat(buf, "  P:");
                        MyString::concat(buf, pid);
                        MyString::concat(buf, " D:");
                        MyString::concat(buf, did);
                        MyString::concat(buf, "  ");
                        MyString::concat(buf, sys.appointments[i].getDate());
                        MyString::concat(buf, " ");
                        MyString::concat(buf,
                            sys.appointments[i].getTimeSlot());
                        MyString::concat(buf, "  ");
                        MyString::concat(buf, sys.appointments[i].getStatus());
                        lines.push_back(buf);
                    }
                    if (lines.empty()) lines.push_back("No appointments.");
                    showData("All Appointments", lines);
                }
                if (!eventHandled && viewUnpaidBtn.isClicked(window, event)) {
                    std::vector<std::string> lines;
                    char todayBuf[15]; Validator::getTodayDate(todayBuf);
                    for (int i = 0; i < static_cast<int>(sys.bills.size()); i++) {
                        if (MyString::compare(
                            sys.bills[i].getStatus(), "unpaid") == 0) {
                            char buf[200], bid[10], amt[20];
                            MyString::fromInt(sys.bills[i].getId(), bid);
                            MyString::fromFloat(sys.bills[i].getAmount(), amt);
                            MyString::copy(buf, "Bill #");
                            MyString::concat(buf, bid);
                            MyString::concat(buf, "  Rs.");
                            MyString::concat(buf, amt);
                            MyString::concat(buf, "  ");
                            MyString::concat(buf, sys.bills[i].getDate());
                            if (Validator::daysDifference(
                                sys.bills[i].getDate(), todayBuf) > 7)
                                MyString::concat(buf, "  [OVERDUE]");
                            lines.push_back(buf);
                        }
                    }
                    if (lines.empty()) lines.push_back("No unpaid bills.");
                    showData("Unpaid Bills", lines);
                }
                if (!eventHandled && viewAllBtn.isClicked(window, event)) {
                    std::vector<std::string> lines;
                    char cnt[10];
                    MyString::fromInt(
                        static_cast<int>(sys.patients.size()), cnt);
                    lines.push_back("Patients: " + std::string(cnt));
                    MyString::fromInt(
                        static_cast<int>(sys.doctors.size()), cnt);
                    lines.push_back("Doctors: " + std::string(cnt));
                    MyString::fromInt(
                        static_cast<int>(sys.appointments.size()), cnt);
                    lines.push_back("Appointments: " + std::string(cnt));
                    MyString::fromInt(
                        static_cast<int>(sys.bills.size()), cnt);
                    lines.push_back("Bills: " + std::string(cnt));
                    showData("All Records Summary", lines);
                }
            }
        } // end pollEvent

        // DRAW 
        drawBackground();

        if (currentScreen == SCREEN_LOGIN) {
            drawHeader("Login");
            drawLoginScreen();
            idField.draw(window);
            passField.draw(window);
            patientBtn.draw(window);
            doctorBtn.draw(window);
            adminBtn.draw(window);
        }
        else if (currentScreen == SCREEN_PATIENT_MENU) {
            drawHeader("Patient Dashboard");
            drawPatientMenu();
            bookBtn.draw(window);     cancelAppBtn.draw(window);
            viewAppBtn.draw(window);  viewMedBtn.draw(window);
            viewBillBtn.draw(window); payBillBtn.draw(window);
            topUpBtn.draw(window);    viewPresBtn.draw(window);
            logoutBtn.draw(window);
        }
        else if (currentScreen == SCREEN_DOCTOR_MENU) {
            drawHeader("Doctor Dashboard");
            drawDoctorMenu();
            viewPatientsBtn.draw(window); viewDocAppBtn.draw(window);
            markCompleteBtn.draw(window); markNoshowBtn.draw(window);
            addPresBtn.draw(window);      viewPatHistBtn.draw(window);
            logoutBtn.draw(window);
        }
        else if (currentScreen == SCREEN_ADMIN_MENU) {
            drawHeader("Admin Dashboard");
            drawAdminMenu();
            addDocBtn.draw(window);       removeDocBtn.draw(window);
            addPatBtn.draw(window);       viewAllPatsBtn.draw(window);
            viewAllDocsBtn.draw(window);  viewAllApptsBtn.draw(window);
            viewUnpaidBtn.draw(window);   dischargePatBtn.draw(window);
            viewAllBtn.draw(window);      reportBtn.draw(window);
            secLogBtn.draw(window);       removePatBtn.draw(window);
            logoutBtn.draw(window);
        }
        else if (currentScreen == SCREEN_FORM) {
            drawHeader(formTitle);
            drawForm();
            float formBottom = formStartY
                + static_cast<float>(formFields.size())
                * (formFieldH + H * 0.04f) + H * 0.03f;
            submitBtn.setPosition(cx - pw * 0.5f, formBottom);
            cancelBtn.setPosition(cx + pw * 0.02f, formBottom);
            submitBtn.draw(window);
            cancelBtn.draw(window);
        }

        if (showPanel) drawInfoPanel();
        drawStatus();
        window.display();
    }
}