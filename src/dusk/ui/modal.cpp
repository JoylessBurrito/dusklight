#include "modal.hpp"

#include "string_button.hpp"

namespace dusk::ui {

Modal::Modal(Props props) : WindowSmall("modal", "modal-dialog"), mProps(std::move(props)) {
    if (!mProps.variant.empty()) {
        mRoot->SetClass(mProps.variant, true);
    }

    auto* header = append(mDialog, "div");
    header->SetClass("modal-header", true);

    auto* title = append(header, "div");
    title->SetClass("modal-title", true);
    title->SetInnerRML(mProps.title);

    if (!mProps.icon.empty()) {
        auto* icon = append(header, "icon");
        icon->SetClass(mProps.icon, true);
    }

    auto* body = append(mDialog, "div");
    body->SetClass("modal-body", true);
    body->SetInnerRML(mProps.bodyRml);

    auto* actions = append(mDialog, "div");
    actions->SetClass("modal-actions", true);

    for (auto& action : mProps.actions) {
        add_action(action);
    }

    mDoAud_seStartMenu(kSoundWindowOpen);
}

bool Modal::focus() {
    if (!mButtons.empty()) {
        return mButtons.front()->focus();
    }
    return false;
}

void Modal::add_action(ModalAction action) {
    auto actions = mDialog->QuerySelector(".modal-actions");
    auto btn = std::make_unique<Button>(actions, action.label);
    btn->root()->SetClass("modal-btn", true);
    btn->on_pressed([this, callback = std::move(action.onPressed)] {
        if (callback) {
            callback(*this);
        }
    });
    mButtons.push_back(std::move(btn));
}

void Modal::set_body(const Rml::String& bodyRml) {
    auto body = mDialog->QuerySelector(".modal-body");
    body->SetInnerRML(bodyRml);
}

void Modal::set_icon(const Rml::String& iconStr) {
    auto icon = mDialog->QuerySelector("icon");
    icon->SetClassNames({iconStr});
}

void Modal::dismiss() {
    if (mProps.onDismiss) {
        mProps.onDismiss(*this);
        return;
    }
    pop();
}

bool Modal::handle_nav_command(Rml::Event& event, NavCommand cmd) {
    if (cmd == NavCommand::Cancel || cmd == NavCommand::Menu) {
        mDoAud_seStartMenu(kSoundWindowClose);
        dismiss();
        return true;
    }

    int direction = 0;
    if (cmd == NavCommand::Left) {
        direction = -1;
    } else if (cmd == NavCommand::Right) {
        direction = 1;
    } else {
        return false;
    }

    auto* target = event.GetTargetElement();
    for (int i = 0; i < static_cast<int>(mButtons.size()); ++i) {
        if (mButtons[i]->contains(target)) {
            const int next = i + direction;
            if (next >= 0 && next < static_cast<int>(mButtons.size()) && mButtons[next]->focus()) {
                mDoAud_seStartMenu(kSoundItemFocus);
                return true;
            }
            return false;
        }
    }
    return false;
}

TextInputModal::TextInputModal(Props props) : Modal(props) {
    auto modalBody = mDialog->QuerySelector(".modal-body");

    mInput = std::make_unique<StringButton>(modalBody, StringButton::Props{
        .getValue = [this]{return mInputText;},
        .setValue = [this](Rml::String value) { mInputText = value; },
    });

    mInput->start_editing();
}

bool TextInputModal::handle_nav_command(Rml::Event& event, NavCommand cmd) {
    auto retVal = Modal::handle_nav_command(event, cmd);
    if (!retVal) {
        if (mInput->root()->IsPseudoClassSet("focus") && cmd == NavCommand::Down) {
            mButtons[0]->focus();
            retVal = true;
        } else if (!mInput->root()->IsPseudoClassSet("focus") && cmd == NavCommand::Up) {
            mInput->focus();
            retVal = true;
        }
    }

    return retVal;
}

void TextInputModal::update() {
    mInput->update();
    Document::update();
}

MultiTextInputModal::MultiTextInputModal(Props props) : Modal(props) {}

void MultiTextInputModal::add_input_text(const Rml::String& label, const Rml::String& startValue) {
    auto modalBody = mDialog->QuerySelector(".modal-body");

    bool isFirst = mInputs.empty();

    mInputs.push_back(InputEntry {});
    auto& entry = mInputs.back();
    int idx = mInputs.size() - 1;

    entry.text = startValue;
    entry.button = std::make_unique<StringButton>(modalBody, StringButton::Props{
        .key = label,
        .getValue = [idx, this]{return mInputs[idx].text;},
        .setValue = [idx, this](Rml::String value) { mInputs[idx].text = value; },
    });

    if (isFirst)
        entry.button->start_editing();
}

bool MultiTextInputModal::handle_nav_command(Rml::Event& event, NavCommand cmd) {
    auto retVal = Modal::handle_nav_command(event, cmd);
    if (!retVal) {
        if (cmd == NavCommand::Down) {
            if (mTextSelIdx < mInputs.size()) {
                mTextSelIdx++;
                if (mTextSelIdx >= mInputs.size()) {
                    mButtons[0]->focus();
                }else {
                    mInputs[mTextSelIdx].button->focus();
                }
            }else {
                mTextSelIdx = 0;
                mInputs[mTextSelIdx].button->focus();
            }

            mDoAud_seStartMenu(kSoundItemFocus);
            retVal = true;
        }else if (cmd == NavCommand::Up && mTextSelIdx >= 0) {
            mTextSelIdx--;
            if (mTextSelIdx < 0) {
                mTextSelIdx = mInputs.size();
                mButtons[0]->focus();
            }else {
                mInputs[mTextSelIdx].button->focus();
            }

            mDoAud_seStartMenu(kSoundItemFocus);
            retVal = true;
        }
    }

    return retVal;
}

void MultiTextInputModal::update() {
    for (const auto& input : mInputs) {
        input.button->update();
    }
    Modal::update();
}
}  // namespace dusk::ui
