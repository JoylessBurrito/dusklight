#pragma once

#include "button.hpp"
#include "window.hpp"

namespace dusk::ui {
class Modal;

struct ModalAction {
    Rml::String label;
    std::function<void(Modal&)> onPressed;
};

class Modal : public WindowSmall {
public:
    struct Props {
        Rml::String title;
        Rml::String bodyRml;
        std::vector<ModalAction> actions;
        std::function<void(Modal&)> onDismiss;
        Rml::String variant;
        Rml::String icon = "";
    };

    explicit Modal(Props props);

    bool focus() override;
    void add_action(ModalAction action);
    void set_body(const Rml::String& bodyRml);
    void set_icon(const Rml::String& icon);

protected:
    bool handle_nav_command(Rml::Event& event, NavCommand cmd) override;
    void dismiss();

    Props mProps;
    std::vector<std::unique_ptr<Button> > mButtons;
};

class StringButton;

class TextInputModal : public Modal {
public:
    explicit TextInputModal(Props props);

    Rml::String get_input_text() {return mInputText;}
    void update() override;

protected:
    bool handle_nav_command(Rml::Event& event, NavCommand cmd) override;
private:
    std::unique_ptr<StringButton> mInput{};
    Rml::String mInputText{};
};

class MultiTextInputModal : public Modal {
public:
    explicit MultiTextInputModal(Props props);

    void add_input_text(const Rml::String& label, const Rml::String& startValue = "");

    Rml::String get_input_text(int idx) {return mInputs[idx].text; }

    void update() override;

protected:
    bool handle_nav_command(Rml::Event& event, NavCommand cmd) override;
private:
    struct InputEntry {
        std::unique_ptr<StringButton> button;
        Rml::String text;
    };

    std::vector<InputEntry> mInputs{};
    int mTextSelIdx = 0;
};


}  // namespace dusk::ui
